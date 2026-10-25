#pragma once
#include <string>

namespace scapms {
struct Appointment {
    int id = 0;
    int patientId = 0;
    std::string dateTime;
    std::string reason;
    std::string status = "Pending"; // Pending, Completed, Cancelled
};
}
