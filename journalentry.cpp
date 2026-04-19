#include "journalentry.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>

// ─────────────────────────────────────────────────────────────────────────────
// JournalEntry implementation
// ─────────────────────────────────────────────────────────────────────────────

// ── Constructors ──────────────────────────────────────────────────────────────

// Default constructor — produces an invalid entry (m_dateTime is null).
JournalEntry::JournalEntry() {}

// Value constructor — stores both required fields.
JournalEntry::JournalEntry(const QDateTime &dt, const QString &body)
    : m_dateTime(dt), m_body(body) {}

// ── Accessors (encapsulation) ─────────────────────────────────────────────────

QDateTime JournalEntry::dateTime() const { return m_dateTime; }
QString   JournalEntry::body()     const { return m_body;     }
QString   JournalEntry::filePath() const { return m_filePath; }

// An entry is valid only when it carries a real timestamp and non-empty text.
bool JournalEntry::isValid() const {
    return m_dateTime.isValid() && !m_body.trimmed().isEmpty();
}

// ── Operator overloads ────────────────────────────────────────────────────────

// operator< is deliberately reversed so that a NEWER entry sorts "before"
// an older one. When QList<JournalEntry> is sorted with std::sort, the result
// is automatically newest-first — no external comparator needed.
bool JournalEntry::operator<(const JournalEntry &other) const {
    return m_dateTime > other.m_dateTime;
}

// Two entries are considered equal if they share both timestamp and body text.
bool JournalEntry::operator==(const JournalEntry &other) const {
    return m_dateTime == other.m_dateTime && m_body == other.m_body;
}

// ── File I/O ──────────────────────────────────────────────────────────────────

// saveToFile() — writes this entry to disk in a consistent plain-text format.
// File name encodes the timestamp so that files sort chronologically in the
// filesystem even without any metadata.
bool JournalEntry::saveToFile(const QString &dir) {
    if (!isValid()) return false;

    // Create the directory if it doesn't already exist.
    QDir d(dir);
    if (!d.exists()) d.mkpath(".");

    // File name: yyyy-MM-dd_HH-mm-ss.txt  (colons forbidden in some filesystems)
    const QString filename = m_dateTime.toString("yyyy-MM-dd_HH-mm-ss") + ".txt";
    const QString path     = dir + "/" + filename;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    // Write a human-readable header followed by the user's text.
    QTextStream out(&file);
    out << "=== MindEase Journal Entry ===\n";
    out << "Date: " << m_dateTime.toString("dddd, MMMM d, yyyy") << "\n";
    out << "Time: " << m_dateTime.toString("h:mm AP")            << "\n";
    out << "──────────────────────────────\n\n";   // 30 × U+2500
    out << m_body << "\n";
    file.close();

    m_filePath = path;    // update so filePath() is usable immediately after save
    return true;
}

// loadFromFile() — static factory that reads an existing entry file and
// returns a fully-constructed JournalEntry.
// Being static means no instance is needed to call it:
//   JournalEntry e = JournalEntry::loadFromFile("/path/to/file.txt");
JournalEntry JournalEntry::loadFromFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return JournalEntry{};   // returns an invalid entry on error

    QTextStream in(&file);
    const QString content = in.readAll();
    file.close();

    // ── Parse timestamp from the filename ─────────────────────────────────────
    // Expected filename stem: "yyyy-MM-dd_HH-mm-ss"
    const QFileInfo fi(path);
    const QString   stem     = fi.completeBaseName();          // "2026-04-15_14-30-00"
    const QString   datePart = stem.left(10);                  // "2026-04-15"
    const QString   timePart = stem.mid(11).replace('-', ':'); // "14:30:00"
    const QDateTime dt = QDateTime::fromString(
        datePart + " " + timePart, "yyyy-MM-dd HH:mm:ss");

    // ── Extract body from the file content ────────────────────────────────────
    // The header is separated from the body by a line of 30 × U+2500 ('─').
    const QString kSep = QString(30, QChar(0x2500));
    const int     sepIdx = content.indexOf(kSep);

    // Skip the separator (30 chars) plus the two newlines that follow it (\n\n)
    const QString body = (sepIdx >= 0)
        ? content.mid(sepIdx + kSep.length() + 2).trimmed()
        : content.trimmed();   // fallback: treat whole file as body

    JournalEntry entry(dt, body);
    entry.m_filePath = path;
    return entry;
}
