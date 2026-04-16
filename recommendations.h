#ifndef RECOMMENDATIONS_H
#define RECOMMENDATIONS_H

#include <QWidget>
#include <QStackedWidget>

class Recommendations : public QWidget {
    Q_OBJECT
public:
    explicit Recommendations(QWidget *parent = nullptr);
private:
    void showTopic(int index);
    void showHome();

    QStackedWidget *m_stack;
};

#endif // RECOMMENDATIONS_H
