#include "recommendations.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QStackedWidget>
#include <QDesktopServices>
#include <QUrl>
#include <functional>

// ─────────────────────────────────────────────────────────────────────────────
// Data
// ─────────────────────────────────────────────────────────────────────────────

struct ResourceInfo {
    QString icon;
    QString color;
    QString title;
    QString description;
    QString details;
    QString url;
    QStringList tags;
    QString url2;
    QString url2Label;
};

struct TopicInfo {
    QString icon;
    QString accent;     // background color for icon badge
    QString title;
    QString subtitle;   // shown on the landing card
};

static const QList<TopicInfo> TOPICS = {
    { "📚", "#EEEDFE", "Exam & Study Stress",        "Tutoring, writing help, academic coaching" },
    { "💰", "#E1F5EE", "Finance",                    "Financial aid, emergency funds, free food"  },
    { "🌐", "#FFF3E0", "Immigration",                "Legal help, DACA, undocumented support"     },
    { "💬", "#FCE4EC", "Relationships & Family",     "Personal counseling, peer support"          },
    { "🏥", "#E8F5E9", "Health & Wellness",          "Medical care, mental health, insurance"     },
};

// ─────────────────────────────────────────────────────────────────────────────
// Resource cards (detail pages — to be filled out later)
// ─────────────────────────────────────────────────────────────────────────────

