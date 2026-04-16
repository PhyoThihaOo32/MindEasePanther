#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>

class SettingsView : public QWidget {
    Q_OBJECT
public:
    explicit SettingsView(QWidget *parent = nullptr);

private slots:
    void saveSettings();
    void clearData();

private:
    QCheckBox *remindCheck;
    QComboBox *timeCombo;
    QCheckBox *autosaveCheck;
    QCheckBox *quoteCheck;
    QComboBox *themeCombo;
};

#endif // SETTINGSVIEW_H
