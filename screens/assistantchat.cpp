#include "screens/assistantchat.h"

#include <QColor>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QtGlobal>

static void applyAssistantGlow(QWidget *widget,
                               int blur = 32,
                               int yOffset = 8,
                               const QColor &color = QColor(139, 223, 242, 36)) {
    auto *shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(blur);
    shadow->setOffset(0, yOffset);
    shadow->setColor(color);
    widget->setGraphicsEffect(shadow);
}

AssistantChat::AssistantChat(QWidget *parent)
    : Screen("MindEase Assistant", parent),
      m_network(new QNetworkAccessManager(this)),
      m_scroll(nullptr),
      m_messagesWidget(nullptr),
      m_messagesLayout(nullptr),
      m_input(nullptr),
      m_sendBtn(nullptr),
      m_statusLbl(nullptr),
      m_waitingForReply(false) {

    setStyleSheet(R"(
        QLabel#assistantIntro {
            color: #d7fff1;
            font-size: 15px;
            line-height: 1.35;
        }
        QFrame#assistantPanel,
        QFrame#assistantSidePanel {
            border: 1px solid #8bdff2;
            border-radius: 28px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 rgba(231, 251, 255, 210),
                                        stop:0.46 rgba(255, 255, 255, 224),
                                        stop:1 rgba(233, 255, 243, 205));
        }
        QLabel#assistantPanelTitle,
        QLabel#assistantSideTitle {
            color: #082f49;
            font-size: 18px;
            font-weight: 850;
        }
        QLabel#assistantSmallCopy {
            color: #355d59;
            font-size: 11px;
            line-height: 1.35;
        }
        QLabel#assistantStatus {
            color: #4e706b;
            font-size: 12px;
            border: none;
            background: transparent;
        }
        QPushButton#linkPillBtn {
            background: #eefdf4;
            color: #087a5d;
            border: 1px solid #b8f4cf;
            border-radius: 12px;
            padding: 7px 10px;
            font-size: 11px;
            font-weight: 800;
        }
        QPushButton#linkPillBtn:hover {
            background: #dff6ff;
            border-color: #8bdff2;
            color: #064d5f;
        }
        QLineEdit#assistantInput {
            background: #ffffff;
            color: #082f49;
            border: 2px solid #8bdff2;
            border-radius: 18px;
            padding: 14px 16px;
            font-size: 15px;
            selection-background-color: #8ee2a8;
        }
        QLineEdit#assistantInput:focus {
            border-color: #8ee2a8;
            background: #f8ffff;
        }
    )");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(56, 42, 56, 42);
    root->setSpacing(22);

    buildHeader(root);

    QLabel *intro = new QLabel(
        "A calm student-support chat for stress, overwhelm, homesickness, motivation, and help-seeking.");
    intro->setObjectName("assistantIntro");
    intro->setWordWrap(true);
    root->addWidget(intro);

    QFrame *chatPanel = new QFrame();
    chatPanel->setObjectName("assistantPanel");
    applyAssistantGlow(chatPanel, 34, 8, QColor(139, 223, 242, 42));
    QVBoxLayout *chatLay = new QVBoxLayout(chatPanel);
    chatLay->setContentsMargins(32, 28, 32, 28);
    chatLay->setSpacing(16);

    QLabel *panelTitle = new QLabel("Talk through one next step");
    panelTitle->setObjectName("assistantPanelTitle");
    chatLay->addWidget(panelTitle);

    m_statusLbl = new QLabel("");
    m_statusLbl->setObjectName("assistantStatus");
    m_statusLbl->setWordWrap(true);
    m_statusLbl->setVisible(false);

    m_scroll = new QScrollArea();
    m_scroll->setWidgetResizable(true);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scroll->setMinimumHeight(420);

    m_messagesWidget = new QWidget();
    m_messagesWidget->setStyleSheet("background: transparent; border: none;");
    m_messagesLayout = new QVBoxLayout(m_messagesWidget);
    m_messagesLayout->setContentsMargins(2, 2, 12, 2);
    m_messagesLayout->setSpacing(12);
    m_messagesLayout->addStretch(1);
    m_scroll->setWidget(m_messagesWidget);
    chatLay->addWidget(m_scroll, 1);

    QHBoxLayout *inputLay = new QHBoxLayout();
    inputLay->setSpacing(12);

    m_input = new QLineEdit();
    m_input->setObjectName("assistantInput");
    m_input->setPlaceholderText("Type what is going on in one or two sentences...");
    inputLay->addWidget(m_input, 1);

    m_sendBtn = new QPushButton("Send");
    m_sendBtn->setObjectName("primaryBtn");
    m_sendBtn->setCursor(Qt::PointingHandCursor);
    inputLay->addWidget(m_sendBtn);
    chatLay->addLayout(inputLay);

    connect(m_sendBtn, &QPushButton::clicked, this, &AssistantChat::sendMessage);
    connect(m_input, &QLineEdit::returnPressed, this, &AssistantChat::sendMessage);

    root->addWidget(chatPanel, 1);

    addMessage(
        "Hi, I am MindEase. Tell me what feels heavy right now, and I will help you choose one small, realistic next step.",
        false);
}

