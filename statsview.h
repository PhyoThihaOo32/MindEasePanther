#ifndef STATSVIEW_H
#define STATSVIEW_H

#include <QWidget>
#include <QVector>
#include <QLabel>
#include "moodentry.h"

class StatsView : public QWidget {
    Q_OBJECT
public:
    explicit StatsView(QWidget *parent = nullptr);
    void refreshEntries(const QVector<MoodEntry> &entries);

private:
    QLabel *entriesNum;
    QLabel *avgNum;
    QLabel *streakNum;
    QLabel *chartLabel;
    QLabel *freqLabel;
};

#endif // STATSVIEW_H
