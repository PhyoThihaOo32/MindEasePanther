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
                                        stop:0 #03111e, stop:0.32 #06364a, stop:0.7 #0b5c59, stop:1 #09382d);
        }
        #appShell {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #03111e, stop:0.32 #06364a, stop:0.7 #0b5c59, stop:1 #09382d);
        }

        /* ── Sidebar ──────────────────────────────────────────────────── */
        #sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #061224, stop:0.42 #082b44, stop:1 #0a3a35);
            border-right: 1px solid rgba(139, 223, 242, 0.48);
        }
        #logoBox {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #0a3550, stop:0.52 #0c5a5c, stop:1 #184631);
            border-bottom: 1px solid rgba(139, 223, 242, 0.56);
        }
        #appName {
            font-size: 30px;
            font-weight: 800;
            color: #fff1ad;
            letter-spacing: -0.8px;
        }
        #appSub {
            font-size: 13px;
            color: #d1fff5;
            margin-top: 3px;
            letter-spacing: 0.35px;
        }
        #navHeader {
            font-size: 10px;
            font-weight: 700;
            color: #95edf8;
            letter-spacing: 1.8px;
        }

        /* ── Unified nav buttons ─────────────────────────────────────── */
        #navBtn {
            text-align: left;
            padding: 18px 22px;
            border: 1px solid transparent;
            border-left: 4px solid transparent;
            background: rgba(255, 255, 255, 0.03);
            font-size: 15px;
            color: #d7fff1;
            border-radius: 0px 22px 22px 0px;
            margin: 6px 16px 6px 0;
        }
        #navBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 rgba(17, 56, 74, 0.92), stop:1 rgba(19, 74, 72, 0.9));
            color: #fff1ad;
            border: 1px solid rgba(139, 223, 242, 0.22);
        }
        #navBtn:checked {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 rgba(19, 63, 70, 0.98), stop:1 rgba(22, 90, 90, 0.96));
            color: #fff1ad;
            border-left: 4px solid #8ee2a8;
            border-top: 1px solid rgba(139, 223, 242, 0.3);
            border-right: 1px solid rgba(139, 223, 242, 0.3);
            border-bottom: 1px solid rgba(139, 223, 242, 0.3);
            font-weight: 800;
        }
        #navBtn:focus {
            background: rgba(17, 56, 74, 0.9);
            color: #dffef6;
        }

        /* ── Sidebar footer ──────────────────────────────────────────── */
        #footerDiv {
            color: rgba(139, 223, 242, 0.42);
            background: rgba(139, 223, 242, 0.42);
            max-height: 1px;
            border: none;
        }
        #footerTitle {
            font-size: 11px;
            font-weight: 700;
            color: #f1e8ad;
            letter-spacing: 0.3px;
        }
        #verLabel { font-size: 10px; color: #bdeee5; letter-spacing: 0.25px; }
        #footerNote { font-size: 10px; color: #d4fbff; }

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
            font-size: 34px;
            font-weight: 800;
            color: #fff1b3;
            letter-spacing: -0.8px;
        }
        QLabel#sectionLabel {
            font-size: 10px;
            font-weight: 700;
            color: #9ee6f2;
            letter-spacing: 1.7px;
        }

        /* ── Generic buttons ─────────────────────────────────────────── */
        QPushButton#primaryBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #86e0b4, stop:0.52 #78daf0, stop:1 #efe6a8);
            color: #06172a;
            border: 1px solid rgba(184, 255, 218, 0.78);
            border-radius: 16px;
            padding: 13px 26px;
            font-size: 14px;
            font-weight: 800;
        }
        QPushButton#primaryBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #94e8bf, stop:0.5 #8fe5fb, stop:1 #f4edb8);
        }
        QPushButton#primaryBtn:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #6ad4a4, stop:0.5 #6ccde4, stop:1 #ddd188);
        }
        QPushButton#primaryBtn:disabled {
            background: #31535f;
            color: #9eb8b9;
        }

        QPushButton#outlineBtn {
            background: rgba(11, 42, 60, 0.88);
            color: #f1e8ad;
            border: 1px solid #8bdff2;
            border-radius: 16px;
            padding: 11px 20px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton#outlineBtn:hover {
            background: rgba(18, 63, 70, 0.94);
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
            background: rgba(139, 223, 242, 0.78);
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: rgba(241, 232, 173, 0.95); }
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
