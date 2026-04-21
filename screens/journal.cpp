#include "screens/journal.h"
#include "models/journalentry.h"
#include "core/screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QListWidgetItem>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QShortcut>
#include <QSplitter>
#include <QStandardPaths>
#include <QSizePolicy>

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

void Journal::showStatus(const QString &msg, bool ok) {
    m_statusLbl->setText(msg);
    m_statusLbl->setStyleSheet(
        ok ? "font-size:12px; color:#2E7D32; border:none; background:transparent;"
           : "font-size:12px; color:#C62828; border:none; background:transparent;");
    QTimer::singleShot(4000, m_statusLbl, [this]() { m_statusLbl->setText(""); });
}

void Journal::syncEntryActions() {
    const bool hasSelection = m_entryList && m_entryList->currentItem();
    if (m_deleteBtn)
        m_deleteBtn->setEnabled(hasSelection);
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
// File I/O — delegated to JournalStorage
// ─────────────────────────────────────────────────────────────────────────────

// saveEntry() constructs a JournalEntry object and delegates all disk I/O to
// JournalStorage::saveEntry(). This screen only handles the UI state change.
void Journal::saveEntry() {
    const QString text = m_editor->toPlainText().trimmed();
    if (text.isEmpty()) {
        showStatus("✕  Nothing to save — write something first.", false);
        return;
    }

    // Create a model object, then let the model write itself to disk.
    const QDateTime    now   = QDateTime::currentDateTime();
    JournalEntry       entry(now, text);

    if (!m_storage.saveEntry(entry)) {
        showStatus("✕  Could not save. Check file permissions.", false);
        return;
    }

    showStatus("✔  Entry saved — " + now.toString("MMMM d, yyyy 'at' h:mm AP"));
    m_editor->clear();
    refreshEntryList();
}

void Journal::deleteSelectedEntry() {
    QListWidgetItem *item = m_entryList ? m_entryList->currentItem() : nullptr;
    if (!item) {
        showStatus("Select an entry first.", false);
        return;
    }

    const QString path = item->data(Qt::UserRole).toString();
    const QFileInfo info(path);
    const QString prompt =
        QString("Delete this saved journal entry?\n\n%1\n\nThis removes the local file permanently.")
            .arg(item->text());

    const QMessageBox::StandardButton response = QMessageBox::question(
        this,
        "Delete Journal Entry",
        prompt,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if (response != QMessageBox::Yes)
        return;

    if (!m_storage.deleteEntry(path)) {
        showStatus(
            QString("Could not delete %1.").arg(info.fileName().isEmpty() ? "the selected entry"
                                                                          : info.fileName()),
            false);
        return;
    }

    refreshEntryList();
    showStatus("Entry deleted.", true);
}

void Journal::clearEditor() {
    if (m_editor->toPlainText().trimmed().isEmpty()) return;
    m_editor->clear();
    m_editor->setFocus();
    showStatus("Editor cleared.", true);
}

// refreshEntryList() asks JournalStorage for the current entries and rebuilds
// the list UI from those model objects.
void Journal::refreshEntryList() {
    const QString selectedPath = m_entryList->currentItem()
        ? m_entryList->currentItem()->data(Qt::UserRole).toString()
        : QString{};

    m_entryList->clear();
    m_preview->clear();
    syncEntryActions();

    const QVector<JournalEntry> entries = m_storage.loadEntries();

    if (entries.isEmpty()) {
        m_preview->setPlaceholderText(
            "No entries yet.\nWrite a reflection and save it to start building your archive.");
        syncEntryActions();
        return;
    }

    for (const JournalEntry &entry : entries) {
        QListWidgetItem *item = new QListWidgetItem(
            entry.dateTime().toString("ddd, MMM d yyyy  ·  h:mm AP"));
        item->setData(Qt::UserRole, entry.filePath());
        item->setToolTip(entry.filePath());
        m_entryList->addItem(item);
    }

    if (m_entryList->count() == 0) {
        m_preview->setPlaceholderText("No entries yet.\nSave your first entry to see it here.");
        syncEntryActions();
        return;
    }

    QListWidgetItem *itemToSelect = nullptr;
    for (int i = 0; i < m_entryList->count(); ++i) {
        QListWidgetItem *candidate = m_entryList->item(i);
        if (candidate->data(Qt::UserRole).toString() == selectedPath) {
            itemToSelect = candidate;
            break;
        }
    }

    if (!itemToSelect)
        itemToSelect = m_entryList->item(0);

    if (itemToSelect)
        m_entryList->setCurrentItem(itemToSelect);

    syncEntryActions();
}

// loadSelectedEntry() uses JournalStorage::loadEntry() to read the entry,
// then accesses it through its public accessor methods (encapsulation in action).
void Journal::loadSelectedEntry(QListWidgetItem *item) {
    if (!item) return;

    const QString    path  = item->data(Qt::UserRole).toString();
    const JournalEntry entry = m_storage.loadEntry(path);

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

Journal::Journal(QWidget *parent)
    : Screen("My Journal", parent),
      m_storage(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                + "/MindEase_Journal") {

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(48, 36, 48, 36);
    root->setSpacing(20);

    // Shared header (title label + divider) from the Screen base-class helper.
    // Demonstrates buildHeader() inherited from Screen.
    buildHeader(root);

    // ── Two-column body ──────────────────────────────────────────────────────
    QWidget *body = new QWidget();
    body->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *bodyLay = new QHBoxLayout(body);
    bodyLay->setContentsMargins(0, 0, 0, 0);
    bodyLay->setSpacing(0);

    QSplitter *split = new QSplitter(Qt::Horizontal);
    split->setChildrenCollapsible(false);
    split->setHandleWidth(10);

    // ════════════════════════════════════════════
    // LEFT PANEL — write
    // ════════════════════════════════════════════
    QFrame *writePanel = new QFrame();
    writePanel->setStyleSheet(
        "QFrame { border:1px solid #dfece1; border-radius:24px; background:#fff; }");
    QVBoxLayout *wl = new QVBoxLayout(writePanel);
    wl->setContentsMargins(28, 26, 28, 26);
    wl->setSpacing(14);

    // Date + time header row
    QWidget *dateRow = new QWidget();
    dateRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *drl = new QHBoxLayout(dateRow);
    drl->setContentsMargins(0, 0, 0, 0);
    drl->setSpacing(0);

    m_dateLbl = new QLabel();
    m_dateLbl->setStyleSheet(
        "font-size:18px; font-weight:700; color:#173c2c; border:none; "
        "letter-spacing:-0.15px;");

    m_timeLbl = new QLabel();
    m_timeLbl->setStyleSheet(
        "font-size:15px; color:#6d8272; border:none; font-weight:500;");

    drl->addWidget(m_dateLbl, 1);
    drl->addWidget(m_timeLbl);
    wl->addWidget(dateRow);

    QWidget *privacyRow = new QWidget();
    privacyRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *prl = new QHBoxLayout(privacyRow);
    prl->setContentsMargins(0, 0, 0, 0);
    prl->setSpacing(8);
    QLabel *privacyChip = new QLabel("PRIVATE · STORED LOCALLY");
    privacyChip->setStyleSheet(
        "font-size:10px; font-weight:700; color:#2f7d5c; background:#edf7ee; "
        "border:1px solid #cfe5d2; border-radius:12px; padding:6px 10px; "
        "letter-spacing:1.1px;");
    QLabel *privacyNote = new QLabel("Entries are saved as plain text in your Documents folder.");
    privacyNote->setStyleSheet(
        "font-size:12px; color:#6d8272; border:none; background:transparent;");
    prl->addWidget(privacyChip);
    prl->addWidget(privacyNote, 1);
    wl->addWidget(privacyRow);

    // Prompts hint box (with left accent bar for visual consistency with
    // the Resources screen's left-accented card style)
    QFrame *promptBox = new QFrame();
    promptBox->setStyleSheet(
        "QFrame { background:#edf7ee; border:1px solid #cfe5d2; "
        "         border-left:4px solid #2f7d5c; border-radius:18px; }");
    QVBoxLayout *pl = new QVBoxLayout(promptBox);
    pl->setContentsMargins(16, 12, 16, 14);
    pl->setSpacing(6);

    QLabel *promptTitle = new QLabel("✏️  JOURNAL PROMPTS");
    promptTitle->setStyleSheet(
        "font-size:10px; font-weight:700; color:#2f7d5c; border:none; "
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
            "font-size:12px; color:#4f6255; border:none; background:transparent; "
            "line-height:165%;");
        pl->addWidget(lbl);
    }
    wl->addWidget(promptBox);

    QLabel *editorLabel = new QLabel("WRITE YOUR ENTRY BELOW");
    editorLabel->setStyleSheet(
        "font-size:10px; font-weight:700; color:#8ba493; border:none; "
        "letter-spacing:1.4px;");
    wl->addWidget(editorLabel);

    // Main text editor
    m_editor = new QTextEdit();
    m_editor->setAcceptRichText(false);
    m_editor->setPlaceholderText(
        "Start writing here… There's no right or wrong way to journal.\n\n"
        "You can respond to a prompt above, describe your day, or just write\n"
        "whatever is on your mind right now.");
    m_editor->setStyleSheet(
        "QTextEdit {"
        "  border: 1px solid #cfe5d2;"
        "  border-radius: 18px;"
        "  background: #fafffb;"
        "  font-size: 15px;"
        "  color: #173c2c;"
        "  padding: 18px;"
        "  line-height: 170%;"
        "  selection-background-color: #cfe5d2;"
        "}"
        "QTextEdit:focus {"
        "  border: 1px solid #2f7d5c;"
        "  background: #ffffff;"
        "}");
    m_editor->setTabStopDistance(28);
    m_editor->setMinimumHeight(220);
    wl->addWidget(m_editor, 1);

    // Button row
    QWidget *btnRow = new QWidget();
    btnRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *brl = new QHBoxLayout(btnRow);
    brl->setContentsMargins(0, 2, 0, 0);
    brl->setSpacing(10);

    QPushButton *saveBtn = new QPushButton("💾  Save Entry");
    saveBtn->setObjectName("primaryBtn");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setMinimumHeight(42);
    connect(saveBtn, &QPushButton::clicked, this, &Journal::saveEntry);

    QPushButton *clearBtn = new QPushButton("✕  Clear");
    clearBtn->setObjectName("outlineBtn");
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setMinimumHeight(42);
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
        "QFrame { border:1px solid #dfece1; border-radius:24px; background:#fff; }");
    readPanel->setMinimumWidth(390);
    QVBoxLayout *rl = new QVBoxLayout(readPanel);
    rl->setContentsMargins(26, 24, 26, 24);
    rl->setSpacing(14);

    QWidget *readHeader = new QWidget();
    readHeader->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *rhl = new QHBoxLayout(readHeader);
    rhl->setContentsMargins(0, 0, 0, 0);
    rhl->setSpacing(8);

    QLabel *readTitle = new QLabel("📚  Past Entries");
    readTitle->setStyleSheet(
        "font-size:17px; font-weight:700; color:#173c2c; border:none; "
        "letter-spacing:-0.15px;");
    QLabel *readMeta = new QLabel("Newest first");
    readMeta->setStyleSheet(
        "font-size:11px; font-weight:600; color:#557161; background:#edf7ee; "
        "border:1px solid #cfe5d2; border-radius:12px; padding:5px 10px;");
    rhl->addWidget(readTitle);
    rhl->addStretch();
    rhl->addWidget(readMeta);
    rl->addWidget(readHeader);

    m_entryList = new QListWidget();
    m_entryList->setStyleSheet(
        "QListWidget {"
        "  border:1px solid #dfece1;"
        "  border-radius:18px;"
        "  background:#fafffb;"
        "  font-size:14px;"
        "  color:#173c2c;"
        "  outline:none;"
        "  padding:6px;"
        "}"
        "QListWidget::item {"
        "  padding:12px 12px;"
        "  border-bottom:1px solid #edf4ee;"
        "  border-radius:12px;"
        "}"
        "QListWidget::item:last-child {"
        "  border-bottom:none;"
        "}"
        "QListWidget::item:selected {"
        "  background:#edf7ee;"
        "  color:#2f7d5c;"
        "  font-weight:600;"
        "  border-bottom-color:transparent;"
        "}"
        "QListWidget::item:hover {"
        "  background:#f7fff8;"
        "}");
    m_entryList->setMinimumHeight(170);
    m_entryList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(m_entryList, &QListWidget::itemClicked,
            this, &Journal::loadSelectedEntry);
    connect(m_entryList, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem *current, QListWidgetItem *) {
                loadSelectedEntry(current);
                syncEntryActions();
            });
    rl->addWidget(m_entryList);

    QWidget *entryActionRow = new QWidget();
    entryActionRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *earl = new QHBoxLayout(entryActionRow);
    earl->setContentsMargins(0, 0, 0, 0);
    earl->setSpacing(8);

    QLabel *deleteHint = new QLabel("Remove entries you no longer want to keep.");
    deleteHint->setStyleSheet(
        "font-size:12px; color:#6d8272; border:none; background:transparent;");

    m_deleteBtn = new QPushButton("Delete Selected");
    m_deleteBtn->setCursor(Qt::PointingHandCursor);
    m_deleteBtn->setMinimumHeight(34);
    m_deleteBtn->setStyleSheet(
        "QPushButton {"
        "  font-size:11px;"
        "  font-weight:700;"
        "  color:#B42318;"
        "  background:#fff5f4;"
        "  border:1px solid #f1c7c2;"
        "  border-radius:9px;"
        "  padding:6px 12px;"
        "}"
        "QPushButton:hover:enabled {"
        "  background:#feeceb;"
        "  border-color:#e7a9a1;"
        "}"
        "QPushButton:disabled {"
        "  color:#c8b9b7;"
        "  background:#faf7f6;"
        "  border-color:#efe3e0;"
        "}");
    connect(m_deleteBtn, &QPushButton::clicked, this, &Journal::deleteSelectedEntry);

    earl->addWidget(deleteHint, 1);
    earl->addWidget(m_deleteBtn);
    rl->addWidget(entryActionRow);

    QFrame *rdiv = new QFrame();
    rdiv->setFrameShape(QFrame::HLine);
    rdiv->setStyleSheet("background:#dfece1; border:none;");
    rdiv->setFixedHeight(1);
    rl->addWidget(rdiv);

    QLabel *previewLbl = new QLabel("ENTRY PREVIEW");
    previewLbl->setStyleSheet(
        "font-size:10px; font-weight:700; color:#8ba493; border:none; "
        "letter-spacing:1.4px;");
    rl->addWidget(previewLbl);

    m_preview = new QTextEdit();
    m_preview->setReadOnly(true);
    m_preview->setPlaceholderText("Click an entry on the left to read it here.");
    m_preview->setStyleSheet(
        "QTextEdit {"
        "  border:1px solid #dfece1;"
        "  border-radius:18px;"
        "  background:#fafffb;"
        "  font-size:14px;"
        "  color:#4f6255;"
        "  padding:16px;"
        "  line-height:170%;"
        "}");
    m_preview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rl->addWidget(m_preview, 1);

    // Assemble body
    split->addWidget(writePanel);
    split->addWidget(readPanel);
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 2);
    split->setSizes({820, 460});
    bodyLay->addWidget(split);
    root->addWidget(body, 1);

    // ── Live clock ───────────────────────────────────────────────────────────
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Journal::updateClock);
    m_timer->start(1000);
    updateClock();

    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), m_entryList);
    connect(deleteShortcut, &QShortcut::activated, this, &Journal::deleteSelectedEntry);

    refreshEntryList();
}
