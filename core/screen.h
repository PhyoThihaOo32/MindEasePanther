#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include <QVBoxLayout>

// ─────────────────────────────────────────────────────────────────────────────
// Screen — abstract base class for all application screens
//
// OOP concepts demonstrated
// ─────────────────────────────────────────────────────────────────────────────
//   Abstraction   — Screen defines a common interface for every view in the
//                   application. It cannot be instantiated on its own because
//                   screenId() is a pure virtual function. Callers work with
//                   Screen* and never need to know the concrete type.
//
//   Inheritance   — Recommendations, Toolkit, and Journal all inherit from
//                   Screen, gaining the title accessor and buildHeader() helper
//                   without duplicating code.
//
//   Polymorphism  — onActivated() is a virtual method. MainWindow stores a
//                   QVector<Screen*> and calls screens[i]->onActivated() when
//                   switching screens. Each concrete class decides at runtime
//                   what "activated" means for that screen.
//
//   Encapsulation — m_title is private; external code can only read it through
//                   the public title() const accessor.
// ─────────────────────────────────────────────────────────────────────────────

class Screen : public QWidget {
    Q_OBJECT

public:
    explicit Screen(const QString &title, QWidget *parent = nullptr);
    virtual ~Screen() = default;

    // Returns the screen's human-readable display title.
    QString title() const;

    // ── Pure virtual — makes Screen abstract ─────────────────────────────────
    // Every concrete screen must supply a unique lowercase identifier string.
    // Declaring this as = 0 means Screen cannot be instantiated directly.
    virtual QString screenId() const = 0;

    // ── Virtual activation hook ───────────────────────────────────────────────
    // Called by MainWindow::switchScreen() each time this screen becomes visible.
    // The default implementation does nothing; derived classes may override to
    // refresh data, restart timers, or reset state.
    virtual void onActivated() {}

protected:
    // Shared utility: appends a styled title QLabel followed by a horizontal
    // rule to *root*. Derived classes call this at the top of their constructor
    // to produce a consistent screen header without code duplication.
    void buildHeader(QVBoxLayout *root);

private:
    QString m_title;     // encapsulated; not directly accessible to subclasses
};

#endif // SCREEN_H
