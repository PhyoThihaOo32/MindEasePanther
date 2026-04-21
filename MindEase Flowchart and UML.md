# MindEase Flowchart and UML

This document reflects the current active Qt/C++ application in this repository.
It matches the refactored project structure and includes the current search logic
for BMCC Resources plus journal save and delete behavior.

## 1. Active Project Structure

```text
MindEase/
|- app/
|  |- main.cpp
|  |- mainwindow.h
|  `- mainwindow.cpp
|- core/
|  |- screen.h
|  `- screen.cpp
|- screens/
|  |- recommendations.h
|  |- recommendations.cpp
|  |- toolkit.h
|  |- toolkit.cpp
|  |- journal.h
|  `- journal.cpp
|- models/
|  |- journalentry.h
|  `- journalentry.cpp
`- storage/
   |- journalstorage.h
   `- journalstorage.cpp
```

## 2. Application Flowchart

```mermaid
flowchart TD
    A([Start MindEase]) --> B["app/main.cpp creates QApplication"]
    B --> C["Create MainWindow"]
    C --> D["applyStyle()"]
    D --> E["Create central widget and main layout"]
    E --> F["Instantiate screens as Screen*"]
    F --> F1["Recommendations"]
    F --> F2["Toolkit"]
    F --> F3["Journal"]
    F1 --> G["Add screens to QStackedWidget"]
    F2 --> G
    F3 --> G
    G --> H["buildSidebar()"]
    H --> I["switchScreen(0)"]
    I --> J["Recommendations.onActivated()"]
    J --> K["Show MainWindow"]
    K --> L{"User action"}

    L -->|Sidebar click| M["MainWindow::switchScreen(index)"]
    M --> N["Set current stacked page"]
    N --> O["Update checked nav button"]
    O --> P{"Selected screen"}

    P -->|Resources| Q["Recommendations.onActivated()"]
    Q --> Q1["Clear search and return to home page"]

    P -->|Toolkit| R["Toolkit.onActivated()"]
    R --> R1["Close open accordion folder"]

    P -->|Journal| S["Journal.onActivated()"]
    S --> S1["refreshEntryList()"]
    S1 --> S2["updateClock()"]
```

## 3. BMCC Resources Flowchart

```mermaid
flowchart TD
    A([Open BMCC Resources]) --> B["Recommendations.onActivated()"]
    B --> C["Clear search input"]
    C --> D["applyTopicSearch(empty)"]
    D --> E["Show all topic cards"]
    E --> F["Stack page 0 stays active"]
    F --> G{"User action"}

    G -->|Type in search bar| H["applyTopicSearch(query)"]
    H --> I["Normalize query text"]
    I --> J["Loop through all topics"]
    J --> K["searchTopic(topicIndex, query)"]
    K --> L["scoreTextMatch() against topic title and subtitle"]
    L --> M["scoreTextMatch() against resource title, description, details, and tags"]
    M --> N{"Any match?"}

    N -->|No| O["Hide card and keep default subtitle"]
    N -->|Yes| P["Show card"]
    P --> Q["If subtopic matched, replace subtitle with matching resource summary"]
    O --> R["Update search status label"]
    Q --> R

    R --> S{"User presses Enter or Search button?"}
    S -->|No| G
    S -->|Yes| T["openBestTopicMatch()"]
    T --> U["Rescore all topics and choose highest score"]
    U --> V{"Best match found?"}
    V -->|No| G
    V -->|Yes| W["showTopic(bestIndex)"]
    W --> X["QStackedWidget switches to topic detail page"]
    X --> Y["User reads resource cards and opens external links in browser"]
    Y --> Z{"Back clicked?"}
    Z -->|Yes| AA["showHome()"]
    AA --> F
```

## 4. Mental Health Toolkit Flowchart

```mermaid
flowchart TD
    A([Open Toolkit]) --> B["Toolkit.onActivated()"]
    B --> C{"Is a folder already open?"}
    C -->|Yes| D["toggleFolder(openFolder) closes it"]
    C -->|No| E["Leave accordion collapsed"]
    D --> E
    E --> F{"User clicks a folder card"}
    F --> G["toggleFolder(id)"]
    G --> H["Check if clicked folder is already open"]
    H --> I["Hide all panels"]
    I --> J["Reset all folder card styles"]
    J --> K["Reset all chevrons to closed state"]
    K --> L{"Was same folder already open?"}
    L -->|Yes| M["Stop with all folders collapsed"]
    L -->|No| N["Set openFolder = id"]
    N --> O["Apply accent style to selected card"]
    O --> P["Change selected chevron to open state"]
    P --> Q["Show selected folder panel"]
    Q --> R["User views tool items and external links"]
    R --> F
    M --> F
```

