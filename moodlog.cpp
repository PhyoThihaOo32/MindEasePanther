#include "moodlog.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QMessageBox>

static QString dotColor(const QString &mood) {
    if (mood == "Happy")   return "#534AB7";
    if (mood == "Calm")    return "#0F6E56";
    if (mood == "Neutral") return "#888780";
    if (mood == "Sad")     return "#185FA5";
    if (mood == "Anxious") return "#854F0B";
    if (mood == "Stressed")return "#A32D2D";
    return "#888";
}

MoodLog::MoodLog(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(24, 20, 24, 20);
    main->setSpacing(14);

    QLabel *title = new QLabel("My Log");
    title->setObjectName("screenTitle");
    main->addWidget(title);
    QFrame *div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("color:#eee;"); main->addWidget(div);

    QLabel *sub = new QLabel("RECENT ENTRIES");
    sub->setObjectName("sectionLabel");
    main->addWidget(sub);

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    listContainer = new QWidget();
    listLayout = new QVBoxLayout(listContainer);
    listLayout->setSpacing(8);
    listLayout->setContentsMargins(0,0,0,0);
    listLayout->addStretch();

    scroll->setWidget(listContainer);
    main->addWidget(scroll);

    QPushButton *exportBtn = new QPushButton("Export to file (.txt)");
    exportBtn->setObjectName("outlineBtn");
    exportBtn->setFixedWidth(180);
    main->addWidget(exportBtn);
    // Export just shows a message — file is already saved by MainWindow
    connect(exportBtn, &QPushButton::clicked, this, []() {
        QMessageBox *mb = new QMessageBox();
        mb->setWindowTitle("Exported");
        mb->setText("Your mood log is saved automatically to your App Data folder as mood_log.txt.");
        mb->exec();
    });
}

void MoodLog::refreshEntries(const QVector<MoodEntry> &entries) {
    // Clear existing items (keep the stretch at end)
    QLayoutItem *item;
    while (listLayout->count() > 1) {
        item = listLayout->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }

    for (const MoodEntry &e : entries) {
        QFrame *row = new QFrame();
        row->setStyleSheet("QFrame { border: 1px solid #e0ded8; border-radius: 8px; background: #fff; }");
        QHBoxLayout *rl = new QHBoxLayout(row);
        rl->setContentsMargins(12, 10, 12, 10);
        rl->setSpacing(12);

        // Colored dot
        QLabel *dot = new QLabel("●");
        dot->setStyleSheet(QString("color: %1; font-size: 10px; border: none;").arg(dotColor(e.mood)));
        dot->setFixedWidth(12);

        // Mood + note
        QWidget *info = new QWidget();
        QVBoxLayout *il = new QVBoxLayout(info);
        il->setContentsMargins(0,0,0,0); il->setSpacing(2);
        QLabel *moodLabel = new QLabel(e.emoji + " " + e.mood);
        moodLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #1a1a1a; border: none;");
        il->addWidget(moodLabel);
        if (!e.note.isEmpty()) {
            QLabel *noteLabel = new QLabel(e.note);
            noteLabel->setStyleSheet("font-size: 12px; color: #666; border: none;");
            noteLabel->setWordWrap(true);
            il->addWidget(noteLabel);
        }

        // Timestamp
        QLabel *time = new QLabel(e.timestamp.toString("ddd h:mm ap"));
        time->setStyleSheet("font-size: 11px; color: #aaa; border: none;");
        time->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        rl->addWidget(dot);
        rl->addWidget(info);
        rl->addStretch();
        rl->addWidget(time);

        listLayout->insertWidget(listLayout->count() - 1, row);
    }

    if (entries.isEmpty()) {
        QLabel *empty = new QLabel("No entries yet. Start by logging your mood!");
        empty->setStyleSheet("color: #aaa; font-size: 13px;");
        empty->setAlignment(Qt::AlignCenter);
        listLayout->insertWidget(0, empty);
    }
}
