#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <vector>

class Resource {
private:
    std::string m_topic;
    std::string m_subtopic;
    std::string m_description;
    std::string m_contact;
    std::vector<std::string> m_keywords;

public:
    Resource(const std::string& topic,
             const std::string& subtopic,
             const std::string& description,
             const std::string& contact,
             const std::vector<std::string>& keywords);

    std::string topic() const;
    std::string subtopic() const;
    std::string description() const;
    std::string contact() const;

    int searchScore(const std::string& query) const;
    void print() const;
};

class ResourceSearch {
private:
    std::vector<Resource> m_resources;

public:
    ResourceSearch();

    std::vector<std::string> topics() const;
    std::vector<Resource> resourcesByTopic(const std::string& topic) const;
    std::vector<Resource> search(const std::string& query) const;
    void printTopics() const;
};

#endif
