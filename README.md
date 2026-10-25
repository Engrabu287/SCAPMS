# SCAPMS — Smart Clinic Appointment & Patient Management System

A C++17 desktop GUI project for the COS202 Programming II lab assessment. The GUI uses Qt 6 Widgets and the project uses CMake.

## Features included
- Patient registration, editing, deletion, category-based visual highlighting
- General, Emergency and Follow-Up patient subclasses
- Abstract `PatientRecord` and interface-like `Appointable`
- Runtime polymorphism through virtual patient behaviour
- Appointment scheduling, cancellation and completion
- FIFO waiting queue (`std::queue`)
- Undo stack for patient and appointment administrative changes (`std::stack`)
- Fixed-size 10-entry recent-patient ID cache (`std::array`)
- Manual linear search, binary search, recursive name search
- Manual Selection Sort and Insertion Sort
- Save/load through a file dialog
- Reports: category distribution, pending appointments, frequent completed visits, appointment summary
- Input validation, exception handling, timer-based dashboard refresh
- Ctrl+S save, Ctrl+F search, Ctrl+Q exit

## Important project note
This is an educational clinic-management simulation. It is not a medical record system for real patient data. Do not enter real patient information.

## Build on Windows using MSYS2 MINGW64

Open the **MSYS2 MINGW64** terminal (not MSYS, UCRT64 or a different shell) and install the tools:

```bash
pacman -Syu
```

If the terminal closes after updating, reopen **MSYS2 MINGW64** and run:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-qt6-base
```

From the MINGW64 terminal, change directory to the extracted project folder. Example:

```bash
cd /c/Users/YOUR_USERNAME/Downloads/SCAPMS_Project
cmake -S . -B build -G Ninja
cmake --build build
./build/SCAPMS.exe
```

If CMake cannot find Qt, confirm you are using the MINGW64 terminal and that `mingw-w64-x86_64-qt6-base` is installed.

## Open in VS Code
From the MINGW64 terminal inside the project folder:

```bash
code .
```

Build in the integrated terminal using the same MINGW64 environment.

## Data file format
The Save/Load menu uses a simple comma-separated educational format. For names or reasons containing commas, use words without commas in this version. Use fictional sample data only.

## Algorithms and rationale
- **Linear search:** useful for a small unsorted patient collection; O(n).
- **Binary search:** O(log n) after records are ordered by patient ID; this implementation manually sorts a temporary pointer list by ID before searching.
- **Recursive search:** checks names one by one and recursively advances through the patient list; O(n), demonstrates recursion.
- **Selection sort:** easy to demonstrate and trace for small clinic lists; O(n²).
- **Insertion sort:** simple and efficient for small or nearly sorted lists; worst-case O(n²).

## Known limitations to improve before final submission
- CSV is intentionally simple and does not escape commas in fields.
- The waiting queue is held in memory and is not persisted.
- Appointment date/time is stored as text; stricter date-time parsing can be added.
- The Undo stack restores patient and appointment snapshots. Queue changes are not part of undo snapshots.
- This is a learning project, not a production medical system.

## Submission checklist
1. Compile and run the project.
2. Test every major button and algorithm.
3. Take genuine screenshots of the running application.
4. Review the UML diagram in `docs/UML_Class_Diagram.md`.
5. Complete the project report template in `docs/PROJECT_REPORT_TEMPLATE.md` with your actual testing and challenges.
6. Assemble source code, screenshots, UML and report into the single PDF required by the lecturer.
