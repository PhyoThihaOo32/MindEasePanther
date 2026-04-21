#include "screens/recommendations.h"
#include "core/screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QStackedWidget>
#include <QSizePolicy>
#include <QLineEdit>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
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
    { "📚", "#edf7ee", "Exam & Study Stress",        "Tutoring, writing help, academic coaching" },
    { "💰", "#e2f4e8", "Finance",                    "Financial aid, emergency funds, free food"  },
    { "🌐", "#f1f8e8", "Immigration",                "Legal help, DACA, undocumented support"     },
    { "💬", "#e9f5ef", "Relationships & Family",     "Personal counseling, peer support"          },
    { "🏥", "#dff1e5", "Health & Wellness",          "Medical care, mental health, insurance"     },
};

// ─────────────────────────────────────────────────────────────────────────────
// Resource cards (detail pages — to be filled out later)
// ─────────────────────────────────────────────────────────────────────────────

static const QList<QList<ResourceInfo>> RESOURCES = {

    // 0 — Exam & Study
    {
        { "📚", "#edf7ee", "Learning Resource Center — Tutoring",
          "Free peer tutoring for Math, English, Science, Business, and more. Walk-in or book online. No appointment needed.",
          "📍  199 Chambers St., Room S-510\n"
          "    245 Greenwich St., Fiterman Hall Room F-511\n"
          "🕐  In-person (Chambers): Mon–Thu 10am–6pm  |  Fri–Sat 10am–5pm\n"
          "    In-person (Fiterman): Mon–Wed 10am–3pm  |  Thu 10am–2pm\n"
          "    Remote: Mon–Thu 9am–9pm\n"
          "📞  (212) 220-1383  |  lrctutoring@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/lrc/in-person-tutoring/",
          {"Free", "Walk-in", "In-person & Remote"} },

        { "🤝", "#eef8f1", "Peer Assisted Learning (PAL)",
          "Small-group study sessions (max 5 students) led by a trained PAL leader for supported courses. Great for processing difficult course material together.",
          "📍  199 Chambers St., Room S-510\n"
          "📞  (212) 220-1383  |  lrctutoring@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/lrc/in-person-tutoring/peer-assisted-learning/",
          {"Small Group", "Free", "Collaborative"} },

        { "✏️", "#e2f4e8", "Writing Center",
          "One-on-one help with any writing assignment — essays, research papers, reports, and more. All levels welcome.",
          "📍  199 Chambers St., Room S-510\n"
          "🕐  Mon–Thu 10am–6pm  |  Sat 10am–3pm  |  Sun: online only\n"
          "📞  (212) 220-1384  |  writingcenter@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/writing-center/",
          {"Writing", "Free", "In-person & Online"} },

        { "🎯", "#f1f8e8", "Academic Coaching",
          "Personalized one-on-one coaching for study strategies, time management, organization, and test preparation.",
          "📍  199 Chambers St., Room S-510\n"
          "🕐  Mon–Thu 10am–6pm  |  Fri 10am–5pm\n"
          "📧  AcademicCoaching@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/students/lrc/academic-coaching/",
          {"Study Skills", "Test Prep", "Free"} },

        { "💻", "#e8f7ef", "Online Tutoring",
          "Join a live remote tutoring session from home. Covers the same subjects as in-person — no commute needed.",
          "📞  (212) 220-8359  |  onlinetutoring@bmcc.cuny.edu\n"
          "🕐  Mon–Thu 9am–9pm  (extended remote hours)",
          "https://www.bmcc.cuny.edu/students/lrc/online-tutoring/tutoring-schedule/",
          {"Remote", "Flexible Hours", "Free"},
          "https://bmcc.upswing.io/up/dashboard/home", "Upswing →" },
    },

    // 1 — Finance
    {
        { "💰", "#e2f4e8", "Financial Aid Office",
          "Grants, loans, work-study, and scholarships. Speak with an advisor.",
          "📞 (212) 220-1430  |  finaid@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/finaid/",
          {"Grants", "Scholarships"} },

        { "🆘", "#f1f8e8", "Student Emergency Fund",
          "Emergency grants for textbooks, rent, utilities, and urgent needs.",
          "📍 Room S-230  |  Mon–Fri 8am–6:30pm  |  Sat 10am–1pm\n📞 (212) 220-8195",
          "https://www.bmcc.cuny.edu/student-affairs/arc/student-emergency-fund/",
          {"Emergency", "Fast Support"} },

        { "🍎", "#edf7ee", "Panther Pantry — Free Food",
          "Free food for students facing food insecurity. Walk in, no questions asked.",
          "📍 Room S-230  |  Mon–Fri 8am–6:30pm  |  Sat 10am–1pm",
          "https://www.bmcc.cuny.edu/student-affairs/arc/panther-pantry/",
          {"Free Food", "Walk-in"} },

        { "🏛️", "#e2f4e8", "Advocacy & Resource Center",
          "SNAP assistance, tax prep, housing referrals, and financial counseling.",
          "📍 Room S-230  |  Mon–Fri 8am–6:30pm  |  Sat 10am–1pm",
          "https://www.bmcc.cuny.edu/student-affairs/arc/",
          {"SNAP", "Housing", "Tax Help"} },
    },

    // 2 — Immigration
    {
        { "🌐", "#edf7ee", "Immigrant Resource Center",
          "Confidential support for immigrant and undocumented students.",
          "📞 Albert Lee: (212) 776-6252  |  allee@bmcc.cuny.edu\n📍 Room S-230  |  Mon–Fri 8am–6:30pm",
          "https://www.bmcc.cuny.edu/student-affairs/center-for-belonging-and-inclusion/immigrant-resource-center/",
          {"Confidential", "All Statuses"} },

        { "⚖️", "#e2f4e8", "Free Legal Services — CUNY Citizenship Now!",
          "On-site attorneys monthly for DACA, TPS, family petitions, and naturalization.",
          "📍 Room S-230",
          "https://www.bmcc.cuny.edu/student-affairs/arc/legal-services/",
          {"DACA", "Legal Aid", "Free"} },

        { "🎓", "#f1f8e8", "Scholarships for Immigrant Students",
          "TheDream.US and other scholarships available regardless of status.",
          "",
          "https://www.bmcc.cuny.edu/immigration/scholarships-and-financial-aid/",
          {"Scholarships", "DREAMers"} },

        { "🔒", "#edf7ee", "Know Your Rights at BMCC",
          "CUNY will not share your records with immigration enforcement without a court order.",
          "",
          "qrc:/documents/2025-KYR-Final-01.13.202592.pdf",
          {"Privacy", "Your Rights"} },
    },

    // 3 — Relationships & Family
    {
        { "💬", "#edf7ee", "Personal Counseling — Counseling Center",
          "Free, confidential counseling for relationship, family, and personal issues.",
          "📍 Room S-343  |  Mon, Tue, Thu 9am–6pm  |  Wed, Fri 9am–5pm\n"
          "📞 (212) 220-8140  |  counselingcenter@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/counseling-services/",
          {"Free", "Confidential", "Telehealth OK"} },

        { "🤝", "#e2f4e8", "Togetherall — 24/7 Peer Support",
          "Free anonymous online community for students to support each other.",
          "",
          "https://togetherall.com/en-us/",
          {"Anonymous", "24/7", "Online"} },

        { "📋", "#f1f8e8", "Book a Counseling Appointment",
          "Schedule in person, by phone, or on Zoom. Crisis support also available.",
          "📞 (212) 220-8140",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/request-an-appointment/",
          {"Appointment", "Crisis"} },

        { "🏘️", "#edf7ee", "Community Referrals",
          "Referrals to outside help for housing, domestic support, and legal aid.",
          "",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/counseling-resources/community-resources/",
          {"Housing", "Legal Aid"} },
    },

    // 4 — Health & Wellness
    {
        { "🏥", "#e2f4e8", "Health & Wellness Services",
          "First aid, medical evaluation, health counseling, and wellness workshops.",
          "📍  Room N-380, 199 Chambers St.\n"
          "🕐  Mon – Fri: 9 a.m. – 5 p.m.\n"
          "📞  Tel: (212) 220-8256  |  Fax: (212) 220-2367\n"
          "📧  healthservices@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/student-affairs/health-services/",
          {"Medical", "On-campus", "Free"} },

        { "🧠", "#edf7ee", "Mental Health Counseling",
          "Short-term counseling, crisis support, and referrals to community providers.",
          "📍  Room S-343\n"
          "🕐  Mon, Tue, Thu 9am–6pm  |  Wed, Fri 9am–5pm\n"
          "📞  (212) 220-8140  |  counselingcenter@bmcc.cuny.edu",
          "https://www.bmcc.cuny.edu/student-affairs/counseling/",
          {"Mental Health", "Free", "Crisis"} },

        { "🎖️", "#f3faef", "Veterans Resource Center — Stress & Crisis Support",
          "Dedicated support for veteran students dealing with stress, PTSD, and mental health challenges. Request a virtual or in-person appointment.",
          "📍  199 Chambers St., Room S-115M\n"
          "📞  (212) 220-8035  |  saruiz@bmcc.cuny.edu\n"
          "👤  Samantha Ruiz, LCSW — Manager for Veterans Affairs & Student Development",
          "https://www.bmcc.cuny.edu/student-affairs/veterans-resource-center/dealing-with-stress/",
          {"Veterans", "PTSD", "Free"} },

        { "☎️", "#e9f5ef", "Samaritans — 24-Hour Crisis Hotline",
          "Free and confidential support if you are feeling stressed, overwhelmed, or suicidal — or if you're worried about a friend or family member. Available around the clock.",
          "📞  (212) 673-3000  —  free, confidential, 24 hours a day",
          "",
          {"24/7", "Free", "Confidential"} },

        { "🪖", "#dff1e5", "National Center for PTSD",
          "Information and resources from the U.S. Department of Veterans' Affairs for veterans and their families dealing with PTSD.",
          "",
          "https://www.ptsd.va.gov/",
          {"Veterans", "PTSD", "U.S. VA"} },

        { "💪", "#eef7f1", "Combat Stress Recovery Program",
          "Support program from the Wounded Warrior Project specifically designed to help veterans recover from combat stress and related mental health challenges.",
          "",
          "https://www.woundedwarriorproject.org/programs/mental-wellness",
          {"Veterans", "Combat Stress", "Wounded Warrior"} },

        { "🩺", "#f1f8e8", "Health Insurance Enrollment",
          "Help enrolling in NY State health insurance — open to all students including international and undocumented.",
          "📍  Room S-230",
          "https://www.bmcc.cuny.edu/student-affairs/arc/health-insurance/",
          {"Insurance", "All Students"} },

    },
};

