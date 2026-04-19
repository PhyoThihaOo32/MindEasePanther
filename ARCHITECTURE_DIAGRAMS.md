# MindEase Architecture Diagrams

This document reflects the code currently built by `MindEase.pro` and also notes additional classes present in the repository but not compiled into the running app.

## 1. Runtime Flowchart

```mermaid
flowchart TD
    A([App launch]) --> B[main.cpp creates QApplication]
    B --> C[Construct MainWindow]
    C --> D[applyStyle]
    D --> E[Create central widget and main layout]
    E --> F[Instantiate Screen subclasses]
    F --> F1[Recommendations]
    F --> F2[Toolkit]
    F --> F3[Journal]
    F1 --> G[Add screens to QStackedWidget]
    F2 --> G
    F3 --> G
    G --> H[buildSidebar]
    H --> I[Show MainWindow]
    I --> J{User clicks sidebar item?}
    J -->|BMCC Resources| K[MainWindow.switchScreen 0]
    J -->|Toolkit| L[MainWindow.switchScreen 1]
    J -->|Journal| M[MainWindow.switchScreen 2]

    K --> K1[stack setCurrentIndex 0]
    K1 --> K2[Recommendations.onActivated]
    K2 --> K3[Reset nested stack to home topic picker]

    L --> L1[stack setCurrentIndex 1]
    L1 --> L2[Toolkit.onActivated]
    L2 --> L3{Any folder open?}
    L3 -->|Yes| L4[toggleFolder openFolder to close it]
    L3 -->|No| L5[Leave accordion collapsed]

    M --> M1[stack setCurrentIndex 2]
    M1 --> M2[Journal.onActivated]
    M2 --> M3[refreshEntryList]
    M3 --> M4[updateClock]
```

## 2. Screen-Level Feature Flows

### 2.1 BMCC Resources Flow

```mermaid
flowchart TD
    A[Recommendations screen activated] --> B[Home page shown]
    B --> C[Display 5 stress topic cards]
    C --> D{User selects a topic}
    D -->|Exam and Study Stress| E1[showTopic 0]
    D -->|Finance| E2[showTopic 1]
    D -->|Immigration| E3[showTopic 2]
    D -->|Relationships and Family| E4[showTopic 3]
    D -->|Health and Wellness| E5[showTopic 4]

    E1 --> F[Switch nested QStackedWidget to detail page]
    E2 --> F
    E3 --> F
    E4 --> F
    E5 --> F

    F --> G[Render resource cards from static RESOURCES data]
    G --> H{User action}
    H -->|Visit/Open button| I[QDesktopServices openUrl]
    H -->|Back| J[showHome]
    I --> G
    J --> B
```

### 2.2 Mental Health Toolkit Flow

```mermaid
flowchart TD
    A[Toolkit screen activated] --> B[Accordion starts fully collapsed]
    B --> C[Render 8 folder cards from FOLDERS list]
    C --> D{User clicks a folder}
    D --> E[toggleFolder id]
    E --> F[Hide all panels]
    F --> G[Reset all folder styles]
    G --> H[Reset all chevrons to closed]
    H --> I{Clicked folder already open?}
    I -->|Yes| J[Keep all folders closed]
    I -->|No| K[Mark folder as open]
    K --> L[Apply active accent style]
    L --> M[Set chevron to open]
    M --> N[Show selected panel]
    N --> O[User reads tool items]
    O --> P{External link clicked?}
    P -->|Yes| Q[QDesktopServices openUrl]
    P -->|No| R[Remain on current panel]
    Q --> O
```

### 2.3 Journal Flow

