#include "Resource.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <set>
#include <sstream>

static std::string toLower(std::string text) {
    for (char& ch : text)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return text;
}

static std::vector<std::string> splitWords(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream stream(toLower(text));
    std::string word;

    while (stream >> word)
        words.push_back(word);

    return words;
}

static int scoreText(const std::string& text, const std::string& query, int weight) {
    const std::string lowerText = toLower(text);
    const std::string lowerQuery = toLower(query);

    if (lowerText == lowerQuery)
        return weight + 80;
    if (lowerText.rfind(lowerQuery, 0) == 0)
        return weight + 50;
    if (lowerText.find(lowerQuery) != std::string::npos)
        return weight + 25;

    int score = 0;
    for (const std::string& word : splitWords(lowerQuery)) {
        if (lowerText.find(word) != std::string::npos)
            score += 10;
    }

    return score;
}

Resource::Resource(const std::string& topic,
                   const std::string& subtopic,
                   const std::string& description,
                   const std::string& contact,
                   const std::vector<std::string>& keywords)
    : m_topic(topic),
      m_subtopic(subtopic),
      m_description(description),
      m_contact(contact),
      m_keywords(keywords) {}

std::string Resource::topic() const {
    return m_topic;
}

std::string Resource::subtopic() const {
    return m_subtopic;
}

std::string Resource::description() const {
    return m_description;
}

std::string Resource::contact() const {
    return m_contact;
}

int Resource::searchScore(const std::string& query) const {
    if (query.empty())
        return 0;

    int score = 0;
    score += scoreText(m_topic, query, 100);
    score += scoreText(m_subtopic, query, 90);
    score += scoreText(m_description, query, 50);
    score += scoreText(m_contact, query, 30);

    for (const std::string& keyword : m_keywords)
        score += scoreText(keyword, query, 40);

    return score;
}

void Resource::print() const {
    std::cout << "\nTopic: " << m_topic << '\n';
    std::cout << "Subtopic: " << m_subtopic << '\n';
    std::cout << "Description: " << m_description << '\n';
    std::cout << "Contact/Link: " << m_contact << '\n';
}

ResourceSearch::ResourceSearch() {
    m_resources = {
        Resource("Exam & Study Stress",
                 "Learning Resource Center Tutoring",
                 "Free tutoring for math, English, science, business, and more.",
                 "Room S-510 | lrctutoring@bmcc.cuny.edu",
                 {"study", "exam", "tutoring", "homework", "academic"}),

        Resource("Exam & Study Stress",
                 "Writing Center",
                 "One-on-one help with essays, research papers, and reports.",
                 "Room S-510 | writingcenter@bmcc.cuny.edu",
                 {"writing", "essay", "paper", "english"}),

        Resource("Exam & Study Stress",
                 "Free Software for Students",
                 "Digital Education Center support for Brightspace, Zoom, Office 365, LinkedIn Learning, VoiceThread, and CUNY technology tools.",
                 "digitaleducation@bmcc.cuny.edu | (212) 220-1243 | 199 Chambers St, Room S-510a | Mon-Fri 9 a.m.-5 p.m. | Software: https://www.bmcc.cuny.edu/academics/digitaleducation/student-support/software-for-students/ | Brightspace: https://brightspace.cuny.edu/d2l/home | Zoom: https://www.bmcc.cuny.edu/academics/digitaleducation/student-support/zoom-for-students/ | Office 365: https://www.cuny.edu/about/administration/offices/cis/technology-services/microsoft-office-365-for-education/ | LinkedIn Learning: https://www.linkedin.com/learning-login/ | VoiceThread: https://voicethread.com/howto/ | CUNY Technology Services: https://www.cuny.edu/about/administration/offices/cis/technology-services/#student",
                 {"software", "free software", "brightspace", "zoom", "office 365", "microsoft", "linkedin learning", "voicethread", "digital education", "technology"}),

        Resource("Finance",
                 "Panther Pantry",
                 "Free food for students facing food insecurity.",
                 "Room S-230",
                 {"food", "free food", "pantry", "hungry", "groceries"}),

        Resource("Finance",
                 "Student Emergency Fund",
                 "Emergency support for urgent student needs.",
                 "Room S-230 | (212) 220-8195",
                 {"money", "emergency", "rent", "textbooks", "fund"}),

        Resource("Immigration",
                 "Know Your Rights at BMCC",
                 "Information for immigrant and undocumented students.",
                 "Explain where students can find the Know Your Rights PDF.",
                 {"immigration", "rights", "undocumented", "daca", "legal"}),

        Resource("Relationships & Family",
                 "Personal Counseling",
                 "Free and confidential support for personal, family, and relationship issues.",
                 "Room S-343 | counselingcenter@bmcc.cuny.edu",
                 {"family", "relationship", "counseling", "therapy"}),

        Resource("Health & Wellness",
                 "Mental Health Counseling",
                 "Short-term counseling, crisis support, and referrals.",
                 "Room S-343 | (212) 220-8140",
                 {"mental health", "stress", "crisis", "wellness"}),

        Resource("Health & Wellness",
                 "Campus Fitness",
                 "Fitness Center, Swimming Pool, and Health & Wellness Services.",
                 "BMCC Athletics | (212) 220-8260 | Schedule: https://bmccathletics.com/sports/2012/4/2/RecreationSchedule.aspx | Visit: https://www.bmcc.cuny.edu/students/fitness-center/",
                 {"fitness", "gym", "athletics", "swimming", "pool", "exercise"})
    };
}

std::vector<std::string> ResourceSearch::topics() const {
    std::set<std::string> uniqueTopics;
    for (const Resource& resource : m_resources)
        uniqueTopics.insert(resource.topic());

    return std::vector<std::string>(uniqueTopics.begin(), uniqueTopics.end());
}

std::vector<Resource> ResourceSearch::resourcesByTopic(const std::string& topic) const {
    std::vector<Resource> matches;

    for (const Resource& resource : m_resources) {
        if (resource.topic() == topic)
            matches.push_back(resource);
    }

    return matches;
}

std::vector<Resource> ResourceSearch::search(const std::string& query) const {
    std::vector<Resource> matches;

    for (const Resource& resource : m_resources) {
        if (resource.searchScore(query) > 0)
            matches.push_back(resource);
    }

    std::sort(matches.begin(), matches.end(),
              [&query](const Resource& a, const Resource& b) {
                  return a.searchScore(query) > b.searchScore(query);
              });

    return matches;
}

void ResourceSearch::printTopics() const {
    const std::vector<std::string> topicList = topics();

    std::cout << "\nAvailable topics:\n";
    for (int i = 0; i < static_cast<int>(topicList.size()); ++i)
        std::cout << i + 1 << ". " << topicList[i] << '\n';
}
