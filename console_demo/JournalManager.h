#ifndef JOURNALMANAGER_H
#define JOURNALMANAGER_H

#include "JournalEntry.h"

#include <string>
#include <vector>

class JournalManager {
private:
    std::string m_filePrefix;
    std::string m_indexFile;

    std::string makeFileName() const;
    std::string currentDateTime() const;
    std::vector<std::string> entryFiles() const;
    void addToIndex(const std::string& fileName) const;
    void rewriteIndex(const std::vector<std::string>& files) const;

public:
    explicit JournalManager(const std::string& filePrefix);

    bool saveEntry(const std::string& title, const std::string& body) const;
    std::vector<JournalEntry> loadEntries() const;
    bool deleteEntry(int entryNumber) const;
};

#endif