static QLabel* makeInfoChip(const QString &text,
                            const QString &bg = "#ffffff",
                            const QString &fg = "#5d574f",
                            const QString &border = "#ece9e2") {
    QLabel *chip = new QLabel(text);
    chip->setStyleSheet(
        QString("font-size:11px; font-weight:600; color:%1; background:%2; "
                "border:1px solid %3; border-radius:12px; padding:5px 10px;")
            .arg(fg, bg, border));
    return chip;
}

static bool openResourceTarget(const QString &target) {
    if (target.startsWith("qrc:/")) {
        const QString resourcePath = ":" + target.mid(4);
        QString cacheDirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        if (cacheDirPath.isEmpty())
            cacheDirPath = QDir::tempPath() + "/MindEase";

        QDir cacheDir(cacheDirPath);
        if (!cacheDir.exists() && !cacheDir.mkpath("."))
            return false;

        const QString outputPath = cacheDir.filePath(QFileInfo(resourcePath).fileName());
        if (QFile::exists(outputPath))
            QFile::remove(outputPath);

        if (!QFile::copy(resourcePath, outputPath))
            return false;

        return QDesktopServices::openUrl(QUrl::fromLocalFile(outputPath));
    }

    const QFileInfo localFile(target);
    if (localFile.exists())
        return QDesktopServices::openUrl(QUrl::fromLocalFile(localFile.absoluteFilePath()));

    return QDesktopServices::openUrl(QUrl(target));
}