```mermaid
flowchart TD
    A[Journal screen activated] --> B[refreshEntryList]
    B --> C[Scan Documents/MindEase_Journal for txt files]
    C --> D[JournalEntry.loadFromFile for each file]
    D --> E[Populate QListWidget newest first]
    E --> F[updateClock]

    F --> G{User action}
    G -->|Type entry| H[Edit QTextEdit content]
    G -->|Save Entry| I[saveEntry]
    G -->|Clear| J[clearEditor]
    G -->|Click past entry| K[loadSelectedEntry]

    I --> L{Editor text empty?}
    L -->|Yes| M[Show error status]
    L -->|No| N[Create JournalEntry now and body]
    N --> O[JournalEntry.saveToFile journalDir]
    O --> P{Save success?}
    P -->|No| Q[Show permission or write failure]
    P -->|Yes| R[Show success status]
    R --> S[Clear editor]
    S --> T[refreshEntryList]

    K --> U[Read stored file path from selected item]
    U --> V[JournalEntry.loadFromFile path]
    V --> W{Entry valid?}
    W -->|No| X[Show could not open message]
    W -->|Yes| Y[Format timestamp and body into preview]

    J --> Z[Clear editor and show status]
```

## 3. Primary UML Class Diagram

```mermaid
classDiagram
    class MainWindow {
        -centralWidget : QWidget
        -mainLayout : QHBoxLayout
        -sidebar : QWidget
        -stack : QStackedWidget
        -navButtons : QVector
        -screens : QVector
        +MainWindow(parent : QWidget = nullptr)
        +~MainWindow()
        -buildSidebar()
        -applyStyle()
        -switchScreen(index : int)
    }

    class Screen {
        <<abstract>>
        -m_title : QString
        +Screen(title : QString, parent : QWidget = nullptr)
        +QString title() const
        +QString screenId() const
        +void onActivated()
        #buildHeader(root : QVBoxLayout)
    }

    class Recommendations {
        -m_stack : QStackedWidget
        +Recommendations(parent : QWidget = nullptr)
        +QString screenId() const
        +void onActivated()
        -showTopic(index : int)
        -showHome()
    }

    class Toolkit {
        -panels : QMap
        -folderCards : QMap
        -chevrons : QMap
        -openFolder : QString
        +Toolkit(parent : QWidget = nullptr)
        +QString screenId() const
        +void onActivated()
        -toggleFolder(id : QString)
    }

    class Journal {
        -m_dateLbl : QLabel
        -m_timeLbl : QLabel
        -m_editor : QTextEdit
        -m_statusLbl : QLabel
        -m_entryList : QListWidget
        -m_preview : QTextEdit
        -m_timer : QTimer
        +Journal(parent : QWidget = nullptr)
        +QString screenId() const
        +void onActivated()
        -saveEntry()
        -clearEditor()
        -loadSelectedEntry(item : QListWidgetItem)
        -updateClock()
        -refreshEntryList()
        -showStatus(msg : QString, ok : bool = true)
        -QString journalDir() const
    }

    class JournalEntry {
        -m_dateTime : QDateTime
        -m_body : QString
        -m_filePath : QString
        +JournalEntry()
        +JournalEntry(dt : QDateTime, body : QString)
        +QDateTime dateTime() const
        +QString body() const
        +QString filePath() const
        +bool isValid() const
        +bool operatorLessThan(other : JournalEntry) const
        +bool operatorEquals(other : JournalEntry) const
        +bool saveToFile(dir : QString)
        +JournalEntry loadFromFile(path : QString)
    }

    MainWindow *-- "1" Screen : owns via screens
    Screen <|-- Recommendations
    Screen <|-- Toolkit
    Screen <|-- Journal
    MainWindow *-- Recommendations
    MainWindow *-- Toolkit
    MainWindow *-- Journal
    Recommendations *-- QStackedWidget : nested pages
    Journal ..> JournalEntry : creates and loads
    Journal *-- QTimer
```

## 4. UML Sequence Diagram

### 4.1 Screen Switching

```mermaid
sequenceDiagram
    actor User
    participant SidebarButton as Nav Button
    participant MainWindow
    participant Stack as QStackedWidget
    participant ScreenObj as Screen subclass

    User->>SidebarButton: click
    SidebarButton->>MainWindow: switchScreen(index)
    MainWindow->>Stack: setCurrentIndex(index)
    MainWindow->>MainWindow: update checked nav buttons
    MainWindow->>ScreenObj: onActivated()
    ScreenObj-->>MainWindow: refresh its own state
```