static const QList<QList<ResourceInfo>> RESOURCES = {

    // 0 — Exam & Study
    {
        { "📚", "#EEEDFE", "Learning Resource Center — Tutoring",
          "Free peer tutoring for Math, English, Science, Business, and more. Walk-in or book online. No appointment needed.",
          "📍  199 Chambers St., Room S-510\n"
          "    245 Greenwich St., Fiterman Hall Room F-511\n"
          "🕐  In-person (Chambers): Mon–Thu 10am–6pm  |  Fri–Sat 10am–5pm\n"
          "    In-person (Fiterman): Mon–Wed 10am–3pm  |  Thu 10am–2pm\n"
          "    Remote: Mon–Thu 9am–9pm\n"
          "📞  (212) 220-1383  |  lrctutoring@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/lrc/in-person-tutoring/",
          {"Free", "Walk-in", "In-person & Remote"} },

        { "🤝", "#F3F0FF", "Peer Assisted Learning (PAL)",
          "Small-group study sessions (max 5 students) led by a trained PAL leader for supported courses. Great for processing difficult course material together.",
          "📍  199 Chambers St., Room S-510\n"
          "📞  (212) 220-1383  |  lrctutoring@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/lrc/in-person-tutoring/peer-assisted-learning/",
          {"Small Group", "Free", "Collaborative"} },

        { "✏️", "#E1F5EE", "Writing Center",
          "One-on-one help with any writing assignment — essays, research papers, reports, and more. All levels welcome.",
          "📍  199 Chambers St., Room S-510\n"
          "🕐  Mon–Thu 10am–6pm  |  Sat 10am–3pm  |  Sun: online only\n"
          "📞  (212) 220-1384  |  writingcenter@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/writing-center/",
          {"Writing", "Free", "In-person & Online"} },

        { "🎯", "#FAEEDA", "Academic Coaching",
          "Personalized one-on-one coaching for study strategies, time management, organization, and test preparation.",
          "📍  199 Chambers St., Room S-510\n"
          "🕐  Mon–Thu 10am–6pm  |  Fri 10am–5pm\n"
          "📧  AcademicCoaching@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/lrc/academic-coaching/",
          {"Study Skills", "Test Prep", "Free"} },

        { "💻", "#E8F4FD", "Online Tutoring",
          "Join a live remote tutoring session from home. Covers the same subjects as in-person — no commute needed.",
          "📞  (212) 220-8359  |  onlinetutoring@bmcc.cuny.edu\n"
          "🕐  Mon–Thu 9am–9pm  (extended remote hours)",
          "https://www.bmcc.cuny.edu/students/lrc/online-tutoring/tutoring-schedule/",
          {"Remote", "Flexible Hours", "Free"},
          "https://bmcc.upswing.io/up/dashboard/home", "Upswing →" },
    },

    // 1 — Finance
    {
        { "💰", "#E1F5EE", "Financial Aid Office",
          "Grants, loans, work-study, and scholarships. Speak with an advisor.",
          "📞 (212) 220-1430  |  finaid@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/finaid/",
          {"Grants", "Scholarships"} },

        { "🆘", "#FAEEDA", "Student Emergency Fund",
          "Emergency grants for textbooks, rent, utilities, and urgent needs.",
          "📍 Room S-230  |  Mon–Fri 8am–6:30pm  |  Sat 10am–1pm\n📞 (212) 220-8195",
          "https://www.bmcc.cuny.edu/student-affairs/arc/student-emergency-fund/",
          {"Emergency", "Fast Support"} },

        { "🍎", "#EEEDFE", "Panther Pantry — Free Food",
          "Free food for students facing food insecurity. Walk in, no questions asked.",
          "📍 Room S-230  |  Mon–Fri 8am–6:30pm  |  Sat 10am–1pm",
          "https://www.bmcc.cuny.edu/student-affairs/arc/panther-pantry/",
          {"Free Food", "Walk-in"} },

        { "🏛️", "#E1F5EE", "Advocacy & Resource Center",
          "SNAP assistance, tax prep, housing referrals, and financial counseling.",
          "📍 Room S-230  |  Mon–Fri 8am–6:30pm  |  Sat 10am–1pm",
          "https://www.bmcc.cuny.edu/student-affairs/arc/",
          {"SNAP", "Housing", "Tax Help"} },
    },

    // 2 — Immigration
    {
        { "🌐", "#EEEDFE", "Immigrant Resource Center",
          "Confidential support for immigrant and undocumented students.",
          "📞 Albert Lee: (212) 776-6252  |  allee@bmcc.cuny.edu\n📍 Room S-230  |  Mon–Fri 8am–6:30pm",
          "https://www.bmcc.cuny.edu/student-affairs/center-for-belonging-and-inclusion/immigrant-resource-center/",
          {"Confidential", "All Statuses"} },

        { "⚖️", "#E1F5EE", "Free Legal Services — CUNY Citizenship Now!",
          "On-site attorneys monthly for DACA, TPS, family petitions, and naturalization.",
          "📍 Room S-230",
          "https://www.bmcc.cuny.edu/student-affairs/arc/legal-services/",
          {"DACA", "Legal Aid", "Free"} },

        { "🎓", "#FAEEDA", "Scholarships for Immigrant Students",
          "TheDream.US and other scholarships available regardless of status.",
          "",
          "https://www.bmcc.cuny.edu/immigration/scholarships-and-financial-aid/",
          {"Scholarships", "DREAMers"} },

        { "🔒", "#EEEDFE", "Know Your Rights at BMCC",
          "CUNY will not share your records with immigration enforcement without a court order.",
          "",
          "https://www.bmcc.cuny.edu/immigration/",
          {"Privacy", "Your Rights"} },
    },

    // 3 — Relationships & Family
    {
        { "💬", "#EEEDFE", "Personal Counseling — Counseling Center",
          "Free, confidential counseling for relationship, family, and personal issues.",
          "📍 Room S-343  |  Mon, Tue, Thu 9am–6pm  |  Wed, Fri 9am–5pm\n"
          "📞 (212) 220-8140  |  counselingcenter@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/counseling-services/",
          {"Free", "Confidential", "Telehealth OK"} },

        { "🤝", "#E1F5EE", "Togetherall — 24/7 Peer Support",
          "Free anonymous online community for students to support each other.",
          "",
          "https://togetherall.com/en-us/",
          {"Anonymous", "24/7", "Online"} },

        { "📋", "#FAEEDA", "Book a Counseling Appointment",
          "Schedule in person, by phone, or on Zoom. Crisis support also available.",
          "📞 (212) 220-8140",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/request-an-appointment/",
          {"Appointment", "Crisis"} },

        { "🏘️", "#EEEDFE", "Community Referrals",
          "Referrals to outside help for housing, domestic support, and legal aid.",
          "",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/counseling-resources/community-resources/",
          {"Housing", "Legal Aid"} },
    },

    // 4 — Health & Wellness
    {
        { "🏥", "#E1F5EE", "Health & Wellness Services",
          "First aid, medical evaluation, health counseling, and wellness workshops.",
          "",
          "https://www.bmcc.cuny.edu/student-affairs/health-services/",
          {"Medical", "On-campus"} },

        { "🧠", "#EEEDFE", "Mental Health Counseling",
          "Short-term counseling, crisis support, and referrals to community providers.",
          "📍  Room S-343\n"
          "🕐  Mon, Tue, Thu 9am–6pm  |  Wed, Fri 9am–5pm\n"
          "📞  (212) 220-8140  |  counselingcenter@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/",
          {"Mental Health", "Free", "Crisis"} },

        { "🎖️", "#FFF8E1", "Veterans Resource Center — Stress & Crisis Support",
          "Dedicated support for veteran students dealing with stress, PTSD, and mental health challenges. Request a virtual or in-person appointment.",
          "📍  199 Chambers St., Room S-115M\n"
          "📞  (212) 220-8035  |  saruiz@bmcc.cuny.edu\n"
          "👤  Samantha Ruiz, LCSW — Manager for Veterans Affairs & Student Development",
          "https://www.bmcc.cuny.edu/student-affairs/veterans-resource-center/dealing-with-stress/",
          {"Veterans", "PTSD", "Free"} },

        { "☎️", "#FCE4EC", "Samaritans — 24-Hour Crisis Hotline",
          "Free and confidential support if you are feeling stressed, overwhelmed, or suicidal — or if you're worried about a friend or family member. Available around the clock.",
          "📞  (212) 673-3000  —  free, confidential, 24 hours a day",
          "",
          {"24/7", "Free", "Confidential"} },

        { "🪖", "#E8F5E9", "National Center for PTSD",
          "Information and resources from the U.S. Department of Veterans' Affairs for veterans and their families dealing with PTSD.",
          "",
          "https://www.ptsd.va.gov/",
          {"Veterans", "PTSD", "U.S. VA"} },

        { "💪", "#E3F2FD", "Combat Stress Recovery Program",
          "Support program from the Wounded Warrior Project specifically designed to help veterans recover from combat stress and related mental health challenges.",
          "",
          "https://www.woundedwarriorproject.org/programs/mental-wellness",
          {"Veterans", "Combat Stress", "Wounded Warrior"} },

        { "🩺", "#FAEEDA", "Health Insurance Enrollment",
          "Help enrolling in NY State health insurance — open to all students including international and undocumented.",
          "📍  Room S-230",
          "https://www.bmcc.cuny.edu/student-affairs/arc/health-insurance/",
          {"Insurance", "All Students"} },

    },
};

