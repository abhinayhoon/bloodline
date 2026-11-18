/*
 *
 *  BloodLine - Blood Donation Camp Management System
 *  Built using Object-Oriented Programming in C++
 *
 *
 *  OOP Concepts Demonstrated:
 *    - Classes & Objects
 *    - Inheritance (Single & Multilevel)
 *    - Polymorphism (Virtual functions, Function overloading)
 *    - Encapsulation (Private members + getters/setters)
 *    - Abstraction (Abstract base class with pure virtual fn)
 *    - Operator Overloading (<< for display)
 *    - Composition (Record has-a Donor, has-a Camp)
 *    - Static Members (tracking counts)
 *    - File Handling (save/load to .txt files)
 *    - Constructor / Destructor
 *    - Friend Function
 *
 * ============================================================
 */

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ─────────────────────────────────────────────
//  Utility: clear screen, pause, input helpers
// ─────────────────────────────────────────────

void clearScreen() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void pauseScreen() {
  cout << "\n  Press Enter to continue...";
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string getCurrentDate() {
  time_t now = time(0);
  tm *ltm = localtime(&now);
  ostringstream oss;
  oss << setfill('0') << setw(2) << ltm->tm_mday << "/" << setw(2)
      << (1 + ltm->tm_mon) << "/" << (1900 + ltm->tm_year);
  return oss.str();
}

// ─────────────────────────────────────────────
//  Class: BloodGroup
//  Encapsulates blood type info + compatibility
// ─────────────────────────────────────────────

class BloodGroup {
private:
  string type;     // A, B, AB, O
  string rhFactor; // + or -

public:
  // Default constructor
  BloodGroup() : type("O"), rhFactor("+") {}

  // Parameterized constructor
  BloodGroup(string t, string rh) : type(t), rhFactor(rh) {}

  // Getters (Encapsulation)
  string getType() const { return type; }
  string getRhFactor() const { return rhFactor; }
  string getFullType() const { return type + rhFactor; }

  // Setters
  void setType(string t) { type = t; }
  void setRhFactor(string rh) { rhFactor = rh; }

  // Check if this blood group is a universal donor
  bool isUniversalDonor() const { return (type == "O" && rhFactor == "-"); }

  // Check if this blood group is a universal recipient
  bool isUniversalRecipient() const {
    return (type == "AB" && rhFactor == "+");
  }

  // Get list of blood groups this type can donate to
  vector<string> canDonateTo() const {
    vector<string> compatible;
    string full = getFullType();

    if (full == "O-") {
      compatible = {"O-", "O+", "A-", "A+", "B-", "B+", "AB-", "AB+"};
    } else if (full == "O+") {
      compatible = {"O+", "A+", "B+", "AB+"};
    } else if (full == "A-") {
      compatible = {"A-", "A+", "AB-", "AB+"};
    } else if (full == "A+") {
      compatible = {"A+", "AB+"};
    } else if (full == "B-") {
      compatible = {"B-", "B+", "AB-", "AB+"};
    } else if (full == "B+") {
      compatible = {"B+", "AB+"};
    } else if (full == "AB-") {
      compatible = {"AB-", "AB+"};
    } else if (full == "AB+") {
      compatible = {"AB+"};
    }
    return compatible;
  }

  // Operator overloading: << for display
  friend ostream &operator<<(ostream &os, const BloodGroup &bg);
};

// Friend function: overloaded << operator
ostream &operator<<(ostream &os, const BloodGroup &bg) {
  os << bg.type << bg.rhFactor;
  if (bg.isUniversalDonor())
    os << " (Universal Donor)";
  if (bg.isUniversalRecipient())
    os << " (Universal Recipient)";
  return os;
}

// ─────────────────────────────────────────────
//  Abstract Base Class: Person
//  Demonstrates: Abstraction, Pure Virtual Fn
// ─────────────────────────────────────────────

class Person {
protected:
  string name;
  int age;
  string gender;
  string phone;
  string address;

public:
  // Constructor
  Person() : name(""), age(0), gender(""), phone(""), address("") {}
  Person(string n, int a, string g, string ph, string addr)
      : name(n), age(a), gender(g), phone(ph), address(addr) {}

  // Virtual destructor (important for polymorphism)
  virtual ~Person() {
    // Destructor called
  }

  // Getters (Encapsulation)
  string getName() const { return name; }
  int getAge() const { return age; }
  string getGender() const { return gender; }
  string getPhone() const { return phone; }
  string getAddress() const { return address; }

  // Setters
  void setName(string n) { name = n; }
  void setAge(int a) { age = a; }
  void setGender(string g) { gender = g; }
  void setPhone(string ph) { phone = ph; }
  void setAddress(string addr) { address = addr; }

  // Pure virtual function - makes Person abstract
  virtual void displayInfo() const = 0;

  // Virtual function for polymorphism
  virtual string getRole() const { return "Person"; }
};

// ─────────────────────────────────────────────
//  Class: Donor (inherits from Person)
//  Demonstrates: Inheritance, Polymorphism
// ─────────────────────────────────────────────

class Donor : public Person {
private:
  string donorID;
  BloodGroup bloodGroup; // Composition
  float weight;
  float hemoglobin;
  string lastDonationDate;
  bool consentToShowRecord;
  string email;

  static int donorCount; // Static member

public:
  // Default constructor
  Donor()
      : Person(), weight(0), hemoglobin(0), lastDonationDate("N/A"),
        consentToShowRecord(true), email("") {
    donorCount++;
    donorID = "D-" + to_string(1000 + donorCount);
  }

  // Parameterized constructor
  Donor(string n, int a, string g, string ph, string addr, BloodGroup bg,
        float w, float hb, string email_addr)
      : Person(n, a, g, ph, addr), bloodGroup(bg), weight(w), hemoglobin(hb),
        lastDonationDate("N/A"), consentToShowRecord(true), email(email_addr) {
    donorCount++;
    donorID = "D-" + to_string(1000 + donorCount);
  }

  // Copy constructor
  Donor(const Donor &other) : Person(other) {
    donorID = other.donorID;
    bloodGroup = other.bloodGroup;
    weight = other.weight;
    hemoglobin = other.hemoglobin;
    lastDonationDate = other.lastDonationDate;
    consentToShowRecord = other.consentToShowRecord;
    email = other.email;
  }

  // Destructor
  ~Donor() override {}

  // Getters
  string getDonorID() const { return donorID; }
  BloodGroup getBloodGroup() const { return bloodGroup; }
  float getWeight() const { return weight; }
  float getHemoglobin() const { return hemoglobin; }
  string getLastDonationDate() const { return lastDonationDate; }
  bool getConsent() const { return consentToShowRecord; }
  string getEmail() const { return email; }
  static int getDonorCount() { return donorCount; }

  // Setters
  void setDonorID(string id) { donorID = id; }
  void setBloodGroup(BloodGroup bg) { bloodGroup = bg; }
  void setWeight(float w) { weight = w; }
  void setHemoglobin(float hb) { hemoglobin = hb; }
  void setLastDonationDate(string d) { lastDonationDate = d; }
  void setConsent(bool c) { consentToShowRecord = c; }
  void setEmail(string e) { email = e; }

  // Overriding pure virtual function (Polymorphism)
  void displayInfo() const override {
    cout << "\n  ┌─────────────────────────────────────┐\n";
    cout << "  │  DONOR CARD                         │\n";
    cout << "  ├─────────────────────────────────────┤\n";
    cout << "  │  ID      : " << left << setw(25) << donorID << "│\n";
    cout << "  │  Name    : " << left << setw(25) << name << "│\n";
    cout << "  │  Age     : " << left << setw(25) << age << "│\n";
    cout << "  │  Gender  : " << left << setw(25) << gender << "│\n";
    cout << "  │  Blood   : " << left << setw(25) << bloodGroup.getFullType()
         << "│\n";
    cout << "  │  Weight  : " << left << setw(22) << weight << "kg │\n";
    cout << "  │  Hb      : " << left << setw(20) << hemoglobin << "g/dL │\n";
    cout << "  │  Phone   : " << left << setw(25) << phone << "│\n";
    cout << "  │  Email   : " << left << setw(25) << email << "│\n";
    cout << "  │  Last Don: " << left << setw(25) << lastDonationDate << "│\n";
    cout << "  └─────────────────────────────────────┘\n";
  }

  // Override getRole (Polymorphism)
  string getRole() const override { return "Donor"; }

  // Operator overloading: << for quick display
  friend ostream &operator<<(ostream &os, const Donor &d);

  // Save to file
  void saveToFile(ofstream &file) const {
    file << donorID << "|" << name << "|" << age << "|" << gender << "|"
         << phone << "|" << address << "|" << bloodGroup.getType() << "|"
         << bloodGroup.getRhFactor() << "|" << weight << "|" << hemoglobin
         << "|" << lastDonationDate << "|" << consentToShowRecord << "|"
         << email << "\n";
  }

  // Load from file
  static Donor loadFromFile(const string &line) {
    Donor d;
    istringstream iss(line);
    string token;
    string bgType, bgRh;
    int consent;

    getline(iss, d.donorID, '|');
    getline(iss, d.name, '|');
    getline(iss, token, '|');
    d.age = stoi(token);
    getline(iss, d.gender, '|');
    getline(iss, d.phone, '|');
    getline(iss, d.address, '|');
    getline(iss, bgType, '|');
    getline(iss, bgRh, '|');
    d.bloodGroup = BloodGroup(bgType, bgRh);
    getline(iss, token, '|');
    d.weight = stof(token);
    getline(iss, token, '|');
    d.hemoglobin = stof(token);
    getline(iss, d.lastDonationDate, '|');
    getline(iss, token, '|');
    d.consentToShowRecord = (stoi(token) == 1);
    getline(iss, d.email, '|');

    donorCount--; // compensate for constructor increment
    return d;
  }
};

int Donor::donorCount = 0; // Static member initialization

ostream &operator<<(ostream &os, const Donor &d) {
  os << d.donorID << " | " << d.name << " | " << d.bloodGroup
     << " | Age: " << d.age;
  return os;
}

// ─────────────────────────────────────────────
//  Class: Recipient (inherits from Person)
//  Demonstrates: Inheritance, Polymorphism
// ─────────────────────────────────────────────

class Recipient : public Person {
private:
  string recipientID;
  BloodGroup requiredBloodGroup;
  string hospital;
  string urgencyLevel; // Low, Medium, High, Critical

  static int recipientCount;

public:
  Recipient() : Person(), hospital(""), urgencyLevel("Medium") {
    recipientCount++;
    recipientID = "R-" + to_string(2000 + recipientCount);
  }

  Recipient(string n, int a, string g, string ph, string addr, BloodGroup bg,
            string hosp, string urgency)
      : Person(n, a, g, ph, addr), requiredBloodGroup(bg), hospital(hosp),
        urgencyLevel(urgency) {
    recipientCount++;
    recipientID = "R-" + to_string(2000 + recipientCount);
  }

  ~Recipient() override {}

  // Overriding displayInfo (Polymorphism)
  void displayInfo() const override {
    cout << "\n  ┌─────────────────────────────────────┐\n";
    cout << "  │  RECIPIENT CARD                      │\n";
    cout << "  ├─────────────────────────────────────┤\n";
    cout << "  │  ID       : " << left << setw(24) << recipientID << "│\n";
    cout << "  │  Name     : " << left << setw(24) << name << "│\n";
    cout << "  │  Needs    : " << left << setw(24)
         << requiredBloodGroup.getFullType() << "│\n";
    cout << "  │  Hospital : " << left << setw(24) << hospital << "│\n";
    cout << "  │  Urgency  : " << left << setw(24) << urgencyLevel << "│\n";
    cout << "  └─────────────────────────────────────┘\n";
  }

  string getRole() const override { return "Recipient"; }

  // Getters
  string getRecipientID() const { return recipientID; }
  BloodGroup getRequiredBloodGroup() const { return requiredBloodGroup; }
  string getHospital() const { return hospital; }
  string getUrgencyLevel() const { return urgencyLevel; }
};

int Recipient::recipientCount = 0;

// ─────────────────────────────────────────────
//  Class: EligibilityChecker
//  Checks if a donor is eligible to donate
// ─────────────────────────────────────────────

class EligibilityChecker {
private:
  static const int MIN_AGE = 18;
  static const int MAX_AGE = 65;
  constexpr static const float MIN_WEIGHT = 50.0f;
  constexpr static const float MIN_HEMOGLOBIN = 12.5f;
  static const int MIN_GAP_DAYS = 90;

public:
  struct Result {
    bool eligible;
    vector<string> passedChecks;
    vector<string> failedChecks;
  };

  // Function overloading: check with different params
  static Result check(const Donor &donor) {
    Result result;
    result.eligible = true;

    // Age check
    if (donor.getAge() >= MIN_AGE && donor.getAge() <= MAX_AGE) {
      result.passedChecks.push_back("Age: " + to_string(donor.getAge()) +
                                    " (within 18-65 range)");
    } else {
      result.failedChecks.push_back("Age: " + to_string(donor.getAge()) +
                                    " (must be 18-65)");
      result.eligible = false;
    }

    // Weight check
    if (donor.getWeight() >= MIN_WEIGHT) {
      result.passedChecks.push_back(
          "Weight: " + to_string((int)donor.getWeight()) +
          " kg (above 50 kg minimum)");
    } else {
      result.failedChecks.push_back(
          "Weight: " + to_string((int)donor.getWeight()) +
          " kg (below 50 kg minimum)");
      result.eligible = false;
    }

    // Hemoglobin check
    if (donor.getHemoglobin() >= MIN_HEMOGLOBIN) {
      ostringstream oss;
      oss << fixed << setprecision(1) << donor.getHemoglobin();
      result.passedChecks.push_back("Hemoglobin: " + oss.str() +
                                    " g/dL (above 12.5 threshold)");
    } else {
      ostringstream oss;
      oss << fixed << setprecision(1) << donor.getHemoglobin();
      result.failedChecks.push_back("Hemoglobin: " + oss.str() +
                                    " g/dL (below 12.5 threshold)");
      result.eligible = false;
    }

    return result;
  }

  // Overloaded: check with additional hemoglobin param
  static Result check(const Donor &donor, float currentHemoglobin) {
    Donor temp = donor; // Copy constructor used here
    temp.setHemoglobin(currentHemoglobin);
    return check(temp);
  }

  static void displayResult(const Result &result) {
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║     ELIGIBILITY CHECK RESULT          ║\n";
    cout << "  ╠═══════════════════════════════════════╣\n";

    for (const auto &check : result.passedChecks) {
      cout << "  ║  ✓ " << left << setw(35) << check << "║\n";
    }
    for (const auto &check : result.failedChecks) {
      cout << "  ║  ✗ " << left << setw(35) << check << "║\n";
    }

    cout << "  ╠═══════════════════════════════════════╣\n";
    if (result.eligible) {
      cout << "  ║  RESULT: ELIGIBLE TO DONATE           ║\n";
    } else {
      cout << "  ║  RESULT: NOT ELIGIBLE (DEFERRED)      ║\n";
    }
    cout << "  ╚═══════════════════════════════════════╝\n";
  }
};

// ─────────────────────────────────────────────
//  Class: Camp
//  Represents a blood donation camp event
// ─────────────────────────────────────────────

class Camp {
private:
  string campID;
  string location;
  string date;
  int targetUnits;
  int collectedUnits;
  vector<string> registeredDonorIDs;

  static int campCount;

public:
  Camp() : location(""), date(""), targetUnits(0), collectedUnits(0) {
    campCount++;
    campID = "C-" + to_string(100 + campCount);
  }

  Camp(string loc, string d, int target)
      : location(loc), date(d), targetUnits(target), collectedUnits(0) {
    campCount++;
    campID = "C-" + to_string(100 + campCount);
  }

  // Getters
  string getCampID() const { return campID; }
  string getLocation() const { return location; }
  string getDate() const { return date; }
  int getTargetUnits() const { return targetUnits; }
  int getCollectedUnits() const { return collectedUnits; }
  int getRegistrationCount() const { return registeredDonorIDs.size(); }

  // Setters
  void setCampID(string id) { campID = id; }
  void setLocation(string loc) { location = loc; }
  void setDate(string d) { date = d; }
  void setTargetUnits(int t) { targetUnits = t; }

  // Register a donor to this camp
  void registerDonor(string donorID) { registeredDonorIDs.push_back(donorID); }

  // Record a collected unit
  void recordCollection() { collectedUnits++; }

  // Get progress percentage
  float getProgress() const {
    if (targetUnits == 0)
      return 0;
    return (float)collectedUnits / targetUnits * 100;
  }

  void displayInfo() const {
    cout << "\n  ┌─────────────────────────────────────┐\n";
    cout << "  │  CAMP: " << left << setw(29) << location << "│\n";
    cout << "  ├─────────────────────────────────────┤\n";
    cout << "  │  ID        : " << left << setw(23) << campID << "│\n";
    cout << "  │  Date      : " << left << setw(23) << date << "│\n";
    cout << "  │  Target    : " << left << setw(20) << targetUnits
         << "units│\n";
    cout << "  │  Collected : " << left << setw(20) << collectedUnits
         << "units│\n";

    // Progress bar
    int barWidth = 20;
    int filled = (int)(getProgress() / 100 * barWidth);
    cout << "  │  Progress  : [";
    for (int i = 0; i < barWidth; i++) {
      cout << (i < filled ? "█" : "░");
    }
    cout << "] " << setw(3) << (int)getProgress() << "% │\n";
    cout << "  │  Donors    : " << left << setw(23) << registeredDonorIDs.size()
         << "│\n";
    cout << "  └─────────────────────────────────────┘\n";
  }

  // Save to file
  void saveToFile(ofstream &file) const {
    file << campID << "|" << location << "|" << date << "|" << targetUnits
         << "|" << collectedUnits << "\n";
  }
};

int Camp::campCount = 0;

// ─────────────────────────────────────────────
//  Class: DonationRecord
//  Composition: has-a Donor reference, has-a Camp
// ─────────────────────────────────────────────

class DonationRecord {
private:
  string recordID;
  string donorID;
  string donorName;
  string bloodGroupStr;
  string campLocation;
  string donationDate;
  string status; // Completed, Deferred, Pending

  static int recordCount;

public:
  DonationRecord()
      : donorID(""), donorName(""), bloodGroupStr(""), campLocation(""),
        donationDate(""), status("Pending") {
    recordCount++;
    recordID = "R-" + to_string(2200 + recordCount);
  }

  DonationRecord(string dID, string dName, string bg, string campLoc,
                 string date, string st)
      : donorID(dID), donorName(dName), bloodGroupStr(bg),
        campLocation(campLoc), donationDate(date), status(st) {
    recordCount++;
    recordID = "R-" + to_string(2200 + recordCount);
  }

  // Getters
  string getRecordID() const { return recordID; }
  string getDonorID() const { return donorID; }
  string getDonorName() const { return donorName; }
  string getBloodGroup() const { return bloodGroupStr; }
  string getCampLocation() const { return campLocation; }
  string getDate() const { return donationDate; }
  string getStatus() const { return status; }
  static int getRecordCount() { return recordCount; }

  // Setters
  void setRecordID(string id) { recordID = id; }
  void setStatus(string s) { status = s; }

  void displayInfo() const {
    cout << "  " << left << setw(10) << recordID << setw(10) << donorID
         << setw(20) << donorName << setw(8) << bloodGroupStr << setw(24)
         << campLocation << setw(12) << donationDate << status << "\n";
  }

  // Save to file
  void saveToFile(ofstream &file) const {
    file << recordID << "|" << donorID << "|" << donorName << "|"
         << bloodGroupStr << "|" << campLocation << "|" << donationDate << "|"
         << status << "\n";
  }

  // Load from file
  static DonationRecord loadFromFile(const string &line) {
    DonationRecord r;
    istringstream iss(line);

    getline(iss, r.recordID, '|');
    getline(iss, r.donorID, '|');
    getline(iss, r.donorName, '|');
    getline(iss, r.bloodGroupStr, '|');
    getline(iss, r.campLocation, '|');
    getline(iss, r.donationDate, '|');
    getline(iss, r.status, '|');

    recordCount--; // compensate
    return r;
  }
};

int DonationRecord::recordCount = 0;

// ─────────────────────────────────────────────
//  Class: Account
//  User account for login/registration
// ─────────────────────────────────────────────

class Account {
private:
  string username;
  string password;
  string fullName;
  string role; // admin, donor, volunteer

public:
  Account() : username(""), password(""), fullName(""), role("donor") {}
  Account(string user, string pass, string name, string r)
      : username(user), password(pass), fullName(name), role(r) {}

  // Getters
  string getUsername() const { return username; }
  string getFullName() const { return fullName; }
  string getRole() const { return role; }

  // Verify password
  bool verifyPassword(const string &pass) const { return password == pass; }

  void displayInfo() const {
    cout << "  Username: " << username << " | Name: " << fullName
         << " | Role: " << role << "\n";
  }

  // Save to file
  void saveToFile(ofstream &file) const {
    file << username << "|" << password << "|" << fullName << "|" << role
         << "\n";
  }

  // Load from file
  static Account loadFromFile(const string &line) {
    Account a;
    istringstream iss(line);
    getline(iss, a.username, '|');
    getline(iss, a.password, '|');
    getline(iss, a.fullName, '|');
    getline(iss, a.role, '|');
    return a;
  }
};

// ─────────────────────────────────────────────
//  Class: BloodLineSystem
//  The main manager - ties everything together
// ─────────────────────────────────────────────

class BloodLineSystem {
private:
  vector<Donor> donors;
  vector<Recipient> recipients;
  vector<Camp> camps;
  vector<DonationRecord> records;
  vector<Account> accounts;
  Account *currentUser;

  // File paths
  const string DONORS_FILE = "donors.txt";
  const string RECORDS_FILE = "records.txt";
  const string ACCOUNTS_FILE = "accounts.txt";
  const string CAMPS_FILE = "camps.txt";

public:
  BloodLineSystem() : currentUser(nullptr) {
    loadData();
    // Add default camps if none exist
    if (camps.empty()) {
      camps.push_back(Camp("RamSay Hospital, Nainital", "12/08/2026", 80));
      camps.push_back(Camp("GEHU Bhimtal Campus", "19/08/2026", 150));
      camps.push_back(Camp("Central Himalayan Environment Association(CHEA)",
                           "02/09/2026", 60));
      camps.push_back(
          Camp("Graphic Era, Deemed to be University", "14/09/2026", 45));
    }
  }

  ~BloodLineSystem() {
    saveData();
    if (currentUser)
      delete currentUser;
  }

  // ── File I/O ──

  void saveData() {
    // Save donors
    ofstream dFile(DONORS_FILE);
    if (dFile.is_open()) {
      for (const auto &d : donors) {
        d.saveToFile(dFile);
      }
      dFile.close();
    }

    // Save records
    ofstream rFile(RECORDS_FILE);
    if (rFile.is_open()) {
      for (const auto &r : records) {
        r.saveToFile(rFile);
      }
      rFile.close();
    }

    // Save accounts
    ofstream aFile(ACCOUNTS_FILE);
    if (aFile.is_open()) {
      for (const auto &a : accounts) {
        a.saveToFile(aFile);
      }
      aFile.close();
    }
  }

  void loadData() {
    // Load donors
    ifstream dFile(DONORS_FILE);
    if (dFile.is_open()) {
      string line;
      while (getline(dFile, line)) {
        if (!line.empty()) {
          donors.push_back(Donor::loadFromFile(line));
        }
      }
      dFile.close();
    }

    // Load records
    ifstream rFile(RECORDS_FILE);
    if (rFile.is_open()) {
      string line;
      while (getline(rFile, line)) {
        if (!line.empty()) {
          records.push_back(DonationRecord::loadFromFile(line));
        }
      }
      rFile.close();
    }

    // Load accounts
    ifstream aFile(ACCOUNTS_FILE);
    if (aFile.is_open()) {
      string line;
      while (getline(aFile, line)) {
        if (!line.empty()) {
          accounts.push_back(Account::loadFromFile(line));
        }
      }
      aFile.close();
    }
  }

  // ── Account Management ──

  void createAccount() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║        CREATE NEW ACCOUNT             ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    string username, password, fullName, role;

    cout << "  Enter username    : ";
    getline(cin, username);

    // Check if username already exists
    for (const auto &a : accounts) {
      if (a.getUsername() == username) {
        cout << "\n  ✗ Username already taken. Try another.\n";
        pauseScreen();
        return;
      }
    }

    cout << "  Enter password    : ";
    getline(cin, password);
    cout << "  Enter full name   : ";
    getline(cin, fullName);

    cout << "  Select role:\n";
    cout << "    1. Donor\n";
    cout << "    2. Volunteer\n";
    cout << "    3. Admin\n";
    cout << "  Choice: ";
    string choice;
    getline(cin, choice);

    if (choice == "1")
      role = "donor";
    else if (choice == "2")
      role = "volunteer";
    else if (choice == "3")
      role = "admin";
    else
      role = "donor";

    accounts.push_back(Account(username, password, fullName, role));
    saveData();

    cout << "\n  ✓ Account created successfully!\n";
    cout << "  Welcome, " << fullName << "!\n";
    pauseScreen();
  }

  bool login() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║             LOGIN                     ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    string username, password;
    cout << "  Username: ";
    getline(cin, username);
    cout << "  Password: ";
    getline(cin, password);

    for (auto &a : accounts) {
      if (a.getUsername() == username && a.verifyPassword(password)) {
        currentUser = new Account(a);
        cout << "\n  ✓ Welcome back, " << a.getFullName() << "!\n";
        pauseScreen();
        return true;
      }
    }

    cout << "\n  ✗ Invalid credentials. Try again.\n";
    pauseScreen();
    return false;
  }

  // ── Donor Registration ──

  void registerDonor() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║       REGISTER NEW DONOR              ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    string name, gender, phone, address, email;
    int age;
    float weight, hemoglobin;
    string bgType, bgRh;

    cout << "  Full name       : ";
    getline(cin, name);

    cout << "  Age             : ";
    cin >> age;
    cin.ignore();

    cout << "  Gender (M/F/O)  : ";
    getline(cin, gender);

    cout << "  Phone           : ";
    getline(cin, phone);

    cout << "  Email           : ";
    getline(cin, email);

    cout << "  Address         : ";
    getline(cin, address);

    cout << "  Blood group type (A/B/AB/O): ";
    getline(cin, bgType);

    cout << "  Rh factor (+/-) : ";
    getline(cin, bgRh);

    cout << "  Weight (kg)     : ";
    cin >> weight;

    cout << "  Hemoglobin (g/dL): ";
    cin >> hemoglobin;
    cin.ignore();

    BloodGroup bg(bgType, bgRh);
    Donor newDonor(name, age, gender, phone, address, bg, weight, hemoglobin,
                   email);

    // Consent
    cout << "\n  Allow your donation records to be visible to others? (y/n): ";
    string consent;
    getline(cin, consent);
    newDonor.setConsent(consent == "y" || consent == "Y");

    donors.push_back(newDonor);
    saveData();

    cout << "\n  ✓ Donor registered successfully!\n";
    newDonor.displayInfo();
    pauseScreen();
  }

  // ── Eligibility Check ──

  void checkEligibility() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║      CHECK ELIGIBILITY                ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    if (donors.empty()) {
      cout << "  No donors registered yet. Register first.\n";
      pauseScreen();
      return;
    }

    // List donors
    cout << "  Registered Donors:\n";
    for (size_t i = 0; i < donors.size(); i++) {
      cout << "    " << (i + 1) << ". " << donors[i] << "\n";
    }

    cout << "\n  Select donor number: ";
    int choice;
    cin >> choice;
    cin.ignore();

    if (choice < 1 || choice > (int)donors.size()) {
      cout << "  Invalid selection.\n";
      pauseScreen();
      return;
    }

    // Demonstrating polymorphism: calling through base pointer
    Person *personPtr = &donors[choice - 1];
    cout << "\n  Checking eligibility for " << personPtr->getName()
         << " (Role: " << personPtr->getRole() << ")\n";

    EligibilityChecker::Result result =
        EligibilityChecker::check(donors[choice - 1]);
    EligibilityChecker::displayResult(result);
    pauseScreen();
  }

  // ── Record a Donation ──

  void recordDonation() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║      RECORD A DONATION                ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    if (donors.empty()) {
      cout << "  No donors registered. Register a donor first.\n";
      pauseScreen();
      return;
    }

    // Select donor
    cout << "  Select Donor:\n";
    for (size_t i = 0; i < donors.size(); i++) {
      cout << "    " << (i + 1) << ". " << donors[i] << "\n";
    }
    cout << "\n  Choice: ";
    int dChoice;
    cin >> dChoice;
    cin.ignore();

    if (dChoice < 1 || dChoice > (int)donors.size()) {
      cout << "  Invalid selection.\n";
      pauseScreen();
      return;
    }

    // Check eligibility first
    EligibilityChecker::Result result =
        EligibilityChecker::check(donors[dChoice - 1]);
    EligibilityChecker::displayResult(result);

    string status;
    if (result.eligible) {
      status = "Completed";
      cout << "\n  ✓ Donor is eligible. Proceeding with donation.\n";
    } else {
      status = "Deferred";
      cout << "\n  ✗ Donor is deferred. Recording with deferred status.\n";
    }

    // Select camp
    cout << "\n  Select Camp:\n";
    for (size_t i = 0; i < camps.size(); i++) {
      cout << "    " << (i + 1) << ". " << camps[i].getLocation() << " ("
           << camps[i].getDate() << ")\n";
    }
    cout << "\n  Choice: ";
    int cChoice;
    cin >> cChoice;
    cin.ignore();

    if (cChoice < 1 || cChoice > (int)camps.size()) {
      cout << "  Invalid selection.\n";
      pauseScreen();
      return;
    }

    // Create record
    DonationRecord record(
        donors[dChoice - 1].getDonorID(), donors[dChoice - 1].getName(),
        donors[dChoice - 1].getBloodGroup().getFullType(),
        camps[cChoice - 1].getLocation(), getCurrentDate(), status);

    records.push_back(record);

    if (status == "Completed") {
      camps[cChoice - 1].recordCollection();
      camps[cChoice - 1].registerDonor(donors[dChoice - 1].getDonorID());
      donors[dChoice - 1].setLastDonationDate(getCurrentDate());
    }

    saveData();

    cout << "\n  ✓ Donation record created: " << record.getRecordID() << "\n";
    pauseScreen();
  }

  // ── View Donor Records ──

  void viewRecords() {
    clearScreen();
    cout << "\n  "
            "╔═════════════════════════════════════════════════════════════════"
            "══════════════════════╗\n";
    cout << "  ║                              DONATION RECORDS                 "
            "                      ║\n";
    cout << "  "
            "╚═════════════════════════════════════════════════════════════════"
            "══════════════════════╝\n\n";

    if (records.empty()) {
      cout << "  No records yet. Make a donation first.\n";
      pauseScreen();
      return;
    }

    cout << "  " << left << setw(10) << "Record" << setw(10) << "Donor"
         << setw(20) << "Name" << setw(8) << "Blood" << setw(24) << "Camp"
         << setw(12) << "Date"
         << "Status\n";
    cout << "  " << string(84, '-') << "\n";

    for (const auto &r : records) {
      r.displayInfo();
    }
    pauseScreen();
  }

  // ── View All Donors ──

  void viewDonors() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║         ALL REGISTERED DONORS         ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    if (donors.empty()) {
      cout << "  No donors registered yet.\n";
      pauseScreen();
      return;
    }

    for (const auto &d : donors) {
      if (d.getConsent()) {
        d.displayInfo();
      } else {
        cout << "\n  [" << d.getDonorID()
             << " - Record hidden by donor's request]\n";
      }
    }

    cout << "\n  Total registered donors: " << donors.size() << "\n";
    pauseScreen();
  }

  // ── Search by Blood Group ──

  void searchByBloodGroup() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║     SEARCH DONORS BY BLOOD GROUP      ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    string bgType, bgRh;
    cout << "  Blood group type (A/B/AB/O): ";
    getline(cin, bgType);
    cout << "  Rh factor (+/-): ";
    getline(cin, bgRh);

    string searchGroup = bgType + bgRh;
    bool found = false;

    cout << "\n  Donors with blood group " << searchGroup << ":\n";
    cout << "  " << string(40, '-') << "\n";

    for (const auto &d : donors) {
      if (d.getBloodGroup().getFullType() == searchGroup && d.getConsent()) {
        cout << "  " << d << "\n";
        found = true;
      }
    }

    if (!found) {
      cout << "  No donors found with blood group " << searchGroup << "\n";
    }

    // Show compatibility info
    BloodGroup bg(bgType, bgRh);
    cout << "\n  Compatibility info for " << bg << ":\n";
    cout << "  Can donate to: ";
    for (const auto &c : bg.canDonateTo()) {
      cout << c << " ";
    }
    cout << "\n";

    pauseScreen();
  }

  // ── View Camp Info ──

  void viewCamps() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║       CAMP INFORMATION                ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n";

    for (const auto &c : camps) {
      c.displayInfo();
    }
    pauseScreen();
  }

  // ── Display Blood Group Info ──

  void displayBloodGroupInfo() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║     BLOOD GROUP INFORMATION           ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    string groups[] = {"O-", "O+", "A+", "A-", "B+", "B-", "AB+", "AB-"};
    string notes[] = {"Universal donor - can donate to all blood types",
                      "Most common blood type globally",
                      "Second most common blood type",
                      "Rare - only 6% of the population",
                      "Common in South Asian populations",
                      "Rare - only 2% of the population",
                      "Universal recipient - can receive from all",
                      "Rarest blood type - less than 1%"};

    for (int i = 0; i < 8; i++) {
      cout << "  " << left << setw(6) << groups[i] << " - " << notes[i] << "\n";
    }

    cout << "\n  ── Compatibility Chart ──\n\n";
    cout << "  " << left << setw(8) << "Type"
         << "Can donate to\n";
    cout << "  " << string(50, '-') << "\n";

    string types[] = {"O", "O", "A", "A", "B", "B", "AB", "AB"};
    string rhs[] = {"-", "+", "-", "+", "-", "+", "-", "+"};

    for (int i = 0; i < 8; i++) {
      BloodGroup bg(types[i], rhs[i]);
      cout << "  " << left << setw(8) << bg.getFullType();
      for (const auto &c : bg.canDonateTo()) {
        cout << c << " ";
      }
      cout << "\n";
    }

    pauseScreen();
  }

  // ── Benefits of Blood Donation ──

  void displayBenefits() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║   BENEFITS OF BLOOD DONATION          ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    cout << "  HEALTH BENEFITS:\n";
    cout << "  ─────────────────\n";
    cout << "  1. Reduces harmful iron stores in the body\n";
    cout << "  2. Lowers risk of heart disease and cancer\n";
    cout << "  3. Burns approximately 650 calories per donation\n";
    cout << "  4. Stimulates blood cell production\n";
    cout << "  5. Free health screening with every donation\n\n";

    cout << "  EMOTIONAL BENEFITS:\n";
    cout << "  ─────────────────────\n";
    cout << "  1. Sense of purpose and fulfillment\n";
    cout << "  2. Connection to community\n";
    cout << "  3. Knowing you saved up to 3 lives\n\n";

    cout << "  THINGS TO KNOW BEFORE DONATING:\n";
    cout << "  ────────────────────────────────\n";
    cout << "  - Eat a healthy meal 2-3 hours before\n";
    cout << "  - Drink plenty of water\n";
    cout << "  - Get a good night's sleep\n";
    cout << "  - Bring a valid photo ID\n";
    cout << "  - The process takes about 10-15 minutes\n";
    cout << "  - You can donate every 90 days (whole blood)\n\n";

    cout << "  AFTER DONATION:\n";
    cout << "  ────────────────\n";
    cout << "  - Rest for 10-15 minutes at the camp\n";
    cout << "  - Drink extra fluids for the next 24-48 hours\n";
    cout << "  - Avoid heavy lifting for 5 hours\n";
    cout << "  - Keep the bandage on for at least 4 hours\n";
    cout << "  - Eat iron-rich foods (spinach, red meat, beans)\n";

    pauseScreen();
  }

  // ── Mission & Vision ──

  void displayMissionVision() {
    clearScreen();
    cout << "\n  ╔═══════════════════════════════════════╗\n";
    cout << "  ║       ABOUT BLOODLINE                 ║\n";
    cout << "  ╚═══════════════════════════════════════╝\n\n";

    cout << "  OUR MISSION:\n";
    cout << "  ─────────────\n";
    cout << "  To simplify blood donation camp management\n";
    cout << "  and make the process transparent, efficient,\n";
    cout << "  and accessible to every community.\n\n";

    cout << "  OUR VISION:\n";
    cout << "  ────────────\n";
    cout << "  A world where no patient suffers due to a\n";
    cout << "  shortage of blood. Every drop counts, and\n";
    cout << "  every donor is a hero.\n\n";

    cout << "  WHY BLOODLINE?\n";
    cout << "  ───────────────\n";
    cout << "  - Built by students, for real-world impact\n";
    cout << "  - Tracks donors, camps, and records in one place\n";
    cout << "  - Eligibility checking prevents unsafe donations\n";
    cout << "  - Privacy-first: donors control record visibility\n";
    cout << "  - Open and transparent system design\n\n";

    cout << "  OOP PRINCIPLES USED:\n";
    cout << "  ────────────────────\n";
    cout << "  - Classes & Objects (Donor, Camp, Record, etc.)\n";
    cout << "  - Inheritance (Person -> Donor, Recipient)\n";
    cout << "  - Polymorphism (Virtual displayInfo())\n";
    cout << "  - Encapsulation (Private data + getters/setters)\n";
    cout << "  - Abstraction (Abstract Person class)\n";
    cout << "  - Operator Overloading (<< for display)\n";
    cout << "  - File Handling (save/load data)\n";
    cout << "  - Composition (Record has Donor + Camp)\n";
    cout << "  - Static Members (counting instances)\n";

    pauseScreen();
  }

  // - Main Menu -

  void showMainMenu() {
    while (true) {
      clearScreen();
      cout << "\n";
      cout << "  =======================================\n";
      cout << "  |                                     |\n";
      cout << "  |     ♥  B L O O D L I N E  ♥         |\n";
      cout << "  |     Blood Donation Camp Manager     |\n";
      cout << "  |                                     |\n";
      cout << "  =======================================\n\n";

      if (currentUser) {
        cout << "  Logged in as: " << currentUser->getFullName() << " ("
             << currentUser->getRole() << ")\n\n";
      }

      cout << "  - ACCOUNT ---------------------------\n";
      cout << "   1.  Create Account\n";
      cout << "   2.  Login\n\n";
      cout << "  - DONOR MANAGEMENT ------------------\n";
      cout << "   3.  Register as Donor\n";
      cout << "   4.  Check Eligibility\n";
      cout << "   5.  Record a Donation\n";
      cout << "   6.  View All Donors\n";
      cout << "   7.  Search Donors by Blood Group\n\n";
      cout << "  - RECORDS & CAMPS -------------------\n";
      cout << "   8.  View Donation Records\n";
      cout << "   9.  View Camp Information\n\n";
      cout << "  - INFORMATION -----------------------\n";
      cout << "  10.  Blood Group Information\n";
      cout << "  11.  Benefits of Blood Donation\n";
      cout << "  12.  About BloodLine (Mission/Vision)\n\n";
      cout << "   0.  Exit\n\n";
      cout << "  Enter choice: ";

      int choice;
      cin >> choice;
      cin.ignore();

      switch (choice) {
      case 1:
        createAccount();
        break;
      case 2:
        login();
        break;
      case 3:
        registerDonor();
        break;
      case 4:
        checkEligibility();
        break;
      case 5:
        recordDonation();
        break;
      case 6:
        viewDonors();
        break;
      case 7:
        searchByBloodGroup();
        break;
      case 8:
        viewRecords();
        break;
      case 9:
        viewCamps();
        break;
      case 10:
        displayBloodGroupInfo();
        break;
      case 11:
        displayBenefits();
        break;
      case 12:
        displayMissionVision();
        break;
      case 0:
        saveData();
        cout << "\n  Thank you for using BloodLine!\n";
        cout << "  Every donor is a hero. ♥\n\n";
        return;
      default:
        cout << "\n  Invalid choice. Try again.\n";
        pauseScreen();
      }
    }
  }
};

// ─────────────────────────────────────────────
//  Main Function
// ─────────────────────────────────────────────

int main() {
  BloodLineSystem system;
  system.showMainMenu();
  return 0;
}
