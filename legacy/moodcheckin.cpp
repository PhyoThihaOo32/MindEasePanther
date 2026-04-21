#include "moodcheckin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>

MoodCheckin::MoodCheckin(QWidget *parent) : QWidget(parent) {
    moodOptions = {
        {"Happy",   "😊"},
        {"Calm",    "😌"},
        {"Neutral", "😐"},
        {"Sad",     "😔"},
        {"Anxious", "😰"},
        {"Stressed","😤"}
    };

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(24, 20, 24, 20);
    main->setSpacing(16);

    // Top bar
    QWidget *topBar = new QWidget();
    QHBoxLayout *tb = new QHBoxLayout(topBar);
    tb->setContentsMargins(0,0,0,0);
    QLabel *title = new QLabel("Mood Check-in");
    title->setObjectName("screenTitle");
    QLabel *dateLabel = new QLabel(QDate::currentDate().toString("ddd, MMM d yyyy"));
    dateLabel->setStyleSheet("font-size:12px; color:#888;");
    tb->addWidget(title);
    tb->addStretch();
    tb->addWidget(dateLabel);
    main->addWidget(topBar);

    // Divider
    QFrame *div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("color:#eee;"); main->addWidget(div);

    // Mood label
    QLabel *moodLabel = new QLabel("HOW ARE YOU FEELING RIGHT NOW?");
    moodLabel->setObjectName("sectionLabel");
    main->addWidget(moodLabel);

    // Mood buttons grid
    QWidget *grid = new QWidget();
    QHBoxLayout *gl = new QHBoxLayout(grid);
    gl->setContentsMargins(0,0,0,0);
    gl->setSpacing(10);

    moodGroup = new QButtonGroup(this);
    moodGroup->setExclusive(true);

    for (int i = 0; i < moodOptions.size(); i++) {
        QPushButton *btn = new QPushButton(moodOptions[i].emoji + "\n" + moodOptions[i].label);
        btn->setObjectName("moodBtn");
        btn->setCheckable(true);
        btn->setMinimumHeight(64);
        moodGroup->addButton(btn, i);
        gl->addWidget(btn);
    }
    main->addWidget(grid);

    // Note label
    QLabel *noteLabel = new QLabel("ADD A NOTE (OPTIONAL)");
    noteLabel->setObjectName("sectionLabel");
    main->addWidget(noteLabel);

    noteEdit = new QPlainTextEdit();
    noteEdit->setPlaceholderText("What's on your mind today?");
    noteEdit->setMaximumHeight(90);
    main->addWidget(noteEdit);

    // Buttons
    QWidget *btnRow = new QWidget();
    QHBoxLayout *bl = new QHBoxLayout(btnRow);
    bl->setContentsMargins(0,0,0,0);
    bl->setSpacing(10);

    QPushButton *saveBtn = new QPushButton("Save entry");
    saveBtn->setObjectName("primaryBtn");
    QPushButton *toolkitBtn = new QPushButton("Open Toolkit →");
    toolkitBtn->setObjectName("outlineBtn");

    bl->addWidget(saveBtn);
    bl->addWidget(toolkitBtn);
    bl->addStretch();
    main->addWidget(btnRow);
    main->addStretch();

    connect(saveBtn, &QPushButton::clicked, this, &MoodCheckin::onSave);
    connect(toolkitBtn, &QPushButton::clicked, this, [this]() {
        // Ask MainWindow to switch to toolkit (index 4)
        if (parentWidget() && parentWidget()->parentWidget()) {
            QMetaObject::invokeMethod(parentWidget()->parentWidget()->parentWidget(),
                                      "switchScreen", Qt::DirectConnection,
                                      Q_ARG(int, 4));
        }
    });
}

void MoodCheckin::onSave() {
    QAbstractButton *checked = moodGroup->checkedButton();
    if (!checked) {
        QMessageBox::information(this, "Select a mood", "Please select how you're feeling first.");
        return;
    }
    int id = moodGroup->id(checked);
    MoodEntry entry(moodOptions[id].label, moodOptions[id].emoji,
                    noteEdit->toPlainText().trimmed());
    noteEdit->clear();
    moodGroup->setExclusive(false);
    checked->setChecked(false);
    moodGroup->setExclusive(true);
    emit moodSaved(entry);
}
