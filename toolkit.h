#ifndef TOOLKIT_H
#define TOOLKIT_H

#include <QWidget>
#include <QMap>
#include <QFrame>
#include <QLabel>

class Toolkit : public QWidget {
    Q_OBJECT
public:
    explicit Toolkit(QWidget *parent = nullptr);

private:
    void toggleFolder(const QString &id);

    QMap<QString, QFrame*>  panels;
    QMap<QString, QWidget*> folderCards;
    QMap<QString, QLabel*>  chevrons;     // track all chevron labels for reset
    QString openFolder;
};

#endif // TOOLKIT_H
