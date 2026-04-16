#include "journal.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QSizePolicy>

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

QString Journal::journalDir() const {
    QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return docs + "/MindEase_Journal";
}

void Journal::showStatus(const QString &msg, bool ok) {
    m_statusLbl->setText(msg);
    m_statusLbl->setStyleSheet(
        ok ? "font-size:12px; color:#2E7D32; border:none; background:transparent;"
           : "font-size:12px; color:#C62828; border:none; background:transparent;");
    // Auto-clear after 4 seconds
    QTimer::singleShot(4000, m_statusLbl, [this]() { m_statusLbl->setText(""); });
}

// ─────────────────────────────────────────────────────────────────────────────
// File I/O
// ─────────────────────────────────────────────────────────────────────────────

void Journal::saveEntry() {
    QString text = m_editor->toPlainText().trimmed();
    if (text.isEmpty()) {
        showStatus("✕  Nothing to save — write something first.", false);
        return;
    }

    // Ensure the journal directory exists
    QDir dir(journalDir());
    if (!dir.exists()) dir.mkpath(".");

    QDateTime now = QDateTime::currentDateTime();
    QString filename = now.toString("yyyy-MM-dd_HH-mm-ss") + ".txt";
    QString filepath = journalDir() + "/" + filename;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showStatus("✕  Could not save. Check file permissions.", false);
        return;
    }

    QTextStream out(&file);
    // Human-readable header at the top of every saved entry
    out << "=== MindEase Journal Entry ===\n";
    out << "Date: " << now.toString("dddd, MMMM d, yyyy") << "\n";
    out << "Time: " << now.toString("h:mm AP") << "\n";
    out << "──────────────────────────────\n\n";
    out << text << "\n";
    file.close();

    showStatus("✔  Entry saved — " + now.toString("MMMM d, yyyy 'at' h:mm AP"));
    m_editor->clear();
    refreshEntryList();
}

void Journal::clearEditor() {
    if (m_editor->toPlainText().trimmed().isEmpty()) return;
    m_editor->clear();
    showStatus("Editor cleared.", true);
}

void Journal::refreshEntryList() {
    m_entryList->clear();

    QDir dir(journalDir());
    if (!dir.exists()) return;

    // List all .txt files, newest first
    QStringList files = dir.entryList({"*.txt"}, QDir::Files, QDir::Name | QDir::Reversed);

    for (const QString &fname : files) {
        // Parse datetime from filename: yyyy-MM-dd_HH-mm-ss.txt
        QString stem = fname;
        stem.remove(".txt");                         // "2026-04-15_14-30-00"
        QString datePart = stem.left(10);            // "2026-04-15"
        QString timePart = stem.mid(11).replace('-', ':'); // "14:30:00"

        QDateTime dt = QDateTime::fromString(
            datePart + " " + timePart,
            "yyyy-MM-dd HH:mm:ss");

        QString display;
        if (dt.isValid()) {
            display = dt.toString("ddd, MMM d yyyy  ·  h:mm AP");
        } else {
            display = fname; // fallback: show raw filename
        }

        QListWidgetItem *item = new QListWidgetItem(display);
        item->setData(Qt::UserRole, journalDir() + "/" + fname);
        item->setToolTip(journalDir() + "/" + fname);
        m_entryList->addItem(item);
    }

    if (m_entryList->count() == 0)
        m_preview->setPlaceholderText("No entries yet.\nSave your first entry to see it here.");
}

void Journal::loadSelectedEntry(QListWidgetItem *item) {
    if (!item) return;
    QString path = item->data(Qt::UserRole).toString();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_preview->setPlainText("Could not open this entry.");
        return;
    }
    QTextStream in(&file);
    m_preview->setPlainText(in.readAll());
    file.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// Clock
// ─────────────────────────────────────────────────────────────────────────────

