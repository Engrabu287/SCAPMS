# SCAPMS UML Class Diagram

The Mermaid diagram below can be rendered by Mermaid-compatible Markdown viewers.

```mermaid
classDiagram
    class Appointable {
        <<abstract interface>>
        +scheduleAppointment(dateTime, reason) bool
        +cancelAppointment(appointmentId) bool
    }
    class PatientRecord {
        <<abstract>>
        -id_ int
        -name_ string
        -age_ int
        -contact_ string
        +category() string*
        +careNote() string*
        +clone() unique_ptr*
        +scheduleAppointment(dateTime, reason) bool
        +cancelAppointment(appointmentId) bool
    }
    class GeneralPatient
    class EmergencyPatient
    class FollowUpPatient
    class Appointment {
        +id int
        +patientId int
        +dateTime string
        +reason string
        +status string
    }
    class ClinicManager {
        -patients_ vector
        -appointments_ vector
        -waiting_ queue
        -undoStack_ stack
        -recentCache_ array[10]
        +addPatient()
        +updatePatient()
        +deletePatient()
        +findLinear()
        +findBinary()
        +findRecursiveByName()
        +sortedPatients()
        +schedule()
        +undo()
        +save()
        +load()
        +reports()
    }
    class MainWindow {
        -manager_ ClinicManager
        +buildUi()
        +refreshAll()
    }
    Appointable <|.. PatientRecord
    PatientRecord <|-- GeneralPatient
    PatientRecord <|-- EmergencyPatient
    PatientRecord <|-- FollowUpPatient
    ClinicManager o-- "0..*" PatientRecord : manages
    ClinicManager o-- "0..*" Appointment : manages
    MainWindow --> ClinicManager : uses
    Appointment --> PatientRecord : patientId association
```

`PatientRecord` is abstract because it defines pure virtual category, care-note and clone functions. Its subclasses override these functions, demonstrating runtime polymorphism.
