#include "JournalManager.h"
#include "Resource.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

struct ToolkitOption {
    std::string title;
    std::string details;
};

struct ToolkitCategory {
    std::string title;
    std::vector<ToolkitOption> options;
};

static void clearInput() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readNumber(const std::string& prompt) {
    std::cout << prompt;

    int value;
    if (!(std::cin >> value)) {
        std::cin.clear();
        clearInput();
        return -1;
    }

    clearInput();
    return value;
}

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

static std::string readMultilineBody() {
    std::cout << "Write your journal body. Type a single . on its own line to finish.\n";

    std::string body;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == ".")
            break;
        body += line + '\n';
    }

    return body;
}

static void browseResourcesByTopic(const ResourceSearch& searcher) {
    int topicChoice = -1;

    while (topicChoice != 0) {
        const std::vector<std::string> topics = searcher.topics();

        std::cout << "\nBMCC Resource Topics\n";
        for (int i = 0; i < static_cast<int>(topics.size()); ++i)
            std::cout << i + 1 << ". " << topics[i] << '\n';
        std::cout << "0. Back to resource menu\n";

        topicChoice = readNumber("Choose a topic: ");

        if (topicChoice == 0)
            return;
        if (topicChoice < 1 || topicChoice > static_cast<int>(topics.size())) {
            std::cout << "Invalid topic choice.\n";
            continue;
        }

        const std::string selectedTopic = topics[topicChoice - 1];
        const std::vector<Resource> resources = searcher.resourcesByTopic(selectedTopic);

        int subtopicChoice = -1;
        while (subtopicChoice != 0) {
            std::cout << "\n" << selectedTopic << " Subtopics\n";
            for (int i = 0; i < static_cast<int>(resources.size()); ++i)
                std::cout << i + 1 << ". " << resources[i].subtopic() << '\n';
            std::cout << "0. Back to topics\n";

            subtopicChoice = readNumber("Choose a subtopic to view: ");

            if (subtopicChoice == 0)
                break;
            if (subtopicChoice < 1 || subtopicChoice > static_cast<int>(resources.size())) {
                std::cout << "Invalid subtopic choice.\n";
                continue;
            }

            resources[subtopicChoice - 1].print();
        }
    }
}

static void searchResourcesByKeyword(const ResourceSearch& searcher) {
    const std::string query = readLine("\nType a topic, subtopic, or keyword: ");
    const std::vector<Resource> matches = searcher.search(query);

    if (matches.empty()) {
        std::cout << "No matching resources found.\n";
        return;
    }

    std::cout << "\nSearch Results\n";
    for (int i = 0; i < static_cast<int>(matches.size()); ++i) {
        std::cout << i + 1 << ". " << matches[i].topic()
                  << " - " << matches[i].subtopic() << '\n';
    }
    std::cout << "0. Back to resource menu\n";

    const int choice = readNumber("Choose a result to view: ");
    if (choice < 1 || choice > static_cast<int>(matches.size()))
        return;

    matches[choice - 1].print();
}

static void showAllResourceTopics(const ResourceSearch& searcher) {
    std::cout << "\nAll Topics and Subtopics\n";
    const std::vector<std::string> topics = searcher.topics();

    for (const std::string& topic : topics) {
        std::cout << "\n" << topic << '\n';
        const std::vector<Resource> resources = searcher.resourcesByTopic(topic);
        for (int i = 0; i < static_cast<int>(resources.size()); ++i)
            std::cout << "  " << i + 1 << ". " << resources[i].subtopic() << '\n';
    }
}

static void resourceMenu(const ResourceSearch& searcher) {
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n==============================\n";
        std::cout << "Option 1: BMCC Resources\n";
        std::cout << "==============================\n";
        std::cout << "1. Browse by numbered topic\n";
        std::cout << "2. Search topic or subtopic\n";
        std::cout << "3. Show all topics and subtopics\n";
        std::cout << "0. Back to main menu\n";

        choice = readNumber("Choose: ");

        switch (choice) {
        case 1:
            browseResourcesByTopic(searcher);
            break;
        case 2:
            searchResourcesByKeyword(searcher);
            break;
        case 3:
            showAllResourceTopics(searcher);
            break;
        case 0:
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}

static void showToolkitCategory(const ToolkitCategory& category) {
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n" << category.title << '\n';
        for (int i = 0; i < static_cast<int>(category.options.size()); ++i)
            std::cout << i + 1 << ". " << category.options[i].title << '\n';
        std::cout << "0. Back to toolkit menu\n";

        choice = readNumber("Choose what to view: ");

        if (choice == 0)
            return;
        if (choice < 1 || choice > static_cast<int>(category.options.size())) {
            std::cout << "Invalid choice.\n";
            continue;
        }

        const ToolkitOption& option = category.options[choice - 1];
        std::cout << "\n" << option.title << '\n';
        std::cout << option.details << '\n';
    }
}

