# 🌿 MindEase — BMCC Wellness Companion

> A desktop wellness application built for BMCC students, providing instant access to campus mental health resources, a curated self-care toolkit, and a private personal journal — all in one place.

**Course:** CSC211H Advanced Programming Techniques (Honors)  
**Institution:** Borough of Manhattan Community College (BMCC), CUNY  
**Semester:** Spring 2026  
**Language:** C++17 · Qt 6 Widgets  

---

## About the Project

MindEase is a Qt/C++ desktop application designed to support the mental health and academic success of BMCC students. College life brings a unique mix of stressors — exams, finances, immigration concerns, family pressures, and health challenges — and MindEase puts targeted, real campus resources at students' fingertips without the friction of searching the web.

The app was built as an Honors capstone project for CSC211H, demonstrating applied C++ concepts including object-oriented design, Qt signal/slot architecture, file I/O, dynamic UI construction, and CSS-based styling.

---

## Features

### ✦ BMCC Resources
Students choose what they're stressed about and receive a curated list of BMCC offices with full contact details, hours, and direct browser links — no searching needed.

- 📚 **Exam & Study Stress** — LRC Tutoring (both campuses + remote), PAL, Writing Center, Academic Coaching, Online Tutoring / Upswing
- 💰 **Finance** — Financial Aid Office, Student Emergency Fund, Panther Pantry, Advocacy & Resource Center
- 🌐 **Immigration** — Immigrant Resource Center (confidential), CUNY Legal Services / DACA, Scholarships, Know Your Rights
- 💬 **Relationships & Family** — Personal Counseling, Togetherall 24/7, Appointment Booking, Community Referrals
- 🏥 **Health & Wellness** — Health Services, Mental Health Counseling, Veterans Resource Center (Samantha Ruiz, LCSW), Samaritans 24-hr Hotline, National PTSD Center, Combat Stress Recovery, Health Insurance Enrollment

Each card shows description, location, phone, email, office hours, color-coded topic tags, and one-click "Visit →" links that open in the default browser via `QDesktopServices`.

### ⚙ Mental Health Toolkit
An accordion-style library of 8 evidence-based self-care folders, content sourced directly from the BMCC Counseling Center's Mental Health Toolkit:

| Folder | Focus |
|---|---|
| 📚 Academic Stress | Planners, procrastination tools, BMCC academic support |
| 🤝 Connection & Community | Counseling groups, clubs, NYC crisis hotlines |
| 🌈 Empowered Identity & Care | Racial trauma, LGBTQIA+, disability justice |
| ✏️ Gratitude & Journaling | Prompts, 7-day challenge, recommended reading |
| 🧘 Mindfulness & Meditation | Guided meditations, Calm, Headspace, breathing |
| 🎵 Music for Your Mind | Study, sleep, and relaxation playlists |
| 🥗 Nutrition & Energy | Brain foods, Panther Pantry, campus fitness, NYC parks |
| 🌙 Sleep & Relaxation | Sleep calculator, hygiene tips, doom-scrolling guide |

Each folder expands/collapses smoothly with a chevron indicator and per-folder color theming. External links open in the browser.

### 📓 My Journal
A fully private, local journaling screen with persistent file storage:

- Live clock (updates every second) showing the full date and time
- Four guided gratitude prompts displayed as writing inspiration
- Large, focused text editor with a writing-friendly font
- **Save Entry** — writes the entry to `~/Documents/MindEase_Journal/YYYY-MM-DD_HH-MM-SS.txt` with a clean header
- Past entries panel — all saved journals listed newest-first with date/time labels
- Click any entry to read its full content in the preview pane
- Entries are plain `.txt` files readable in any text editor — no proprietary format, no cloud, no account

---

## Screenshots

> *(Add screenshots after building — Qt Creator → Build → Run → screenshot the window)*

| | |
|---|---|
| BMCC Resources — landing | Resource detail page |
| Mental Health Toolkit | My Journal |

---

## Tech Stack

| | |
|---|---|
| **Language** | C++17 |
| **GUI Framework** | Qt 6 (Widgets module) |
| **Build System** | qmake |
| **Platform** | macOS (Qt 6.11.0 + Xcode) |
| **File Storage** | `QFile` · `QTextStream` · `QStandardPaths` |

**Key Qt classes used:**
`QMainWindow`, `QStackedWidget`, `QPushButton` (as clickable cards), `QFrame`, `QScrollArea`, `QTextEdit`, `QListWidget`, `QTimer`, `QDateTime`, `QDesktopServices`, `QMap`, `QVector`

---

## C++ Concepts Demonstrated

