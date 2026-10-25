#include "scapms/clinicmanager.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>

namespace scapms {

ClinicManager::ClinicManager() { recentCache_.fill(-1); }

std::string ClinicManager::lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return value;
}
std::unique_ptr<PatientRecord> ClinicManager::makePatient(int id, const std::string& name, int age,
                                                          const std::string& contact, const std::string& category) {
    if (category == "Emergency") return std::make_unique<EmergencyPatient>(id, name, age, contact);
    if (category == "Follow-Up") return std::make_unique<FollowUpPatient>(id, name, age, contact);
    if (category == "General") return std::make_unique<GeneralPatient>(id, name, age, contact);
    throw std::invalid_argument("Choose a valid patient category.");
}
ClinicManager::PatientList ClinicManager::clonePatients(const PatientList& source) {
    PatientList copy;
    for (const auto& p : source) copy.push_back(p->clone());
    return copy;
}
void ClinicManager::saveUndoSnapshot() {
    undoStack_.push(Snapshot{clonePatients(patients_), appointments_, nextPatientId_, nextAppointmentId_});
}
int ClinicManager::addPatient(const std::string& name, int age, const std::string& contact, const std::string& category) {
    saveUndoSnapshot();
    auto patient = makePatient(nextPatientId_, name, age, contact, category);
    patients_.push_back(std::move(patient));
    recentCache_[cacheCursor_++ % recentCache_.size()] = nextPatientId_;
    return nextPatientId_++;
}
void ClinicManager::updatePatient(int id, const std::string& name, int age, const std::string& contact, const std::string& category) {
    auto* existing = findById(id);
    if (!existing) throw std::invalid_argument("Patient ID was not found.");
    saveUndoSnapshot();
    auto replacement = makePatient(id, name, age, contact, category);
    for (auto& p : patients_) if (p->id() == id) { p = std::move(replacement); break; }
}
void ClinicManager::deletePatient(int id) {
    auto it = std::find_if(patients_.begin(), patients_.end(), [id](const auto& p){ return p->id() == id; });
    if (it == patients_.end()) throw std::invalid_argument("Patient ID was not found.");
    saveUndoSnapshot();
    patients_.erase(it);
    appointments_.erase(std::remove_if(appointments_.begin(), appointments_.end(),
        [id](const Appointment& a){ return a.patientId == id; }), appointments_.end());
    std::queue<int> kept;
    while (!waiting_.empty()) { int x = waiting_.front(); waiting_.pop(); if (x != id) kept.push(x); }
    waiting_ = std::move(kept);
}
PatientRecord* ClinicManager::findById(int id) {
    for (auto& p : patients_) if (p->id() == id) return p.get();
    return nullptr;
}
const PatientRecord* ClinicManager::findById(int id) const {
    for (const auto& p : patients_) if (p->id() == id) return p.get();
    return nullptr;
}
PatientRecord* ClinicManager::findLinear(int id) {
    for (auto& p : patients_) if (p->id() == id) {
        recentCache_[cacheCursor_++ % recentCache_.size()] = id; return p.get();
    }
    return nullptr;
}
PatientRecord* ClinicManager::findBinary(int id) {
    // Manual binary search over a temporary sorted pointer list; source order is unchanged.
    std::vector<PatientRecord*> sorted;
    for (auto& p : patients_) sorted.push_back(p.get());
    for (size_t i=0; i<sorted.size(); ++i)
        for (size_t j=i+1; j<sorted.size(); ++j)
            if (sorted[j]->id() < sorted[i]->id()) std::swap(sorted[i], sorted[j]);
    int left = 0, right = static_cast<int>(sorted.size()) - 1;
    while (left <= right) {
        int mid = left + (right-left)/2;
        if (sorted[mid]->id() == id) {
            recentCache_[cacheCursor_++ % recentCache_.size()] = id; return sorted[mid];
        }
        if (sorted[mid]->id() < id) left = mid + 1; else right = mid - 1;
    }
    return nullptr;
}
PatientRecord* ClinicManager::findRecursiveByName(const std::string& name) {
    const std::string target = lower(name);
    std::function<PatientRecord*(size_t)> search = [&](size_t i) -> PatientRecord* {
        if (i >= patients_.size()) return nullptr;
        if (lower(patients_[i]->name()).find(target) != std::string::npos) return patients_[i].get();
        return search(i+1);
    };
    if (target.empty()) return nullptr;
    return search(0);
}
std::vector<PatientView> ClinicManager::patients() const {
    std::vector<PatientView> out;
    for (const auto& p : patients_) out.push_back({p->id(), p->name(), p->age(), p->contact(), p->category(), p->careNote()});
    return out;
}
std::vector<Appointment> ClinicManager::appointments() const { return appointments_; }

