#include "mainwindow.h"
#include "recommendations.h"
#include "toolkit.h"
#include "journal.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("MindEase — BMCC Wellness Companion");
    setMinimumSize(1000, 700);

    applyStyle();

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Build screens
    recScreen     = new Recommendations(this);
    toolkitScreen = new Toolkit(this);
    journalScreen = new Journal(this);

    // Stack
    stack = new QStackedWidget(this);
    stack->addWidget(recScreen);     // 0 — BMCC Resources
    stack->addWidget(toolkitScreen); // 1 — Mental Health Toolkit
    stack->addWidget(journalScreen); // 2 — My Journal

    buildSidebar();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stack);
}

MainWindow::~MainWindow() {}

void MainWindow::buildSidebar() {
    sidebar = new QWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(200);

    QVBoxLayout *sl = new QVBoxLayout(sidebar);
    sl->setContentsMargins(0, 0, 0, 0);
    sl->setSpacing(0);

    // Logo
    QWidget *logo = new QWidget();
    logo->setObjectName("logoBox");
    QVBoxLayout *ll = new QVBoxLayout(logo);
    ll->setContentsMargins(16, 14, 16, 14);
    QLabel *appName = new QLabel("MindEase");
    appName->setObjectName("appName");
    QLabel *appSub  = new QLabel("BMCC Wellness Companion");
    appSub->setObjectName("appSub");
    ll->addWidget(appName);
    ll->addWidget(appSub);
    sl->addWidget(logo);

    // Nav items  (index matches stack order)
    struct NavDef { QString label; QString objName; };
    QList<NavDef> navDefs = {
        { "✦  BMCC Resources",       "navBtn"        },
        { "⚙  Mental Health Toolkit","navBtnToolkit" },
        { "📓  My Journal",           "navBtnJournal" },
    };

    for (int i = 0; i < navDefs.size(); i++) {
        QPushButton *btn = new QPushButton(navDefs[i].label);
        btn->setObjectName(navDefs[i].objName);
        btn->setCheckable(true);
        btn->setChecked(i == 0);
        connect(btn, &QPushButton::clicked, this, [this, i]() { switchScreen(i); });
        navButtons.append(btn);
        sl->addWidget(btn);
    }

    sl->addStretch();

    QLabel *ver = new QLabel("v1.0  ·  BMCC / Qt");
    ver->setObjectName("verLabel");
    ver->setContentsMargins(16, 0, 16, 12);
    sl->addWidget(ver);
}

void MainWindow::switchScreen(int index) {
    stack->setCurrentIndex(index);
    for (int i = 0; i < navButtons.size(); i++)
        navButtons[i]->setChecked(i == index);
}

void MainWindow::applyStyle() {
    setStyleSheet(R"(
        QMainWindow { background: #ffffff; }

        #sidebar {
            background: #f5f4f0;
            border-right: 1px solid #e0ded8;
        }
        #logoBox {
            border-bottom: 1px solid #e0ded8;
        }
        #appName {
            font-size: 16px;
            font-weight: 600;
            color: #1a1a1a;
        }
        #appSub {
            font-size: 11px;
            color: #888;
            margin-top: 1px;
        }
        #navBtn {
            text-align: left;
            padding: 9px 16px;
            border: none;
            border-left: 2px solid transparent;
            background: transparent;
            font-size: 13px;
            color: #666;
            border-radius: 0px;
        }
        #navBtn:hover {
            background: #ffffff;
            color: #1a1a1a;
        }
        #navBtn:checked {
            background: #ffffff;
            color: #534AB7;
            border-left: 2px solid #534AB7;
            font-weight: 600;
        }
        #navBtnToolkit {
            text-align: left;
            padding: 9px 16px;
            border: none;
            border-left: 2px solid transparent;
            background: transparent;
            font-size: 13px;
            color: #0F6E56;
            font-weight: 600;
            border-radius: 0px;
        }
        #navBtnToolkit:hover { background: #ffffff; }
        #navBtnToolkit:checked {
            background: #ffffff;
            border-left: 2px solid #0F6E56;
        }
        #navBtnJournal {
            text-align: left;
            padding: 9px 16px;
            border: none;
            border-left: 2px solid transparent;
            background: transparent;
            font-size: 13px;
            color: #A0620C;
            font-weight: 600;
            border-radius: 0px;
        }
        #navBtnJournal:hover { background: #ffffff; }
        #navBtnJournal:checked {
            background: #ffffff;
            border-left: 2px solid #A0620C;
        }
        #verLabel { font-size: 11px; color: #aaa; }

        QLabel#screenTitle {
            font-size: 16px;
            font-weight: 600;
            color: #1a1a1a;
        }
        QLabel#sectionLabel {
            font-size: 11px;
            font-weight: 600;
            color: #888;
            letter-spacing: 1px;
        }
        QPushButton#outlineBtn {
            background: transparent;
            color: #333;
            border: 1px solid #ccc;
            border-radius: 8px;
            padding: 8px 18px;
            font-size: 13px;
        }
        QPushButton#outlineBtn:hover { background: #f5f4f0; }
        QScrollArea { border: none; background: transparent; }
        QScrollBar:vertical { width: 6px; background: transparent; }
        QScrollBar::handle:vertical { background: #ddd; border-radius: 3px; }
    )");
}
