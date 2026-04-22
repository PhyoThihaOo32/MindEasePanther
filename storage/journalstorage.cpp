#include "storage/journalstorage.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>

#include <algorithm>

JournalStorage::JournalStorage() {}

JournalStorage::JournalStorage(const QString &directoryPath)
    : m_directoryPath(directoryPath) {}

QString JournalStorage::directoryPath() const {
    return resolveDirectoryPath();
}

bool JournalStorage::saveEntry(const JournalEntry &entry) const {
    if (!entry.isValid()) return false;

    QDir dir(resolveDirectoryPath());
    if (!dir.exists() && !dir.mkpath("."))
        return false;

    QFile file(buildEntryPath(entry.dateTime()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << formatEntry(entry);
    return true;
}

bool JournalStorage::deleteEntry(const QString &path) const {
    if (path.trimmed().isEmpty())
        return false;

    const QDir storageDir(resolveDirectoryPath());
    const QString storageRoot = storageDir.absolutePath() + QDir::separator();
    const QFileInfo targetInfo(path);

    // Only remove plain-text journal files that live inside the configured
    // MindEase journal directory. This keeps UI deletion from becoming a
    // generic file remover if a bad path ever reaches the list widget.
    if (!targetInfo.absoluteFilePath().startsWith(storageRoot)
        || targetInfo.suffix().compare("txt", Qt::CaseInsensitive) != 0)
        return false;

    QFile file(targetInfo.absoluteFilePath());
    if (!file.exists())
        return false;

    return file.remove();
}

QVector<JournalEntry> JournalStorage::loadEntries() const {
    QVector<JournalEntry> entries;

    QDir dir(resolveDirectoryPath());
    if (!dir.exists())
        return entries;

    const QStringList files = dir.entryList({"*.txt"}, QDir::Files, QDir::Name);
    for (const QString &fileName : files) {
        const JournalEntry entry = loadEntry(dir.filePath(fileName));
        if (entry.isValid())
            entries.append(entry);
    }

    std::sort(entries.begin(), entries.end());
    return entries;
}

JournalEntry JournalStorage::loadEntry(const QString &path) const {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return JournalEntry{};

    QTextStream in(&file);
    return parseEntryFile(path, in.readAll());
}

QString JournalStorage::resolveDirectoryPath() const {
    if (!m_directoryPath.isEmpty())
        return m_directoryPath;

    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
           + "/MindEase_Journal";
}

QString JournalStorage::buildEntryPath(const QDateTime &dateTime) const {
    const QString basePath = resolveDirectoryPath() + "/"
        + dateTime.toString("yyyy-MM-dd_HH-mm-ss-zzz");

    QString candidate = basePath + ".txt";
    int suffix = 1;
    while (QFileInfo::exists(candidate)) {
        candidate = QString("%1_%2.txt")
            .arg(basePath)
            .arg(suffix++, 2, 10, QChar('0'));
    }

    return candidate;
}

QString JournalStorage::formatEntry(const JournalEntry &entry) const {
    QString content;
    QTextStream out(&content);
    out << "=== MindEase Journal Entry ===\n";
    out << "Date: " << entry.dateTime().toString("dddd, MMMM d, yyyy") << "\n";
    out << "Time: " << entry.dateTime().toString("h:mm AP")            << "\n";
    out << "──────────────────────────────\n\n";
    out << entry.body() << "\n";
    return content;
}

JournalEntry JournalStorage::parseEntryFile(const QString &path,
                                            const QString &content) const {
    const QFileInfo fileInfo(path);
    const QString stem     = fileInfo.completeBaseName();
    const int modernStampLength = QString("yyyy-MM-dd_HH-mm-ss-zzz").length();
    QDateTime dateTime = QDateTime::fromString(
        stem.left(modernStampLength), "yyyy-MM-dd_HH-mm-ss-zzz");

    if (!dateTime.isValid()) {
        const QString datePart = stem.left(10);
        const QString timePart = stem.mid(11, 8).replace('-', ':');
        dateTime = QDateTime::fromString(
            datePart + " " + timePart, "yyyy-MM-dd HH:mm:ss");
    }

    const QString separator = QString(30, QChar(0x2500));
    const int separatorIndex = content.indexOf(separator);
    const QString body = (separatorIndex >= 0)
        ? content.mid(separatorIndex + separator.length() + 2).trimmed()
        : content.trimmed();

    return JournalEntry(dateTime, body, path);
}