| Concept | Where it appears |
|---|---|
| **OOP / Classes** | `MainWindow`, `Recommendations`, `Toolkit`, `Journal` — each a self-contained `QWidget` subclass |
| **Inheritance** | All screens inherit `QWidget`; `MainWindow` inherits `QMainWindow` |
| **Encapsulation** | Private member variables and methods in every class; data structs (`ResourceInfo`, `FolderDef`, `ToolItem`) scoped to their `.cpp` file |
| **Aggregate initialization** | C++17 brace-init for `ResourceInfo`, `TopicInfo`, `FolderDef`, `ToolItem` structs |
| **Lambdas** | Signal connections use `[this, id]()` lambdas; `addUrlBtn` is a local lambda inside `makeResourceCard` |
| **Qt Signals & Slots** | `QPushButton::clicked` → `switchScreen`, `toggleFolder`, `saveEntry`, `loadSelectedEntry`, `updateClock` |
| **File I/O** | `journal.cpp` — `saveEntry()` writes, `loadSelectedEntry()` reads, using `QFile` + `QTextStream` |
| **STL / Qt containers** | `QList<ResourceInfo>`, `QMap<QString, QFrame*>`, `QMap<QString, QLabel*>`, `QVector<QPushButton*>` |
| **Dynamic UI** | Resource cards, topic cards, accordion folders, and tool items all built at runtime in loops from static data |
| **Qt Stylesheets (QSS)** | Full CSS-like theming: `border-left`, `border-radius`, `hover`, `checked`, object-name selectors |

---

## Project Structure

```
MindEase/
├── MindEase.pro              ← qmake project file (open this in Qt Creator)
├── main.cpp                  ← App entry point
├── mainwindow.h / .cpp       ← App shell — sidebar navigation + screen switcher
├── recommendations.h / .cpp  ← BMCC Resources (topic picker + 5 detail pages)
├── toolkit.h / .cpp          ← Mental Health Toolkit (8 accordion folders)
├── journal.h / .cpp          ← My Journal (write, save, browse past entries)
└── README.md
```

Journal entries are saved to:
```
~/Documents/MindEase_Journal/
    2026-04-15_14-30-00.txt
    2026-04-16_09-05-42.txt
    ...
```

Each file looks like:
```
=== MindEase Journal Entry ===
Date: Wednesday, April 15, 2026
Time: 2:30 PM
──────────────────────────────

Today I felt really overwhelmed before my CSC211 midterm, but I took a
few deep breaths and used the Pomodoro technique...
```

---

## How to Build & Run

### Prerequisites
- Qt 6.x — download from [qt.io/download](https://www.qt.io/download-open-source)
- Qt Creator IDE (included in the Qt installer)
- macOS with Xcode command-line tools

### Qt Creator (recommended)
1. Open **Qt Creator**
2. **File → Open File or Project** → select `MindEase.pro`
3. Click **Configure Project** and choose your Desktop kit (Qt 6.x for macOS)
4. Press **▶ Run** or `Cmd + R`

### Command line
```bash
cd MindEase/
qmake MindEase.pro
make -j4
./MindEase          # or: open MindEase.app
```

> **macOS / Qt 6.11 note:** A harmless `__yield` compiler warning may appear during the build. It is suppressed by `QMAKE_CXXFLAGS += -Wno-implicit-function-declaration` in `MindEase.pro`. This is a known Qt 6.11 + Xcode compatibility issue and does not affect the app in any way.

---

## Design Decisions

**Local-first, privacy-first journal**  
Journal entries are plain `.txt` files written to the user's own `~/Documents` folder. No server, no account, no cloud sync. For a wellness app where students discuss personal struggles, keeping data fully local was a deliberate design choice.

**Two-step flow for BMCC Resources**  
The landing page asks "What are you stressed about?" before showing any resources. Naming the stressor is a small but meaningful act — it helps students self-identify and reduces the cognitive load of scanning a wall of services. The detail page then shows only what's relevant.

**QPushButton as card widget**  
Rather than subclassing `QWidget` for clickable cards, all interactive cards (topic picker, accordion folders) are implemented as `QPushButton` widgets with child layouts and `WA_TransparentForMouseEvents` on child labels. This keeps the code straightforward while preserving native hover, press, and focus behaviors.

**Accordion with QMap chevron tracking**  
The toolkit accordion stores all folder cards, panels, and chevron labels in `QMap<QString, T>` keyed by folder ID. When a folder is toggled, `toggleFolder()` iterates all three maps — ensuring every chevron resets correctly regardless of which folder was previously open.

---

## Resources & Acknowledgments

All BMCC resource data sourced from official BMCC websites:

- [BMCC Counseling Center](https://www.bmcc.cuny.edu/student-affairs/counseling/)
- [BMCC Mental Health Toolkit](https://www.bmcc.cuny.edu/student-affairs/counseling/your-mental-health-toolkit/)
- [BMCC Learning Resource Center](https://www.bmcc.cuny.edu/students/lrc/)
- [BMCC Advocacy & Resource Center](https://www.bmcc.cuny.edu/student-affairs/arc/)
- [BMCC Veterans Resource Center](https://www.bmcc.cuny.edu/student-affairs/veterans-resource-center/)
- [BMCC Immigrant Resource Center](https://www.bmcc.cuny.edu/student-affairs/center-for-belonging-and-inclusion/immigrant-resource-center/)
- [BMCC Financial Aid](https://www.bmcc.cuny.edu/finaid/)

---

## License

Built for academic purposes at BMCC / CUNY. Source code is open for educational use. All resource information belongs to the respective BMCC departments and CUNY.
