#include "scapms/mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QPushButton>
#include <QShortcut>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace scapms {
static QString defaultDataPath() {
    QString directory = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation
    );

    QDir().mkpath(directory);

    return directory + "/scapms-data.csv";
}
static QTableWidget* makeTable(int columns, const QStringList& headers) {
    auto* t = new QTableWidget(0, columns);
    t->setHorizontalHeaderLabels(headers);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setAlternatingRowColors(true);
    return t;
}
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("SCAPMS | Smart Clinic Appointment & Patient Management");
    resize(1180, 760);
    buildUi();

QString path = defaultDataPath();

if (QFileInfo::exists(path)) {
    try {
        manager_.load(path.toStdString());
    } catch (const std::exception& e) {
        showError(
            QString("Could not load saved data: %1").arg(e.what())
        );
    }
}

refreshAll();
    refreshTimer_ = new QTimer(this);
    connect(refreshTimer_, &QTimer::timeout, this, [this] {
        statusLabel_->setText("Dashboard refreshed at " + QDateTime::currentDateTime().toString("hh:mm:ss"));
        refreshAll();
    });
    refreshTimer_->start(30000);
    auto* saveShortcut = new QShortcut(QKeySequence::Save, this);
    connect(saveShortcut, &QShortcut::activated, this, [this] {
        QString path = defaultDataPath();
try {
    manager_.save(path.toStdString());
    showInfo("Data saved successfully.");
} catch (const std::exception& e) {
    showError(e.what());
}
    });
    auto* findShortcut = new QShortcut(QKeySequence::Find, this);
    connect(findShortcut, &QShortcut::activated, this, [this] { tabs_->setCurrentIndex(3); searchEdit_->setFocus(); });
    auto* quitShortcut = new QShortcut(QKeySequence("Ctrl+Q"), this);
    connect(quitShortcut, &QShortcut::activated, qApp, &QApplication::quit);
}
void MainWindow::buildUi() {
    auto* central = new QWidget; auto* root = new QVBoxLayout(central);
    auto* heading = new QLabel("<h1>Smart Clinic Appointment &amp; Patient Management System</h1><p>University Clinic Operations Dashboard</p>");
    heading->setTextFormat(Qt::RichText); root->addWidget(heading);
    auto* stats = new QHBoxLayout;
    patientCountLabel_=new QLabel; appointmentCountLabel_=new QLabel; waitingCountLabel_=new QLabel;
    for (auto* l : {patientCountLabel_, appointmentCountLabel_, waitingCountLabel_}) {
        l->setMinimumHeight(44); l->setAlignment(Qt::AlignCenter); l->setStyleSheet("QLabel {background:#eaf2f8;border:1px solid #c8d9e8;border-radius:7px;font-weight:bold;padding:8px;}");
        stats->addWidget(l);
    }
    root->addLayout(stats);
    tabs_=new QTabWidget;
    tabs_->addTab(buildPatientsTab(),"Patient Records");
    tabs_->addTab(buildAppointmentsTab(),"Appointments");
    tabs_->addTab(buildQueueTab(),"Waiting Queue");
    tabs_->addTab(buildSearchTab(),"Search & Sort");
    tabs_->addTab(buildReportsTab(),"Reports");
    root->addWidget(tabs_);
    statusLabel_=new QLabel("Ready"); statusBar()->addWidget(statusLabel_);
    setCentralWidget(central);

    auto* fileMenu = menuBar()->addMenu("&File");
    auto* newAction=fileMenu->addAction("&New / Clear Data");
    auto* saveAction=fileMenu->addAction("&Save");
    auto* loadAction=fileMenu->addAction("&Load");
    fileMenu->addSeparator(); auto* exitAction=fileMenu->addAction("E&xit");
    connect(newAction,&QAction::triggered,this,[this] {
        if (QMessageBox::question(this,"Clear data","Start a new empty session? Unsaved changes will be lost.")==QMessageBox::Yes) {
            manager_=ClinicManager{}; refreshAll();
        }
    });
    connect(saveAction,&QAction::triggered,this,[this] {
        QString p = defaultDataPath();
try {
    manager_.save(p.toStdString());
    showInfo("Data saved successfully.");
} catch (const std::exception& e) {
    showError(e.what());
}
    });
    connect(loadAction,&QAction::triggered,this,[this] {
        QString p=QFileDialog::getOpenFileName(this,"Load data","","CSV Files (*.csv);;All Files (*)");
        if(!p.isEmpty()) try { manager_.load(p.toStdString()); refreshAll(); showInfo("Data loaded successfully."); } catch(const std::exception& e){showError(e.what());}
    });
    connect(exitAction,&QAction::triggered,qApp,&QApplication::quit);
}
QWidget* MainWindow::buildPatientsTab() {
    auto* w=new QWidget; auto* outer=new QVBoxLayout(w); auto* form=new QFormLayout;
    nameEdit_=new QLineEdit; nameEdit_->setPlaceholderText("e.g. Amina Musa");
    ageEdit_=new QLineEdit; ageEdit_->setPlaceholderText("0–120");
    contactEdit_=new QLineEdit; contactEdit_->setPlaceholderText("Phone or other contact");
    categoryCombo_=new QComboBox; categoryCombo_->addItems({"General","Emergency","Follow-Up"});
    categoryCombo_->setToolTip("Emergency patients are marked high priority.");
    form->addRow("Full name:",nameEdit_); form->addRow("Age:",ageEdit_); form->addRow("Contact:",contactEdit_); form->addRow("Patient type:",categoryCombo_);
    outer->addLayout(form);
    auto* buttons=new QHBoxLayout; auto* add=new QPushButton("Register Patient"); auto* edit=new QPushButton("Edit Selected"); auto* del=new QPushButton("Delete Selected"); auto* undo=new QPushButton("Undo Last Admin Operation");
    add->setToolTip("Register a new patient (Ctrl+S saves the data file)."); del->setToolTip("Delete the selected patient and associated appointments.");
    buttons->addWidget(add); buttons->addWidget(edit); buttons->addWidget(del); buttons->addWidget(undo); outer->addLayout(buttons);
    patientTable_=makeTable(6,{"Patient ID","Name","Age","Contact","Category","Care / Priority"}); outer->addWidget(patientTable_);
    connect(add,&QPushButton::clicked,this,[this] {
        try {
            bool ok=false; int age=ageEdit_->text().toInt(&ok); if(!ok) throw std::invalid_argument("Enter a valid numeric age.");
            int id=manager_.addPatient(nameEdit_->text().trimmed().toStdString(),age,contactEdit_->text().trimmed().toStdString(),categoryCombo_->currentText().toStdString());
            nameEdit_->clear(); ageEdit_->clear(); contactEdit_->clear(); refreshAll(); showInfo("Patient registered. Patient ID: "+QString::number(id));
        } catch(const std::exception& e){showError(e.what());}
    });
    connect(edit,&QPushButton::clicked,this,[this] {
        int row=patientTable_->currentRow(); if(row<0){showError("Select a patient first.");return;}
        int id=patientTable_->item(row,0)->text().toInt();
        bool ok=false; QString name=QInputDialog::getText(this,"Edit patient","Full name:",QLineEdit::Normal,patientTable_->item(row,1)->text(),&ok); if(!ok)return;
        int age=QInputDialog::getInt(this,"Edit patient","Age:",patientTable_->item(row,2)->text().toInt(),0,120,1,&ok); if(!ok)return;
        QString contact=QInputDialog::getText(this,"Edit patient","Contact:",QLineEdit::Normal,patientTable_->item(row,3)->text(),&ok); if(!ok)return;
        QStringList cats={"General","Emergency","Follow-Up"}; QString cat=QInputDialog::getItem(this,"Edit patient","Category:",cats,cats.indexOf(patientTable_->item(row,4)->text()),false,&ok); if(!ok)return;
        try{manager_.updatePatient(id,name.toStdString(),age,contact.toStdString(),cat.toStdString());refreshAll();showInfo("Patient updated.");}catch(const std::exception& e){showError(e.what());}
    });
    connect(del,&QPushButton::clicked,this,[this] {
        int row=patientTable_->currentRow(); if(row<0){showError("Select a patient first.");return;}
        int id=patientTable_->item(row,0)->text().toInt();
        if(QMessageBox::question(this,"Delete patient","Delete this patient and their appointments?")==QMessageBox::Yes)
            try{manager_.deletePatient(id);refreshAll();showInfo("Patient deleted. Use Undo if this was accidental.");}catch(const std::exception& e){showError(e.what());}
    });
    connect(undo,&QPushButton::clicked,this,[this] {
        try{manager_.undo();refreshAll();showInfo("Last recorded administrative operation was undone.");}catch(const std::exception& e){showError(e.what());}
    });
    return w;
}
QWidget* MainWindow::buildAppointmentsTab() {
    auto* w=new QWidget; auto* outer=new QVBoxLayout(w); auto* form=new QFormLayout;
    appointmentPatientIdEdit_=new QLineEdit; appointmentPatientIdEdit_->setPlaceholderText("Patient ID");
    appointmentDateEdit_=new QLineEdit; appointmentDateEdit_->setPlaceholderText("YYYY-MM-DD HH:MM");
    appointmentReasonEdit_=new QLineEdit; appointmentReasonEdit_->setPlaceholderText("Reason for visit");
    form->addRow("Patient ID:",appointmentPatientIdEdit_); form->addRow("Date and time:",appointmentDateEdit_); form->addRow("Reason:",appointmentReasonEdit_); outer->addLayout(form);
    auto* buttons=new QHBoxLayout; auto* schedule=new QPushButton("Schedule"); auto* cancel=new QPushButton("Cancel Selected"); auto* complete=new QPushButton("Mark Completed");
    buttons->addWidget(schedule); buttons->addWidget(cancel); buttons->addWidget(complete); outer->addLayout(buttons);
    appointmentTable_=makeTable(6,{"Appointment ID","Patient ID","Patient Name","Date / Time","Reason","Status"}); outer->addWidget(appointmentTable_);
    connect(schedule,&QPushButton::clicked,this,[this]{
        bool ok=false; int id=appointmentPatientIdEdit_->text().toInt(&ok);
        if(!ok){showError("Enter a valid patient ID.");return;}
        try{int ap=manager_.schedule(id,appointmentDateEdit_->text().trimmed().toStdString(),appointmentReasonEdit_->text().trimmed().toStdString());refreshAll();showInfo("Appointment scheduled. Appointment ID: "+QString::number(ap));}
        catch(const std::exception& e){showError(e.what());}
    });
    connect(cancel,&QPushButton::clicked,this,[this]{
        int row=appointmentTable_->currentRow(); if(row<0){showError("Select an appointment first.");return;}
        try{manager_.cancelAppointment(appointmentTable_->item(row,0)->text().toInt());refreshAll();showInfo("Appointment cancelled.");}catch(const std::exception& e){showError(e.what());}
    });
    connect(complete,&QPushButton::clicked,this,[this]{
        int row=appointmentTable_->currentRow(); if(row<0){showError("Select an appointment first.");return;}
        try{manager_.completeAppointment(appointmentTable_->item(row,0)->text().toInt());refreshAll();showInfo("Appointment marked completed.");}catch(const std::exception& e){showError(e.what());}
    });
    return w;
}
QWidget* MainWindow::buildQueueTab() {
    auto* w=new QWidget; auto* outer=new QVBoxLayout(w); auto* row=new QHBoxLayout;
    queuePatientIdEdit_=new QLineEdit; queuePatientIdEdit_->setPlaceholderText("Registered patient ID");
    auto* add=new QPushButton("Add to Queue"); auto* next=new QPushButton("Call Next Patient");
    row->addWidget(queuePatientIdEdit_); row->addWidget(add); row->addWidget(next); outer->addLayout(row);
    queueTable_=makeTable(3,{"Patient ID","Patient Name","Category"}); outer->addWidget(queueTable_);
    connect(add,&QPushButton::clicked,this,[this]{
        bool ok=false;int id=queuePatientIdEdit_->text().toInt(&ok);if(!ok){showError("Enter a valid patient ID.");return;}
        try{manager_.enqueuePatient(id);refreshAll();showInfo("Patient added to waiting queue.");}catch(const std::exception& e){showError(e.what());}
    });
    connect(next,&QPushButton::clicked,this,[this]{showInfo(QString::fromStdString(manager_.callNextPatient()));refreshAll();});
    return w;
}
QWidget* MainWindow::buildSearchTab() {
    auto* w=new QWidget; auto* outer=new QVBoxLayout(w); auto* form=new QHBoxLayout;
    searchEdit_=new QLineEdit; searchEdit_->setPlaceholderText("Patient ID or name keyword");
    searchAlgorithmCombo_=new QComboBox; searchAlgorithmCombo_->addItems({"Linear Search by ID","Binary Search by ID","Recursive Search by name"});
    auto* search=new QPushButton("Search"); form->addWidget(searchEdit_); form->addWidget(searchAlgorithmCombo_); form->addWidget(search); outer->addLayout(form);
    auto* sortRow=new QHBoxLayout; sortAlgorithmCombo_=new QComboBox; sortAlgorithmCombo_->addItems({"Selection Sort","Insertion Sort"});
    sortFieldCombo_=new QComboBox; sortFieldCombo_->addItems({"Patient Name","Patient ID","Age"}); auto* sort=new QPushButton("Sort Records");
    sortRow->addWidget(new QLabel("Algorithm:"));sortRow->addWidget(sortAlgorithmCombo_);sortRow->addWidget(new QLabel("Sort by:"));sortRow->addWidget(sortFieldCombo_);sortRow->addWidget(sort);outer->addLayout(sortRow);
    auto* results=makeTable(6,{"Patient ID","Name","Age","Contact","Category","Care / Priority"}); outer->addWidget(results);
    connect(search,&QPushButton::clicked,this,[this,results]{
        QString q=searchEdit_->text().trimmed(); if(q.isEmpty()){showError("Enter a patient ID or name.");return;}
        PatientRecord* p=nullptr; QString method=searchAlgorithmCombo_->currentText();
        if(method=="Recursive Search by name") p=manager_.findRecursiveByName(q.toStdString());
        else {bool ok=false;int id=q.toInt(&ok);if(!ok){showError("For ID search, enter a numeric patient ID.");return;}
            p=method=="Binary Search by ID"?manager_.findBinary(id):manager_.findLinear(id);}
        results->setRowCount(0);
        if(!p){showInfo("No matching patient found.");return;}
        results->insertRow(0); QStringList vals={QString::number(p->id()),QString::fromStdString(p->name()),QString::number(p->age()),QString::fromStdString(p->contact()),QString::fromStdString(p->category()),QString::fromStdString(p->careNote())};
        for(int c=0;c<vals.size();++c)results->setItem(0,c,new QTableWidgetItem(vals[c]));
        if(p->category()=="Emergency")for(int c=0;c<6;++c)results->item(0,c)->setBackground(QColor("#ffd6d6"));
    });
    connect(sort,&QPushButton::clicked,this,[this,results]{
        auto rows=manager_.sortedPatients(sortAlgorithmCombo_->currentText().toStdString(),sortFieldCombo_->currentText().toStdString());
        results->setRowCount(0);
        for(const auto& p:rows){int r=results->rowCount();results->insertRow(r);QStringList vals={QString::number(p.id),QString::fromStdString(p.name),QString::number(p.age),QString::fromStdString(p.contact),QString::fromStdString(p.category),QString::fromStdString(p.careNote)};
            for(int c=0;c<vals.size();++c)results->setItem(r,c,new QTableWidgetItem(vals[c]));
            if(p.category=="Emergency")for(int c=0;c<6;++c)results->item(r,c)->setBackground(QColor("#ffd6d6"));
        }
        showInfo("Records sorted with the manually implemented "+sortAlgorithmCombo_->currentText()+".");
    });
    return w;
}
QWidget* MainWindow::buildReportsTab() {
    auto* w=new QWidget;auto* outer=new QVBoxLayout(w);auto* refresh=new QPushButton("Generate / Refresh Reports");
    reportsText_=new QTextEdit;reportsText_->setReadOnly(true);reportsText_->setFont(QFont("Consolas",10));
    outer->addWidget(refresh);outer->addWidget(reportsText_);
    connect(refresh,&QPushButton::clicked,this,&MainWindow::refreshReports);
    return w;
}
void MainWindow::refreshPatients(const std::vector<PatientView>* overrideRows) {
    auto rows=overrideRows?*overrideRows:manager_.patients();patientTable_->setRowCount(0);
    for(const auto& p:rows){int r=patientTable_->rowCount();patientTable_->insertRow(r);
        QStringList vals={QString::number(p.id),QString::fromStdString(p.name),QString::number(p.age),QString::fromStdString(p.contact),QString::fromStdString(p.category),QString::fromStdString(p.careNote)};
        for(int c=0;c<vals.size();++c)patientTable_->setItem(r,c,new QTableWidgetItem(vals[c]));
        if(p.category=="Emergency")for(int c=0;c<6;++c){patientTable_->item(r,c)->setBackground(QColor("#ffd6d6"));patientTable_->item(r,c)->setToolTip("Emergency patient - high priority");}
        else if(p.category=="Follow-Up")for(int c=0;c<6;++c)patientTable_->item(r,c)->setBackground(QColor("#fff2cc"));
    }
}
void MainWindow::refreshAppointments() {
    appointmentTable_->setRowCount(0);auto ap=manager_.appointments();auto patients=manager_.patients();
    for(const auto& a:ap){std::string name="(patient missing)";for(const auto& p:patients)if(p.id==a.patientId)name=p.name;
        int r=appointmentTable_->rowCount();appointmentTable_->insertRow(r);
        QStringList vals={QString::number(a.id),QString::number(a.patientId),QString::fromStdString(name),QString::fromStdString(a.dateTime),QString::fromStdString(a.reason),QString::fromStdString(a.status)};
        for(int c=0;c<vals.size();++c)appointmentTable_->setItem(r,c,new QTableWidgetItem(vals[c]));
        if(a.status=="Cancelled")for(int c=0;c<6;++c)appointmentTable_->item(r,c)->setForeground(QColor("#888888"));
    }
}
void MainWindow::refreshQueue() {
    queueTable_->setRowCount(0);for(const auto& s:manager_.queueView()){QStringList x=QString::fromStdString(s).split(" | ");int r=queueTable_->rowCount();queueTable_->insertRow(r);for(int c=0;c<x.size()&&c<3;++c)queueTable_->setItem(r,c,new QTableWidgetItem(x[c]));}
}
void MainWindow::refreshReports() {
    auto reports=manager_.reports();QString out;for(size_t i=0;i<reports.size();++i){out+=QString::fromStdString(reports[i]);out+="\n\n---------------------------------------------\n\n";}
    reportsText_->setPlainText(out);
}
void MainWindow::refreshAll() {
    patientCountLabel_->setText("Registered Patients: "+QString::number(manager_.patientCount()));
    appointmentCountLabel_->setText("Appointments: "+QString::number(manager_.appointmentCount()));
    waitingCountLabel_->setText("Waiting: "+QString::number(manager_.waitingCount()));
    refreshPatients();refreshAppointments();refreshQueue();refreshReports();
}
void MainWindow::showError(const QString& message){QMessageBox::warning(this,"SCAPMS - Attention",message);}
void MainWindow::showInfo(const QString& message){statusLabel_->setText(message);statusBar()->showMessage(message,5000);}
} // namespace scapms