static std::vector<ToolkitCategory> buildToolkitCategories() {
    std::vector<ToolkitCategory> categories;

    categories.push_back({
        "Academic Stress",
        {
            {"Study Plan", "Pick the next assignment, break it into 3 small tasks, then work for 25 minutes."},
            {"Exam Prep", "Review notes, practice old problems, and ask for tutoring before the deadline."},
            {"Campus Help", "Use tutoring, writing support, professor office hours, or academic advising."}
        }
    });

    categories.push_back({
        "Mindfulness & Meditation",
        {
            {"Breathing", "Try 4 slow breaths: inhale, hold, exhale, pause. Repeat until your body settles."},
            {"Grounding", "Name 5 things you see, 4 things you feel, 3 things you hear, 2 things you smell, and 1 thing you taste."},
            {"Short Reset", "Step away from the screen, relax your shoulders, and drink water before returning."}
        }
    });

    categories.push_back({
        "Food, Sleep, and Energy",
        {
            {"Food Support", "If food is the issue, check Panther Pantry or emergency student support."},
            {"Sleep Reset", "Set a regular sleep time, reduce phone use before bed, and avoid late caffeine."},
            {"Energy Check", "Drink water, take a short walk, and eat something simple before studying."}
        }
    });

    categories.push_back({
        "Connection & Support",
        {
            {"Talk to Someone", "Reach out to a friend, advisor, counselor, professor, or trusted family member."},
            {"When It Is Urgent", "If there is immediate danger, call 911 or contact 988 for crisis support."},
            {"Peer Support", "Look for clubs, study groups, and campus events to reduce isolation."}
        }
    });

    return categories;
}

static void toolkitMenu() {
    const std::vector<ToolkitCategory> categories = buildToolkitCategories();
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n==============================\n";
        std::cout << "Option 2: Mental Health Toolkit\n";
        std::cout << "==============================\n";
        for (int i = 0; i < static_cast<int>(categories.size()); ++i)
            std::cout << i + 1 << ". " << categories[i].title << '\n';
        std::cout << "0. Back to main menu\n";

        choice = readNumber("Choose a toolkit category: ");

        if (choice == 0)
            return;
        if (choice < 1 || choice > static_cast<int>(categories.size())) {
            std::cout << "Invalid choice.\n";
            continue;
        }

        showToolkitCategory(categories[choice - 1]);
    }
}

static void saveJournalEntry(const JournalManager& journal) {
    const std::string title = readLine("Entry title: ");
    const std::string body = readMultilineBody();

    if (journal.saveEntry(title, body))
        std::cout << "Journal entry saved.\n";
    else
        std::cout << "Could not save. Title and body are required.\n";
}

static void listJournalEntries(const JournalManager& journal) {
    const std::vector<JournalEntry> entries = journal.loadEntries();

    if (entries.empty()) {
        std::cout << "No journal entries yet.\n";
        return;
    }

    std::cout << "\nSaved journal entries:\n";
    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
        std::cout << i + 1 << ". " << entries[i].title()
                  << " (" << entries[i].dateTime() << ")\n";
    }

    const int choice = readNumber("\nEnter entry number to read, 0 to return: ");

    if (choice <= 0 || choice > static_cast<int>(entries.size()))
        return;

    const JournalEntry& entry = entries[choice - 1];
    std::cout << "\nTitle: " << entry.title() << '\n';
    std::cout << "Date: " << entry.dateTime() << "\n\n";
    std::cout << entry.body() << '\n';
}

static void deleteJournalEntry(const JournalManager& journal) {
    const std::vector<JournalEntry> entries = journal.loadEntries();

    if (entries.empty()) {
        std::cout << "No journal entries to delete.\n";
        return;
    }

    std::cout << "\nChoose entry to delete:\n";
    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
        std::cout << i + 1 << ". " << entries[i].title()
                  << " (" << entries[i].dateTime() << ")\n";
    }

    const int choice = readNumber("Entry number, 0 to cancel: ");
    if (choice == 0)
        return;

    if (journal.deleteEntry(choice))
        std::cout << "Entry deleted.\n";
    else
        std::cout << "Invalid entry number.\n";
}

static void journalMenu(const JournalManager& journal) {
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n==============================\n";
        std::cout << "Option 3: My Journal\n";
        std::cout << "==============================\n";
        std::cout << "1. Write a new entry\n";
        std::cout << "2. List and read entries\n";
        std::cout << "3. Delete an entry\n";
        std::cout << "0. Back to main menu\n";

        choice = readNumber("Choose: ");

        switch (choice) {
        case 1:
            saveJournalEntry(journal);
            break;
        case 2:
            listJournalEntries(journal);
            break;
        case 3:
            deleteJournalEntry(journal);
            break;
        case 0:
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}

int main() {
    ResourceSearch resourceSearch;
    JournalManager journal("journal");

    int choice = -1;
    while (choice != 0) {
        std::cout << "\n==============================\n";
        std::cout << "MindEase Console Demo\n";
        std::cout << "==============================\n";
        std::cout << "1. BMCC Resources\n";
        std::cout << "2. Mental Health Toolkit\n";
        std::cout << "3. My Journal\n";
        std::cout << "0. Exit\n";

        choice = readNumber("Choose: ");

        switch (choice) {
        case 1:
            resourceMenu(resourceSearch);
            break;
        case 2:
            toolkitMenu();
            break;
        case 3:
            journalMenu(journal);
            break;
        case 0:
            std::cout << "Goodbye.\n";
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }

    return 0;
}
