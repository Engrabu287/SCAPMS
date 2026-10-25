#pragma once
#include "scapms/clinicmanager.h"
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>

class QTabWidget;
class QTimer;

namespace scapms {
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
private:
    ClinicManager manager_;
    QTabWidget* tabs_;
    QTableWidget* patientTable_;
    QTableWidget* appointmentTable_;
    QTableWidget* queueTable_;
    QLineEdit *nameEdit_, *ageEdit_, *contactEdit_, *patientIdEdit_;
    QComboBox *categoryCombo_, *sortAlgorithmCombo_, *sortFieldCombo_, *searchAlgorithmCombo_;
    QLineEdit *searchEdit_, *appointmentPatientIdEdit_, *appointmentDateEdit_, *appointmentReasonEdit_, *queuePatientIdEdit_, *appointmentIdEdit_;
    QTextEdit* reportsText_;
    QLabel *patientCountLabel_, *appointmentCountLabel_, *waitingCountLabel_, *statusLabel_;
    QTimer* refreshTimer_;

    void buildUi();
    QWidget* buildPatientsTab();
    QWidget* buildAppointmentsTab();
    QWidget* buildQueueTab();
    QWidget* buildSearchTab();
    QWidget* buildReportsTab();
    void refreshAll();
    void refreshPatients(const std::vector<PatientView>* overrideRows = nullptr);
    void refreshAppointments();
    void refreshQueue();
    void refreshReports();
    void showError(const QString& message);
    void showInfo(const QString& message);
};
}
