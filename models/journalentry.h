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
//   Simple model      — this class now represents journal data only.
//                       All file I/O is handled by JournalStorage, which keeps
//                       persistence logic separate from the model and UI.
// ─────────────────────────────────────────────────────────────────────────────

class JournalEntry {
public:
    // Default-constructs an *invalid* empty entry (isValid() == false).
    JournalEntry();

    // Constructs a valid entry from a known timestamp and body text.
    JournalEntry(const QDateTime &dt, const QString &body,
                 const QString &filePath = QString());

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

private:
    QDateTime m_dateTime;   // timestamp of the entry
    QString   m_body;       // the user's written text (post-header body only)
    QString   m_filePath;   // absolute path on disk; empty until saved/loaded
};

#endif // JOURNALENTRY_H