static QString normalizedText(const QString &text) {
    return text.simplified().toLower();
}

struct TopicSearchResult {
    int score = -1;
    QStringList matchedSubtopics;
};

static int scoreTextMatch(const QString &text,
                          const QString &query,
                          const QStringList &terms,
                          int exact,
                          int prefix,
                          int contains,
                          int allTermsBase) {
    const QString normalized = normalizedText(text);
    if (normalized.isEmpty()) return -1;

    int score = -1;
    if (normalized == query) score = qMax(score, exact);
    else if (normalized.startsWith(query)) score = qMax(score, prefix);
    else if (normalized.contains(query)) score = qMax(score, contains);

    int termHits = 0;
    for (const QString &term : terms) {
        if (normalized.contains(term))
            termHits++;
    }

    if (!terms.isEmpty() && termHits == terms.size())
        score = qMax(score, allTermsBase + termHits * 10);

    return score;
}

static TopicSearchResult searchTopic(int topicIndex, const QString &queryText) {
    const QString query = normalizedText(queryText);
    if (query.isEmpty()) return {};

    TopicSearchResult result;
    const TopicInfo &topic = TOPICS[topicIndex];
    const QStringList terms = query.split(' ', Qt::SkipEmptyParts);

    result.score = qMax(result.score,
                        scoreTextMatch(topic.title, query, terms, 420, 320, 240, 260));
    result.score = qMax(result.score,
                        scoreTextMatch(topic.subtitle, query, terms, 220, 170, 120, 190));

    for (const ResourceInfo &resource : RESOURCES[topicIndex]) {
        int resourceScore = -1;
        resourceScore = qMax(resourceScore,
                             scoreTextMatch(resource.title, query, terms, 260, 220, 190, 210));
        resourceScore = qMax(resourceScore,
                             scoreTextMatch(resource.description, query, terms, 170, 150, 130, 150));
        resourceScore = qMax(resourceScore,
                             scoreTextMatch(resource.details, query, terms, 110, 100, 90, 100));

        for (const QString &tag : resource.tags) {
            resourceScore = qMax(resourceScore,
                                 scoreTextMatch(tag, query, terms, 210, 180, 150, 170));
        }

        if (resourceScore >= 0) {
            result.score = qMax(result.score, resourceScore);
            result.matchedSubtopics.append(resource.title);
        }
    }

    result.matchedSubtopics.removeDuplicates();
    return result;
}

