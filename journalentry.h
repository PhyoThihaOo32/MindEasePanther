#ifndef JOURNALENTRY_H
#define JOURNALENTRY_H

#include <QString>
#include <QDateTime>

// ─────────────────────────────────────────────────────────────────────────────
// JournalEntry — value class representing one saved wellness journal entry
//
// OOP concepts demonstrated
// ─────────────────────────────────────────────────────────────────────────────
//   Encapsulation     — all data members (m_dateTime, m_body, m_filePath) are
//                       private. External code can only read them through the
//                       public const accessors dateTime(), body(), filePath().
//                       No public setters exist; an entry's content is set once
//                       at construction and never mutated from outside the class.
//
//   Operator overload — operator< is overloaded so that "newer" entries compare
//                       as "less than" older ones. This allows std::sort and
//                       Qt's sort utilities to naturally produce a newest-first
//                       ordering without any external comparator.
//                       operator== tests whether two entries represent the same
//                       timestamp and body text.
//
//   File I/O          — saveToFile() writes this entry to disk as a formatted
//                       plain-text file in a given directory.
//                       loadFromFile() (static factory) reconstructs an entry
//                       object by reading an existing file, demonstrating that
//                       file I/O is naturally modelled as class behaviour.
//
//   Static factory    — loadFromFile() is a static member function: it belongs
//                       to the class rather than any instance. It encapsulates
//                       the knowledge of how to parse the on-disk format and
//                       returns a fully-constructed JournalEntry value.
// ─────────────────────────────────────────────────────────────────────────────

class JournalEntry {
public:
    // Default-constructs an *invalid* empty entry (isValid() == false).
    JournalEntry();

    // Constructs a valid entry from a known timestamp and body text.
    JournalEntry(const QDateTime &dt, const QString &body);

    // ── Accessors (encapsulation: no public data members) ────────────────────
    QDateTime dateTime() const;   // when the entry was written
    QString   body()     const;   // the user's text (without the file header)
    QString   filePath() const;   // absolute path; set after save or load
    bool      isValid()  const;   // true iff dateTime is valid AND body non-empty

    // ── Operator overloads ────────────────────────────────────────────────────
    // Returns true when *this is NEWER than other, so that std::sort produces
    // a newest-first list. (Intentionally reverses the natural < direction.)
    bool operator<(const JournalEntry &other) const;

    // Two entries are equal iff they share the same timestamp AND body text.
    bool operator==(const JournalEntry &other) const;

    // ── File I/O ──────────────────────────────────────────────────────────────
    // Write this entry to <dir>/yyyy-MM-dd_HH-mm-ss.txt with a human-readable
    // header block. Creates the directory if it does not exist.
    // Returns true on success; on success, filePath() is updated.
    bool saveToFile(const QString &dir);

    // Static factory — read and parse an entry from an existing file.
    // The timestamp is derived from the filename (yyyy-MM-dd_HH-mm-ss.txt).
    // Returns an invalid JournalEntry if the file cannot be opened.
    static JournalEntry loadFromFile(const QString &path);

private:
    QDateTime m_dateTime;   // timestamp of the entry
    QString   m_body;       // the user's written text (post-header body only)
    QString   m_filePath;   // absolute path on disk; empty until saved/loaded
};

#endif // JOURNALENTRY_H
