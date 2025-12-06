# Hospital Patient Management System (C++)

This is a *console-based Hospital Patient Management System* implemented in C++.

It manages:

- Patient registration (admission)
- Room allocation (General / ICU)
- Tracking critical patients
- Discharge with automatic bill calculation
- Patient stay duration
- Persistent storage in a text file (hospital_records.txt)

The system uses a *doubly linked list (DLL)* to store patients in memory and saves/loads all data to/from a file.

---

## 1. Features

### 🏥 Core Features

1. *Add Patient*
   - Assigns a unique *Patient ID* automatically.
   - Stores:
     - Name  
     - Age  
     - Gender  
     - Disease  
     - Critical status (priority: 1 = Critical, 0 = Normal)  
   - Automatically sets:
     - Admission date & time (current system time)
     - Status = Admitted
   - Patient is inserted in the linked list:
     - Critical patients → near the *head*
     - Normal patients → near the *tail*

2. *Discharge Patient*
   - Marks the patient as *discharged*.
   - Stores *discharge date & time*.
   - Calculates:
     - Number of days stayed (based on admission & discharge timestamps)
     - Total bill = days * room charge
   - Room charge:
     - General Ward → ₹1000 per day  
     - ICU → ₹5000 per day  

3. *Search Patient*
   - Search by *Patient ID*.
   - Shows:
     - Personal details (name, age, gender, disease)
     - Priority (Critical / Normal)
     - Room & ward type
     - Admission & discharge time
     - Status (Admitted / Discharged)
     - Total bill (if discharged)

4. *Display All Patients*
   - Lists *every patient* in the system in current DLL order:
     - ID, Name, Age, Gender, Disease
     - Priority, Room No, Ward Type
     - Status

5. *Room Allocation System*
   - Automatically allocates rooms based on priority:
     - Normal patients → *General Ward* (rooms: 101, 102, 103, …)
     - Critical patients → *ICU* (rooms: 201, 202, 203, …)
   - Prevents:
     - Allocating rooms to discharged patients
     - Allocating again if room is already assigned

6. *Track Critical Patients*
   - Submenu:
     1. Show all critical patients  
     2. Mark a patient as critical & *move to head* of DLL  
     3. Remove critical status & *move to tail* of DLL  
   - Demonstrates *priority management* using doubly linked list operations.

7. *Patient Stay Duration*
   - Shows approximate days stayed in hospital for a given patient:
     - Uses admission time and:
       - Discharge time, if already discharged  
       - Current time, if still admitted  

8. *Bill Menu*
   - Displays detailed bill for discharged patients:
     - Name, Ward Type, Room No
     - Admission & Discharge time
     - Days stayed
     - Room charge per day
     - Total bill

9. *Data Persistence*
   - All data is stored in hospital_records.txt.
   - On program start → *loads data* from file.
   - On updates → *saves data* to file.

---

## 2. Data Structures & File Format

### 2.1 In-Memory Structure

Each patient is stored as a struct Patient node in a *doubly linked list*:

```cpp
struct Patient {
    int id;
    string name;
    int age;
    string gender;
    string disease;
    int priority;        // 1 = critical, 0 = normal
    int roomNo;
    string wardType;     // "General", "ICU", or "NA"
    int status;          // 0 = admitted, 1 = discharged

    long long admissionEpoch;
    long long dischargeEpoch;
    string admissionDateTime;
    string dischargeDateTime;

    double totalBill;

    Patient* prev;
    Patient* next;
};
