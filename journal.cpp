#include "journal.h"
#include "journalentry.h"
#include "screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QListWidgetItem>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QSizePolicy>

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

// Returns the path where all journal entries are stored.
QString Journal::journalDir() const {
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
           + "/MindEase_Journal";
}

void Journal::showStatus(const QString &msg, bool ok) {
    m_statusLbl->setText(msg);
    m_statusLbl->setStyleSheet(
        ok ? "font-size:12px; color:#2E7D32; border:none; background:transparent;"
           : "font-size:12px; color:#C62828; border:none; background:transparent;");
    QTimer::singleShot(4000, m_statusLbl, [this]() { m_statusLbl->setText(""); });
}

// ─────────────────────────────────────────────────────────────────────────────
// Activation hook (overrides Screen::onActivated)
// ─────────────────────────────────────────────────────────────────────────────

// Called by MainWindow::switchScreen() each time the user navigates here.
// Refreshes the entry list (e.g. if a file was added externally) and syncs
// the date/time display. Demonstrates runtime polymorphism: MainWindow calls
// screens[i]->onActivated() on a Screen* and the correct override runs.
void Journal::onActivated() {
    refreshEntryList();
    updateClock();
}

// ─────────────────────────────────────────────────────────────────────────────
// File I/O — delegated to JournalEntry
// ─────────────────────────────────────────────────────────────────────────────

// saveEntry() constructs a JournalEntry object and delegates all disk I/O to
// JournalEntry::saveToFile(). This screen only handles the UI state change.
void Journal::saveEntry() {
    const QString text = m_editor->toPlainText().trimmed();
    if (text.isEmpty()) {
        showStatus("✕  Nothing to save — write something first.", false);
        return;
    }

    // Create a model object, then let the model write itself to disk.
    const QDateTime    now   = QDateTime::currentDateTime();
    JournalEntry       entry(now, text);

    if (!entry.saveToFile(journalDir())) {
        showStatus("✕  Could not save. Check file permissions.", false);
        return;
    }

    showStatus("✔  Entry saved — " + now.toString("MMMM d, yyyy 'at' h:mm AP"));
    m_editor->clear();
    refreshEntryList();
}

void Journal::clearEditor() {
    if (m_editor->toPlainText().trimmed().isEmpty()) return;
    m_editor->clear();
    showStatus("Editor cleared.", true);
}

// refreshEntryList() re-scans the journal directory and rebuilds the list.
// It calls JournalEntry::loadFromFile() for each file to parse the timestamp,
// demonstrating the static factory pattern in practice.
void Journal::refreshEntryList() {
    m_entryList->clear();

    QDir dir(journalDir());
    if (!dir.exists()) return;

    // Collect all .txt files, newest first (reversed alphabetical order works
    // because filenames encode date/time in yyyy-MM-dd_HH-mm-ss format).
    const QStringList files = dir.entryList({"*.txt"}, QDir::Files,
                                            QDir::Name | QDir::Reversed);
    for (const QString &fname : files) {
        // Use the JournalEntry model to parse the file rather than duplicating
        // the parsing logic here.
        JournalEntry entry = JournalEntry::loadFromFile(journalDir() + "/" + fname);

        const QString display = entry.isValid()
            ? entry.dateTime().toString("ddd, MMM d yyyy  ·  h:mm AP")
            : fname;   // fallback: raw filename if parsing fails

        QListWidgetItem *item = new QListWidgetItem(display);
        item->setData(Qt::UserRole, entry.isValid() ? entry.filePath() : journalDir() + "/" + fname);
        item->setToolTip(item->data(Qt::UserRole).toString());
        m_entryList->addItem(item);
    }

    if (m_entryList->count() == 0)
        m_preview->setPlaceholderText("No entries yet.\nSave your first entry to see it here.");
}

// loadSelectedEntry() uses JournalEntry::loadFromFile() to read the entry,
// then accesses it through its public accessor methods (encapsulation in action).
void Journal::loadSelectedEntry(QListWidgetItem *item) {
    if (!item) return;

    const QString    path  = item->data(Qt::UserRole).toString();
    const JournalEntry entry = JournalEntry::loadFromFile(path);

    if (!entry.isValid()) {
        m_preview->setPlainText("Could not open this entry.");
        return;
    }

    // Build the preview text using accessor methods — no direct field access.
    QString preview;
    preview += entry.dateTime().toString("dddd, MMMM d, yyyy") + "\n";
    preview += entry.dateTime().toString("h:mm AP")            + "\n";
    preview += QString(30, QChar(0x2500))                      + "\n\n";
    preview += entry.body();
    m_preview->setPlainText(preview);
}