void Journal::updateClock() {
    QDateTime now = QDateTime::currentDateTime();
    m_dateLbl->setText(now.toString("dddd, MMMM d, yyyy"));
    m_timeLbl->setText(now.toString("h:mm AP"));
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Journal::Journal(QWidget *parent) : QWidget(parent) {

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 20, 28, 20);
    root->setSpacing(14);

    // ── Screen title ─────────────────────────────────────────────────────
    QLabel *title = new QLabel("My Journal");
    title->setObjectName("screenTitle");
    root->addWidget(title);

    QFrame *hdiv = new QFrame();
    hdiv->setFrameShape(QFrame::HLine);
    hdiv->setStyleSheet("color:#eee;");
    root->addWidget(hdiv);

    // ── Two-column body ──────────────────────────────────────────────────
    QWidget *body = new QWidget();
    body->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *bodyLay = new QHBoxLayout(body);
    bodyLay->setContentsMargins(0, 0, 0, 0);
    bodyLay->setSpacing(16);

    // ════════════════════════════════════════════
    // LEFT PANEL — write
    // ════════════════════════════════════════════
    QFrame *writePanel = new QFrame();
    writePanel->setStyleSheet(
        "QFrame { border:1.5px solid #e8e6e0; border-radius:12px; background:#fff; }");
    QVBoxLayout *wl = new QVBoxLayout(writePanel);
    wl->setContentsMargins(18, 16, 18, 16);
    wl->setSpacing(10);

    // Date + time header
    QWidget *dateRow = new QWidget();
    dateRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *drl = new QHBoxLayout(dateRow);
    drl->setContentsMargins(0, 0, 0, 0);
    drl->setSpacing(0);

    m_dateLbl = new QLabel();
    m_dateLbl->setStyleSheet(
        "font-size:14px; font-weight:700; color:#1a1a1a; border:none;");

    m_timeLbl = new QLabel();
    m_timeLbl->setStyleSheet(
        "font-size:13px; color:#888; border:none;");

    drl->addWidget(m_dateLbl, 1);
    drl->addWidget(m_timeLbl);
    wl->addWidget(dateRow);

    // Prompts hint box
    QFrame *promptBox = new QFrame();
    promptBox->setStyleSheet(
        "QFrame { background:#F5F4FB; border:1px solid #DDDAF8; border-radius:9px; }");
    QVBoxLayout *pl = new QVBoxLayout(promptBox);
    pl->setContentsMargins(13, 10, 13, 10);
    pl->setSpacing(4);

    QLabel *promptTitle = new QLabel("✏️  Journal Prompts");
    promptTitle->setStyleSheet(
        "font-size:12px; font-weight:700; color:#534AB7; border:none;");
    pl->addWidget(promptTitle);

    const QStringList prompts = {
        "1.  What are three things that happened today that brought you peace?",
        "2.  What went well this week, even if something else was hard?",
        "3.  What everyday object near you are you grateful for?",
        "4.  Who made a positive difference in your life this month?"
    };
    for (const QString &p : prompts) {
        QLabel *lbl = new QLabel(p);
        lbl->setWordWrap(true);
        lbl->setStyleSheet("font-size:11px; color:#555; border:none;");
        pl->addWidget(lbl);
    }
    wl->addWidget(promptBox);

    // Divider label
    QLabel *editorLabel = new QLabel("Write your entry below:");
    editorLabel->setStyleSheet("font-size:11px; color:#aaa; border:none;");
    wl->addWidget(editorLabel);

    // Main text editor
    m_editor = new QTextEdit();
    m_editor->setPlaceholderText(
        "Start writing here… There's no right or wrong way to journal.\n\n"
        "You can respond to a prompt above, describe your day, or just write\n"
        "whatever is on your mind right now.");
    m_editor->setStyleSheet(
        "QTextEdit {"
        "  border: 1.5px solid #e0ddf8;"
        "  border-radius: 9px;"
        "  background: #fafafa;"
        "  font-size: 13px;"
        "  color: #1a1a1a;"
        "  padding: 10px;"
        "  line-height: 160%;"
        "}"
        "QTextEdit:focus {"
        "  border-color: #7C74E0;"
        "  background: #ffffff;"
        "}");
    m_editor->setMinimumHeight(200);
    wl->addWidget(m_editor, 1);

    // Buttons row
    QWidget *btnRow = new QWidget();
    btnRow->setStyleSheet("border:none; background:transparent;");
    QHBoxLayout *brl = new QHBoxLayout(btnRow);
    brl->setContentsMargins(0, 0, 0, 0);
    brl->setSpacing(8);

    QPushButton *saveBtn = new QPushButton("💾  Save Entry");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton {"
        "  font-size:13px; font-weight:700;"
        "  color:#fff; background:#7C74E0;"
        "  border:none; border-radius:9px; padding:9px 22px;"
        "}"
        "QPushButton:hover { background:#6A62CF; }");
    connect(saveBtn, &QPushButton::clicked, this, &Journal::saveEntry);

    QPushButton *clearBtn = new QPushButton("✕  Clear");
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setStyleSheet(
        "QPushButton {"
        "  font-size:13px; color:#888; background:transparent;"
        "  border:1.5px solid #e0ddd8; border-radius:9px; padding:9px 16px;"
        "}"
        "QPushButton:hover { border-color:#bbb; color:#555; }");
    connect(clearBtn, &QPushButton::clicked, this, &Journal::clearEditor);

    m_statusLbl = new QLabel("");
    m_statusLbl->setStyleSheet("font-size:12px; color:#2E7D32; border:none; background:transparent;");

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
        "QFrame { border:1.5px solid #e8e6e0; border-radius:12px; background:#fff; }");
    readPanel->setFixedWidth(300);
    QVBoxLayout *rl = new QVBoxLayout(readPanel);
    rl->setContentsMargins(16, 14, 16, 14);
    rl->setSpacing(10);

    QLabel *readTitle = new QLabel("📚  Past Entries");
    readTitle->setStyleSheet(
        "font-size:14px; font-weight:700; color:#1a1a1a; border:none;");
    rl->addWidget(readTitle);

    // Entry list
    m_entryList = new QListWidget();
    m_entryList->setStyleSheet(
        "QListWidget {"
        "  border:1.5px solid #ece9e2;"
        "  border-radius:8px;"
        "  background:#fafaf8;"
        "  font-size:12px;"
        "  color:#333;"
        "  outline:none;"
        "}"
        "QListWidget::item {"
        "  padding:8px 10px;"
        "  border-bottom:1px solid #f0ede6;"
        "}"
        "QListWidget::item:selected {"
        "  background:#EEEDFE;"
        "  color:#534AB7;"
        "  border-radius:6px;"
        "}"
        "QListWidget::item:hover {"
        "  background:#f5f4fb;"
        "}");
    m_entryList->setMinimumHeight(160);
    m_entryList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(m_entryList, &QListWidget::itemClicked,
            this, &Journal::loadSelectedEntry);
    rl->addWidget(m_entryList);

    // Divider
    QFrame *rdiv = new QFrame();
    rdiv->setFrameShape(QFrame::HLine);
    rdiv->setStyleSheet("background:#f0ede6; border:none;");
    rdiv->setFixedHeight(1);
    rl->addWidget(rdiv);

    // Preview label
    QLabel *previewLbl = new QLabel("Entry Preview");
    previewLbl->setStyleSheet("font-size:11px; color:#aaa; border:none;");
    rl->addWidget(previewLbl);

    // Preview text area (read-only)
    m_preview = new QTextEdit();
    m_preview->setReadOnly(true);
    m_preview->setPlaceholderText(
        "Click an entry on the left to read it here.");
    m_preview->setStyleSheet(
        "QTextEdit {"
        "  border:1.5px solid #ece9e2;"
        "  border-radius:8px;"
        "  background:#fafaf8;"
        "  font-size:12px;"
        "  color:#333;"
        "  padding:8px;"
        "  line-height:160%;"
        "}");
    m_preview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rl->addWidget(m_preview, 1);

    // Assemble body
    bodyLay->addWidget(writePanel, 1);
    bodyLay->addWidget(readPanel);
    root->addWidget(body, 1);

    // ── Live clock ───────────────────────────────────────────────────────
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Journal::updateClock);
    m_timer->start(1000);
    updateClock();

    // ── Load entry list ──────────────────────────────────────────────────
    refreshEntryList();
}