### 4.2 Journal Save Operation

```mermaid
sequenceDiagram
    actor User
    participant Journal
    participant JournalEntry
    participant FileSystem as Local File System
    participant EntryList as QListWidget

    User->>Journal: click Save Entry
    Journal->>Journal: read and trim editor text
    alt text is empty
        Journal->>Journal: showStatus(error)
    else text exists
        Journal->>JournalEntry: construct with current datetime and body
        Journal->>JournalEntry: saveToFile(journalDir)
        JournalEntry->>FileSystem: create directory if needed
        JournalEntry->>FileSystem: write txt file with header and body
        alt save fails
            Journal->>Journal: showStatus(failure)
        else save succeeds
            Journal->>Journal: clear editor
            Journal->>Journal: refreshEntryList()
            Journal->>FileSystem: scan journal directory
            Journal->>JournalEntry: loadFromFile(each file)
            Journal->>EntryList: repopulate items newest first
        end
    end
```

## 5. Data and Dependency Notes

### 5.1 Current compiled app

- `main.cpp`, `mainwindow.*`, `screen.*`, `recommendations.*`, `toolkit.*`, `journal.*`, and `journalentry.*` are the only files compiled by `MindEase.pro`.
- The running application has exactly 3 sidebar screens:
  - `BMCC Resources`
  - `Mental Health Toolkit`
  - `My Journal`

### 5.2 Repository classes not currently compiled

The following classes exist in the repository, but they are not listed in `MindEase.pro`, so they are not part of the current built application:

- `MoodEntry`
- `MoodCheckin`
- `MoodLog`
- `StatsView`
- `SettingsView`

These appear to belong to an earlier or alternate product direction focused on mood tracking and settings persistence.

## 6. Legacy / Uncompiled UML Snapshot

```mermaid
classDiagram
    class MoodEntry {
        +mood : QString
        +emoji : QString
        +note : QString
        +timestamp : QDateTime
        +MoodEntry()
        +MoodEntry(mood : QString, emoji : QString, note : QString, timestamp : QDateTime)
        +QString toCsvLine() const
        +MoodEntry fromCsvLine(line : QString)
    }

    class MoodCheckin {
        -moodGroup : QButtonGroup
        -noteEdit : QPlainTextEdit
        -moodOptions : QList
        +MoodCheckin(parent : QWidget = nullptr)
        +onSave()
        +moodSaved(entry : MoodEntry)
    }

    class MoodLog {
        -listLayout : QVBoxLayout
        -listContainer : QWidget
        +MoodLog(parent : QWidget = nullptr)
        +refreshEntries(entries : QVector)
    }

    class StatsView {
        -entriesNum : QLabel
        -avgNum : QLabel
        -streakNum : QLabel
        -chartLabel : QLabel
        -freqLabel : QLabel
        +StatsView(parent : QWidget = nullptr)
        +refreshEntries(entries : QVector)
    }

    class SettingsView {
        -remindCheck : QCheckBox
        -timeCombo : QComboBox
        -autosaveCheck : QCheckBox
        -quoteCheck : QCheckBox
        -themeCombo : QComboBox
        +SettingsView(parent : QWidget = nullptr)
        -saveSettings()
        -clearData()
    }

    MoodCheckin ..> MoodEntry : emits
    MoodLog ..> MoodEntry : displays
    StatsView ..> MoodEntry : aggregates
```

## 7. Suggested Lucidchart Layout

If you rebuild this in Lucidchart, use three separate pages:

1. `System Flow`
   - App Launch
   - MainWindow Shell
   - Sidebar Navigation
   - Three runtime screens
   - External browser and local file system endpoints

2. `UML - Current Build`
   - `MainWindow`
   - `Screen`
   - `Recommendations`
   - `Toolkit`
   - `Journal`
   - `JournalEntry`

3. `UML - Legacy Repo Classes`
   - `MoodEntry`
   - `MoodCheckin`
   - `MoodLog`
   - `StatsView`
   - `SettingsView`
