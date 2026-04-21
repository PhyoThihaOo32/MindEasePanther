#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector>

// Forward declaration — MainWindow only needs to know that Screen exists.
class Screen;

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow — top-level application window
//
// Owns the sidebar and a QStackedWidget of Screen* objects. Uses runtime
// polymorphism: screens are stored as Screen* and switchScreen() calls the
// virtual onActivated() on whichever concrete subclass is currently active.
// ─────────────────────────────────────────────────────────────────────────────

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
    QVector<Screen*>      screens;    // polymorphic ownership list
};

#endif // MAINWINDOW_H
