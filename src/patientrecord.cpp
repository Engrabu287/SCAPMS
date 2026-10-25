#include "scapms/patientrecord.h"
#include <cctype>
#include <utility>

namespace scapms {
PatientRecord::PatientRecord(int id, std::string name, int age, std::string contact)
    : id_(id), name_(std::move(name)), age_(age), contact_(std::move(contact)) {
    if (id_ <= 0) throw std::invalid_argument("Patient ID must be a positive number.");
    setName(name_);
    setAge(age_);
    setContact(contact_);
}
void PatientRecord::setName(const std::string& name) {
    if (name.find_first_not_of(" \t\r\n") == std::string::npos)
        throw std::invalid_argument("Patient name cannot be empty.");
    name_ = name;
}
void PatientRecord::setAge(int age) {
    if (age < 0 || age > 120) throw std::invalid_argument("Patient age must be between 0 and 120.");
    age_ = age;
}
void PatientRecord::setContact(const std::string& contact) {
    if (contact.find_first_not_of(" \t\r\n") == std::string::npos)
        throw std::invalid_argument("Contact information cannot be empty.");
    contact_ = contact;
}
bool PatientRecord::scheduleAppointment(const std::string& dateTime, const std::string& reason) {
    if (dateTime.empty() || reason.empty()) return false;
    setAppointmentInfo(dateTime, reason);
    return true;
}
bool PatientRecord::cancelAppointment(int appointmentId) {
    return appointmentId > 0; // The manager performs the actual appointment removal.
}
void PatientRecord::setAppointmentInfo(const std::string& dateTime, const std::string& reason) {
    lastAppointmentDate_ = dateTime;
    lastAppointmentReason_ = reason;
}
std::unique_ptr<PatientRecord> GeneralPatient::clone() const { return std::make_unique<GeneralPatient>(*this); }
std::unique_ptr<PatientRecord> EmergencyPatient::clone() const { return std::make_unique<EmergencyPatient>(*this); }
std::unique_ptr<PatientRecord> FollowUpPatient::clone() const { return std::make_unique<FollowUpPatient>(*this); }
} // namespace scapms
