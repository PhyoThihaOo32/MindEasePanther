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

    QFile file(path);
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
    return resolveDirectoryPath() + "/"
           + dateTime.toString("yyyy-MM-dd_HH-mm-ss") + ".txt";
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
    const QString datePart = stem.left(10);
    const QString timePart = stem.mid(11).replace('-', ':');
    const QDateTime dateTime = QDateTime::fromString(
        datePart + " " + timePart, "yyyy-MM-dd HH:mm:ss");

    const QString separator = QString(30, QChar(0x2500));
    const int separatorIndex = content.indexOf(separator);
    const QString body = (separatorIndex >= 0)
        ? content.mid(separatorIndex + separator.length() + 2).trimmed()
        : content.trimmed();

    return JournalEntry(dateTime, body, path);
}
