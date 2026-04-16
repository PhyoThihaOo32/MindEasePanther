#ifndef JOURNAL_H
#define JOURNAL_H

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>
#include <QTimer>

// ─────────────────────────────────────────────────────────────────────────────
// Journal — personal wellness journal with local file persistence
//
// Entries are saved to:
//   ~/Documents/MindEase_Journal/YYYY-MM-DD_HH-MM-SS.txt
// Each file begins with a human-readable header, then the body text.
// ─────────────────────────────────────────────────────────────────────────────

class Journal : public QWidget {
    Q_OBJECT

public:
    explicit Journal(QWidget *parent = nullptr);

private slots:
    void saveEntry();
    void clearEditor();
    void loadSelectedEntry(QListWidgetItem *item);
    void updateClock();

private:
    void     refreshEntryList();
    void     showStatus(const QString &msg, bool ok = true);
    QString  journalDir() const;

    // ── Write panel ───────────────────────────────────────────────────────
    QLabel      *m_dateLbl;     // "Wednesday, April 15, 2026"
    QLabel      *m_timeLbl;     // live clock  "2:30:05 PM"
    QTextEdit   *m_editor;      // main writing area
    QLabel      *m_statusLbl;   // "Entry saved!" / "Editor cleared"

    // ── Read panel ────────────────────────────────────────────────────────
    QListWidget *m_entryList;   // list of past entries
    QTextEdit   *m_preview;     // read-only preview of selected entry

    QTimer      *m_timer;       // 1-second tick for live clock
};

#endif // JOURNAL_H