void ClinicManager::selectionSort(PatientList& list, const std::string& field) {
    auto less = [&](const auto& a, const auto& b) {
        if (field == "Age") return a->age() < b->age();
        if (field == "Patient ID") return a->id() < b->id();
        return lower(a->name()) < lower(b->name());
    };
    for (size_t i=0; i<list.size(); ++i) {
        size_t best = i;
        for (size_t j=i+1; j<list.size(); ++j) if (less(list[j], list[best])) best = j;
        if (best != i) std::swap(list[i], list[best]);
    }
}
void ClinicManager::insertionSort(PatientList& list, const std::string& field) {
    auto less = [&](const auto& a, const auto& b) {
        if (field == "Age") return a->age() < b->age();
        if (field == "Patient ID") return a->id() < b->id();
        return lower(a->name()) < lower(b->name());
    };
    for (size_t i=1; i<list.size(); ++i) {
        auto key = std::move(list[i]);
        size_t j=i;
        while (j>0 && less(key, list[j-1])) { list[j] = std::move(list[j-1]); --j; }
        list[j] = std::move(key);
    }
}
std::vector<PatientView> ClinicManager::sortedPatients(const std::string& algorithm, const std::string& field) {
    PatientList copy = clonePatients(patients_);
    if (algorithm == "Insertion Sort") insertionSort(copy, field); else selectionSort(copy, field);
    std::vector<PatientView> out;
    for (const auto& p : copy) out.push_back({p->id(), p->name(), p->age(), p->contact(), p->category(), p->careNote()});
    return out;
}
int ClinicManager::schedule(int patientId, const std::string& dateTime, const std::string& reason) {
    auto* p = findById(patientId);
    if (!p) throw std::invalid_argument("Register the patient before scheduling an appointment.");
    if (dateTime.empty() || reason.empty()) throw std::invalid_argument("Date/time and reason are required.");
    saveUndoSnapshot();
    p->scheduleAppointment(dateTime, reason);
    appointments_.push_back({nextAppointmentId_, patientId, dateTime, reason, "Pending"});
    return nextAppointmentId_++;
}
void ClinicManager::cancelAppointment(int appointmentId) {
    auto it = std::find_if(appointments_.begin(), appointments_.end(), [appointmentId](const Appointment& a){ return a.id == appointmentId; });
    if (it == appointments_.end()) throw std::invalid_argument("Appointment ID was not found.");
    saveUndoSnapshot(); it->status = "Cancelled";
}
void ClinicManager::completeAppointment(int appointmentId) {
    auto it = std::find_if(appointments_.begin(), appointments_.end(), [appointmentId](const Appointment& a){ return a.id == appointmentId; });
    if (it == appointments_.end()) throw std::invalid_argument("Appointment ID was not found.");
    saveUndoSnapshot(); it->status = "Completed";
}
void ClinicManager::enqueuePatient(int patientId) {
    if (!findById(patientId)) throw std::invalid_argument("Patient ID was not found.");
    waiting_.push(patientId);
}
std::string ClinicManager::callNextPatient() {
    if (waiting_.empty()) return "The waiting queue is empty.";
    int id = waiting_.front(); waiting_.pop();
    auto* p = findById(id);
    return p ? ("Now serving: " + std::to_string(id) + " - " + p->name()) : "Patient no longer exists.";
}
std::vector<std::string> ClinicManager::queueView() const {
    std::queue<int> q = waiting_; std::vector<std::string> out;
    while (!q.empty()) {
        int id=q.front(); q.pop(); auto* p=findById(id);
        if (p) out.push_back(std::to_string(id) + " | " + p->name() + " | " + p->category());
    }
    return out;
}
void ClinicManager::undo() {
    if (undoStack_.empty()) throw std::invalid_argument("There is no administrative operation to undo.");
    Snapshot s = std::move(undoStack_.top()); undoStack_.pop();
    patients_ = std::move(s.patients); appointments_ = std::move(s.appointments);
    nextPatientId_ = s.nextPatientId; nextAppointmentId_ = s.nextAppointmentId;
}
bool ClinicManager::canUndo() const { return !undoStack_.empty(); }
void ClinicManager::save(const std::string& filePath) const {
    std::ofstream f(filePath);
    if (!f) throw std::runtime_error("Could not open the selected file for saving.");
    f << "SCAPMS,1\n";
    for (const auto& p : patients_)
        f << "P," << p->id() << ',' << p->category() << ',' << p->age() << ',' << p->name() << ',' << p->contact() << '\n';
    for (const auto& a : appointments_)
        f << "A," << a.id << ',' << a.patientId << ',' << a.dateTime << ',' << a.status << ',' << a.reason << '\n';
    if (!f) throw std::runtime_error("An error occurred while writing the data file.");
}
void ClinicManager::load(const std::string& filePath) {
    std::ifstream f(filePath);
    if (!f) throw std::runtime_error("Could not open the selected file.");
    PatientList newPatients; std::vector<Appointment> newAppointments;
    std::string line; int maxP=1000, maxA=5000;
    while (std::getline(f,line)) {
        if (line.empty() || line.rfind("SCAPMS,",0)==0) continue;
        std::stringstream ss(line); std::vector<std::string> x; std::string part;
        while (std::getline(ss,part,',')) x.push_back(part);
        if (x.size() >= 6 && x[0]=="P") {
            int id=std::stoi(x[1]), age=std::stoi(x[3]);
            newPatients.push_back(makePatient(id,x[4],age,x[5],x[2])); maxP=std::max(maxP,id);
        } else if (x.size() >= 6 && x[0]=="A") {
            Appointment a{std::stoi(x[1]),std::stoi(x[2]),x[3],x[5],x[4]};
            newAppointments.push_back(a); maxA=std::max(maxA,a.id);
        }
    }
    saveUndoSnapshot(); patients_=std::move(newPatients); appointments_=std::move(newAppointments);
    nextPatientId_=maxP+1; nextAppointmentId_=maxA+1; waiting_={}; recentCache_.fill(-1);
}
std::vector<std::string> ClinicManager::reports() const {
    std::vector<std::string> out;
    int general=0, emergency=0, follow=0, pending=0, completed=0, cancelled=0;
    std::vector<std::pair<int,int>> counts;
    for (const auto& p : patients_) {
        if (p->category()=="General") ++general; else if (p->category()=="Emergency") ++emergency; else ++follow;
        counts.push_back({p->id(),0});
    }
    for (const auto& a : appointments_) {
        for (auto& c : counts) if (c.first==a.patientId && a.status=="Completed") ++c.second;
        if (a.status=="Pending") ++pending; else if (a.status=="Completed") ++completed; else ++cancelled;
    }
    std::sort(counts.begin(),counts.end(),[](auto a,auto b){return a.second>b.second;});
    out.push_back("PATIENT CATEGORY DISTRIBUTION\nGeneral: " + std::to_string(general) +
                  "\nEmergency: " + std::to_string(emergency) + "\nFollow-Up: " + std::to_string(follow));
    std::ostringstream summary; summary << "DAILY / OVERALL APPOINTMENT SUMMARY\nTotal: " << appointments_.size()
        << "\nPending: " << pending << "\nCompleted: " << completed << "\nCancelled: " << cancelled;
    out.push_back(summary.str());
    std::ostringstream frequent; frequent << "MOST FREQUENTLY ATTENDED PATIENTS (completed appointments)\n";
    if (counts.empty()) frequent << "No patient records available.\n";
    else {
        int shown=0;
        for (const auto& c : counts) {
            if (shown++==5) break;
            const auto* p=findById(c.first);
            if (p) frequent << p->name() << " (ID " << p->id() << "): " << c.second << '\n';
        }
    }
    out.push_back(frequent.str());
    std::ostringstream pendingText; pendingText << "PATIENTS WITH PENDING APPOINTMENTS\n";
    bool any=false;
    for (const auto& a : appointments_) if (a.status=="Pending") {
        const auto* p=findById(a.patientId);
        if (p) { pendingText << p->name() << " | " << a.dateTime << " | " << a.reason << '\n'; any=true; }
    }
    if (!any) pendingText << "No pending appointments.\n";
    out.push_back(pendingText.str());
    return out;
}
int ClinicManager::patientCount() const { return static_cast<int>(patients_.size()); }
int ClinicManager::appointmentCount() const { return static_cast<int>(appointments_.size()); }
int ClinicManager::waitingCount() const { return static_cast<int>(waiting_.size()); }
} // namespace scapms
