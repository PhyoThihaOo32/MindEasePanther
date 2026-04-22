#ifndef ASSISTANTCHAT_H
#define ASSISTANTCHAT_H

#include "core/screen.h"

#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

class QNetworkReply;

class AssistantChat : public Screen {
    Q_OBJECT

public:
    explicit AssistantChat(QWidget *parent = nullptr);

    QString screenId() const override { return "assistant"; }

private slots:
    void sendMessage();

private:
    QWidget    *makeMessageRow(const QString &text, bool fromUser, bool crisis) const;

    void addMessage(const QString &text, bool fromUser, bool crisis = false);
    void handleReply(QNetworkReply *reply);
    void scrollToBottom();
    void setBusy(bool busy);
    QString backendUrl() const;

    QNetworkAccessManager *m_network;
    QScrollArea           *m_scroll;
    QWidget               *m_messagesWidget;
    QVBoxLayout           *m_messagesLayout;
    QLineEdit             *m_input;
    QPushButton           *m_sendBtn;
    QLabel                *m_statusLbl;
    bool                   m_waitingForReply;
};

#endif // ASSISTANTCHAT_H
