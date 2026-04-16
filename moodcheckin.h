#ifndef MOODCHECKIN_H
#define MOODCHECKIN_H

#include <QWidget>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QButtonGroup>
#include "moodentry.h"

class MoodCheckin : public QWidget {
    Q_OBJECT
public:
    explicit MoodCheckin(QWidget *parent = nullptr);

signals:
    void moodSaved(const MoodEntry &entry);

private slots:
    void onSave();

private:
    QButtonGroup  *moodGroup;
    QPlainTextEdit *noteEdit;

    struct MoodOption { QString label; QString emoji; };
    QList<MoodOption> moodOptions;
};

#endif // MOODCHECKIN_H
