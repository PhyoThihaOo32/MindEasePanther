#include "app/mainwindow.h"
#include "core/screen.h"
#include "screens/recommendations.h"
#include "screens/toolkit.h"
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
    setMinimumSize(1280, 820);
    resize(1440, 900);
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
    sidebar->setFixedWidth(260);

    QVBoxLayout *sl = new QVBoxLayout(sidebar);
    sl->setContentsMargins(0, 0, 0, 0);
    sl->setSpacing(0);

    // ── Logo block ───────────────────────────────────────────────────────────
    QWidget *logo = new QWidget();
    logo->setObjectName("logoBox");
    QVBoxLayout *ll = new QVBoxLayout(logo);
    ll->setContentsMargins(22, 22, 22, 20);
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
    navHeader->setContentsMargins(22, 14, 22, 8);
    sl->addWidget(navHeader);

    // ── Nav items (index matches stack order) ────────────────────────────────
    struct NavDef { QString icon; QString label; };
    const QList<NavDef> navDefs = {
        { "✦", "BMCC Resources"         },
        { "◆", "Mental Health Toolkit"  },
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
    fl->setContentsMargins(22, 14, 22, 18);
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
            background: #f6fbf4;
        }
        #appShell {
            background: #f6fbf4;
        }

        /* ── Sidebar ──────────────────────────────────────────────────── */
        #sidebar {
            background: #ffffff;
            border-right: 1px solid #dfece1;
        }
        #logoBox {
            background: #f1f8f1;
            border-bottom: 1px solid #dfece1;
        }
        #appName {
            font-size: 24px;
            font-weight: 700;
            color: #173c2c;
            letter-spacing: -0.4px;
        }
        #appSub {
            font-size: 12px;
            color: #6d8272;
            margin-top: 3px;
            letter-spacing: 0.2px;
        }
        #navHeader {
            font-size: 10px;
            font-weight: 700;
            color: #8ba493;
            letter-spacing: 1.4px;
        }

        /* ── Unified nav buttons ─────────────────────────────────────── */
        #navBtn {
            text-align: left;
            padding: 15px 20px;
            border: none;
            border-left: 3px solid transparent;
            background: transparent;
            font-size: 14px;
            color: #4f6255;
            border-radius: 0px 16px 16px 0px;
            margin: 4px 14px 4px 0;
        }
        #navBtn:hover {
            background: #edf7ee;
            color: #173c2c;
        }
        #navBtn:checked {
            background: #f7fff8;
            color: #2f7d5c;
            border-left: 3px solid #2f7d5c;
            font-weight: 700;
        }
        #navBtn:focus {
            background: #edf7ee;
            color: #245f47;
        }

        /* ── Sidebar footer ──────────────────────────────────────────── */
        #footerDiv {
            color: #dfece1;
            background: #dfece1;
            max-height: 1px;
            border: none;
        }
        #footerTitle {
            font-size: 11px;
            font-weight: 700;
            color: #557161;
            letter-spacing: 0.3px;
        }
        #verLabel { font-size: 10px; color: #8ba493; letter-spacing: 0.2px; }
        #footerNote { font-size: 10px; color: #6d8272; }

        #screenStack {
            background: #f6fbf4;
        }
        QWidget#screenRoot,
        QWidget#screenSurface,
        QWidget#screenViewport {
            background: #f6fbf4;
        }

        /* ── Shared screen typography ────────────────────────────────── */
        QLabel#screenTitle {
            font-size: 28px;
            font-weight: 700;
            color: #173c2c;
            letter-spacing: -0.35px;
        }
        QLabel#sectionLabel {
            font-size: 10px;
            font-weight: 700;
            color: #8ba493;
            letter-spacing: 1.4px;
        }

        /* ── Generic buttons ─────────────────────────────────────────── */
        QPushButton#primaryBtn {
            background: #2f7d5c;
            color: #ffffff;
            border: none;
            border-radius: 14px;
            padding: 12px 26px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton#primaryBtn:hover { background: #286d50; }
        QPushButton#primaryBtn:pressed { background: #20533f; }

        QPushButton#outlineBtn {
            background: transparent;
            color: #2f7d5c;
            border: 1px solid #b9d9c2;
            border-radius: 14px;
            padding: 11px 20px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton#outlineBtn:hover {
            background: #edf7ee;
            border-color: #8fc5a2;
            color: #245f47;
        }

        /* ── Scrollbars ──────────────────────────────────────────────── */
        QScrollArea {
            border: none;
            background: #f6fbf4;
        }
        QScrollBar:vertical {
            width: 8px;
            background: transparent;
            margin: 4px 2px 4px 0;
        }
        QScrollBar::handle:vertical {
            background: #b9d9c2;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: #8fc5a2; }
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