QWidget *AssistantChat::makeMessageRow(const QString &text, bool fromUser, bool crisis) const {
    QFrame *row = new QFrame();
    row->setStyleSheet("background: transparent; border: none;");
    row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QHBoxLayout *rowLay = new QHBoxLayout(row);
    rowLay->setContentsMargins(0, 0, 0, 0);
    rowLay->setSpacing(8);

    QFrame *bubble = new QFrame();
    bubble->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    bubble->setMaximumWidth(fromUser ? 860 : 980);
    bubble->setMinimumWidth(fromUser ? 300 : 420);
    bubble->setStyleSheet(crisis
        ? "QFrame { background:#fff8f3; border:1px solid #f5b8a8; border-radius:20px; }"
        : fromUser
            ? "QFrame { background:#dff6ff; border:1px solid #8bdff2; border-radius:20px; }"
            : "QFrame { background:qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #f8ffff, stop:1 #e9fff3); border:1px solid #b8f4cf; border-radius:20px; }");
    if (fromUser) {
        applyAssistantGlow(bubble, 20, 3, QColor(139, 223, 242, 46));
    }

    QVBoxLayout *bubbleLay = new QVBoxLayout(bubble);
    bubbleLay->setContentsMargins(18, 14, 18, 14);

    QLabel *msg = new QLabel(text);
    msg->setWordWrap(true);
    msg->setTextFormat(Qt::PlainText);
    msg->setTextInteractionFlags(Qt::TextSelectableByMouse);
    msg->setMinimumWidth(fromUser ? 240 : 360);
    msg->setStyleSheet(crisis
        ? "color:#7c2d12; font-size:14px; line-height:1.35; border:none; background:transparent;"
        : fromUser
            ? "color:#082f49; font-size:14px; line-height:1.35; border:none; background:transparent;"
            : "color:#082f49; font-size:14px; line-height:1.35; border:none; background:transparent;");
    bubbleLay->addWidget(msg);

    if (fromUser) {
        rowLay->addStretch();
        rowLay->addWidget(bubble);
    } else {
        rowLay->addWidget(bubble);
        rowLay->addStretch();
    }

    return row;
}

void AssistantChat::addMessage(const QString &text, bool fromUser, bool crisis) {
    QWidget *row = makeMessageRow(text, fromUser, crisis);
    const int insertIndex = qMax(0, m_messagesLayout->count() - 1);
    m_messagesLayout->insertWidget(insertIndex, row);

    scrollToBottom();
}

void AssistantChat::sendMessage() {
    if (m_waitingForReply) return;

    const QString message = m_input->text().trimmed();
    if (message.isEmpty()) return;

    addMessage(message, true);
    m_input->clear();
    setBusy(true);
    scrollToBottom();

    QJsonObject payload;
    payload["message"] = message;

    QNetworkRequest request((QUrl(backendUrl())));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_network->post(
        request,
        QJsonDocument(payload).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleReply(reply);
    });
}

void AssistantChat::handleReply(QNetworkReply *reply) {
    const QByteArray body = reply->readAll();
    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const bool networkError = reply->error() != QNetworkReply::NoError;
    const QString errorText = reply->errorString();
    reply->deleteLater();

    setBusy(false);

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
    const QJsonObject obj = doc.isObject() ? doc.object() : QJsonObject{};

    if (networkError) {
        addMessage(
            "I could not reach the MindEase backend.\n\n"
            "Start it with: cd backend && npm install && npm run dev\n\n"
            "Details: " + errorText,
            false);
        return;
    }

    if (statusCode >= 400) {
        const QString serverError = obj.value("error").toString("The backend returned an error.");
        addMessage(serverError, false);
        return;
    }

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        addMessage("The backend returned an unreadable response. Please try again.", false);
        return;
    }

    const QString botReply = obj.value("reply").toString().trimmed();
    const bool crisis = obj.value("crisis").toBool(false);

    addMessage(
        botReply.isEmpty()
            ? "I am here with you. Try naming the main stressor, then choose one small next step."
            : botReply,
        false,
        crisis);
    scrollToBottom();
}

void AssistantChat::scrollToBottom() {
    if (!m_scroll || !m_scroll->verticalScrollBar()) return;

    auto scroll = [this]() {
        if (m_scroll && m_scroll->verticalScrollBar()) {
            QScrollBar *bar = m_scroll->verticalScrollBar();
            bar->setValue(bar->maximum());
        }
    };

    QTimer::singleShot(0, this, scroll);
    QTimer::singleShot(60, this, scroll);
    QTimer::singleShot(140, this, scroll);
}

void AssistantChat::setBusy(bool busy) {
    m_waitingForReply = busy;
    m_sendBtn->setEnabled(!busy);
    m_input->setEnabled(!busy);
    m_statusLbl->clear();
    m_statusLbl->setVisible(false);

    if (!busy) {
        m_input->setFocus();
    }
}

QString AssistantChat::backendUrl() const {
    QString baseUrl = qEnvironmentVariable("MINDEASE_API_URL", "http://127.0.0.1:8788");
    while (baseUrl.endsWith('/')) {
        baseUrl.chop(1);
    }
    return baseUrl + "/api/chat";
}
