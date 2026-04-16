#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector>

class Recommendations;
class Toolkit;
class Journal;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchScreen(int index);

private:
    void buildSidebar();
    void applyStyle();

    QWidget        *centralWidget;
    QHBoxLayout    *mainLayout;
    QWidget        *sidebar;
    QStackedWidget *stack;

    QVector<QPushButton*> navButtons;

    Recommendations *recScreen;
    Toolkit         *toolkitScreen;
    Journal         *journalScreen;
};

#endif // MAINWINDOW_H
