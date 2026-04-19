#ifndef JOURNAL_H
#define JOURNAL_H

#include "screen.h"
#include "journalentry.h"
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>
#include <QTimer>

// ─────────────────────────────────────────────────────────────────────────────
// Journal — personal wellness journal screen with local file persistence
//
// Inherits Screen; uses JournalEntry as its data model. Demonstrates:
//   • Inheritance   — extends Screen (which extends QWidget)
//   • Polymorphism  — overrides screenId() and onActivated() from Screen
//   • Composition   — owns a JournalEntry model class for file I/O operations
//
// Entries are saved to ~/Documents/MindEase_Journal/yyyy-MM-dd_HH-mm-ss.txt
// All disk reads and writes are delegated to JournalEntry, keeping this class
// focused on UI state and user interaction.
// ─────────────────────────────────────────────────────────────────────────────

class Journal : public Screen {
    Q_OBJECT

public:
    explicit Journal(QWidget *parent = nullptr);

    // ── Implement pure virtual (required by Screen) ───────────────────────────
    QString screenId() const override { return "journal"; }

    // ── Override activation hook ──────────────────────────────────────────────
    // Refreshes the entry list and clock when the user navigates to this screen.
    void onActivated() override;

private slots:
    void saveEntry();
    void clearEditor();
    void loadSelectedEntry(QListWidgetItem *item);
    void updateClock();

private:
    void    refreshEntryList();
    void    showStatus(const QString &msg, bool ok = true);
    QString journalDir() const;

    // ── Write panel ───────────────────────────────────────────────────────────
    QLabel      *m_dateLbl;     // "Wednesday, April 15, 2026"
    QLabel      *m_timeLbl;     // live clock  "2:30 PM"
    QTextEdit   *m_editor;      // main writing area
    QLabel      *m_statusLbl;   // "Entry saved!" / error messages

    // ── Read panel ────────────────────────────────────────────────────────────
    QListWidget *m_entryList;   // list of past entries (newest first)
    QTextEdit   *m_preview;     // read-only preview of the selected entry

    QTimer      *m_timer;       // 1-second tick → updateClock()
};

#endif // JOURNAL_H
