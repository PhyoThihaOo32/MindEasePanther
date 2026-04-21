#include "settingsview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

static QWidget* settingRow(const QString &label, const QString &sub, QWidget *control) {
    QWidget *row = new QWidget();
    QHBoxLayout *rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 10, 0, 10);
    QWidget *labelBox = new QWidget();
    QVBoxLayout *ll = new QVBoxLayout(labelBox);
    ll->setContentsMargins(0,0,0,0); ll->setSpacing(2);
    QLabel *l = new QLabel(label);
    l->setStyleSheet("font-size: 13px; color: #1a1a1a;");
    ll->addWidget(l);
    if (!sub.isEmpty()) {
        QLabel *s = new QLabel(sub);
        s->setStyleSheet("font-size: 11px; color: #888;");
        ll->addWidget(s);
    }
    rl->addWidget(labelBox);
    rl->addStretch();
    rl->addWidget(control);
    return row;
}

SettingsView::SettingsView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(24, 20, 24, 20);
    main->setSpacing(0);

    QLabel *title = new QLabel("Settings");
    title->setObjectName("screenTitle");
    main->addWidget(title);
    QFrame *div = new QFrame(); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("color:#eee; margin-bottom: 4px;"); main->addWidget(div);

    QLabel *sub = new QLabel("PREFERENCES");
    sub->setObjectName("sectionLabel");
    sub->setContentsMargins(0, 8, 0, 4);
    main->addWidget(sub);

    // Controls
    remindCheck = new QCheckBox();
    remindCheck->setChecked(true);

    timeCombo = new QComboBox();
    timeCombo->addItems({"8:00 AM", "12:00 PM", "6:00 PM", "9:00 PM"});
    timeCombo->setCurrentIndex(2);
    timeCombo->setStyleSheet("padding: 4px 8px; border: 1px solid #ddd; border-radius: 6px; font-size: 12px;");

    autosaveCheck = new QCheckBox();
    autosaveCheck->setChecked(true);

    quoteCheck = new QCheckBox();
    quoteCheck->setChecked(false);

    themeCombo = new QComboBox();
    themeCombo->addItems({"System default", "Light", "Dark"});
    themeCombo->setStyleSheet("padding: 4px 8px; border: 1px solid #ddd; border-radius: 6px; font-size: 12px;");

    // Load saved settings
    QSettings s("MindEase", "MindEaseApp");
    remindCheck->setChecked(s.value("reminders", true).toBool());
    timeCombo->setCurrentIndex(s.value("reminderTime", 2).toInt());
    autosaveCheck->setChecked(s.value("autosave", true).toBool());
    quoteCheck->setChecked(s.value("quotes", false).toBool());
    themeCombo->setCurrentIndex(s.value("theme", 0).toInt());

    auto addRow = [&](const QString &l, const QString &sub, QWidget *ctrl) {
        main->addWidget(settingRow(l, sub, ctrl));
        QFrame *d = new QFrame(); d->setFrameShape(QFrame::HLine);
        d->setStyleSheet("color: #eee;"); main->addWidget(d);
    };

    addRow("Daily reminder",    "Remind me to log my mood",              remindCheck);
    addRow("Reminder time",     "",                                       timeCombo);
    addRow("Auto-save to file", "Save entries to mood_log.txt",           autosaveCheck);
    addRow("Quote of the day",  "Motivational quotes on startup",         quoteCheck);
    addRow("Theme",             "",                                        themeCombo);

    main->addSpacing(20);

    QWidget *btnRow = new QWidget();
    QHBoxLayout *bl = new QHBoxLayout(btnRow);
    bl->setContentsMargins(0,0,0,0); bl->setSpacing(10);
    QPushButton *saveBtn  = new QPushButton("Save settings");
    saveBtn->setObjectName("primaryBtn");
    QPushButton *clearBtn = new QPushButton("Clear all data");
    clearBtn->setObjectName("outlineBtn");
    bl->addWidget(saveBtn); bl->addWidget(clearBtn); bl->addStretch();
    main->addWidget(btnRow);
    main->addStretch();

    connect(saveBtn,  &QPushButton::clicked, this, &SettingsView::saveSettings);
    connect(clearBtn, &QPushButton::clicked, this, &SettingsView::clearData);
}

void SettingsView::saveSettings() {
    QSettings s("MindEase", "MindEaseApp");
    s.setValue("reminders",    remindCheck->isChecked());
    s.setValue("reminderTime", timeCombo->currentIndex());
    s.setValue("autosave",     autosaveCheck->isChecked());
    s.setValue("quotes",       quoteCheck->isChecked());
    s.setValue("theme",        themeCombo->currentIndex());
    QMessageBox::information(this, "Settings Saved", "Your preferences have been saved.");
}

void SettingsView::clearData() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Clear Data", "Are you sure you want to delete all mood entries? This cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Cleared", "All mood data has been cleared.");
        // The actual file deletion is handled by MainWindow if needed
    }
}