## 5. Journal Flowchart

```mermaid
flowchart TD
    A([Open Journal]) --> B["Journal.onActivated()"]
    B --> C["refreshEntryList()"]
    C --> D["JournalStorage.loadEntries()"]
    D --> E["Read all .txt files from Documents/MindEase_Journal"]
    E --> F["Parse each file into JournalEntry"]
    F --> G["Sort newest first"]
    G --> H["Populate QListWidget"]
    H --> I["Select previous item or newest item"]
    I --> J["loadSelectedEntry()"]
    J --> K["Preview selected entry"]
    K --> L{"User action"}

    L -->|Write and Save| M["saveEntry()"]
    M --> N{"Editor empty?"}
    N -->|Yes| O["Show error status"]
    N -->|No| P["Create JournalEntry(now, text)"]
    P --> Q["JournalStorage.saveEntry(entry)"]
    Q --> R["Create directory if needed"]
    R --> S["Write plain-text file"]
    S --> T["Clear editor"]
    T --> U["refreshEntryList()"]

    L -->|Select past entry| V["loadSelectedEntry(item)"]
    V --> W["JournalStorage.loadEntry(path)"]
    W --> X["Read file and parse header/body"]
    X --> Y["Render preview text"]

    L -->|Delete selected| Z["deleteSelectedEntry()"]
    Z --> ZA{"Entry selected?"}
    ZA -->|No| ZB["Show error status"]
    ZA -->|Yes| ZC["Show confirmation dialog"]
    ZC --> ZD{"User confirms?"}
    ZD -->|No| L
    ZD -->|Yes| ZE["JournalStorage.deleteEntry(path)"]
    ZE --> ZF["Remove local .txt file"]
    ZF --> ZG["refreshEntryList()"]
    ZG --> ZH["Show success status"]
    ZH --> L

    O --> L
    U --> L
    Y --> L
    ZB --> L
```

## 6. UML Class Diagram

```mermaid
classDiagram
    class MainWindow {
        -QWidget* centralWidget
        -QHBoxLayout* mainLayout
        -QWidget* sidebar
        -QStackedWidget* stack
        -QVector~QPushButton*~ navButtons
        -QVector~Screen*~ screens
        +MainWindow(QWidget* parent = nullptr)
        +~MainWindow()
        -buildSidebar()
        -applyStyle()
        -switchScreen(int index)
    }

    class Screen {
        <<abstract>>
        -QString m_title
        +Screen(const QString& title, QWidget* parent = nullptr)
        +QString title() const
        +QString screenId() const
        +void onActivated()
        #buildHeader(QVBoxLayout* root)
    }

    class Recommendations {
        -QStackedWidget* m_stack
        -QLineEdit* m_searchInput
        -QLabel* m_searchStatus
        -QVector~QWidget*~ m_topicCards
        -QVector~QLabel*~ m_topicSubtitleLabels
        +Recommendations(QWidget* parent = nullptr)
        +QString screenId() const
        +void onActivated()
        -applyTopicSearch(const QString& query)
        -openBestTopicMatch()
        -showTopic(int index)
        -showHome()
    }

    class Toolkit {
        -QMap~QString,QFrame*~ panels
        -QMap~QString,QWidget*~ folderCards
        -QMap~QString,QLabel*~ chevrons
        -QString openFolder
        +Toolkit(QWidget* parent = nullptr)
        +QString screenId() const
        +void onActivated()
        -toggleFolder(const QString& id)
    }

    class Journal {
        -QLabel* m_dateLbl
        -QLabel* m_timeLbl
        -QTextEdit* m_editor
        -QLabel* m_statusLbl
        -QListWidget* m_entryList
        -QTextEdit* m_preview
        -QPushButton* m_deleteBtn
        -QTimer* m_timer
        -JournalStorage m_storage
        +Journal(QWidget* parent = nullptr)
        +QString screenId() const
        +void onActivated()
        -saveEntry()
        -deleteSelectedEntry()
        -clearEditor()
        -loadSelectedEntry(QListWidgetItem* item)
        -updateClock()
        -refreshEntryList()
        -syncEntryActions()
        -showStatus(const QString& msg, bool ok = true)
    }

    class JournalEntry {
        -QDateTime m_dateTime
        -QString m_body
        -QString m_filePath
        +JournalEntry()
        +JournalEntry(const QDateTime& dt, const QString& body, const QString& filePath = QString())
        +QDateTime dateTime() const
        +QString body() const
        +QString filePath() const
        +bool isValid() const
        +bool operator<(const JournalEntry& other) const
        +bool operator==(const JournalEntry& other) const
    }

    class JournalStorage {
        -QString m_directoryPath
        +JournalStorage()
        +JournalStorage(const QString& directoryPath)
        +QString directoryPath() const
        +bool saveEntry(const JournalEntry& entry) const
        +bool deleteEntry(const QString& path) const
        +QVector~JournalEntry~ loadEntries() const
        +JournalEntry loadEntry(const QString& path) const
        -QString resolveDirectoryPath() const
        -QString buildEntryPath(const QDateTime& dateTime) const
        -QString formatEntry(const JournalEntry& entry) const
        -JournalEntry parseEntryFile(const QString& path, const QString& content) const
    }

    class TopicInfo {
        +QString icon
        +QString accent
        +QString title
        +QString subtitle
    }

    class ResourceInfo {
        +QString icon
        +QString color
        +QString title
        +QString description
        +QString details
        +QString url
        +QStringList tags
        +QString url2
        +QString url2Label
    }

    class TopicSearchResult {
        +int score
        +QStringList matchedSubtopics
    }

    class FolderDef {
        +QString id
        +QString icon
        +QString accent
        +QString accentBorder
        +QString name
        +QString tagline
        +QString intro
        +QList~ToolItem~ items
    }

    class ToolItem {
        +QString icon
        +QString title
        +QString body
        +QString url
        +QString urlLabel
    }

    Screen <|-- Recommendations
    Screen <|-- Toolkit
    Screen <|-- Journal
    MainWindow *-- Screen
    Journal *-- JournalStorage
    Journal ..> JournalEntry
    JournalStorage ..> JournalEntry
    Recommendations ..> TopicInfo
    Recommendations ..> ResourceInfo
    Recommendations ..> TopicSearchResult
    Toolkit ..> FolderDef
    FolderDef *-- ToolItem
```

