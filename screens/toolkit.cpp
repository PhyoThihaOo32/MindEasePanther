#include "screens/toolkit.h"
#include "core/screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QSizePolicy>
#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>
#include <QUrl>
#include <QColor>

// ─────────────────────────────────────────────────────────────────────────────
// Data structures
// ─────────────────────────────────────────────────────────────────────────────

struct ToolItem {
    QString icon;
    QString title;
    QString body;
    QString url;
    QString urlLabel;
    QString url2 = {};
    QString url2Label = {};
};

struct FolderDef {
    QString id;
    QString icon;
    QString accent;      // light fill color
    QString accentBorder; // slightly darker border color when active
    QString name;
    QString tagline;
    QString intro;
    QList<ToolItem> items;
};

// ─────────────────────────────────────────────────────────────────────────────
// Folder definitions
// ─────────────────────────────────────────────────────────────────────────────

static const QList<FolderDef> FOLDERS = {

    {   "academic", "📚", "#dcfff1", "#8ee2a8",
        "Academic Stress",
        "Stay focused, manage deadlines, keep your cool during exams.",
        "School can be tough, but your well-being doesn't have to be! Get tips for staying "
        "focused, managing deadlines, and keeping your cool during exams. Here you'll find "
        "some of our favorite resources to help navigate school-related stressors and succeed "
        "throughout the semester.",
        {
            { "🗓️", "Weekly Planner",
              "Break tasks into manageable chunks so you always know what to prioritize. "
              "A weekly planner reduces semester overwhelm by giving you a clear, day-by-day roadmap.",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/your-mental-health-toolkit/",
              "Open Toolkit →" },
            { "🧩", "Procrastination Worksheet",
              "Identify what's holding you back and create actionable steps to move forward. "
              "Pinpoint your procrastination triggers and build a plan that actually works for you.",
              "https://freetodolist.com/", "Open Worksheet →" },
            { "☕", "Smart Study Break Ideas",
              "• 5-min refresh: stretch or take a short walk\n"
              "• Creative break: doodle or free-write in a journal\n"
              "• Snack smart: fruit, nuts, or a protein bar\n"
              "• Mindful moment: try a 5-minute guided meditation\n"
              "• Social recharge: check in with a friend or study buddy", "", "" },
            { "🏫", "BMCC Academic Support",
              "Free tutoring, Writing Center, Academic Coaching, library quiet spaces, "
              "and the Study Space Guide — all on campus, free for every student.",
              "https://www.bmcc.cuny.edu/learning-assistance/", "Visit →" },
            { "🧠", "BMCC Counseling — Academic Stress",
              "Counselors help with stress reduction, time management, and academic success strategies. "
              "Free and confidential for all enrolled students.\n"
              "📍 Room S-343   📞 (212) 220-8140",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/academics/", "Book Appointment →" },
        }
    },

    {   "connection", "🤝", "#e7fbff", "#8bdff2",
        "Connection & Community",
        "Groups, clubs, peer support, and NYC mental health resources.",
        "You don't have to face college alone. Whether you're looking for a supportive group, "
        "want to meet new people, or need a community that understands you — BMCC and New York "
        "City have you covered. Strong social connections are one of the best protections for mental health.",
        {
            { "🫂", "Counseling Groups & Workshops",
              "CBT 4 Anxiety, Grown Folx, Queer Connections — safe spaces to share experiences, "
              "learn new coping skills, and tackle challenges together. Single-session workshops "
              "are also available throughout the semester.",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/groups-and-workshops/groups/",
              "View Groups →" },
            { "📅", "Upcoming Workshops",
              "Drop-in and pop-up workshops on stress management, art-based stress relief, "
              "mindfulness, and more — check what's coming up this semester.",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/groups-and-workshops/workshops/",
              "View Schedule →" },
            { "🎓", "Student Clubs & Activities",
              "Join or start a club to build social connections and meet people who share your "
              "interests. BMCC has clubs for every interest — academic, cultural, and recreational.",
              "https://www.bmcc.cuny.edu/student-affairs/student-activities/clubs/",
              "Find a Club →" },
            { "📞", "NYC Crisis Resources (24/7)",
              "Hotlines and texting services for emergency mental health support: suicidal feelings, "
              "depression, substance use, and more. Available 24 hours a day, 7 days a week.",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/counseling-resources/nyc-resources/",
              "See All Hotlines →" },
            { "🌐", "Community Referrals",
              "Outside referrals for housing support, domestic assistance, legal aid, and substance "
              "abuse services — curated by BMCC's Counseling Center.",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/counseling-resources/community-resources/",
              "Visit →" },
        }
    },

    {   "identity", "🌈", "#fbf4d0", "#f1e8ad",
        "Empowered Identity & Care",
        "Racial trauma, LGBTQIA+, and disability justice resources.",
        "Your identity is your strength. This section offers affirming resources for students "
        "navigating racial trauma, LGBTQIA+ experiences, disability, and intersecting identities. "
        "You belong here — and support is available for every part of who you are.",
        {
            { "✊", "Racial Trauma Resources",
              "Validates experiences of racial trauma, inter-generational trauma, and repeated "
              "racism. Resources to learn and heal at your own pace — because healing is not linear.",
              "https://www.bmcc.cuny.edu/about-bmcc/race-equity-and-inclusion/resources/racial-trauma-resources/",
              "Open BMCC Resources →" },
            { "📖", "Anti-Racist Resources",
              "Articles, TED Talks, and infographics to help you become a more effective ally "
              "and deepen your understanding of anti-racism and systemic inequity.", "", "" },
            { "🏳️‍🌈", "LGBTQIA+ Support & Hotlines",
              "An affirming space for all sexual orientations, gender identities, and expressions.\n\n"
              "Trans Lifeline (peer support): (877) 565-8860\n"
              "LGBT National Hotline: (888) 843-4564\n"
              "Trevor Lifeline (youth crisis): (866) 488-7386", "", "" },
            { "♿", "Disability Justice — 10 Principles",
              "A framework for understanding disability in relation to race, gender, and other "
              "intersecting identities. Rooted in justice, not just accommodation.",
              "https://www.sinsinvalid.org/blog/10-principles-of-disability-justice", "Read →" },
            { "💬", "BMCC Counseling — Identity Support",
              "BMCC counselors provide affirming, culturally-competent support for all students. "
              "Free and confidential.\n📍 Room S-343   📞 (212) 220-8140",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/counseling-services/", "Visit →" },
        }
    },

    {   "gratitude", "✏️", "#fbf4d0", "#f1e8ad",
        "Gratitude & Journaling",
        "Prompts, 7-day challenges, and science-backed practices.",
        "Gratitude and journaling are powerful, research-backed tools for shifting your mindset "
        "and improving well-being. Even just a few minutes a day can enhance self-esteem, nurture "
        "relationships, and boost life satisfaction. Start small — no perfect writing required.",
        {
            { "🔬", "Why It Works",
              "Daily gratitude practice is linked to lower stress, improved sleep, stronger "
              "relationships, and greater resilience. Writing it down makes it stick far more "
              "than just thinking it.", "", "" },
            { "📝", "Journal Prompts to Try Today",
              "1. What are three things that happened today that brought you peace?\n"
              "2. What went well this week, even if something else was hard?\n"
              "3. What everyday object near you are you grateful for?\n"
              "4. Who made a positive difference in your life this month?", "", "" },
            { "🗓️", "7-Day Gratitude Challenge",
              "Each day includes a small guided activity to help you reflect on what you're "
              "thankful for. By the end of the week you'll likely notice a real shift in mood.",
              "https://www.bmcc.cuny.edu/student-affairs/counseling/your-mental-health-toolkit/",
              "Open Toolkit →" },
            { "📚", "Recommended Reading",
              "The Artist's Way — Julia Cameron\n"
              "The Great Book of Journaling — Eric Maisel\n"
              "Thanks! How the New Science of Gratitude Can Make You Happier — Robert Emmons", "", "" },
        }
    },

    {   "mindfulness", "🧘", "#dcfff1", "#8ee2a8",
        "Mindfulness & Meditation",
        "Guided meditations, breathing exercises, and lovingkindness.",
        "Mindfulness is simply paying attention — on purpose, in the present moment, without "
        "judgment. It doesn't require hours of practice. Even 5 minutes a day can reduce anxiety, "
        "improve focus, and help you feel more grounded. Start wherever you are.",
        {
            { "💛", "Lovingkindness Meditation",
              "A complete guide with 6-, 12-, and 20-minute recorded sessions. Learn what it is, "
              "how to deal with challenges, and how to get started — even if you've never meditated.",
              "https://www.elliemillertherapy.com/blog/loving-kindness-meditation-a-complete-guide",
              "Open Guide →" },
            { "🌿", "Calm — Free Guided Meditations",
              "YouTube guided meditations from Calm, tailored to stress, anxiety, focus, and "
              "sleep difficulty. No subscription needed.",
              "https://www.youtube.com/@calm", "Watch on YouTube →" },
            { "🎧", "Headspace Meditation Classes",
              "Structured meditation courses for beginners and experienced practitioners. "
              "Topics include stress, sleep, focus, and anxiety.",
              "https://www.headspace.com/meditation", "Visit →" },
            { "▶️", "Free YouTube Guided Meditation",
              "A popular open-access guided meditation — no account, no cost. "
              "Great starting point for first-timers.",
              "https://www.youtube.com/watch?v=8x-QmEB4REg", "Watch →" },
            { "📖", "Verywell Mind — Meditation Library",
              "A curated library of guided meditations for anxiety, sleep, grief, and more, "
              "with clear explanations of each technique.",
              "https://www.verywellmind.com/guided-meditations-for-anxiety-stress-and-more-4692781",
              "Visit →" },
        }
    },

    {   "music", "🎵", "#dff6ff", "#8bdff2",
        "Music for Your Mind",
        "Curated playlists for studying, relaxing, and deep focus.",
        "Music is one of the fastest ways to shift your mood and mental state. Whether you need "
        "to focus, unwind, or fall asleep, the right playlist makes a real difference. "
        "Here are some favorites — try them and find what works for you.",
        {
            { "😴", "Calming & Sleep Music",
              "For winding down, reducing anxiety, or falling asleep faster.",
              "https://www.youtube.com/watch?v=1ZYbU82GVz4", "YouTube: Soothing Sleep Music →" },
            { "☕", "Study & Focus Music",
              "Background music for deep concentration — lo-fi beats, coffee shop ambiance, and jazz.",
              "https://www.youtube.com/watch?v=5qap5aO4i9A", "YouTube: Lo-Fi Hip Hop →" },
            { "🎵", "Spotify — Peaceful Piano",
              "Calm instrumental playlist for relaxation and stress relief. Works for studying too.",
              "https://open.spotify.com/playlist/37i9dQZF1DX3rxVfibe1L0",
              "Open Spotify →" },
            { "🌿", "Lofi Station",
              "A clean, simple lofi music station for studying, relaxing, and staying focused.",
              "https://www.lofi.cafe/", "Open Lofi Station →" },
        }
    },

    {   "nutrition", "🥗", "#e9fff3", "#8ee2a8",
        "Nutrition & Energy",
        "Fuel your brain, campus fitness, and on-campus food resources.",
        "What you eat and how you move directly affects your mood, focus, and energy levels. "
        "Small changes — swapping sugary drinks for water or adding a short walk to your day — "
        "can make a significant difference in how you feel throughout the semester.",
        {
            { "🧠", "Food & Your Mind",
              "Limit sugar — high-sugar diets are linked to increased risk of depression and poor "
              "concentration. Healthy quick energy: green tea or water.\n\n"
              "Brain-boosting foods: leafy greens, small oily fish (sardines, mackerel), beans, "
              "lentils, yogurt, and cheese all support brain function and mood stability.", "", "" },
            { "🍎", "Panther Pantry (Free, On Campus)",
              "Nutritionally balanced non-perishable, refrigerated, and frozen food items for "
              "students facing food insecurity. Walk in — no questions asked, no appointment needed.\n"
              "📍 Room S-230   🕐 Mon–Fri 8am–6:30pm  |  Sat 10am–1pm",
              "https://www.bmcc.cuny.edu/student-affairs/arc/panther-pantry/", "Visit →" },
            { "🏋️", "Campus Fitness",
              "On campus: Fitness Center, Swimming Pool, and Health & Wellness Services — "
              "accessible exercise options for every student.\n\n"
              "CONTACT: BMCC Athletics\n"
              "📞 (212) 220-8260",
              "https://bmccathletics.com/sports/2012/4/2/RecreationSchedule.aspx",
              "Schedule →",
              "https://www.bmcc.cuny.edu/students/fitness-center/",
              "Visit →" },
            { "🌳", "NYC Free Outdoor Fitness",
              "Free outdoor fitness classes in NYC parks, adaptive programs for students with "
              "disabilities, and fitness meetup groups across the city.",
              "https://www.nycgovparks.org/programs/recreation/free-fitness-classes",
              "NYC Parks →" },
        }
    },

    {   "sleep", "🌙", "#e7fbff", "#8bdff2",
        "Sleep & Relaxation",
        "Sleep calculators, proven tips, and how to stop doom-scrolling.",
        "Sleep is one of the most powerful tools for mental and physical health — and one of "
        "the most overlooked by college students. Poor sleep affects mood, memory, focus, and "
        "immune function. Here are practical, evidence-based resources to help you rest better.",
        {
            { "⏰", "Sleep Calculator",
              "Find your ideal bedtime based on your wake-up time. Sleeping in complete "
              "90-minute cycles maximizes how restored you feel in the morning.",
              "https://sleepeducation.org/healthy-sleep/bedtime-calculator/", "Calculate →" },
            { "📘", "Young Adult Sleep Toolkit",
              "Covers sleep cycles, healthy bedtime routines, and how to optimize rest. "
              "From the National Sleep Foundation — helpful for any age.",
              "https://www.thensf.org/sleep-in-adolescents/", "Visit →" },
            { "💡", "Tips for Better Sleep",
              "• Exercise by day → sleep deeper at night\n"
              "• Keep consistent mealtimes to signal your body when to rest\n"
              "• Morning light exposure sets your circadian rhythm\n"
              "• Avoid bright screens 30–60 minutes before bed\n"
              "• Keep your bedroom cool, dark, and quiet",
              "https://www.sleepfoundation.org/sleep-hygiene", "National Sleep Foundation →" },
            { "📵", "Stop Doom-Scrolling",
              "Blue light from screens suppresses melatonin and delays sleep onset. "
              "Put your phone face-down 30 minutes before bed. Try a phone-free wind-down: "
              "read a book, stretch, or listen to calming audio instead.", "", "" },
            { "🎧", "Relaxation Audio",
              "Rain sounds, white noise, or guided sleep meditations can help quiet a racing mind "
              "and ease the transition into sleep.",
              "https://www.youtube.com/watch?v=1ZYbU82GVz4", "YouTube: Sleep Music →" },
        }
    },
};

static QLabel* makeToolkitChip(const QString &text,
                               const QString &bg = "#082f49",
                               const QString &fg = "#f1e8ad",
                               const QString &border = "#8bdff2") {
    QLabel *chip = new QLabel(text);
    chip->setStyleSheet(
        QString("font-size:11px; font-weight:600; color:%1; background:%2; "
                "border:1px solid %3; border-radius:12px; padding:5px 10px;")
            .arg(fg, bg, border));
    return chip;
}

static void applyToolkitShadow(QWidget *widget,
                               int blur = 24,
                               int yOffset = 6,
                               const QColor &color = QColor(142, 226, 168, 28)) {
    auto *shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(blur);
    shadow->setOffset(0, yOffset);
    shadow->setColor(color);
    widget->setGraphicsEffect(shadow);
}

// ─────────────────────────────────────────────────────────────────────────────
// UI builders
// ─────────────────────────────────────────────────────────────────────────────

static QWidget* makeToolItem(const ToolItem &item) {
    QFrame *row = new QFrame();
    row->setStyleSheet("QFrame { border:none; background:transparent; }");
    QHBoxLayout *hl = new QHBoxLayout(row);
    hl->setContentsMargins(0, 10, 0, 10);
    hl->setSpacing(14);

    // Icon badge
    QLabel *iconLbl = new QLabel(item.icon);
    iconLbl->setFixedSize(40, 40);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet(
        "font-size:19px; background:#e7fbff; border:1px solid #8bdff2; border-radius:12px;");

    // Text block
    QWidget *textBlock = new QWidget();
    textBlock->setStyleSheet("background:transparent; border:none;");
    QVBoxLayout *vl = new QVBoxLayout(textBlock);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(5);

    QLabel *titleLbl = new QLabel(item.title);
    titleLbl->setStyleSheet(
        "font-size:14px; font-weight:800; color:#173c2c; border:none; "
        "letter-spacing:-0.1px;");
    vl->addWidget(titleLbl);

    if (!item.body.isEmpty()) {
        QLabel *bodyLbl = new QLabel(item.body);
        bodyLbl->setWordWrap(true);
        bodyLbl->setStyleSheet(
            "font-size:13px; color:#4f6255; border:none; line-height:168%;");
        vl->addWidget(bodyLbl);
    }

    if (!item.url.isEmpty() || !item.url2.isEmpty()) {
        QWidget *buttonRow = new QWidget();
        buttonRow->setStyleSheet("background:transparent; border:none;");
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonRow);
        buttonLayout->setContentsMargins(0, 2, 0, 0);
        buttonLayout->setSpacing(8);

        auto addLinkButton = [&](const QString &url, const QString &label) {
            if (url.isEmpty())
                return;

            QPushButton *linkBtn = new QPushButton(label.isEmpty() ? "Visit →" : label);
            const QString u = url;
            QObject::connect(linkBtn, &QPushButton::clicked, [u]() {
                QDesktopServices::openUrl(QUrl(u));
            });
            linkBtn->setStyleSheet(
                "QPushButton { font-size:12px; font-weight:800; color:#06172a; "
                "  background:qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                "                              stop:0 #8ee2a8, stop:0.55 #8bdff2, stop:1 #f1e8ad);"
                "  border:1px solid #b8ffda; border-radius:11px; "
                "  padding:6px 14px; margin-top:3px; }"
                "QPushButton:hover { background:#8bdff2; color:#06172a; border-color:#f1e8ad; }");
            linkBtn->setCursor(Qt::PointingHandCursor);
            linkBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            buttonLayout->addWidget(linkBtn);
        };

        addLinkButton(item.url, item.urlLabel);
        addLinkButton(item.url2, item.url2Label);
        buttonLayout->addStretch();
        vl->addWidget(buttonRow);
    }

    hl->addWidget(iconLbl, 0, Qt::AlignTop);
    hl->addWidget(textBlock, 1);
    return row;
}

static QFrame* makeDivider() {
    QFrame *f = new QFrame();
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet("background:#dfece1; border:none;");
    f->setFixedHeight(1);
    return f;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

// ── Activation hook ───────────────────────────────────────────────────────────
// Called by MainWindow when the user switches to this screen.
// Overrides the virtual onActivated() from Screen.
void Toolkit::onActivated() {
    // Close the open folder so the user always lands on a clean accordion view.
    if (!openFolder.isEmpty())
        toggleFolder(openFolder);   // toggleFolder treats an already-open id as close
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Toolkit::Toolkit(QWidget *parent) : Screen("Mental Health Toolkit", parent) {
    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(56, 42, 56, 42);
    main->setSpacing(22);

    // Shared header (title label + divider) via the Screen base-class utility.
    buildHeader(main);

    // Intro banner
    QFrame *banner = new QFrame();
    banner->setStyleSheet(
        "QFrame { background:qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                                  stop:0 #082f49, stop:0.58 #0f5a53, stop:1 #173f30);"
        "         border:1px solid #8bdff2; border-radius:28px; }");
    applyToolkitShadow(banner, 28, 8, QColor(142, 226, 168, 30));
    QHBoxLayout *bannerLay = new QHBoxLayout(banner);
    bannerLay->setContentsMargins(32, 26, 32, 26);
    bannerLay->setSpacing(22);
    QLabel *bannerIcon = new QLabel("🌱");
    bannerIcon->setStyleSheet("font-size:30px; border:none; background:transparent;");
    QLabel *bannerText = new QLabel(
        "Taking care of your well-being is just as important as acing that next exam. "
        "Click any folder below to explore tools, tips, and resources curated for BMCC students.");
    bannerText->setWordWrap(true);
    bannerText->setStyleSheet(
        "font-size:15px; color:#d7fff1; border:none; background:transparent; "
        "line-height:170%;");
    bannerLay->addWidget(bannerIcon, 0, Qt::AlignTop);
    bannerLay->addWidget(bannerText, 1);
    main->addWidget(banner);

    QWidget *metaRow = new QWidget();
    metaRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *metaLay = new QHBoxLayout(metaRow);
    metaLay->setContentsMargins(0, 0, 0, 0);
    metaLay->setSpacing(8);
    metaLay->addWidget(makeToolkitChip("8 self-care folders", "#0b2a3c", "#f1e8ad", "#8bdff2"));
    metaLay->addWidget(makeToolkitChip("Official BMCC-based content", "#0b2a3c", "#bdeee5", "#8ee2a8"));
    metaLay->addWidget(makeToolkitChip("External links open in browser", "#0b2a3c", "#d7fff1", "#8bdff2"));
    metaLay->addStretch();
    main->addWidget(metaRow);

    // Section label above the folder list
    QLabel *listLabel = new QLabel("TOOLBOX");
    listLabel->setObjectName("sectionLabel");
    main->addSpacing(2);
    main->addWidget(listLabel);

    // Scrollable folder accordion
    QScrollArea *scroll = new QScrollArea();
    scroll->setObjectName("screenSurface");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->viewport()->setObjectName("screenViewport");
    QWidget *inner = new QWidget();
    inner->setObjectName("screenSurface");
    QVBoxLayout *il = new QVBoxLayout(inner);
    il->setContentsMargins(2, 2, 10, 8);
    il->setSpacing(10);

    for (const FolderDef &fd : FOLDERS) {

        // ── Folder card button ────────────────────────────────────────────
        QPushButton *card = new QPushButton();
        card->setMinimumHeight(112);
        card->setCursor(Qt::PointingHandCursor);
        card->setStyleSheet(
            "QPushButton {"
            "  border: 1px solid #8bdff2;"
            "  border-radius: 24px;"
            "  background: qradialgradient(cx:0.18, cy:0.12, radius:1.15,"
            "                              fx:0.18, fy:0.12,"
            "                              stop:0 #dff8ff, stop:0.38 #ffffff,"
            "                              stop:0.72 #f8ffff, stop:1 #e7fbff);"
            "  text-align: left;"
            "}"
            "QPushButton:hover {"
            "  border: 1px solid #f1e8ad;"
            "  background: #f1fffb;"
            "}");
        applyToolkitShadow(card, 22, 6, QColor(139, 223, 242, 22));

        QHBoxLayout *cardLay = new QHBoxLayout(card);
        cardLay->setContentsMargins(26, 14, 26, 14);
        cardLay->setSpacing(20);

        // Icon badge
        QLabel *iconBadge = new QLabel(fd.icon);
        iconBadge->setFixedSize(56, 56);
        iconBadge->setAlignment(Qt::AlignCenter);
        iconBadge->setStyleSheet(
            QString("background:%1; border-radius:18px; font-size:24px; border:none;")
                .arg(fd.accent));
        iconBadge->setAttribute(Qt::WA_TransparentForMouseEvents);

        // Name + tagline
        QWidget *cardText = new QWidget();
        cardText->setStyleSheet("border:none; background:transparent;");
        cardText->setAttribute(Qt::WA_TransparentForMouseEvents);
        QVBoxLayout *cardTextLay = new QVBoxLayout(cardText);
        cardTextLay->setContentsMargins(0, 0, 0, 0);
        cardTextLay->setSpacing(3);
        QLabel *nameLbl = new QLabel(fd.name);
        nameLbl->setStyleSheet(
            "font-size:18px; font-weight:800; color:#173c2c; border:none; "
            "letter-spacing:-0.15px;");
        QLabel *tagLbl = new QLabel(fd.tagline);
        tagLbl->setWordWrap(true);
        tagLbl->setStyleSheet("font-size:13px; color:#6d8272; border:none;");
        cardTextLay->addWidget(nameLbl);
        cardTextLay->addWidget(tagLbl);

        // Chevron
        QLabel *chev = new QLabel("›");
        chev->setFixedWidth(20);
        chev->setAlignment(Qt::AlignCenter);
        chev->setStyleSheet("font-size:28px; color:#8bdff2; border:none;");
        chev->setAttribute(Qt::WA_TransparentForMouseEvents);

        cardLay->addWidget(iconBadge);
        cardLay->addWidget(cardText, 1);
        cardLay->addWidget(chev);

        // ── Expandable panel ──────────────────────────────────────────────
        QFrame *panel = new QFrame();
        panel->setStyleSheet(
            QString("QFrame {"
                    "  border: 1px solid %1;"
                    "  border-top: none;"
                    "  border-radius: 0 0 24px 24px;"
                    "  background: qradialgradient(cx:0.18, cy:0.12, radius:1.15,"
                    "                              fx:0.18, fy:0.12,"
                    "                              stop:0 #dff8ff, stop:0.38 #ffffff,"
                    "                              stop:0.72 #f8ffff, stop:1 #e7fbff);"
                    "}").arg(fd.accentBorder));
        panel->setVisible(false);

        QVBoxLayout *panelLay = new QVBoxLayout(panel);
        panelLay->setContentsMargins(24, 20, 24, 24);
        panelLay->setSpacing(0);

        // Intro box
        QFrame *introBox = new QFrame();
        introBox->setStyleSheet(
            QString("QFrame { background:%1; border-radius:14px; border:1px solid #dfece1; }")
                .arg(fd.accent));
        QVBoxLayout *introLay = new QVBoxLayout(introBox);
        introLay->setContentsMargins(16, 14, 16, 14);
        QLabel *introLbl = new QLabel(fd.intro);
        introLbl->setWordWrap(true);
        introLbl->setStyleSheet(
            "font-size:13px; color:#0b2a3c; border:none; background:transparent; "
            "line-height:170%;");
        introLay->addWidget(introLbl);
        panelLay->addWidget(introBox);
        panelLay->addSpacing(10);

        // Tool items with dividers
        for (int j = 0; j < fd.items.size(); j++) {
            if (j > 0) panelLay->addWidget(makeDivider());
            panelLay->addWidget(makeToolItem(fd.items[j]));
        }

        // Store references
        folderCards[fd.id] = card;
        panels[fd.id]      = panel;
        chevrons[fd.id]    = chev;

        // Connect click — capture by value to avoid dangling refs
        QString fid = fd.id;
        connect(card, &QPushButton::clicked, this, [this, fid]() {
            toggleFolder(fid);
        });

        il->addWidget(card);
        il->addWidget(panel);
    }

    il->addStretch();
    scroll->setWidget(inner);
    main->addWidget(scroll);
}

// ─────────────────────────────────────────────────────────────────────────────
// Toggle — fixes chevron reset bug by iterating all stored chevrons
// ─────────────────────────────────────────────────────────────────────────────

void Toolkit::toggleFolder(const QString &id) {
    const bool wasOpen = (openFolder == id);

    // ── Close everything ──────────────────────────────────────────────────
    for (auto it = panels.begin(); it != panels.end(); ++it)
        it.value()->setVisible(false);

    for (auto it = folderCards.begin(); it != folderCards.end(); ++it) {
        if (auto *btn = qobject_cast<QPushButton*>(it.value()))
            btn->setStyleSheet(
                "QPushButton {"
                "  border: 1px solid #8bdff2;"
                "  border-radius: 24px;"
                "  background: qradialgradient(cx:0.18, cy:0.12, radius:1.15,"
                "                              fx:0.18, fy:0.12,"
                "                              stop:0 #dff8ff, stop:0.38 #ffffff,"
                "                              stop:0.72 #f8ffff, stop:1 #e7fbff);"
                "  text-align: left;"
                "}"
                "QPushButton:hover {"
                "  border: 1px solid #f1e8ad;"
                "  background: #f1fffb;"
                "}");
    }

    // ── Reset ALL chevrons (fixes the stuck-chevron bug) ──────────────────
    for (auto it = chevrons.begin(); it != chevrons.end(); ++it)
        it.value()->setText("›");

    openFolder = "";

    if (wasOpen) return; // was open — just close it

    // ── Open selected folder ──────────────────────────────────────────────
    openFolder = id;

    // Find this folder's accent colors
    QString accent       = "#e7fbff";
    QString accentBorder = "#8bdff2";
    for (const FolderDef &fd : FOLDERS) {
        if (fd.id == id) { accent = fd.accent; accentBorder = fd.accentBorder; break; }
    }

    if (auto *btn = qobject_cast<QPushButton*>(folderCards.value(id)))
        btn->setStyleSheet(
            QString("QPushButton {"
                    "  border: 1px solid %1;"
                    "  border-bottom: none;"
                    "  border-radius: 24px 24px 0 0;"
                    "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                    "                              stop:0 %2, stop:1 #e7fbff);"
                    "  text-align: left;"
                    "}").arg(accentBorder, accent));

    if (chevrons.contains(id))
        chevrons[id]->setText("∨");

    if (panels.contains(id))
        panels[id]->setVisible(true);
}