// ─────────────────────────────────────────────────────────────────────────────
// Builders
// ─────────────────────────────────────────────────────────────────────────────

static QWidget* makeResourceCard(const ResourceInfo &r) {
    // Outer wrapper: provides the left accent bar + card border
    QFrame *wrapper = new QFrame();
    // border-left provides the accent bar without a child widget,
    // so it correctly clips to border-radius on all platforms (incl. macOS).
    wrapper->setStyleSheet(
        QString("QFrame {"
                "  border: 1px solid #e8e6e0;"
                "  border-left: 4px solid %1;"
                "  border-radius: 10px;"
                "  background: #ffffff;"
                "}").arg(r.color));

    QVBoxLayout *cl = new QVBoxLayout(wrapper);
    cl->setContentsMargins(14, 13, 14, 12);
    cl->setSpacing(8);

    // ── Header: icon + title ──────────────────────────────────────────────
    QWidget *header = new QWidget();
    header->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *hl = new QHBoxLayout(header);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(10);

    QLabel *iconLbl = new QLabel(r.icon);
    iconLbl->setFixedSize(36, 36);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet(
        QString("background:%1; border-radius:8px; font-size:18px; border:none;").arg(r.color));

    QLabel *titleLbl = new QLabel(r.title);
    titleLbl->setWordWrap(true);
    titleLbl->setStyleSheet("font-size:14px; font-weight:700; color:#1a1a1a; border:none;");

    hl->addWidget(iconLbl);
    hl->addWidget(titleLbl, 1);
    cl->addWidget(header);

    // ── Description ───────────────────────────────────────────────────────
    QLabel *descLbl = new QLabel(r.description);
    descLbl->setWordWrap(true);
    descLbl->setStyleSheet("font-size:12px; color:#4a4a4a; border:none; line-height:150%;");
    cl->addWidget(descLbl);

    // ── Contact / Details block ───────────────────────────────────────────
    if (!r.details.isEmpty()) {
        QFrame *detBox = new QFrame();
        detBox->setStyleSheet(
            "QFrame {"
            "  background: #f8f7f4;"
            "  border: 1px solid #ece9e2;"
            "  border-radius: 7px;"
            "}");
        QVBoxLayout *dl = new QVBoxLayout(detBox);
        dl->setContentsMargins(10, 8, 10, 8);
        dl->setSpacing(3);

        for (const QString &line : r.details.split('\n')) {
            if (line.trimmed().isEmpty()) continue;
            QLabel *lbl = new QLabel(line.trimmed());
            lbl->setWordWrap(true);
            lbl->setStyleSheet("font-size:11px; color:#666; border:none; background:transparent;");
            dl->addWidget(lbl);
        }
        cl->addWidget(detBox);
    }

    // ── Footer: tags + buttons ────────────────────────────────────────────
    QWidget *footer = new QWidget();
    footer->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *fl = new QHBoxLayout(footer);
    fl->setContentsMargins(0, 2, 0, 0);
    fl->setSpacing(6);

    static const QStringList tagStyles = {
        "background:#EEEDFE; color:#534AB7;",
        "background:#E1F5EE; color:#0F6E56;",
        "background:#FEF3E2; color:#925A0E;"
    };
    for (int i = 0; i < r.tags.size(); i++) {
        QLabel *t = new QLabel(r.tags[i]);
        t->setStyleSheet(
            QString("font-size:10px; font-weight:600; padding:2px 8px; "
                    "border-radius:10px; border:none; %1")
                .arg(tagStyles[i % tagStyles.size()]));
        fl->addWidget(t);
    }
    fl->addStretch();

    auto addUrlBtn = [&](const QString &url, const QString &label) {
        QPushButton *btn = new QPushButton(label);
        QString u = url;
        QObject::connect(btn, &QPushButton::clicked, [u]() {
            QDesktopServices::openUrl(QUrl(u));
        });
        btn->setStyleSheet(
            "QPushButton { font-size:11px; font-weight:600; color:#534AB7; background:#EEEDFE; "
            "border:none; border-radius:7px; padding:4px 13px; }"
            "QPushButton:hover { background:#dddaf8; }");
        btn->setCursor(Qt::PointingHandCursor);
        fl->addWidget(btn);
    };

    if (!r.url.isEmpty())  addUrlBtn(r.url,  "Visit →");
    if (!r.url2.isEmpty()) addUrlBtn(r.url2, r.url2Label.isEmpty() ? "Open →" : r.url2Label);

    cl->addWidget(footer);
    return wrapper;
}

