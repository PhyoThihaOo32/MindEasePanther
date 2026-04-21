#include "models/journalentry.h"

// ─────────────────────────────────────────────────────────────────────────────
// JournalEntry implementation
// ─────────────────────────────────────────────────────────────────────────────

// ── Constructors ──────────────────────────────────────────────────────────────

// Default constructor — produces an invalid entry (m_dateTime is null).
JournalEntry::JournalEntry() {}

// Value constructor — stores both required fields.
JournalEntry::JournalEntry(const QDateTime &dt, const QString &body,
                           const QString &filePath)
    : m_dateTime(dt), m_body(body), m_filePath(filePath) {}

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
