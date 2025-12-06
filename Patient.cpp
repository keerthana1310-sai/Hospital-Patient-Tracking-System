#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <limits>
#include <sstream>
using namespace std;

struct Patient {
    int id;
    string name;
    int age;
    string gender;
    string disease;
    int priority;       // 1 = critical, 0 = normal
    int roomNo;
    string wardType;    // "General", "ICU", or "NA"
    int status;         // 0 = admitted, 1 = discharged

    long long admissionEpoch;
    long long dischargeEpoch;
    string admissionDateTime;
    string dischargeDateTime;

    double totalBill;

    Patient* prev;
    Patient* next;
};

Patient* head = nullptr;
Patient* tail = nullptr;

int nextPatientId = 1;
int nextGeneralRoom = 101; // 101,102,... for normal (General)
int nextIcuRoom = 201;     // 201,202,... for critical (ICU)

// -----------------------------
// Utility: current date & time
// -----------------------------
string getCurrentDateTime() {
    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%d-%m-%Y_%H:%M:%S", lt); // dd-mm-yyyy_HH:MM:SS
    return string(buf);
}

// -----------------------------
// Room charge per day
// -----------------------------
int getRoomCharge(const string& ward) {
    if (ward == "General") return 1000;
    if (ward == "ICU") return 5000;
    return 0;
}

// -----------------------------
// Calculate days stayed
// -----------------------------
int calculateDays(long long inEpoch, long long outEpoch) {
    if (inEpoch == 0) return 0;
    if (outEpoch <= inEpoch) return 1;
    long long diff = outEpoch - inEpoch;
    int days = static_cast<int>(diff / (60 * 60 * 24));
    if (days < 1) days = 1;
    return days;
}

// -----------------------------
// Search by ID
// -----------------------------
Patient* searchPatient(int id) {
    Patient* cur = head;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}

