# MindEase Console Demo

This folder is a simple pure C++ console version for class demonstration.
It uses only standard C++ headers, so it does not require Qt or any GUI library.

## Concepts Demonstrated

- Classes and objects
- Encapsulation with private data members
- Composition through `JournalManager` using `JournalEntry`
- Search algorithm for topics and subtopics
- File I/O using plain `.txt` journal files
- Separate `.h` and `.cpp` files
- Branching with numbered menus and submenus

## Demo Flow

```text
1. BMCC Resources
   1. Browse by numbered topic
   2. Search topic or subtopic
   3. Show all topics and subtopics

2. Mental Health Toolkit
   1. Academic Stress
   2. Mindfulness & Meditation
   3. Food, Sleep, and Energy
   4. Connection & Support

3. My Journal
   1. Write a new entry
   2. List and read entries
   3. Delete an entry
```

## Build

```bash
g++ *.cpp -o main
```

## Run

```bash
./main
```

You can also use the Makefile:

```bash
make
./mindease_console_demo
```

Journal files are saved as simple text files:

```text
journal_index.txt
journal_entry_YYYY-MM-DD_HH-MM-SS.txt
```
