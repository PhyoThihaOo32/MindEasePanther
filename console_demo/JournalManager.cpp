#include "JournalManager.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

JournalManager::JournalManager(const std::string& filePrefix)
    : m_filePrefix(filePrefix), m_indexFile(filePrefix + "_index.txt") {}

std::string JournalManager::currentDateTime() const {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    const std::tm localTime = *std::localtime(&nowTime);

    std::ostringstream out;
    out << std::put_time(&localTime, "%Y-%m-%d %I:%M %p");
    return out.str();
}

std::string JournalManager::makeFileName() const {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    const std::tm localTime = *std::localtime(&nowTime);

    std::ostringstream out;
    out << m_filePrefix << "_entry_"
        << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S")
        << ".txt";
    return out.str();
}

std::vector<std::string> JournalManager::entryFiles() const {
    std::vector<std::string> files;
    std::ifstream in(m_indexFile);
    std::string fileName;

    while (std::getline(in, fileName)) {
        if (!fileName.empty())
            files.push_back(fileName);
    }

    std::sort(files.rbegin(), files.rend());
    return files;
}

void JournalManager::addToIndex(const std::string& fileName) const {
    std::ofstream out(m_indexFile, std::ios::app);
    out << fileName << '\n';
}

void JournalManager::rewriteIndex(const std::vector<std::string>& files) const {
    std::ofstream out(m_indexFile);
    for (const std::string& fileName : files)
        out << fileName << '\n';
}

bool JournalManager::saveEntry(const std::string& title, const std::string& body) const {
    if (title.empty() || body.empty())
        return false;

    const JournalEntry entry(title, body, currentDateTime());
    const std::string fileName = makeFileName();
    std::ofstream out(fileName);

    if (!out)
        return false;

    out << entry.toFileText();
    out.close();
    addToIndex(fileName);
    return true;
}

std::vector<JournalEntry> JournalManager::loadEntries() const {
    std::vector<JournalEntry> entries;

    for (const std::string& fileName : entryFiles()) {
        std::ifstream in(fileName);
        std::stringstream buffer;
        buffer << in.rdbuf();

        JournalEntry entry = JournalEntry::fromFileText(buffer.str());
        if (entry.isValid())
            entries.push_back(entry);
    }

    return entries;
}

bool JournalManager::deleteEntry(int entryNumber) const {
    std::vector<std::string> files = entryFiles();

    if (entryNumber < 1 || entryNumber > static_cast<int>(files.size()))
        return false;

    const std::string fileToDelete = files[entryNumber - 1];
    if (std::remove(fileToDelete.c_str()) != 0)
        return false;

    files.erase(files.begin() + entryNumber - 1);
    rewriteIndex(files);
    return true;
}
