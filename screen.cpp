#include "screen.h"
#include <QLabel>
#include <QFrame>

// ─────────────────────────────────────────────────────────────────────────────
// Screen — abstract base class implementation
// ─────────────────────────────────────────────────────────────────────────────

// Constructor: forwards parent to QWidget and stores the display title.
Screen::Screen(const QString &title, QWidget *parent)
    : QWidget(parent), m_title(title) {}

// Accessor — the only public way to read the private m_title member.
QString Screen::title() const { return m_title; }

// Shared header builder — derived classes call this at the start of their
// constructor to get a consistent title label + divider line without copying
// the same layout code into every screen.
void Screen::buildHeader(QVBoxLayout *root) {
    QLabel *lbl = new QLabel(m_title);
    lbl->setObjectName("screenTitle");
    root->addWidget(lbl);

    QFrame *div = new QFrame();
    div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("color: #eee;");
    root->addWidget(div);
}