## 7. UML Sequence Diagram: Navigation

```mermaid
sequenceDiagram
    participant User
    participant Sidebar as Sidebar Button
    participant MainWindow
    participant Stack as QStackedWidget
    participant Screen as Screen*

    User->>Sidebar: Click navigation button
    Sidebar->>MainWindow: switchScreen(index)
    MainWindow->>Stack: setCurrentIndex(index)
    MainWindow->>MainWindow: Update checked nav buttons
    MainWindow->>Screen: onActivated()
    alt Resources
        Screen-->>MainWindow: Clear search and go home
    else Toolkit
        Screen-->>MainWindow: Close open accordion
    else Journal
        Screen-->>MainWindow: Refresh entries and update clock
    end
```

## 8. UML Sequence Diagram: Resources Search

```mermaid
sequenceDiagram
    participant User
    participant Input as Search Input
    participant Rec as Recommendations
    participant Search as searchTopic()
    participant Stack as QStackedWidget

    User->>Input: Type query
    Input->>Rec: applyTopicSearch(query)
    Rec->>Rec: Normalize query
    loop For each topic
        Rec->>Search: searchTopic(topicIndex, query)
        Search->>Search: score topic title/subtitle
        Search->>Search: score resource title/description/details/tags
        Search-->>Rec: TopicSearchResult
    end
    Rec->>Rec: Show matching cards and subtopic summaries
    User->>Input: Press Enter / click Search
    Input->>Rec: openBestTopicMatch()
    Rec->>Rec: Pick highest score
    Rec->>Stack: setCurrentIndex(bestIndex + 1)
```

## 9. UML Sequence Diagram: Journal Save and Delete

```mermaid
sequenceDiagram
    participant User
    participant Journal
    participant Storage as JournalStorage
    participant Entry as JournalEntry
    participant FS as File System

    alt Save entry
        User->>Journal: Click Save Entry
        Journal->>Journal: Validate editor text
        Journal->>Entry: Create JournalEntry(now, text)
        Journal->>Storage: saveEntry(entry)
        Storage->>FS: Ensure directory exists
        Storage->>FS: Write yyyy-MM-dd_HH-mm-ss.txt
        Storage-->>Journal: success/failure
        Journal->>Journal: Clear editor and refresh list
    else Delete entry
        User->>Journal: Click Delete Selected
        Journal->>Journal: Read selected file path
        Journal->>User: Show confirmation dialog
        User-->>Journal: Confirm deletion
        Journal->>Storage: deleteEntry(path)
        Storage->>FS: Remove selected .txt file
        Storage-->>Journal: success/failure
        Journal->>Journal: Refresh list and preview
    end
```

## 10. Key Design Notes

- `MainWindow` owns the three concrete screens and talks to them through `Screen*`.
- `Screen` is the abstraction layer shared by every page.
- `Recommendations` combines topic browsing with a weighted subtopic search algorithm.
- `Toolkit` is an accordion-style wellness library driven by static folder data.
- `Journal` owns UI state only; persistent file operations are delegated to `JournalStorage`.
- `JournalEntry` is the plain data model for one saved entry.

