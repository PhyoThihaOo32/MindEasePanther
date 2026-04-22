#include "JournalEntry.h"

#include <sstream>

JournalEntry::JournalEntry() {}

JournalEntry::JournalEntry(const std::string& title,
                           const std::string& body,
                           const std::string& dateTime)
    : m_title(title), m_body(body), m_dateTime(dateTime) {}

std::string JournalEntry::title() const {
    return m_title;
}

std::string JournalEntry::body() const {
    return m_body;
}

std::string JournalEntry::dateTime() const {
    return m_dateTime;
}

bool JournalEntry::isValid() const {
    return !m_title.empty() && !m_body.empty() && !m_dateTime.empty();
}

std::string JournalEntry::toFileText() const {
    std::ostringstream out;
    out << "Title: " << m_title << '\n';
    out << "Date: " << m_dateTime << '\n';
    out << "---\n";
    out << m_body << '\n';
    return out.str();
}

JournalEntry JournalEntry::fromFileText(const std::string& text) {
    std::istringstream in(text);
    std::string titleLine;
    std::string dateLine;
    std::string divider;

    std::getline(in, titleLine);
    std::getline(in, dateLine);
    std::getline(in, divider);

    std::string body;
    std::string line;
    while (std::getline(in, line))
        body += line + '\n';

    const std::string titlePrefix = "Title: ";
    const std::string datePrefix = "Date: ";

    const std::string title = titleLine.rfind(titlePrefix, 0) == 0
        ? titleLine.substr(titlePrefix.length())
        : "";

    const std::string dateTime = dateLine.rfind(datePrefix, 0) == 0
        ? dateLine.substr(datePrefix.length())
        : "";

    return JournalEntry(title, body, dateTime);
}

