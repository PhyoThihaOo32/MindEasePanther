#ifndef MOODLOG_H
#define MOODLOG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include "moodentry.h"

class MoodLog : public QWidget {
    Q_OBJECT
public:
    explicit MoodLog(QWidget *parent = nullptr);
    void refreshEntries(const QVector<MoodEntry> &entries);

private:
    QVBoxLayout *listLayout;
    QWidget     *listContainer;
};

#endif // MOODLOG_H