static QString summarizedSubtopics(const QStringList &subtopics) {
    if (subtopics.isEmpty()) return {};
    if (subtopics.size() == 1)
        return subtopics.first();
    if (subtopics.size() == 2)
        return subtopics.join(", ");
    return QString("%1, %2, +%3 more")
        .arg(subtopics[0], subtopics[1])
        .arg(subtopics.size() - 2);
}

// ─────────────────────────────────────────────────────────────────────────────
// Builders
// ─────────────────────────────────────────────────────────────────────────────

static QWidget* makeResourceCard(const ResourceInfo &r) {
    // Outer wrapper: border + left accent bar. Using border-left rather than a
    // child QFrame means the bar correctly clips to border-radius on all OSes.
    QFrame *wrapper = new QFrame();
    wrapper->setStyleSheet(
        QString("QFrame {"
                "  border: 1px solid #ece9e2;"
                "  border-left: 4px solid %1;"
                "  border-radius: 12px;"
                "  background: #ffffff;"
                "}").arg(r.color));
    wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QVBoxLayout *cl = new QVBoxLayout(wrapper);
    cl->setContentsMargins(18, 16, 18, 16);
    cl->setSpacing(10);

    // ── Header: icon + title ──────────────────────────────────────────────
    QWidget *header = new QWidget();
    header->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *hl = new QHBoxLayout(header);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(12);

    QLabel *iconLbl = new QLabel(r.icon);
    iconLbl->setFixedSize(38, 38);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet(
        QString("background:%1; border-radius:10px; font-size:19px; border:none;").arg(r.color));

    QLabel *titleLbl = new QLabel(r.title);
    titleLbl->setWordWrap(true);
    titleLbl->setStyleSheet(
        "font-size:14px; font-weight:700; color:#1a1a1a; border:none; "
        "letter-spacing:-0.1px;");

    hl->addWidget(iconLbl, 0, Qt::AlignTop);
    hl->addWidget(titleLbl, 1);
    cl->addWidget(header);

    // ── Description ───────────────────────────────────────────────────────
    QLabel *descLbl = new QLabel(r.description);
    descLbl->setWordWrap(true);
    descLbl->setStyleSheet(
        "font-size:12px; color:#555; border:none; line-height:160%;");
    cl->addWidget(descLbl);

    // ── Contact / Details block ───────────────────────────────────────────
    if (!r.details.isEmpty()) {
        QFrame *detBox = new QFrame();
        detBox->setStyleSheet(
            "QFrame {"
            "  background: #faf9f5;"
            "  border: 1px solid #ece9e2;"
            "  border-radius: 8px;"
            "}");
        QVBoxLayout *dl = new QVBoxLayout(detBox);
        dl->setContentsMargins(12, 10, 12, 10);
        dl->setSpacing(4);

        for (const QString &line : r.details.split('\n')) {
            if (line.trimmed().isEmpty()) continue;
            QLabel *lbl = new QLabel(line.trimmed());
            lbl->setWordWrap(true);
            lbl->setStyleSheet(
                "font-size:11px; color:#6d675e; border:none; background:transparent; "
                "line-height:155%;");
            dl->addWidget(lbl);
        }
        cl->addWidget(detBox);
    }

    // ── Footer: tags + buttons ────────────────────────────────────────────
    QWidget *footer = new QWidget();
    footer->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *fl = new QHBoxLayout(footer);
    fl->setContentsMargins(0, 4, 0, 0);
    fl->setSpacing(6);

    static const QStringList tagStyles = {
        "background:#edf7ee; color:#2f7d5c;",
        "background:#e2f4e8; color:#245f47;",
        "background:#f1f8e8; color:#557161;"
    };
    for (int i = 0; i < r.tags.size(); i++) {
        QLabel *t = new QLabel(r.tags[i]);
        t->setStyleSheet(
            QString("font-size:10px; font-weight:700; padding:3px 10px; "
                    "border-radius:11px; border:none; letter-spacing:0.2px; %1")
                .arg(tagStyles[i % tagStyles.size()]));
        fl->addWidget(t);
    }
    fl->addStretch();

    auto addUrlBtn = [&](const QString &url, const QString &label) {
        QPushButton *btn = new QPushButton(label);
        const QString u = url;
        QObject::connect(btn, &QPushButton::clicked, [u]() {
            openResourceTarget(u);
        });
        btn->setStyleSheet(
            "QPushButton { font-size:11px; font-weight:700; color:#2f7d5c; "
            "  background:#edf7ee; border:none; border-radius:10px; padding:6px 15px; }"
            "QPushButton:hover { background:#dcefe0; color:#245f47; }");
        btn->setCursor(Qt::PointingHandCursor);
        fl->addWidget(btn);
    };

    if (!r.url.isEmpty())  addUrlBtn(r.url, r.url.startsWith("qrc:/") ? "Open PDF →" : "Visit →");
    if (!r.url2.isEmpty()) addUrlBtn(r.url2, r.url2Label.isEmpty() ? "Open →" : r.url2Label);

    cl->addWidget(footer);
    return wrapper;
}

