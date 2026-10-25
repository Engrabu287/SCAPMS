#pragma once
#include "scapms/appointment.h"
#include "scapms/patientrecord.h"
#include <array>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>

namespace scapms {

struct PatientView {
    int id;
    std::string name;
    int age;
    std::string contact;
    std::string category;
    std::string careNote;
};

class ClinicManager {
public:
    ClinicManager();
    int addPatient(const std::string& name, int age, const std::string& contact, const std::string& category);
    void updatePatient(int id, const std::string& name, int age, const std::string& contact, const std::string& category);
    void deletePatient(int id);
    PatientRecord* findLinear(int id);
    PatientRecord* findBinary(int id);
    PatientRecord* findRecursiveByName(const std::string& name);
    std::vector<PatientView> patients() const;
    std::vector<Appointment> appointments() const;
    std::vector<PatientView> sortedPatients(const std::string& algorithm, const std::string& field);
    int schedule(int patientId, const std::string& dateTime, const std::string& reason);
    void cancelAppointment(int appointmentId);
    void completeAppointment(int appointmentId);
    void enqueuePatient(int patientId);
    std::string callNextPatient();
    std::vector<std::string> queueView() const;
    void undo();
    bool canUndo() const;
    void save(const std::string& filePath) const;
    void load(const std::string& filePath);
    std::vector<std::string> reports() const;
    int patientCount() const;
    int appointmentCount() const;
    int waitingCount() const;

private:
    using PatientList = std::vector<std::unique_ptr<PatientRecord>>;
    struct Snapshot {
        std::vector<std::unique_ptr<PatientRecord>> patients;
        std::vector<Appointment> appointments;
        int nextPatientId;
        int nextAppointmentId;
    };
    PatientList patients_;
    std::vector<Appointment> appointments_;
    std::queue<int> waiting_;
    std::array<int, 10> recentCache_{};
    size_t cacheCursor_ = 0;
    int nextPatientId_ = 1001;
    int nextAppointmentId_ = 5001;
    std::stack<Snapshot> undoStack_;

    void saveUndoSnapshot();
    static std::string lower(std::string value);
    PatientRecord* findById(int id);
    const PatientRecord* findById(int id) const;
    static std::unique_ptr<PatientRecord> makePatient(int id, const std::string& name, int age,
                                                      const std::string& contact, const std::string& category);
    static PatientList clonePatients(const PatientList& source);
    static void selectionSort(PatientList& list, const std::string& field);
    static void insertionSort(PatientList& list, const std::string& field);
};

} // namespace scapms
