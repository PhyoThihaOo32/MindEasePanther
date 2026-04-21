# MindEase Flowchart and UML

These diagrams reflect the current refactored project structure:

- `app/` for startup and application shell
- `core/` for shared abstractions
- `screens/` for UI screens
- `models/` for data objects
- `storage/` for file I/O services
- `legacy/` for older mood-tracking experiments not built by `MindEase.pro`

## 1. Project Structure Diagram

```mermaid
flowchart TD
    A["MindEase/"] --> B["app/"]
    A --> C["core/"]
    A --> D["screens/"]
    A --> E["models/"]
    A --> F["storage/"]
    A --> G["legacy/"]

    B --> B1["main.cpp"]
    B --> B2["mainwindow.h/.cpp"]

    C --> C1["screen.h/.cpp"]

    D --> D1["recommendations.h/.cpp"]
    D --> D2["toolkit.h/.cpp"]
    D --> D3["journal.h/.cpp"]

    E --> E1["journalentry.h/.cpp"]

    F --> F1["journalstorage.h/.cpp"]

    G --> G1["moodentry.*"]
    G --> G2["moodcheckin.*"]
    G --> G3["moodlog.*"]
    G --> G4["statsview.*"]
    G --> G5["settingsview.*"]
```

## 2. Main System Flowchart

```mermaid
flowchart TD
    A([Start application]) --> B["app/main.cpp creates QApplication"]
    B --> C["Construct MainWindow"]
    C --> D["applyStyle()"]
    D --> E["Create central widget + sidebar + QStackedWidget"]
    E --> F["Instantiate screens as Screen*"]
    F --> F1["Recommendations"]
    F --> F2["Toolkit"]
    F --> F3["Journal"]
    F1 --> G["Add screens to stack"]
    F2 --> G
    F3 --> G
    G --> H["buildSidebar()"]
    H --> I["show MainWindow"]
    I --> J{"User selects a screen"}

    J -->|BMCC Resources| K["switchScreen(0)"]
    J -->|Mental Health Toolkit| L["switchScreen(1)"]
    J -->|My Journal| M["switchScreen(2)"]

    K --> K1["stack.setCurrentIndex(0)"]
    K1 --> K2["Recommendations.onActivated()"]
    K2 --> K3["Reset nested stack to home page"]

    L --> L1["stack.setCurrentIndex(1)"]
    L1 --> L2["Toolkit.onActivated()"]
    L2 --> L3["Close any open accordion folder"]

    M --> M1["stack.setCurrentIndex(2)"]
    M1 --> M2["Journal.onActivated()"]
    M2 --> M3["refreshEntryList()"]
    M3 --> M4["JournalStorage.loadEntries()"]
    M4 --> M5["Populate saved entries list"]
    M5 --> M6["updateClock()"]
```

## 3. Screen Flowcharts

### 3.1 BMCC Resources Screen

```mermaid
flowchart TD
    A["Recommendations screen opens"] --> B["Show topic-picker home page"]
    B --> C["Display stress topic cards"]
    C --> D{"User clicks a topic"}
    D --> E["showTopic(index)"]
    E --> F["Switch nested QStackedWidget page"]
    F --> G["Render resource cards from static RESOURCES data"]
    G --> H{"User action"}
    H -->|Open link| I["QDesktopServices::openUrl()"]
    H -->|Back| J["showHome()"]
    I --> G
    J --> B
```

### 3.2 Mental Health Toolkit Screen

```mermaid
flowchart TD
    A["Toolkit screen opens"] --> B["Render accordion folders from FOLDERS"]
    B --> C{"User clicks a folder"}
    C --> D["toggleFolder(id)"]
    D --> E["Hide all panels"]
    E --> F["Reset all folder styles"]
    F --> G["Reset all chevrons"]
    G --> H{"Was this folder already open?"}
    H -->|Yes| I["Leave all folders closed"]
    H -->|No| J["Apply active style to selected folder"]
    J --> K["Show matching panel"]
    K --> L["Display intro + tool items"]
    L --> M{"External link clicked?"}
    M -->|Yes| N["QDesktopServices::openUrl()"]
    M -->|No| L
```

### 3.3 Journal Screen

