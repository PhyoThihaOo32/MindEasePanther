#include "app/mainwindow.h"
#include "core/screen.h"
#include "screens/recommendations.h"
#include "screens/toolkit.h"
#include "screens/assistantchat.h"
#include "screens/journal.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("MindEase — BMCC Wellness Companion");
    setMinimumSize(1120, 760);
    resize(1500, 960);
    setWindowState(Qt::WindowMaximized);

    applyStyle();

    centralWidget = new QWidget(this);
    centralWidget->setObjectName("appShell");
    setCentralWidget(centralWidget);

    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Build screens (polymorphic — stored as Screen*)
    screens.append(new Recommendations(this));
    screens.append(new Toolkit(this));
    screens.append(new AssistantChat(this));
    screens.append(new Journal(this));

    // Stack
    stack = new QStackedWidget(this);
    stack->setObjectName("screenStack");
    for (Screen *s : screens)
        stack->addWidget(s);

    buildSidebar();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stack);
    mainLayout->setStretch(1, 1);

    switchScreen(0);
}

MainWindow::~MainWindow() {}

void MainWindow::buildSidebar() {
    sidebar = new QWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(288);

    QVBoxLayout *sl = new QVBoxLayout(sidebar);
    sl->setContentsMargins(0, 0, 0, 0);
    sl->setSpacing(0);

    // ── Logo block ───────────────────────────────────────────────────────────
    QWidget *logo = new QWidget();
    logo->setObjectName("logoBox");
    QVBoxLayout *ll = new QVBoxLayout(logo);
    ll->setContentsMargins(26, 26, 26, 22);
    ll->setSpacing(2);

    QLabel *appName = new QLabel("MindEase");
    appName->setObjectName("appName");
    QLabel *appSub  = new QLabel("BMCC Wellness Companion");
    appSub->setObjectName("appSub");
    ll->addWidget(appName);
    ll->addWidget(appSub);
    sl->addWidget(logo);

    // Small section label above nav
    QLabel *navHeader = new QLabel("MENU");
    navHeader->setObjectName("navHeader");
    navHeader->setContentsMargins(26, 18, 26, 8);
    sl->addWidget(navHeader);

    // ── Nav items (index matches stack order) ────────────────────────────────
    struct NavDef { QString icon; QString label; };
    const QList<NavDef> navDefs = {
        { "✦", "BMCC Resources"         },
        { "◆", "Mental Health Toolkit"  },
        { "☼", "MindEase Assistant"      },
        { "✎", "My Journal"             },
    };

    for (int i = 0; i < navDefs.size(); i++) {
        QPushButton *btn = new QPushButton(
            QString("  %1   %2").arg(navDefs[i].icon, navDefs[i].label));
        btn->setObjectName("navBtn");
        btn->setCheckable(true);
        btn->setChecked(i == 0);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, i]() { switchScreen(i); });
        navButtons.append(btn);
        sl->addWidget(btn);
    }

    sl->addStretch();

    // ── Footer block ─────────────────────────────────────────────────────────
    QFrame *footerDiv = new QFrame();
    footerDiv->setObjectName("footerDiv");
    footerDiv->setFrameShape(QFrame::HLine);
    sl->addWidget(footerDiv);

    QWidget *footer = new QWidget();
    QVBoxLayout *fl = new QVBoxLayout(footer);
    fl->setContentsMargins(26, 16, 26, 20);
    fl->setSpacing(2);

    QLabel *footerTitle = new QLabel("Honors Project");
    footerTitle->setObjectName("footerTitle");
    QLabel *ver = new QLabel("v1.0  ·  CSC211H  ·  Qt 6");
    ver->setObjectName("verLabel");
    QLabel *footNote = new QLabel("Private, local-first wellness support");
    footNote->setObjectName("footerNote");

    fl->addWidget(footerTitle);
    fl->addWidget(ver);
    fl->addWidget(footNote);
    sl->addWidget(footer);
}

void MainWindow::switchScreen(int index) {
    stack->setCurrentIndex(index);
    for (int i = 0; i < navButtons.size(); i++)
        navButtons[i]->setChecked(i == index);

    // Polymorphic dispatch — each Screen subclass decides what to refresh.
    if (index >= 0 && index < screens.size())
        screens[index]->onActivated();
}

