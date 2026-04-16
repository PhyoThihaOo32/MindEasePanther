#include "toolkit.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QPushButton>
#include <QSizePolicy>
#include <QDesktopServices>
#include <QUrl>

// ─────────────────────────────────────────────────────────────────────────────
// Data structures
// ─────────────────────────────────────────────────────────────────────────────

struct ToolItem {
    QString icon;
    QString title;
    QString body;
    QString url;
    QString urlLabel;
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

    {   "academic", "📚", "#EEEDFE", "#7C74E0",
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
              "Pinpoint your procrastination triggers and build a plan that actually works for you.", "", "" },
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

    {   "connection", "🤝", "#E1F5EE", "#34A87A",
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

    {   "identity", "🌈", "#FCE4EC", "#D4607A",
        "Empowered Identity & Care",
        "Racial trauma, LGBTQIA+, and disability justice resources.",
        "Your identity is your strength. This section offers affirming resources for students "
        "navigating racial trauma, LGBTQIA+ experiences, disability, and intersecting identities. "
        "You belong here — and support is available for every part of who you are.",
        {
            { "✊", "Racial Trauma Resources",
              "Validates experiences of racial trauma, inter-generational trauma, and repeated "
              "racism. Resources to learn and heal at your own pace — because healing is not linear.", "", "" },
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

    {   "gratitude", "✏️", "#FFF8E1", "#D4A017",
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

    {   "mindfulness", "🧘", "#E0F7FA", "#0097A7",
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

    {   "music", "🎵", "#F3E5F5", "#8E44AD",
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
            { "🍎", "Apple Music Playlists",
              "Apple Music Chill — curated for unwinding after a long day.\n"
              "Apple Music Studying — engineered for sustained focus and productivity.", "", "" },
            { "🎶", "Amazon Music",
              "Calming Music and Study Beats playlists available through Amazon Music on any device.", "", "" },
        }
    },

    {   "nutrition", "🥗", "#E8F5E9", "#2E7D32",
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
              "accessible exercise options for every student.",
              "https://www.bmcc.cuny.edu/student-affairs/health-services/", "Visit →" },
            { "🌳", "NYC Free Outdoor Fitness",
              "Free outdoor fitness classes in NYC parks, adaptive programs for students with "
              "disabilities, and fitness meetup groups across the city.",
              "https://www.nycgovparks.org/programs/recreation/free-fitness-classes",
              "NYC Parks →" },
        }
    },

    {   "sleep", "🌙", "#E8EAF6", "#3949AB",
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

// ─────────────────────────────────────────────────────────────────────────────
// UI builders
// ─────────────────────────────────────────────────────────────────────────────

static QWidget* makeToolItem(const ToolItem &item) {
    QFrame *row = new QFrame();
    row->setStyleSheet("QFrame { border:none; background:transparent; }");
    QHBoxLayout *hl = new QHBoxLayout(row);
    hl->setContentsMargins(0, 6, 0, 6);
    hl->setSpacing(12);

    // Icon badge
    QLabel *iconLbl = new QLabel(item.icon);
    iconLbl->setFixedSize(34, 34);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet(
        "font-size:17px; background:#f5f4f0; border-radius:8px; border:none;");

    // Text block
    QWidget *textBlock = new QWidget();
    textBlock->setStyleSheet("background:transparent; border:none;");
    QVBoxLayout *vl = new QVBoxLayout(textBlock);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(4);

    QLabel *titleLbl = new QLabel(item.title);
    titleLbl->setStyleSheet(
        "font-size:13px; font-weight:700; color:#1a1a1a; border:none;");
    vl->addWidget(titleLbl);

    if (!item.body.isEmpty()) {
        QLabel *bodyLbl = new QLabel(item.body);
        bodyLbl->setWordWrap(true);
        bodyLbl->setStyleSheet("font-size:12px; color:#555; border:none; line-height:160%;");
        vl->addWidget(bodyLbl);
    }

    if (!item.url.isEmpty()) {
        QPushButton *linkBtn = new QPushButton(
            item.urlLabel.isEmpty() ? "Visit →" : item.urlLabel);
        QString u = item.url;
        QObject::connect(linkBtn, &QPushButton::clicked, [u]() {
            QDesktopServices::openUrl(QUrl(u));
        });
        linkBtn->setStyleSheet(
            "QPushButton { font-size:11px; font-weight:600; color:#534AB7; background:#EEEDFE; "
            "border:none; border-radius:6px; padding:4px 12px; margin-top:2px; }"
            "QPushButton:hover { background:#dddaf8; }");
        linkBtn->setCursor(Qt::PointingHandCursor);
        linkBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        vl->addWidget(linkBtn);
    }

    hl->addWidget(iconLbl, 0, Qt::AlignTop);
    hl->addWidget(textBlock, 1);
    return row;
}

static QFrame* makeDivider() {
    QFrame *f = new QFrame();
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet("background:#f0ede6; border:none;");
    f->setFixedHeight(1);
    return f;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Toolkit::Toolkit(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(24, 20, 24, 20);
    main->setSpacing(14);

    // Header
    QLabel *title = new QLabel("Mental Health Toolkit");
    title->setObjectName("screenTitle");
    main->addWidget(title);

    QFrame *hdiv = new QFrame();
    hdiv->setFrameShape(QFrame::HLine);
    hdiv->setStyleSheet("color:#eee;");
    main->addWidget(hdiv);

    // Intro banner
    QFrame *banner = new QFrame();
    banner->setStyleSheet(
        "QFrame { background:#f5f4f0; border-radius:10px; border:none; }");
    QHBoxLayout *bannerLay = new QHBoxLayout(banner);
    bannerLay->setContentsMargins(14, 12, 14, 12);
    bannerLay->setSpacing(10);
    QLabel *bannerIcon = new QLabel("🌱");
    bannerIcon->setStyleSheet("font-size:22px; border:none;");
    QLabel *bannerText = new QLabel(
        "Taking care of your well-being is just as important as acing that next exam. "
        "Click any folder below to explore tools, tips, and resources curated just for BMCC students.");
    bannerText->setWordWrap(true);
    bannerText->setStyleSheet("font-size:12px; color:#555; border:none;");
    bannerLay->addWidget(bannerIcon, 0, Qt::AlignTop);
    bannerLay->addWidget(bannerText, 1);
    main->addWidget(banner);

    // Scrollable folder accordion
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    QWidget *inner = new QWidget();
    QVBoxLayout *il = new QVBoxLayout(inner);
    il->setContentsMargins(0, 0, 4, 0);
    il->setSpacing(6);

    for (const FolderDef &fd : FOLDERS) {

        // ── Folder card button ────────────────────────────────────────────
        QPushButton *card = new QPushButton();
        card->setFixedHeight(64);
        card->setCursor(Qt::PointingHandCursor);
        card->setStyleSheet(
            "QPushButton {"
            "  border: 1.5px solid #e8e6e0;"
            "  border-radius: 10px;"
            "  background: #ffffff;"
            "}"
            "QPushButton:hover {"
            "  border-color: #9E97E8;"
            "  background: #fafafa;"
            "}");

        QHBoxLayout *cardLay = new QHBoxLayout(card);
        cardLay->setContentsMargins(14, 0, 16, 0);
        cardLay->setSpacing(12);

        // Icon badge
        QLabel *iconBadge = new QLabel(fd.icon);
        iconBadge->setFixedSize(40, 40);
        iconBadge->setAlignment(Qt::AlignCenter);
        iconBadge->setStyleSheet(
            QString("background:%1; border-radius:10px; font-size:21px; border:none;")
                .arg(fd.accent));
        iconBadge->setAttribute(Qt::WA_TransparentForMouseEvents);

        // Name + tagline
        QWidget *cardText = new QWidget();
        cardText->setStyleSheet("border:none; background:transparent;");
        cardText->setAttribute(Qt::WA_TransparentForMouseEvents);
        QVBoxLayout *cardTextLay = new QVBoxLayout(cardText);
        cardTextLay->setContentsMargins(0, 0, 0, 0);
        cardTextLay->setSpacing(2);
        QLabel *nameLbl = new QLabel(fd.name);
        nameLbl->setStyleSheet(
            "font-size:13px; font-weight:700; color:#1a1a1a; border:none;");
        QLabel *tagLbl = new QLabel(fd.tagline);
        tagLbl->setStyleSheet("font-size:11px; color:#888; border:none;");
        cardTextLay->addWidget(nameLbl);
        cardTextLay->addWidget(tagLbl);

        // Chevron
        QLabel *chev = new QLabel("›");
        chev->setFixedWidth(18);
        chev->setAlignment(Qt::AlignCenter);
        chev->setStyleSheet("font-size:20px; color:#bbb; border:none;");
        chev->setAttribute(Qt::WA_TransparentForMouseEvents);

        cardLay->addWidget(iconBadge);
        cardLay->addWidget(cardText, 1);
        cardLay->addWidget(chev);

        // ── Expandable panel ──────────────────────────────────────────────
        QFrame *panel = new QFrame();
        panel->setStyleSheet(
            QString("QFrame {"
                    "  border: 1.5px solid %1;"
                    "  border-top: none;"
                    "  border-radius: 0 0 10px 10px;"
                    "  background: #ffffff;"
                    "}").arg(fd.accentBorder));
        panel->setVisible(false);

        QVBoxLayout *panelLay = new QVBoxLayout(panel);
        panelLay->setContentsMargins(16, 14, 16, 16);
        panelLay->setSpacing(0);

        // Intro box
        QFrame *introBox = new QFrame();
        introBox->setStyleSheet(
            QString("QFrame { background:%1; border-radius:8px; border:none; }")
                .arg(fd.accent));
        QVBoxLayout *introLay = new QVBoxLayout(introBox);
        introLay->setContentsMargins(12, 10, 12, 10);
        QLabel *introLbl = new QLabel(fd.intro);
        introLbl->setWordWrap(true);
        introLbl->setStyleSheet("font-size:12px; color:#444; border:none;");
        introLay->addWidget(introLbl);
        panelLay->addWidget(introBox);
        panelLay->addSpacing(12);

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
                "  border: 1.5px solid #e8e6e0;"
                "  border-radius: 10px;"
                "  background: #ffffff;"
                "}"
                "QPushButton:hover {"
                "  border-color: #9E97E8;"
                "  background: #fafafa;"
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
    QString accent       = "#EEEDFE";
    QString accentBorder = "#7C74E0";
    for (const FolderDef &fd : FOLDERS) {
        if (fd.id == id) { accent = fd.accent; accentBorder = fd.accentBorder; break; }
    }

    if (auto *btn = qobject_cast<QPushButton*>(folderCards.value(id)))
        btn->setStyleSheet(
            QString("QPushButton {"
                    "  border: 1.5px solid %1;"
                    "  border-bottom: none;"
                    "  border-radius: 10px 10px 0 0;"
                    "  background: %2;"
                    "}").arg(accentBorder, accent));

    if (chevrons.contains(id))
        chevrons[id]->setText("∨");

    if (panels.contains(id))
        panels[id]->setVisible(true);
}