// ─────────────────────────────────────────────────────────────────────────────
// Clock
// ─────────────────────────────────────────────────────────────────────────────

void Journal::updateClock() {
    const QDateTime now = QDateTime::currentDateTime();
    m_dateLbl->setText(now.toString("dddd, MMMM d, yyyy"));
    m_timeLbl->setText(now.toString("h:mm AP"));
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Journal::Journal(QWidget *parent) : Screen("My Journal", parent) {

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(32, 24, 32, 24);
    root->setSpacing(16);

    // Shared header (title label + divider) from the Screen base-class helper.
    // Demonstrates buildHeader() inherited from Screen.
    buildHeader(root);

    // ── Two-column body ──────────────────────────────────────────────────────
    QWidget *body = new QWidget();
    body->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *bodyLay = new QHBoxLayout(body);
    bodyLay->setContentsMargins(0, 0, 0, 0);
    bodyLay->setSpacing(18);

    // ════════════════════════════════════════════
    // LEFT PANEL — write
    // ════════════════════════════════════════════
    QFrame *writePanel = new QFrame();
    writePanel->setStyleSheet(
        "QFrame { border:1px solid #ece9e2; border-radius:14px; background:#fff; }");
    QVBoxLayout *wl = new QVBoxLayout(writePanel);
    wl->setContentsMargins(22, 20, 22, 20);
    wl->setSpacing(12);

    // Date + time header row
    QWidget *dateRow = new QWidget();
    dateRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *drl = new QHBoxLayout(dateRow);
    drl->setContentsMargins(0, 0, 0, 0);
    drl->setSpacing(0);

    m_dateLbl = new QLabel();
    m_dateLbl->setStyleSheet(
        "font-size:14px; font-weight:700; color:#1a1a1a; border:none; "
        "letter-spacing:-0.15px;");

    m_timeLbl = new QLabel();
    m_timeLbl->setStyleSheet(
        "font-size:13px; color:#8a857c; border:none; font-weight:500;");

    drl->addWidget(m_dateLbl, 1);
    drl->addWidget(m_timeLbl);
    wl->addWidget(dateRow);

    // Prompts hint box (with left accent bar for visual consistency with
    // the Resources screen's left-accented card style)
    QFrame *promptBox = new QFrame();
    promptBox->setStyleSheet(
        "QFrame { background:#f5f4fb; border:1px solid #e4e0f5; "
        "         border-left:3px solid #7C74E0; border-radius:10px; }");
    QVBoxLayout *pl = new QVBoxLayout(promptBox);
    pl->setContentsMargins(16, 12, 16, 14);
    pl->setSpacing(6);

    QLabel *promptTitle = new QLabel("✏️  JOURNAL PROMPTS");
    promptTitle->setStyleSheet(
        "font-size:10px; font-weight:700; color:#534AB7; border:none; "
        "background:transparent; letter-spacing:1.4px;");
    pl->addWidget(promptTitle);
    pl->addSpacing(2);

    const QStringList prompts = {
        "1.  What are three things that happened today that brought you peace?",
        "2.  What went well this week, even if something else was hard?",
        "3.  What everyday object near you are you grateful for?",
        "4.  Who made a positive difference in your life this month?"
    };
    for (const QString &p : prompts) {
        QLabel *lbl = new QLabel(p);
        lbl->setWordWrap(true);
        lbl->setStyleSheet(
            "font-size:11px; color:#4d4758; border:none; background:transparent; "
            "line-height:165%;");
        pl->addWidget(lbl);
    }
    wl->addWidget(promptBox);

    QLabel *editorLabel = new QLabel("WRITE YOUR ENTRY BELOW");
    editorLabel->setStyleSheet(
        "font-size:10px; font-weight:700; color:#a8a49b; border:none; "
        "letter-spacing:1.4px;");
    wl->addWidget(editorLabel);

    // Main text editor
    m_editor = new QTextEdit();
    m_editor->setPlaceholderText(
        "Start writing here… There's no right or wrong way to journal.\n\n"
        "You can respond to a prompt above, describe your day, or just write\n"
        "whatever is on your mind right now.");
    m_editor->setStyleSheet(
        "QTextEdit {"
        "  border: 1px solid #e4e0f5;"
        "  border-radius: 10px;"
        "  background: #fafaf8;"
        "  font-size: 13px;"
        "  color: #1a1a1a;"
        "  padding: 14px;"
        "  line-height: 170%;"
        "  selection-background-color: #e4e0f5;"
        "}"
        "QTextEdit:focus {"
        "  border: 1px solid #7C74E0;"
        "  background: #ffffff;"
        "}");
    m_editor->setMinimumHeight(220);
    wl->addWidget(m_editor, 1);

    // Button row
    QWidget *btnRow = new QWidget();
    btnRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *brl = new QHBoxLayout(btnRow);
    brl->setContentsMargins(0, 2, 0, 0);
    brl->setSpacing(10);

    QPushButton *saveBtn = new QPushButton("💾  Save Entry");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton {"
        "  font-size:13px; font-weight:700;"
        "  color:#fff; background:#534AB7;"
        "  border:none; border-radius:10px; padding:10px 24px;"
        "}"
        "QPushButton:hover { background:#3d358f; }"
        "QPushButton:pressed { background:#2e2870; }");
    connect(saveBtn, &QPushButton::clicked, this, &Journal::saveEntry);

    QPushButton *clearBtn = new QPushButton("✕  Clear");
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setStyleSheet(
        "QPushButton {"
        "  font-size:13px; font-weight:600; color:#8a857c; background:transparent;"
        "  border:1px solid #ece9e2; border-radius:10px; padding:10px 18px;"
        "}"
        "QPushButton:hover { border-color:#c8c3ba; color:#5d574f; background:#faf9f5; }");
    connect(clearBtn, &QPushButton::clicked, this, &Journal::clearEditor);

    m_statusLbl = new QLabel("");
    m_statusLbl->setStyleSheet(
        "font-size:12px; color:#2E7D32; border:none; background:transparent;");

    brl->addWidget(saveBtn);
    brl->addWidget(clearBtn);
    brl->addSpacing(8);
    brl->addWidget(m_statusLbl, 1);
    wl->addWidget(btnRow);

    // ════════════════════════════════════════════
    // RIGHT PANEL — past entries
    // ════════════════════════════════════════════
    QFrame *readPanel = new QFrame();
    readPanel->setStyleSheet(
        "QFrame { border:1px solid #ece9e2; border-radius:14px; background:#fff; }");
    readPanel->setFixedWidth(320);
    QVBoxLayout *rl = new QVBoxLayout(readPanel);
    rl->setContentsMargins(20, 18, 20, 18);
    rl->setSpacing(12);

    QLabel *readTitle = new QLabel("📚  Past Entries");
    readTitle->setStyleSheet(
        "font-size:14px; font-weight:700; color:#1a1a1a; border:none; "
        "letter-spacing:-0.15px;");
    rl->addWidget(readTitle);

    m_entryList = new QListWidget();
    m_entryList->setStyleSheet(
        "QListWidget {"
        "  border:1px solid #ece9e2;"
        "  border-radius:10px;"
        "  background:#faf9f5;"
        "  font-size:12px;"
        "  color:#333;"
        "  outline:none;"
        "  padding:4px;"
        "}"
        "QListWidget::item {"
        "  padding:9px 10px;"
        "  border-bottom:1px solid #f2efe7;"
        "  border-radius:6px;"
        "}"
        "QListWidget::item:last-child {"
        "  border-bottom:none;"
        "}"
        "QListWidget::item:selected {"
        "  background:#EEEDFE;"
        "  color:#534AB7;"
        "  font-weight:600;"
        "  border-bottom-color:transparent;"
        "}"
        "QListWidget::item:hover {"
        "  background:#f5f4fb;"
        "}");
    m_entryList->setMinimumHeight(170);
    m_entryList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(m_entryList, &QListWidget::itemClicked,
            this, &Journal::loadSelectedEntry);
    rl->addWidget(m_entryList);

    QFrame *rdiv = new QFrame();
    rdiv->setFrameShape(QFrame::HLine);
    rdiv->setStyleSheet("background:#f2efe7; border:none;");
    rdiv->setFixedHeight(1);
    rl->addWidget(rdiv);

    QLabel *previewLbl = new QLabel("ENTRY PREVIEW");
    previewLbl->setStyleSheet(
        "font-size:10px; font-weight:700; color:#a8a49b; border:none; "
        "letter-spacing:1.4px;");
    rl->addWidget(previewLbl);

    m_preview = new QTextEdit();
    m_preview->setReadOnly(true);
    m_preview->setPlaceholderText("Click an entry on the left to read it here.");
    m_preview->setStyleSheet(
        "QTextEdit {"
        "  border:1px solid #ece9e2;"
        "  border-radius:10px;"
        "  background:#faf9f5;"
        "  font-size:12px;"
        "  color:#4d4758;"
        "  padding:12px;"
        "  line-height:170%;"
        "}");
    m_preview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rl->addWidget(m_preview, 1);

    // Assemble body
    bodyLay->addWidget(writePanel, 1);
    bodyLay->addWidget(readPanel);
    root->addWidget(body, 1);

    // ── Live clock ───────────────────────────────────────────────────────────
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Journal::updateClock);
    m_timer->start(1000);
    updateClock();

    refreshEntryList();
}