void MainWindow::applyStyle() {
    setStyleSheet(R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #06172a, stop:0.48 #073b4c, stop:1 #0b3f35);
        }
        #appShell {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #06172a, stop:0.48 #073b4c, stop:1 #0b3f35);
        }

        /* ── Sidebar ──────────────────────────────────────────────────── */
        #sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #071426, stop:0.48 #082f49, stop:1 #063b35);
            border-right: 1px solid #8bdff2;
        }
        #logoBox {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #082f49, stop:0.58 #0b4d54, stop:1 #123f2d);
            border-bottom: 1px solid #8bdff2;
        }
        #appName {
            font-size: 28px;
            font-weight: 800;
            color: #f1e8ad;
            letter-spacing: -0.65px;
        }
        #appSub {
            font-size: 13px;
            color: #bdeee5;
            margin-top: 3px;
            letter-spacing: 0.3px;
        }
        #navHeader {
            font-size: 10px;
            font-weight: 700;
            color: #9ee6f2;
            letter-spacing: 1.4px;
        }

        /* ── Unified nav buttons ─────────────────────────────────────── */
        #navBtn {
            text-align: left;
            padding: 17px 22px;
            border: none;
            border-left: 4px solid transparent;
            background: transparent;
            font-size: 15px;
            color: #d7fff1;
            border-radius: 0px 20px 20px 0px;
            margin: 5px 18px 5px 0;
        }
        #navBtn:hover {
            background: #11384a;
            color: #f1e8ad;
        }
        #navBtn:checked {
            background: #123f46;
            color: #f1e8ad;
            border-left: 4px solid #8ee2a8;
            font-weight: 800;
        }
        #navBtn:focus {
            background: #11384a;
            color: #bdeee5;
        }

        /* ── Sidebar footer ──────────────────────────────────────────── */
        #footerDiv {
            color: #8bdff2;
            background: #8bdff2;
            max-height: 1px;
            border: none;
        }
        #footerTitle {
            font-size: 11px;
            font-weight: 700;
            color: #f1e8ad;
            letter-spacing: 0.3px;
        }
        #verLabel { font-size: 10px; color: #bdeee5; letter-spacing: 0.2px; }
        #footerNote { font-size: 10px; color: #c4f7ff; }

        #screenStack {
            background: transparent;
        }
        QWidget#screenRoot,
        QWidget#screenSurface,
        QWidget#screenViewport {
            background: transparent;
        }

        /* ── Shared screen typography ────────────────────────────────── */
        QLabel#screenTitle {
            font-size: 32px;
            font-weight: 800;
            color: #f1e8ad;
            letter-spacing: -0.65px;
        }
        QLabel#sectionLabel {
            font-size: 10px;
            font-weight: 700;
            color: #9ee6f2;
            letter-spacing: 1.4px;
        }

        /* ── Generic buttons ─────────────────────────────────────────── */
        QPushButton#primaryBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #8ee2a8, stop:0.5 #8bdff2, stop:1 #f1e8ad);
            color: #06172a;
            border: 1px solid #b8ffda;
            border-radius: 14px;
            padding: 12px 26px;
            font-size: 14px;
            font-weight: 800;
        }
        QPushButton#primaryBtn:hover { background: #8bdff2; }
        QPushButton#primaryBtn:pressed { background: #8ee2a8; }
        QPushButton#primaryBtn:disabled {
            background: #31535f;
            color: #9eb8b9;
        }

        QPushButton#outlineBtn {
            background: #0b2a3c;
            color: #f1e8ad;
            border: 1px solid #8bdff2;
            border-radius: 14px;
            padding: 11px 20px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton#outlineBtn:hover {
            background: #123f46;
            border-color: #8ee2a8;
            color: #bdeee5;
        }

        /* ── Scrollbars ──────────────────────────────────────────────── */
        QScrollArea {
            border: none;
            background: transparent;
        }
        QScrollBar:vertical {
            width: 10px;
            background: transparent;
            margin: 6px 2px 6px 0;
        }
        QScrollBar::handle:vertical {
            background: #8bdff2;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: #f1e8ad; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
            background: transparent;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }

        QSplitter::handle:horizontal {
            background: transparent;
            width: 10px;
        }
    )");
}