// Large clickable topic card for the landing page
static QWidget* makeTopicCard(const TopicInfo &t,
                              std::function<void()> onClick,
                              QLabel **subtitleLabelOut = nullptr) {
    QPushButton *card = new QPushButton();
    card->setMinimumHeight(104);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(
        "QPushButton {"
        "  border: 1px solid #dfece1;"
        "  border-radius: 22px;"
        "  background: #ffffff;"
        "  text-align: left;"
        "}"
        "QPushButton:hover {"
        "  border: 1px solid #b9d9c2;"
        "  background: #f7fff8;"
        "}"
        "QPushButton:pressed {"
        "  background: #edf7ee;"
        "}"
    );

    QHBoxLayout *hl = new QHBoxLayout(card);
    hl->setContentsMargins(24, 0, 24, 0);
    hl->setSpacing(18);

    // Icon badge
    QLabel *iconLbl = new QLabel(t.icon);
    iconLbl->setFixedSize(58, 58);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet(
        QString("background:%1; border-radius:18px; font-size:25px; border:none;").arg(t.accent));
    iconLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Text block
    QWidget *textBlock = new QWidget();
    textBlock->setStyleSheet("border:none; background:transparent;");
    textBlock->setAttribute(Qt::WA_TransparentForMouseEvents);
    QVBoxLayout *vl = new QVBoxLayout(textBlock);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(3);
    QLabel *titleLbl = new QLabel(t.title);
    titleLbl->setStyleSheet(
        "font-size:17px; font-weight:700; color:#173c2c; border:none; "
        "letter-spacing:-0.15px;");
    QLabel *subLbl = new QLabel(t.subtitle);
    subLbl->setWordWrap(true);
    subLbl->setStyleSheet("font-size:13px; color:#6d8272; border:none;");
    vl->addWidget(titleLbl);
    vl->addWidget(subLbl);

    if (subtitleLabelOut)
        *subtitleLabelOut = subLbl;

    // Arrow
    QLabel *arrow = new QLabel("→");
    arrow->setStyleSheet("font-size:20px; color:#9fbfa9; border:none;");
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
    scroll->setObjectName("screenSurface");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->viewport()->setObjectName("screenViewport");
    QWidget *inner = new QWidget();
    inner->setObjectName("screenSurface");
    QVBoxLayout *il = new QVBoxLayout(inner);
    il->setSpacing(12);
    il->setContentsMargins(0, 6, 6, 6);
    for (const ResourceInfo &r : RESOURCES[topicIndex])
        il->addWidget(makeResourceCard(r));
    il->addStretch();
    scroll->setWidget(inner);
    return scroll;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

// ── Activation hook ───────────────────────────────────────────────────────────
// Resets to the topic-picker home page whenever the user navigates to this
// screen. Overrides the virtual onActivated() from Screen.
void Recommendations::onActivated() {
    if (m_searchInput) {
        m_searchInput->clear();
        applyTopicSearch(QString{});
    }
    m_stack->setCurrentIndex(0);
}

void Recommendations::applyTopicSearch(const QString &query) {
    const QString normalized = normalizedText(query);

    if (normalized.isEmpty()) {
        for (QWidget *card : m_topicCards)
            card->setVisible(true);

        for (int i = 0; i < m_topicSubtitleLabels.size() && i < TOPICS.size(); ++i) {
            m_topicSubtitleLabels[i]->setText(TOPICS[i].subtitle);
            m_topicSubtitleLabels[i]->setStyleSheet(
                "font-size:13px; color:#6d8272; border:none;");
        }

        m_searchStatus->setText(
            "Type a topic like finance, immigration, family, or wellness.");
        m_searchStatus->setStyleSheet(
            "font-size:11px; color:#8a857c; border:none; background:transparent;");
        return;
    }

    int visibleCount = 0;
    int bestIndex = -1;
    int bestScore = -1;
    QString bestSubtopicSummary;

    for (int i = 0; i < m_topicCards.size(); ++i) {
        const TopicSearchResult result = searchTopic(i, normalized);
        const bool matches = result.score >= 0;
        m_topicCards[i]->setVisible(matches);

        if (i < m_topicSubtitleLabels.size()) {
            if (matches && !result.matchedSubtopics.isEmpty()) {
                m_topicSubtitleLabels[i]->setText(
                    QString("Subtopic match: %1").arg(summarizedSubtopics(result.matchedSubtopics)));
                m_topicSubtitleLabels[i]->setStyleSheet(
                    "font-size:13px; color:#2f7d5c; font-weight:600; border:none;");
            } else {
                m_topicSubtitleLabels[i]->setText(TOPICS[i].subtitle);
                m_topicSubtitleLabels[i]->setStyleSheet(
                    "font-size:13px; color:#6d8272; border:none;");
            }
        }

        if (matches) {
            visibleCount++;
            if (result.score > bestScore) {
                bestScore = result.score;
                bestIndex = i;
                bestSubtopicSummary = summarizedSubtopics(result.matchedSubtopics);
            }
        }
    }

    if (visibleCount == 0) {
        m_searchStatus->setText(
            "No topic matched that search. Try words like food, immigration, stress, or counseling.");
        m_searchStatus->setStyleSheet(
            "font-size:11px; color:#C62828; border:none; background:transparent;");
        return;
    }

    if (visibleCount == 1) {
        if (bestSubtopicSummary.isEmpty()) {
            m_searchStatus->setText("1 matching topic found. Press Enter to open it.");
        } else {
            m_searchStatus->setText(
                QString("1 matching topic found via subtopic: %1. Press Enter to open it.")
                    .arg(bestSubtopicSummary));
        }
    } else {
        if (bestSubtopicSummary.isEmpty()) {
            m_searchStatus->setText(
                QString("%1 matching topics found. Press Enter to open the best match.")
                    .arg(visibleCount));
        } else {
            m_searchStatus->setText(
                QString("%1 matching topics found. Best subtopic match: %2. Press Enter to open it.")
                    .arg(visibleCount)
                    .arg(bestSubtopicSummary));
        }
    }

    if (bestIndex >= 0) {
        m_searchStatus->setStyleSheet(
            "font-size:12px; color:#2f7d5c; border:none; background:transparent;");
    }
}

void Recommendations::openBestTopicMatch() {
    const QString query = m_searchInput ? m_searchInput->text() : QString{};
    const QString normalized = normalizedText(query);
    if (normalized.isEmpty()) return;

    int bestIndex = -1;
    int bestScore = -1;
    for (int i = 0; i < TOPICS.size(); ++i) {
        const TopicSearchResult result = searchTopic(i, normalized);
        if (result.score > bestScore) {
            bestScore = result.score;
            bestIndex = i;
        }
    }

    if (bestIndex >= 0)
        showTopic(bestIndex);
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Recommendations::Recommendations(QWidget *parent) : Screen("BMCC Resources", parent) {
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_stack = new QStackedWidget();
    m_stack->setObjectName("screenSurface");

    // ── Page 0: Landing / topic picker ───────────────────────────────────
    QScrollArea *homeScroll = new QScrollArea();
    homeScroll->setObjectName("screenSurface");
    homeScroll->setWidgetResizable(true);
    homeScroll->setFrameShape(QFrame::NoFrame);
    homeScroll->viewport()->setObjectName("screenViewport");

    QWidget *homePage = new QWidget();
    homePage->setObjectName("screenSurface");
    QVBoxLayout *hl = new QVBoxLayout(homePage);
    hl->setContentsMargins(56, 44, 56, 44);
    hl->setSpacing(14);

    QLabel *greeting = new QLabel("WELCOME");
    greeting->setStyleSheet(
        "font-size:11px; font-weight:700; color:#8ba493; "
        "letter-spacing:1.6px;");

    QLabel *heading = new QLabel("What are you stressed about?");
    heading->setStyleSheet(
        "font-size:34px; font-weight:700; color:#173c2c; "
        "letter-spacing:-0.5px;");

    QLabel *sub = new QLabel(
        "Choose a topic below and we'll show you what BMCC has available for you — "
        "tutoring, financial help, counseling, and more.");
    sub->setWordWrap(true);
    sub->setStyleSheet("font-size:15px; color:#4f6255; line-height:170%;");

    hl->addWidget(greeting);
    hl->addSpacing(2);
    hl->addWidget(heading);
    hl->addWidget(sub);

    QFrame *heroCard = new QFrame();
    heroCard->setStyleSheet(
        "QFrame { background:#eaf7ee; border:1px solid #cfe5d2; border-radius:24px; }");
    QHBoxLayout *heroLay = new QHBoxLayout(heroCard);
    heroLay->setContentsMargins(28, 24, 28, 24);
    heroLay->setSpacing(24);

    QWidget *heroTextBox = new QWidget();
    heroTextBox->setStyleSheet("border:none; background:transparent;");
    QVBoxLayout *heroTextLay = new QVBoxLayout(heroTextBox);
    heroTextLay->setContentsMargins(0, 0, 0, 0);
    heroTextLay->setSpacing(6);

    QLabel *heroTitle = new QLabel("Start with the stressor, not the office.");
    heroTitle->setStyleSheet(
        "font-size:20px; font-weight:700; color:#173c2c; border:none;");
    QLabel *heroBody = new QLabel(
        "MindEase narrows the list for you first, then surfaces direct links, office details, "
        "and support options without making you search across BMCC websites.");
    heroBody->setWordWrap(true);
    heroBody->setStyleSheet(
        "font-size:14px; color:#4f6255; border:none; line-height:170%;");
    heroTextLay->addWidget(heroTitle);
    heroTextLay->addWidget(heroBody);

    QWidget *chipRow = new QWidget();
    chipRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *chipLay = new QHBoxLayout(chipRow);
    chipLay->setContentsMargins(0, 0, 0, 0);
    chipLay->setSpacing(8);
    chipLay->addWidget(makeInfoChip("5 stress topics", "#ffffff", "#2f7d5c", "#b9d9c2"));
    chipLay->addWidget(makeInfoChip("Curated contacts", "#ffffff", "#245f47", "#cfe5d2"));
    chipLay->addWidget(makeInfoChip("Links open in browser", "#ffffff", "#557161", "#dfece1"));
    chipLay->addStretch();
    heroTextLay->addWidget(chipRow);

    QLabel *heroSide = new QLabel(
        "Need urgent help?\nIf you are in immediate danger, call 911 or contact 988.");
    heroSide->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    heroSide->setStyleSheet(
        "font-size:13px; font-weight:600; color:#245f47; background:#f7fff8; "
        "border:1px solid #cfe5d2; border-radius:18px; padding:16px 18px;");
    heroSide->setMinimumWidth(300);

    heroLay->addWidget(heroTextBox, 1);
    heroLay->addWidget(heroSide);

    hl->addSpacing(18);
    hl->addWidget(heroCard);
    hl->addSpacing(18);

    QLabel *chooserLabel = new QLabel("CHOOSE A SUPPORT PATH");
    chooserLabel->setObjectName("sectionLabel");
    hl->addWidget(chooserLabel);
    hl->addSpacing(4);

    QFrame *searchCard = new QFrame();
    searchCard->setStyleSheet(
        "QFrame { background:#ffffff; border:1px solid #dfece1; border-radius:22px; }");
    QVBoxLayout *searchOuter = new QVBoxLayout(searchCard);
    searchOuter->setContentsMargins(20, 18, 20, 16);
    searchOuter->setSpacing(10);

    QWidget *searchRow = new QWidget();
    searchRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *searchLay = new QHBoxLayout(searchRow);
    searchLay->setContentsMargins(0, 0, 0, 0);
    searchLay->setSpacing(10);

    QLabel *searchIcon = new QLabel("⌕");
    searchIcon->setAlignment(Qt::AlignCenter);
    searchIcon->setFixedSize(42, 42);
    searchIcon->setStyleSheet(
        "font-size:18px; color:#2f7d5c; background:#edf7ee; border:1px solid #cfe5d2; "
        "border-radius:14px;");

    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText(
        "Search by topic or keyword, e.g. finance, food, immigration, counseling");
    m_searchInput->setClearButtonEnabled(true);
    m_searchInput->setStyleSheet(
        "QLineEdit {"
        "  border:1px solid #b9d9c2;"
        "  border-radius:14px;"
        "  background:#f7fff8;"
        "  padding:12px 14px;"
        "  font-size:14px;"
        "  color:#173c2c;"
        "}"
        "QLineEdit:focus {"
        "  border:1px solid #2f7d5c;"
        "  background:#ffffff;"
        "}");

    QPushButton *searchBtn = new QPushButton("Search");
    searchBtn->setObjectName("primaryBtn");
    searchBtn->setMinimumHeight(46);
    searchBtn->setCursor(Qt::PointingHandCursor);

    searchLay->addWidget(searchIcon);
    searchLay->addWidget(m_searchInput, 1);
    searchLay->addWidget(searchBtn);

    m_searchStatus = new QLabel();
    m_searchStatus->setWordWrap(true);

    searchOuter->addWidget(searchRow);
    searchOuter->addWidget(m_searchStatus);
    hl->addWidget(searchCard);
    hl->addSpacing(8);

    connect(m_searchInput, &QLineEdit::textChanged,
            this, &Recommendations::applyTopicSearch);
    connect(m_searchInput, &QLineEdit::returnPressed,
            this, &Recommendations::openBestTopicMatch);
    connect(searchBtn, &QPushButton::clicked,
            this, &Recommendations::openBestTopicMatch);

    QWidget *topicList = new QWidget();
    topicList->setStyleSheet("border:none; background:transparent;");
    QVBoxLayout *topicListLay = new QVBoxLayout(topicList);
    topicListLay->setContentsMargins(0, 0, 0, 0);
    topicListLay->setSpacing(10);

    for (int i = 0; i < TOPICS.size(); i++) {
        int idx = i;
        QLabel *subtitleLabel = nullptr;
        QWidget *card = makeTopicCard(
            TOPICS[i],
            [this, idx]() { showTopic(idx); },
            &subtitleLabel);
        m_topicCards.append(card);
        m_topicSubtitleLabels.append(subtitleLabel);
        topicListLay->addWidget(card);
    }
    topicListLay->addStretch();
    hl->addWidget(topicList);
    hl->addStretch();

    homeScroll->setWidget(homePage);
    m_stack->addWidget(homeScroll); // index 0

    applyTopicSearch(QString{});

    // ── Pages 1–5: Resource detail pages ─────────────────────────────────
    for (int i = 0; i < TOPICS.size(); i++) {
        QWidget *page = new QWidget();
        page->setObjectName("screenSurface");
        QVBoxLayout *pl = new QVBoxLayout(page);
        pl->setContentsMargins(48, 36, 48, 36);
        pl->setSpacing(18);

        // Back button + topic title row
        QWidget *topRow = new QWidget();
        topRow->setStyleSheet("border:none;");
        QHBoxLayout *trl = new QHBoxLayout(topRow);
        trl->setContentsMargins(0, 0, 0, 0);
        trl->setSpacing(12);

        QPushButton *backBtn = new QPushButton("←  Back");
        backBtn->setStyleSheet(
            "QPushButton { font-size:13px; font-weight:600; color:#2f7d5c; "
            "  background:transparent; border:1px solid #b9d9c2; "
            "  border-radius:12px; padding:8px 16px; }"
            "QPushButton:hover { background:#edf7ee; border-color:#2f7d5c; }");
        backBtn->setCursor(Qt::PointingHandCursor);
        connect(backBtn, &QPushButton::clicked, this, &Recommendations::showHome);

        QLabel *iconLbl = new QLabel(TOPICS[i].icon);
        iconLbl->setFixedSize(40, 40);
        iconLbl->setAlignment(Qt::AlignCenter);
        iconLbl->setStyleSheet(
            QString("background:%1; border-radius:10px; font-size:20px; border:none;")
                .arg(TOPICS[i].accent));

        QLabel *titleLbl = new QLabel(TOPICS[i].title);
        titleLbl->setStyleSheet(
            "font-size:23px; font-weight:700; color:#173c2c; border:none; "
            "letter-spacing:-0.3px;");

        trl->addWidget(backBtn);
        trl->addSpacing(4);
        trl->addWidget(iconLbl);
        trl->addWidget(titleLbl, 1);

        QFrame *div = new QFrame();
        div->setFrameShape(QFrame::HLine);
        div->setStyleSheet("color:#ece9e2;");

        QLabel *sectionLbl = new QLabel("AVAILABLE AT BMCC");
        sectionLbl->setObjectName("sectionLabel");

        pl->addWidget(topRow);
        pl->addWidget(div);
        pl->addSpacing(4);

        QFrame *summary = new QFrame();
        summary->setStyleSheet(
            "QFrame { background:#ffffff; border:1px solid #dfece1; border-radius:18px; }");
        QVBoxLayout *sl = new QVBoxLayout(summary);
        sl->setContentsMargins(16, 14, 16, 14);
        sl->setSpacing(8);

        QLabel *summaryText = new QLabel(
            "This section gathers the most relevant BMCC and trusted external support options "
            "for this topic, including direct links, locations, hours, and contact details.");
        summaryText->setWordWrap(true);
        summaryText->setStyleSheet(
            "font-size:13px; color:#4f6255; border:none; line-height:170%;");

        QWidget *metaRow = new QWidget();
        metaRow->setStyleSheet("border:none; background:transparent;");
        QHBoxLayout *ml = new QHBoxLayout(metaRow);
        ml->setContentsMargins(0, 0, 0, 0);
        ml->setSpacing(8);
        ml->addWidget(makeInfoChip(QString("%1 resources").arg(RESOURCES[i].size()),
                                   "#edf7ee", "#2f7d5c", "#cfe5d2"));
        ml->addWidget(makeInfoChip(TOPICS[i].subtitle, "#f7fff8", "#557161", "#dfece1"));
        ml->addStretch();

        sl->addWidget(summaryText);
        sl->addWidget(metaRow);

        pl->addWidget(summary);
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
