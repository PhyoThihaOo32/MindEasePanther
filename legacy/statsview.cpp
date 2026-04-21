#include "statsview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QMap>
#include <cmath>

static int moodScore(const QString &mood) {
    if (mood == "Happy")    return 5;
    if (mood == "Calm")     return 4;
    if (mood == "Neutral")  return 3;
    if (mood == "Anxious")  return 2;
    if (mood == "Sad")      return 2;
    if (mood == "Stressed") return 1;
    return 3;
}

static QWidget* statCard(const QString &num, const QString &lbl, QLabel **numOut) {
    QWidget *card = new QWidget();
    card->setStyleSheet("background: #f5f4f0; border-radius: 8px;");
    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->setContentsMargins(12,12,12,12); cl->setSpacing(3);
    QLabel *n = new QLabel(num);
    n->setStyleSheet("font-size: 24px; font-weight: 700; color: #1a1a1a;");
    n->setAlignment(Qt::AlignCenter);
    QLabel *l = new QLabel(lbl);
    l->setStyleSheet("font-size: 11px; color: #888;");
    l->setAlignment(Qt::AlignCenter);
    cl->addWidget(n); cl->addWidget(l);
    if (numOut) *numOut = n;
    return card;
}

StatsView::StatsView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(24, 20, 24, 20);
    main->setSpacing(14);

    QLabel *title = new QLabel("Stats & Trends");
    title->setObjectName("screenTitle");
    main->addWidget(title);
    QFrame *div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("color:#eee;"); main->addWidget(div);

    // Stat cards row
    QWidget *statRow = new QWidget();
    QHBoxLayout *sl = new QHBoxLayout(statRow);
    sl->setContentsMargins(0,0,0,0); sl->setSpacing(10);
    sl->addWidget(statCard("0", "Entries this month", &entriesNum));
    sl->addWidget(statCard("—", "Avg mood score",     &avgNum));
    sl->addWidget(statCard("0", "Day streak",         &streakNum));
    main->addWidget(statRow);

    // ASCII chart
    QLabel *chartTitle = new QLabel("MOOD THIS WEEK (ASCII CHART)");
    chartTitle->setObjectName("sectionLabel");
    main->addWidget(chartTitle);

    chartLabel = new QLabel("Log some moods to see your chart!");
    chartLabel->setStyleSheet("font-family: monospace; font-size: 12px; color: #555;"
                               "background: #f5f4f0; border-radius: 8px; padding: 14px;");
    chartLabel->setWordWrap(true);
    main->addWidget(chartLabel);

    QLabel *freqTitle = new QLabel("MOST FREQUENT MOODS");
    freqTitle->setObjectName("sectionLabel");
    main->addWidget(freqTitle);

    freqLabel = new QLabel("No data yet.");
    freqLabel->setStyleSheet("font-size: 13px; color: #555;");
    freqLabel->setWordWrap(true);
    main->addWidget(freqLabel);

    main->addStretch();
}

void StatsView::refreshEntries(const QVector<MoodEntry> &entries) {
    if (entries.isEmpty()) {
        entriesNum->setText("0");
        avgNum->setText("—");
        streakNum->setText("0");
        chartLabel->setText("Log some moods to see your chart!");
        freqLabel->setText("No data yet.");
        return;
    }

    // Count this month
    QDate today = QDate::currentDate();
    int monthCount = 0;
    double totalScore = 0;
    QMap<QString,int> moodCounts;

    for (const MoodEntry &e : entries) {
        if (e.timestamp.date().month() == today.month() &&
            e.timestamp.date().year()  == today.year()) {
            monthCount++;
        }
        totalScore += moodScore(e.mood);
        moodCounts[e.mood]++;
    }
    double avg = totalScore / entries.size();

    // Streak: consecutive days with at least one entry
    int streak = 0;
    QDate check = today;
    while (true) {
        bool found = false;
        for (const MoodEntry &e : entries) {
            if (e.timestamp.date() == check) { found = true; break; }
        }
        if (!found) break;
        streak++;
        check = check.addDays(-1);
    }

    entriesNum->setText(QString::number(monthCount));
    avgNum->setText(QString::number(avg, 'f', 1));
    streakNum->setText(QString::number(streak));

    // ASCII chart — last 7 days
    QString chart;
    for (int score = 5; score >= 1; score--) {
        QString moodName;
        if (score == 5) moodName = "Happy";
        else if (score == 4) moodName = "Calm";
        else if (score == 3) moodName = "Neutral";
        else if (score == 2) moodName = "Anxious";
        else moodName = "Stressed";

        chart += QString::number(score) + " | ";
        for (int d = 6; d >= 0; d--) {
            QDate day = today.addDays(-d);
            bool hasScore = false;
            for (const MoodEntry &e : entries) {
                if (e.timestamp.date() == day && moodScore(e.mood) == score) {
                    hasScore = true; break;
                }
            }
            chart += hasScore ? "███ " : "    ";
        }
        chart += moodName + "\n";
    }
    chart += "   +";
    for (int d = 6; d >= 0; d--) {
        QDate day = today.addDays(-d);
        chart += "-" + day.toString("ddd").left(2) + "-";
    }
    chartLabel->setText(chart);

    // Frequency tags
    QList<QPair<int,QString>> sorted;
    for (auto it = moodCounts.begin(); it != moodCounts.end(); ++it)
        sorted.append({it.value(), it.key()});
    std::sort(sorted.begin(), sorted.end(), [](auto &a, auto &b){ return a.first > b.first; });

    QString freqText;
    for (auto &p : sorted) {
        int pct = qRound(100.0 * p.first / entries.size());
        freqText += p.second + " — " + QString::number(pct) + "%    ";
    }
    freqLabel->setText(freqText.trimmed());
}
