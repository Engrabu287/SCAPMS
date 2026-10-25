#pragma once
#include <memory>
#include <stdexcept>
#include <string>

namespace scapms {

class Appointable {
public:
    virtual ~Appointable() = default;
    virtual bool scheduleAppointment(const std::string& dateTime, const std::string& reason) = 0;
    virtual bool cancelAppointment(int appointmentId) = 0;
};

class PatientRecord : public Appointable {
public:
    PatientRecord(int id, std::string name, int age, std::string contact);
    virtual ~PatientRecord() = default;

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    int age() const { return age_; }
    const std::string& contact() const { return contact_; }

    void setName(const std::string& name);
    void setAge(int age);
    void setContact(const std::string& contact);

    virtual std::string category() const = 0;
    virtual std::string careNote() const = 0;
    virtual std::unique_ptr<PatientRecord> clone() const = 0;

    bool scheduleAppointment(const std::string& dateTime, const std::string& reason) override;
    bool cancelAppointment(int appointmentId) override;

    // Minimal patient-level appointment tracking; full appointment records are managed by ClinicManager.
    const std::string& lastAppointmentDate() const { return lastAppointmentDate_; }
    const std::string& lastAppointmentReason() const { return lastAppointmentReason_; }

protected:
    void setAppointmentInfo(const std::string& dateTime, const std::string& reason);
    int id_;
    std::string name_;
    int age_;
    std::string contact_;
    std::string lastAppointmentDate_;
    std::string lastAppointmentReason_;
};

class GeneralPatient final : public PatientRecord {
public:
    using PatientRecord::PatientRecord;
    std::string category() const override { return "General"; }
    std::string careNote() const override { return "Normal priority"; }
    std::unique_ptr<PatientRecord> clone() const override;
};

class EmergencyPatient final : public PatientRecord {
public:
    using PatientRecord::PatientRecord;
    std::string category() const override { return "Emergency"; }
    std::string careNote() const override { return "HIGH PRIORITY - assess promptly"; }
    std::unique_ptr<PatientRecord> clone() const override;
};

class FollowUpPatient final : public PatientRecord {
public:
    using PatientRecord::PatientRecord;
    std::string category() const override { return "Follow-Up"; }
    std::string careNote() const override { return "Review previous treatment"; }
    std::unique_ptr<PatientRecord> clone() const override;
};

} // namespace scapms