// Large clickable topic card for the landing page
static QWidget* makeTopicCard(const TopicInfo &t, std::function<void()> onClick) {
    QPushButton *card = new QPushButton();
    card->setFixedHeight(72);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(
        "QPushButton {"
        "  border: 1.5px solid #e8e6e0;"
        "  border-radius: 12px;"
        "  background: #ffffff;"
        "}"
        "QPushButton:hover {"
        "  border-color: #6C63FF;"
        "  background: #fafafa;"
        "}"
    );

    QHBoxLayout *hl = new QHBoxLayout(card);
    hl->setContentsMargins(16, 0, 16, 0);
    hl->setSpacing(14);

    // Icon badge
    QLabel *iconLbl = new QLabel(t.icon);
    iconLbl->setFixedSize(44, 44);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet(
        QString("background:%1; border-radius:10px; font-size:22px; border:none;").arg(t.accent));
    iconLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Text block
    QWidget *textBlock = new QWidget();
    textBlock->setStyleSheet("border:none; background:transparent;");
    textBlock->setAttribute(Qt::WA_TransparentForMouseEvents);
    QVBoxLayout *vl = new QVBoxLayout(textBlock);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(2);
    QLabel *titleLbl = new QLabel(t.title);
    titleLbl->setStyleSheet("font-size:14px; font-weight:600; color:#1a1a1a; border:none;");
    QLabel *subLbl = new QLabel(t.subtitle);
    subLbl->setStyleSheet("font-size:12px; color:#888; border:none;");
    vl->addWidget(titleLbl);
    vl->addWidget(subLbl);

    // Arrow
    QLabel *arrow = new QLabel("→");
    arrow->setStyleSheet("font-size:16px; color:#bbb; border:none;");
    arrow->setAttribute(Qt::WA_TransparentForMouseEvents);

    hl->addWidget(iconLbl);
    hl->addWidget(textBlock, 1);
    hl->addWidget(arrow);

    QObject::connect(card, &QPushButton::clicked, onClick);
    return card;
}