```mermaid
flowchart TD
    A["Journal screen opens"] --> B["refreshEntryList()"]
    B --> C["JournalStorage.loadEntries()"]
    C --> D["Scan ~/Documents/MindEase_Journal"]
    D --> E["Parse .txt files into JournalEntry objects"]
    E --> F["Sort newest first"]
    F --> G["Populate QListWidget + preview"]

    G --> H{"User action"}
    H -->|Write text| I["Edit QTextEdit"]
    H -->|Save Entry| J["saveEntry()"]
    H -->|Clear| K["clearEditor()"]
    H -->|Select old entry| L["loadSelectedEntry()"]

    J --> M{"Editor empty?"}
    M -->|Yes| N["Show validation error"]
    M -->|No| O["Create JournalEntry(now, body)"]
    O --> P["JournalStorage.saveEntry(entry)"]
    P --> Q{"Write succeeded?"}
    Q -->|No| R["Show file write error"]
    Q -->|Yes| S["Clear editor"]
    S --> T["refreshEntryList()"]

    L --> U["Read selected file path"]
    U --> V["JournalStorage.loadEntry(path)"]
    V --> W{"Entry valid?"}
    W -->|No| X["Show preview error"]
    W -->|Yes| Y["Render timestamp + body in preview"]

    K --> Z["Clear editor + keep focus"]
```

## 4. UML Class Diagram

```mermaid
classDiagram
    class MainWindow {
        -QWidget* centralWidget
        -QHBoxLayout* mainLayout
        -QWidget* sidebar
        -QStackedWidget* stack
        -QVector<QPushButton*> navButtons
        -QVector<Screen*> screens
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
        +Recommendations(QWidget* parent = nullptr)
        +QString screenId() const
        +void onActivated()
        -showTopic(int index)
        -showHome()
    }

    class Toolkit {
        -QMap<QString,QFrame*> panels
        -QMap<QString,QWidget*> folderCards
        -QMap<QString,QLabel*> chevrons
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
        -QTimer* m_timer
        -JournalStorage m_storage
        +Journal(QWidget* parent = nullptr)
        +QString screenId() const
        +void onActivated()
        -saveEntry()
        -clearEditor()
        -loadSelectedEntry(QListWidgetItem* item)
        -updateClock()
        -refreshEntryList()
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
        +QVector<JournalEntry> loadEntries() const
        +JournalEntry loadEntry(const QString& path) const
        -QString resolveDirectoryPath() const
        -QString buildEntryPath(const QDateTime& dateTime) const
        -QString formatEntry(const JournalEntry& entry) const
        -JournalEntry parseEntryFile(const QString& path, const QString& content) const
    }

    MainWindow *-- Screen : owns polymorphic screens
    Screen <|-- Recommendations
    Screen <|-- Toolkit
    Screen <|-- Journal
    Recommendations *-- QStackedWidget : nested pages
    Journal *-- QTimer
    Journal *-- JournalStorage
    Journal ..> JournalEntry : creates/reads
    JournalStorage ..> JournalEntry : persists
```

## 5. UML Sequence Diagram

### 5.1 Navigation Sequence

```mermaid
sequenceDiagram
    actor User
    participant Sidebar as Sidebar Button
    participant MainWindow
    participant Stack as QStackedWidget
    participant ScreenObj as Screen subclass

    User->>Sidebar: click
    Sidebar->>MainWindow: switchScreen(index)
    MainWindow->>Stack: setCurrentIndex(index)
    MainWindow->>MainWindow: update checked nav buttons
    MainWindow->>ScreenObj: onActivated()
```

### 5.2 Journal Save Sequence

```mermaid
sequenceDiagram
    actor User
    participant Journal
    participant JournalEntry
    participant JournalStorage
    participant FileSystem as Local File System

    User->>Journal: click Save Entry
    Journal->>Journal: read editor text
    alt text is empty
        Journal->>Journal: showStatus(error)
    else text exists
        Journal->>JournalEntry: create entry(now, body)
        Journal->>JournalStorage: saveEntry(entry)
        JournalStorage->>FileSystem: create directory if needed
        JournalStorage->>FileSystem: write txt file
        alt write fails
            Journal->>Journal: showStatus(failure)
        else write succeeds
            Journal->>Journal: clear editor
            Journal->>Journal: refreshEntryList()
            Journal->>JournalStorage: loadEntries()
        end
    end
```

## 6. Simple OOP Summary

- `MainWindow`:
  app shell and navigation controller
- `Screen`:
  shared abstract base class for all screens
- `Recommendations`, `Toolkit`, `Journal`:
  concrete screen classes using inheritance and polymorphism
- `JournalEntry`:
  simple encapsulated data model
- `JournalStorage`:
  single-responsibility file I/O service

