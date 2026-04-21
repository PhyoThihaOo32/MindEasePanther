#ifndef MOODENTRY_H
#define MOODENTRY_H

#include <QString>
#include <QDateTime>

// MoodEntry: represents a single logged mood
class MoodEntry {
public:
    MoodEntry();
    MoodEntry(const QString &mood, const QString &emoji, const QString &note, const QDateTime &timestamp = QDateTime::currentDateTime());

    QString mood;
    QString emoji;
    QString note;
    QDateTime timestamp;

    // Serialize to/from CSV line for File I/O
    QString toCsvLine() const;
    static MoodEntry fromCsvLine(const QString &line);
};

#endif // MOODENTRY_H
