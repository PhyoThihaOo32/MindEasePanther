#include "moodentry.h"
#include <QStringList>

MoodEntry::MoodEntry() {}

MoodEntry::MoodEntry(const QString &mood, const QString &emoji, const QString &note, const QDateTime &timestamp)
    : mood(mood), emoji(emoji), note(note), timestamp(timestamp) {}

QString MoodEntry::toCsvLine() const {
    // Format: timestamp|mood|emoji|note
    QString safeNote = note;
    safeNote.replace("|", " "); // prevent delimiter collision
    return QString("%1|%2|%3|%4")
        .arg(timestamp.toString(Qt::ISODate))
        .arg(mood)
        .arg(emoji)
        .arg(safeNote);
}

MoodEntry MoodEntry::fromCsvLine(const QString &line) {
    QStringList parts = line.split("|");
    if (parts.size() < 4) return MoodEntry();
    MoodEntry e;
    e.timestamp = QDateTime::fromString(parts[0], Qt::ISODate);
    e.mood      = parts[1];
    e.emoji     = parts[2];
    e.note      = parts[3];
    return e;
}
