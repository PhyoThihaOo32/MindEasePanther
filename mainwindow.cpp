#include "mainwindow.h"
#include "screen.h"
#include "recommendations.h"
#include "toolkit.h"
#include "journal.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("MindEase — BMCC Wellness Companion");
    setMinimumSize(1040, 720);

    applyStyle();

    centralWidget = new QWidget(this);
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
    for (Screen *s : screens)
        stack->addWidget(s);

    buildSidebar();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stack);
}

MainWindow::~MainWindow() {}

void MainWindow::buildSidebar() {
    sidebar = new QWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(222);

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

    fl->addWidget(footerTitle);
    fl->addWidget(ver);
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
        QMainWindow { background: #fafaf7; }

        /* ── Sidebar ──────────────────────────────────────────────────── */
        #sidebar {
            background: #faf9f5;
            border-right: 1px solid #ece9e2;
        }
        #logoBox {
            border-bottom: 1px solid #ece9e2;
        }
        #appName {
            font-size: 19px;
            font-weight: 700;
            color: #1a1a1a;
            letter-spacing: -0.3px;
        }
        #appSub {
            font-size: 11px;
            color: #8a857c;
            margin-top: 2px;
            letter-spacing: 0.2px;
        }
        #navHeader {
            font-size: 10px;
            font-weight: 700;
            color: #a8a49b;
            letter-spacing: 1.4px;
        }

        /* ── Unified nav buttons ─────────────────────────────────────── */
        #navBtn {
            text-align: left;
            padding: 11px 18px;
            border: none;
            border-left: 3px solid transparent;
            background: transparent;
            font-size: 13px;
            color: #5d574f;
            border-radius: 0px;
            margin: 1px 10px 1px 0;
        }
        #navBtn:hover {
            background: #f2efe7;
            color: #1a1a1a;
        }
        #navBtn:checked {
            background: #ffffff;
            color: #534AB7;
            border-left: 3px solid #534AB7;
            font-weight: 700;
        }

        /* ── Sidebar footer ──────────────────────────────────────────── */
        #footerDiv {
            color: #ece9e2;
            background: #ece9e2;
            max-height: 1px;
            border: none;
        }
        #footerTitle {
            font-size: 11px;
            font-weight: 700;
            color: #6d675e;
            letter-spacing: 0.3px;
        }
        #verLabel { font-size: 10px; color: #a8a49b; letter-spacing: 0.2px; }

        /* ── Shared screen typography ────────────────────────────────── */
        QLabel#screenTitle {
            font-size: 20px;
            font-weight: 700;
            color: #1a1a1a;
            letter-spacing: -0.3px;
        }
        QLabel#sectionLabel {
            font-size: 10px;
            font-weight: 700;
            color: #a8a49b;
            letter-spacing: 1.4px;
        }

        /* ── Generic buttons ─────────────────────────────────────────── */
        QPushButton#outlineBtn {
            background: transparent;
            color: #333;
            border: 1px solid #ccc;
            border-radius: 8px;
            padding: 8px 18px;
            font-size: 13px;
        }
        QPushButton#outlineBtn:hover { background: #f5f4f0; }

        /* ── Scrollbars ──────────────────────────────────────────────── */
        QScrollArea { border: none; background: transparent; }
        QScrollBar:vertical {
            width: 8px;
            background: transparent;
            margin: 4px 2px 4px 0;
        }
        QScrollBar::handle:vertical {
            background: #d8d4cb;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: #b8b4ab; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
            background: transparent;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
    )");
}