// Scrollable list of resource cards
static QWidget* makeResourcePage(int topicIndex) {
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    QWidget *inner = new QWidget();
    QVBoxLayout *il = new QVBoxLayout(inner);
    il->setSpacing(10);
    il->setContentsMargins(0, 4, 0, 4);
    for (const ResourceInfo &r : RESOURCES[topicIndex])
        il->addWidget(makeResourceCard(r));
    il->addStretch();
    scroll->setWidget(inner);
    return scroll;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Recommendations::Recommendations(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_stack = new QStackedWidget();

    // ── Page 0: Landing / topic picker ───────────────────────────────────
    QWidget *homePage = new QWidget();
    QVBoxLayout *hl = new QVBoxLayout(homePage);
    hl->setContentsMargins(28, 24, 28, 24);
    hl->setSpacing(14);

    QLabel *greeting = new QLabel("Hi there 👋");
    greeting->setStyleSheet("font-size:13px; color:#888;");

    QLabel *heading = new QLabel("What are you stressed about?");
    heading->setStyleSheet("font-size:22px; font-weight:700; color:#1a1a1a;");

    QLabel *sub = new QLabel("Choose a topic and we'll show you what BMCC has available for you.");
    sub->setWordWrap(true);
    sub->setStyleSheet("font-size:13px; color:#666;");

    hl->addWidget(greeting);
    hl->addWidget(heading);
    hl->addWidget(sub);
    hl->addSpacing(8);

    for (int i = 0; i < TOPICS.size(); i++) {
        int idx = i;
        hl->addWidget(makeTopicCard(TOPICS[i], [this, idx]() { showTopic(idx); }));
    }
    hl->addStretch();

    m_stack->addWidget(homePage); // index 0

    // ── Pages 1–5: Resource detail pages ─────────────────────────────────
    for (int i = 0; i < TOPICS.size(); i++) {
        QWidget *page = new QWidget();
        QVBoxLayout *pl = new QVBoxLayout(page);
        pl->setContentsMargins(28, 20, 28, 20);
        pl->setSpacing(12);

        // Back button + topic title row
        QWidget *topRow = new QWidget();
        topRow->setStyleSheet("border:none;");
        QHBoxLayout *trl = new QHBoxLayout(topRow);
        trl->setContentsMargins(0, 0, 0, 0);
        trl->setSpacing(10);

        QPushButton *backBtn = new QPushButton("← Back");
        backBtn->setStyleSheet(
            "QPushButton { font-size:12px; color:#6C63FF; background:transparent; "
            "border:1.5px solid #6C63FF; border-radius:8px; padding:4px 14px; }"
            "QPushButton:hover { background:#EEEDFE; }");
        backBtn->setCursor(Qt::PointingHandCursor);
        connect(backBtn, &QPushButton::clicked, this, &Recommendations::showHome);

        QLabel *iconLbl = new QLabel(TOPICS[i].icon);
        iconLbl->setFixedSize(36, 36);
        iconLbl->setAlignment(Qt::AlignCenter);
        iconLbl->setStyleSheet(
            QString("background:%1; border-radius:8px; font-size:18px; border:none;")
                .arg(TOPICS[i].accent));

        QLabel *titleLbl = new QLabel(TOPICS[i].title);
        titleLbl->setStyleSheet("font-size:17px; font-weight:700; color:#1a1a1a; border:none;");

        trl->addWidget(backBtn);
        trl->addSpacing(6);
        trl->addWidget(iconLbl);
        trl->addWidget(titleLbl, 1);

        QFrame *div = new QFrame();
        div->setFrameShape(QFrame::HLine);
        div->setStyleSheet("color:#eee;");

        QLabel *sectionLbl = new QLabel("AVAILABLE AT BMCC");
        sectionLbl->setObjectName("sectionLabel");

        pl->addWidget(topRow);
        pl->addWidget(div);
        pl->addWidget(sectionLbl);
        pl->addWidget(makeResourcePage(i), 1);

        m_stack->addWidget(page); // indices 1–5
    }

    root->addWidget(m_stack);
    m_stack->setCurrentIndex(0);
}

void Recommendations::showTopic(int index) {
    m_stack->setCurrentIndex(index + 1); // +1 because index 0 is home
}

void Recommendations::showHome() {
    m_stack->setCurrentIndex(0);
}