// -----------------------------
// Move DLL node to head
// -----------------------------
void moveToHead(Patient* p) {
    if (!p || p == head) return;

    if (p == tail) {
        tail = p->prev;
        if (tail) tail->next = nullptr;
    } else {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    p->prev = nullptr;
    p->next = head;
    if (head) head->prev = p;
    head = p;
    if (!tail) tail = p;
}

// -----------------------------
// Move DLL node to tail
// -----------------------------
void moveToTail(Patient* p) {
    if (!p || p == tail) return;

    if (p == head) {
        head = p->next;
        if (head) head->prev = nullptr;
    } else {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    p->next = nullptr;
    p->prev = tail;
    if (tail) tail->next = p;
    tail = p;
    if (!head) head = p;
}

// -----------------------------
// Save to file (one line per patient, '|' separated)
// -----------------------------
void saveToFile() {
    ofstream file("hospital_records.txt");
    if (!file) return;

    Patient* cur = head;
    while (cur) {
        file << cur->id << '|'
             << cur->name << '|'
             << cur->age << '|'
             << cur->gender << '|'
             << cur->disease << '|'
             << cur->priority << '|'
             << cur->roomNo << '|'
             << cur->wardType << '|'
             << cur->status << '|'
             << cur->admissionEpoch << '|'
             << cur->dischargeEpoch << '|'
             << cur->admissionDateTime << '|'
             << cur->dischargeDateTime << '|'
             << cur->totalBill
             << '\n';
        cur = cur->next;
    }
}

// -----------------------------
// Load from file (silent if file doesn't exist)
// -----------------------------
void loadFromFile() {
    ifstream file("hospital_records.txt");
    if (!file) return; // no message

    // clear existing list
    Patient* cur = head;
    while (cur) {
        Patient* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    head = tail = nullptr;

    string line;
    int maxId = 0;
    int maxGenRoom = 100;
    int maxIcuRoom = 200;

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string fields[14];

        for (int i = 0; i < 14; ++i) {
            if (!getline(ss, fields[i], '|')) {
                fields[i].clear();
            }
        }

        Patient* p = new Patient();
        try {
            p->id = stoi(fields[0]);
            p->name = fields[1];
            p->age = stoi(fields[2]);
            p->gender = fields[3];
            p->disease = fields[4];
            p->priority = stoi(fields[5]);
            p->roomNo = stoi(fields[6]);
            p->wardType = fields[7];
            p->status = stoi(fields[8]);
            p->admissionEpoch = stoll(fields[9]);
            p->dischargeEpoch = stoll(fields[10]);
            p->admissionDateTime = fields[11];
            p->dischargeDateTime = fields[12];
            p->totalBill = stod(fields[13]);
        } catch (...) {
            delete p;
            continue;
        }

        p->prev = p->next = nullptr;

        if (!head) {
            head = tail = p;
        } else {
            tail->next = p;
            p->prev = tail;
            tail = p;
        }

        if (p->id > maxId) maxId = p->id;
        if (p->wardType == "General" && p->roomNo > maxGenRoom)
            maxGenRoom = p->roomNo;
        if (p->wardType == "ICU" && p->roomNo > maxIcuRoom)
            maxIcuRoom = p->roomNo;
    }

    nextPatientId   = (maxId > 0)       ? maxId + 1      : 1;
    nextGeneralRoom = (maxGenRoom >= 101) ? maxGenRoom + 1 : 101;
    nextIcuRoom     = (maxIcuRoom >= 201) ? maxIcuRoom + 1 : 201;
}

// -----------------------------
// 1. Add Patients
// -----------------------------
void addPatient() {
    Patient* p = new Patient();
    p->id = nextPatientId++;

    cout << "\n=== ADD PATIENT ===\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Patient Name: ";
    getline(cin, p->name);

    cout << "Enter Age: ";
    cin >> p->age;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Gender (M/F/O): ";
    getline(cin, p->gender);

    cout << "Enter Disease: ";
    getline(cin, p->disease);

    cout << "Is Patient Critical? (1 = Yes, 0 = No): ";
    cin >> p->priority;

    p->roomNo = 0;           // will be assigned later
    p->wardType = "NA";
    p->status = 0;           // admitted

    time_t now = time(nullptr);
    p->admissionEpoch = now;
    p->admissionDateTime = getCurrentDateTime();
    p->dischargeEpoch = 0;
    p->dischargeDateTime = "NA";
    p->totalBill = 0.0;

    p->prev = p->next = nullptr;

    if (!head) {
        head = tail = p;
    } else if (p->priority == 1) {
        // critical → insert at head
        p->next = head;
        head->prev = p;
        head = p;
    } else {
        // normal → insert at tail
        tail->next = p;
        p->prev = tail;
        tail = p;
    }

    saveToFile();

    cout << "\nPatient Added Successfully.\n";
    cout << "Assigned ID         : " << p->id << "\n";
    cout << "Admission Date/Time : " << p->admissionDateTime << "\n";
}

// -----------------------------
// 2. Discharge Patients
// -----------------------------
void dischargePatient() {
    if (!head) {
        cout << "\nNo patients available.\n";
        return;
    }

    cout << "\n=== DISCHARGE PATIENT ===\nEnter Patient ID: ";
    int id;
    cin >> id;

    Patient* p = searchPatient(id);
    if (!p) {
        cout << "Patient not found.\n";
        return;
    }
    if (p->status == 1) {
        cout << "Patient already discharged.\n";
        return;
    }
    if (p->roomNo == 0 || p->wardType == "NA") {
        cout << "Room not allocated. Allocate room before discharge.\n";
        return;
    }

    time_t now = time(nullptr);
    p->dischargeEpoch = now;
    p->dischargeDateTime = getCurrentDateTime();

    int days = calculateDays(p->admissionEpoch, p->dischargeEpoch);
    int rate = getRoomCharge(p->wardType);
    p->totalBill = static_cast<double>(days * rate);
    p->status = 1;

    saveToFile();

    cout << "\n--- DISCHARGE SUMMARY ---\n";
    cout << "Name          : " << p->name << "\n";
    cout << "Ward Type     : " << p->wardType << "\n";
    cout << "Room No       : " << p->roomNo << "\n";
    cout << "Days Stayed   : " << days << "\n";
    cout << "Charge/Day    : " << rate << "\n";
    cout << "Total Bill    : " << p->totalBill << "\n";
}

// -----------------------------
// 3. Search Patients
// -----------------------------
void searchPatients() {
    if (!head) {
        cout << "\nNo patients available.\n";
        return;
    }

    cout << "\n=== SEARCH PATIENT ===\nEnter Patient ID: ";
    int id;
    cin >> id;

    Patient* p = searchPatient(id);
    if (!p) {
        cout << "Patient not found.\n";
        return;
    }

    cout << "\n--- PATIENT DETAILS ---\n";
    cout << "ID          : " << p->id << "\n";
    cout << "Name        : " << p->name << "\n";
    cout << "Age         : " << p->age << "\n";
    cout << "Gender      : " << p->gender << "\n";
    cout << "Disease     : " << p->disease << "\n";
    cout << "Priority    : " << (p->priority ? "Critical" : "Normal") << "\n";
    cout << "Room No     : " << p->roomNo << "\n";
    cout << "Ward Type   : " << p->wardType << "\n";
    cout << "Status      : " << (p->status ? "Discharged" : "Admitted") << "\n";
    cout << "Adm Time    : " << p->admissionDateTime << "\n";
    cout << "Dis Time    : " << p->dischargeDateTime << "\n";
    cout << "Total Bill  : " << p->totalBill << "\n";
}

// -----------------------------
// 4. Display All Patients
// -----------------------------
void displayAllPatients() {
    if (!head) {
        cout << "\nNo patients available.\n";
        return;
    }

    cout << "\n=== ALL PATIENTS ===\n";
    Patient* cur = head;
    while (cur) {
        cout << "---------------------------------\n";
        cout << "ID        : " << cur->id << "\n";
        cout << "Name      : " << cur->name << "\n";
        cout << "Age       : " << cur->age << "\n";
        cout << "Gender    : " << cur->gender << "\n";
        cout << "Disease   : " << cur->disease << "\n";
        cout << "Priority  : " << (cur->priority ? "Critical" : "Normal") << "\n";
        cout << "Room No   : " << cur->roomNo << "\n";
        cout << "Ward Type : " << cur->wardType << "\n";
        cout << "Status    : " << (cur->status ? "Discharged" : "Admitted") << "\n";
        cur = cur->next;
    }
}

// -----------------------------
// 5. Room Allocation System
// (auto based on critical status)
// -----------------------------
void roomAllocationSystem() {
    if (!head) {
        cout << "\nNo patients available.\n";
        return;
    }

    cout << "\n=== ROOM ALLOCATION SYSTEM ===\nEnter Patient ID: ";
    int id;
    cin >> id;

    Patient* p = searchPatient(id);
    if (!p) {
        cout << "Patient not found.\n";
        return;
    }
    if (p->status == 1) {
        cout << "Cannot allocate room to discharged patient.\n";
        return;
    }
    if (p->roomNo != 0 && p->wardType != "NA") {
        cout << "Room already allocated: " << p->roomNo 
             << " (" << p->wardType << ")\n";
        return;
    }

    if (p->priority == 1) {
        // critical → ICU
        p->wardType = "ICU";
        p->roomNo = nextIcuRoom++;
    } else {
        // normal → General
        p->wardType = "General";
        p->roomNo = nextGeneralRoom++;
    }

    saveToFile();

    cout << "\nRoom allocated successfully.\n";
    cout << "Ward Type : " << p->wardType << "\n";
    cout << "Room No   : " << p->roomNo << "\n";
}

// -----------------------------
// 6. Track Critical Patients
// -----------------------------
void trackCriticalPatients() {
    if (!head) {
        cout << "\nNo patients available.\n";
        return;
    }

    cout << "\n=== TRACK CRITICAL PATIENTS ===\n";
    cout << "1. Show All Critical Patients\n";
    cout << "2. Mark Patient as Critical & Move to Head\n";
    cout << "3. Remove Critical Status & Move to Tail\n";
    cout << "Enter your choice: ";

    int ch;
    cin >> ch;

    if (ch == 1) {
        bool any = false;
        Patient* cur = head;
        cout << "\n--- CRITICAL PATIENTS ---\n";
        while (cur) {
            if (cur->priority == 1) {
                any = true;
                cout << "ID: " << cur->id << " | Name: " << cur->name
                     << " | Status: " << (cur->status ? "Discharged" : "Admitted")
                     << "\n";
            }
            cur = cur->next;
        }
        if (!any) cout << "No critical patients.\n";
    }
    else if (ch == 2) {
        int id;
        cout << "Enter Patient ID to mark critical: ";
        cin >> id;
        Patient* p = searchPatient(id);
        if (!p) {
            cout << "Patient not found.\n";
            return;
        }
        if (p->status == 1) {
            cout << "Patient discharged. Cannot change.\n";
            return;
        }
        p->priority = 1;
        moveToHead(p);
        saveToFile();
        cout << "Marked as critical and moved to head.\n";
    }
    else if (ch == 3) {
        int id;
        cout << "Enter Patient ID to remove critical status: ";
        cin >> id;
        Patient* p = searchPatient(id);
        if (!p) {
            cout << "Patient not found.\n";
            return;
        }
        p->priority = 0;
        moveToTail(p);
        saveToFile();
        cout << "Critical status removed and moved to tail.\n";
    }
    else {
        cout << "Invalid choice.\n";
    }
}

// -----------------------------
// 7. Patient Stay Duration
// -----------------------------
void patientStayDuration() {
    if (!head) {
        cout << "\nNo patients available.\n";
        return;
    }

    cout << "\n=== PATIENT STAY DURATION ===\nEnter Patient ID: ";
    int id;
    cin >> id;

    Patient* p = searchPatient(id);
    if (!p) {
        cout << "Patient not found.\n";
        return;
    }

    long long endEpoch = (p->status == 1 && p->dischargeEpoch != 0)
                         ? p->dischargeEpoch
                         : static_cast<long long>(time(nullptr));

    int days = calculateDays(p->admissionEpoch, endEpoch);

    cout << "\n--- STAY DETAILS ---\n";
    cout << "Name              : " << p->name << "\n";
    cout << "Admission Time    : " << p->admissionDateTime << "\n";
    if (p->status == 1)
        cout << "Discharge Time    : " << p->dischargeDateTime << "\n";
    else
        cout << "Discharge Time    : NOT YET (using current time)\n";
    cout << "Days (approx)     : " << days << "\n";
}

// -----------------------------
// 8. Bill Menu
// -----------------------------
void billMenu() {
    if (!head) {
        cout << "\nNo patients available.\n";
        return;
    }

    cout << "\n=== BILL MENU ===\nEnter Patient ID: ";
    int id;
    cin >> id;

    Patient* p = searchPatient(id);
    if (!p) {
        cout << "Patient not found.\n";
        return;
    }
    if (p->status == 0) {
        cout << "Patient not discharged yet.\n";
        return;
    }

    int days = calculateDays(p->admissionEpoch, p->dischargeEpoch);
    int rate = getRoomCharge(p->wardType);

    cout << "\n--- BILL DETAILS ---\n";
    cout << "Name              : " << p->name << "\n";
    cout << "Ward Type         : " << p->wardType << "\n";
    cout << "Room No           : " << p->roomNo << "\n";
    cout << "Admission Time    : " << p->admissionDateTime << "\n";
    cout << "Discharge Time    : " << p->dischargeDateTime << "\n";
    cout << "Days Stayed       : " << days << "\n";
    cout << "Room Charge/Day   : " << rate << "\n";
    cout << "Total Bill        : " << p->totalBill << "\n";
}

// -----------------------------
// MAIN
// -----------------------------
int main() {
    loadFromFile();

    while (true) {
        cout << "\n=========== HOSPITAL MENU ===========\n";
        cout << "1. Add Patients\n";
        cout << "2. Discharge Patients\n";
        cout << "3. Search Patients\n";
        cout << "4. Display All Patients\n";
        cout << "5. Room Allocation System\n";
        cout << "6. Track Critical Patients\n";
        cout << "7. Patient Stay Duration\n";
        cout << "8. Bill\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";

        int ch;
        if (!(cin >> ch)) break;

        switch (ch) {
            case 1: addPatient(); break;
            case 2: dischargePatient(); break;
            case 3: searchPatients(); break;
            case 4: displayAllPatients(); break;
            case 5: roomAllocationSystem(); break;
            case 6: trackCriticalPatients(); break;
            case 7: patientStayDuration(); break;
            case 8: billMenu(); break;
            case 9:
                saveToFile();
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice.\n";
        }
    }

    return 0;
}
