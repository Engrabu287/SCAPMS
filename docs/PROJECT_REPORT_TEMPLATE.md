# COS202 Programming II Lab Assessment
## Smart Clinic Appointment & Patient Management System (SCAPMS)

**Student name:** [Enter name]  
**Matriculation number:** [Enter number]  
**Course:** COS202 Programming II  
**Date:** [Enter submission date]

## 1. Project Description
SCAPMS is a desktop application developed in C++ with the Qt Widgets framework. It is intended to demonstrate how object-oriented programming, data structures, algorithms, file handling, exception handling and event-driven GUI programming can be combined to support a university clinic's basic patient and appointment workflow. It is an educational simulation and should not be used to store real patient data.

## 2. Main Features
The application supports patient registration, editing and deletion; General, Emergency and Follow-Up patient categories; appointment scheduling, cancellation and completion; a FIFO waiting queue; search and sorting; undo for recorded administrative changes; file save/load; and summary reports. Input errors are reported through dialog boxes instead of terminating the program.

## 3. Class Hierarchy and OOP
`PatientRecord` is an abstract base class holding shared patient fields. `GeneralPatient`, `EmergencyPatient` and `FollowUpPatient` inherit from it and override virtual behaviour. `Appointable` is an interface-like abstract class declaring appointment operations. `ClinicManager` separates business/data operations from the Qt interface, while `MainWindow` handles GUI events and refreshes the display. Encapsulation is demonstrated through private fields and controlled setter methods.

## 4. Data Structures
- `std::vector` stores patient and appointment records.
- `std::queue` maintains a first-in, first-out consultation queue.
- `std::stack` stores snapshots for the Undo operation.
- `std::array<int, 10>` stores a fixed-size cache of recently accessed patient IDs.

## 5. Searching Algorithms
Linear search scans the patient list for an ID and is suitable for an unsorted collection. Binary search manually orders a temporary list of patient pointers by ID and repeatedly halves the search interval. Recursive name search checks a patient name and calls itself for the next record until it finds a match or reaches the end.

## 6. Sorting Algorithms
Selection sort repeatedly selects the smallest remaining record. It is easy to trace and explain for a small clinic dataset, though it has O(n²) time complexity. Insertion sort inserts each record into its correct position within the already-processed portion of the list. It is also O(n²) in the worst case and can perform well on small or nearly sorted data. Users can select either algorithm and sort by name, patient ID or age.

## 7. GUI Design and Event-Driven Programming
The Qt GUI uses tabs for Patient Records, Appointments, Waiting Queue, Search & Sort, and Reports. `QVBoxLayout`, `QHBoxLayout` and `QFormLayout` organise controls without relying on fixed coordinates. Qt button signals are connected to slots/lambdas that perform operations and refresh the display. A timer refreshes dashboard information periodically. Keyboard shortcuts include Ctrl+S, Ctrl+F and Ctrl+Q.

## 8. File Handling and Exception Handling
The File menu supports saving and loading data through file dialogs. Operations that can fail are wrapped in `try`/`catch` blocks, and validation errors are displayed through warning dialogs. Patient age is restricted to 0–120, names and contact fields cannot be empty, and appointments require an existing patient plus date/time and reason.

## 9. Reports
The system provides patient-category distribution, appointment status summary, patients with pending appointments, and a list of patients ranked by completed appointment counts.

## 10. Challenges Encountered and Resolution
**Complete this section after running and testing your own build.** Record actual compiler issues, UI issues, data validation issues or algorithm bugs encountered, and explain how you resolved them. Do not claim to have tested features you have not tested.

## 11. Testing Evidence
Insert genuine screenshots of:
1. Main dashboard and patient table
2. Patient registration and validation
3. Appointment management
4. Search and sorting
5. Waiting queue
6. Reports

## 12. Conclusion
Summarise what you learned about object-oriented design, algorithms, data structures, GUI event handling and file persistence. Mention any remaining limitations honestly.
