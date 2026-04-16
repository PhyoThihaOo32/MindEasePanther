# MindEase — Wellness Companion
### C++ / Qt Desktop App — Mental Health Tracker & Toolkit

---

## Project Structure

```
MindEase/
├── MindEase.pro        ← Qt project file (open this in Qt Creator)
├── main.cpp            ← App entry point
├── mainwindow.h/cpp    ← Main window, sidebar navigation, File I/O
├── moodentry.h/cpp     ← MoodEntry data class (serialization)
├── moodcheckin.h/cpp   ← Screen 1: Mood Check-in
├── recommendations.h/cpp ← Screen 2: Recommendations
├── moodlog.h/cpp       ← Screen 3: My Log (reads from file)
├── statsview.h/cpp     ← Screen 4: Stats & Trends (ASCII chart)
├── toolkit.h/cpp       ← Screen 5: Mental Health Toolkit (8 folders)
└── settingsview.h/cpp  ← Screen 6: Settings (QSettings persistence)
```

---

## How to Run on Mac

### Step 1 — Install Qt
1. Go to https://www.qt.io/download-open-source
2. Download **Qt Online Installer**
3. Run the installer — sign up for a free Qt account if prompted
4. In the component selector, install:
   - **Qt 6.x** (latest stable) or Qt 5.15
   - **Qt Creator IDE**
   - **Desktop (macOS)** kit
   - Make sure **Qt Widgets** is included (it is by default)

### Step 2 — Open the Project
1. Open **Qt Creator**
2. Click **File → Open File or Project**
3. Navigate to the `MindEase` folder and select **MindEase.pro**
4. Qt Creator will ask you to configure the project — click **Configure Project**
5. Select the **Desktop** kit (e.g., "Qt 6.x for macOS")

### Step 3 — Build & Run
1. Click the green **▶ Run** button (bottom left), or press `Cmd + R`
2. Qt Creator will compile all files and launch the app
3. The MindEase window will appear!

---

## C++ Concepts Used (for your project report)

| Concept | Where it's used |
|---|---|
| **Classes** | `MoodEntry`, `MainWindow`, `MoodCheckin`, `MoodLog`, `StatsView`, `Toolkit`, `SettingsView` |
| **Inheritance** | All screen classes inherit from `QWidget`; `MainWindow` inherits from `QMainWindow` |
| **File I/O** | `mainwindow.cpp` — `saveEntriesToFile()` and `loadEntriesFromFile()` using `QFile` + `QTextStream` |
| **Exception handling** | `try/catch` blocks wrap all file operations in `mainwindow.cpp` |
| **Signals & Slots** | Qt's event system — `moodSaved` signal connects `MoodCheckin` → `MainWindow` |
| **Encapsulation** | Each screen is a self-contained class with private members |
| **STL / Qt containers** | `QVector<MoodEntry>`, `QMap<QString, QFrame*>`, `QList` |

---

## Data Storage

Mood entries are auto-saved to:
```
~/Library/Application Support/MindEase/mood_log.txt
```

Format (one entry per line):
```
2026-04-15T09:42:00|Happy|😊|Had a great study session today
2026-04-14T18:15:00|Stressed|😤|Project deadline coming up
```

---

## Screens

1. **Mood Check-in** — Select mood emoji, add optional note, save entry
2. **Recommendations** — Rule-based wellness tips (breathing, walking, journaling, music, meditation)
3. **My Log** — All saved entries displayed with timestamps; export button
4. **Stats & Trends** — Monthly count, average mood score, day streak, ASCII mood chart, frequency breakdown
5. **Mental Health Toolkit** — 8 expandable folders based on BMCC Counseling Center resources:
   - 📚 Academic Stress
   - 🤝 Connection & Community
   - 🌈 Empowered Identity & Care
   - ✏️ Gratitude & Journaling
   - 🧘 Mindfulness & Meditation
   - 🎵 Music for Your Mind
   - 🥗 Nutrition & Energy
   - 🌙 Sleep & Relaxation
6. **Settings** — Reminders, auto-save, quote of day, theme (saved via `QSettings`)

---

## Troubleshooting

| Problem | Fix |
|---|---|
| "No kit selected" error | Go to Qt Creator → Preferences → Kits and make sure a Desktop kit is configured |
| Build fails with "Qt not found" | Make sure Qt Widgets module is installed in Qt Maintenance Tool |
| Emoji don't show on older macOS | This is a system font issue — replace emoji strings in .cpp files with text labels if needed |
| App builds but window is blank | Check that `setCentralWidget(centralWidget)` is called in `mainwindow.cpp` |
