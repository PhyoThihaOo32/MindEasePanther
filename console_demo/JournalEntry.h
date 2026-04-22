#ifndef JOURNALENTRY_H
#define JOURNALENTRY_H

#include <string>

class JournalEntry {
private:
    std::string m_title;
    std::string m_body;
    std::string m_dateTime;

public:
    JournalEntry();
    JournalEntry(const std::string& title,
                 const std::string& body,
                 const std::string& dateTime);

    std::string title() const;
    std::string body() const;
    std::string dateTime() const;
    bool isValid() const;

    std::string toFileText() const;
    static JournalEntry fromFileText(const std::string& text);
};

#endif
