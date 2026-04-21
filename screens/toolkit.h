#ifndef TOOLKIT_H
#define TOOLKIT_H

#include "core/screen.h"
#include <QMap>
#include <QFrame>
#include <QLabel>

// ─────────────────────────────────────────────────────────────────────────────
// Toolkit — Mental Health Toolkit screen
//
// Inherits Screen and provides a concrete implementation of the abstract
// interface. Demonstrates:
//   • Inheritance   — extends the custom Screen base class
//   • Polymorphism  — overrides screenId() and onActivated() from Screen
// ─────────────────────────────────────────────────────────────────────────────

class Toolkit : public Screen {
    Q_OBJECT

public:
    explicit Toolkit(QWidget *parent = nullptr);

    // ── Implement pure virtual (required by Screen) ───────────────────────────
    QString screenId() const override { return "toolkit"; }

    // ── Override activation hook ──────────────────────────────────────────────
    void onActivated() override;

private:
    void toggleFolder(const QString &id);

    QMap<QString, QFrame*>  panels;
    QMap<QString, QWidget*> folderCards;
    QMap<QString, QLabel*>  chevrons;     // track all chevron labels for reset
    QString openFolder;
};

#endif // TOOLKIT_H
