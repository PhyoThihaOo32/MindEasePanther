#ifndef JOURNALSTORAGE_H
#define JOURNALSTORAGE_H

#include "models/journalentry.h"
#include <QString>
#include <QVector>

// ─────────────────────────────────────────────────────────────────────────────
// JournalStorage — simple file I/O service for journal entries
//
// Responsibility split:
//   • Journal        → UI and interaction
//   • JournalEntry   → data model
//   • JournalStorage → save/load/list plain-text files on disk
// ─────────────────────────────────────────────────────────────────────────────

class JournalStorage {
public:
    JournalStorage();
    explicit JournalStorage(const QString &directoryPath);

    QString directoryPath() const;

    bool saveEntry(const JournalEntry &entry) const;
    bool deleteEntry(const QString &path) const;
    QVector<JournalEntry> loadEntries() const;
    JournalEntry loadEntry(const QString &path) const;

private:
    QString resolveDirectoryPath() const;
    QString buildEntryPath(const QDateTime &dateTime) const;
    QString formatEntry(const JournalEntry &entry) const;
    JournalEntry parseEntryFile(const QString &path, const QString &content) const;

    QString m_directoryPath;
};

#endif // JOURNALSTORAGE_H
