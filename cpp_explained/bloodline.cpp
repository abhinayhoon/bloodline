// EXPLAINED REFERENCE COPY. The original source is in ../bloodline.cpp and remains unchanged.
// This is a source comment that explains nearby code.
/*
// This is a source comment that explains nearby code.
 * ============================================================
// This is a source comment that explains nearby code.
 *  BloodLine - Blood Donation Camp Management System
// This is a source comment that explains nearby code.
 *  Built using Object-Oriented Programming in C++
// This is a source comment that explains nearby code.
 * ============================================================
// This is a source comment that explains nearby code.
 *
// This is a source comment that explains nearby code.
 *  OOP Concepts Demonstrated:
// This is a source comment that explains nearby code.
 *    - Classes & Objects
// This is a source comment that explains nearby code.
 *    - Inheritance (Single & Multilevel)
// This is a source comment that explains nearby code.
 *    - Polymorphism (Virtual functions, Function overloading)
// This is a source comment that explains nearby code.
 *    - Encapsulation (Private members + getters/setters)
// This is a source comment that explains nearby code.
 *    - Abstraction (Abstract base class with pure virtual fn)
// This is a source comment that explains nearby code.
 *    - Operator Overloading (<< for display)
// This is a source comment that explains nearby code.
 *    - Composition (Record has-a Donor, has-a Camp)
// This is a source comment that explains nearby code.
 *    - Static Members (tracking counts)
// This is a source comment that explains nearby code.
 *    - File Handling (save/load to .txt files)
// This is a source comment that explains nearby code.
 *    - Constructor / Destructor
// This is a source comment that explains nearby code.
 *    - Friend Function
// This is a source comment that explains nearby code.
 *
// This is a source comment that explains nearby code.
 * ============================================================
// This is a source comment that explains nearby code.
 */

// This brings in <algorithm>, so this file can use its declarations.
#include <algorithm>
// This brings in <ctime>, so this file can use its declarations.
#include <ctime>
// This brings in <fstream>, so this file can use its declarations.
#include <fstream>
// This brings in <iomanip>, so this file can use its declarations.
#include <iomanip>
// This brings in <iostream>, so this file can use its declarations.
#include <iostream>
// This brings in <limits>, so this file can use its declarations.
#include <limits>
// This brings in <sstream>, so this file can use its declarations.
#include <sstream>
// This brings in <string>, so this file can use its declarations.
#include <string>
// This brings in <vector>, so this file can use its declarations.
#include <vector>

// This gives a convenient name to a type or namespace used below.
using namespace std;

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Utility: clear screen, pause, input helpers
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
void clearScreen() {
// This is a preprocessor instruction that configures what the compiler sees.
#ifdef _WIN32
// This performs one step of the current operation, then continues to the next statement.
  system("cls");
// This is a preprocessor instruction that configures what the compiler sees.
#else
// This performs one step of the current operation, then continues to the next statement.
  system("clear");
// This is a preprocessor instruction that configures what the compiler sees.
#endif
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
void pauseScreen() {
// This performs one step of the current operation, then continues to the next statement.
  cout << "\n  Press Enter to continue...";
// This performs one step of the current operation, then continues to the next statement.
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
string getCurrentDate() {
// This performs one step of the current operation, then continues to the next statement.
  time_t now = time(0);
// This performs one step of the current operation, then continues to the next statement.
  tm *ltm = localtime(&now);
// This performs one step of the current operation, then continues to the next statement.
  ostringstream oss;
// This is part of the current expression or block; read it together with the nearby lines.
  oss << setfill('0') << setw(2) << ltm->tm_mday << "/" << setw(2)
// This performs one step of the current operation, then continues to the next statement.
      << (1 + ltm->tm_mon) << "/" << (1900 + ltm->tm_year);
// This sends a result back to the code that called this function.
  return oss.str();
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: BloodGroup
// This is a source comment that explains nearby code.
//  Encapsulates blood type info + compatibility
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class BloodGroup {
// The following members are kept inside this class.
private:
// This creates or updates a value that the following logic uses.
  string type;     // A, B, AB, O
// This creates or updates a value that the following logic uses.
  string rhFactor; // + or -

// The following members are available to other parts of the program.
public:
// This is a source comment that explains nearby code.
  // Default constructor
// This is part of the current expression or block; read it together with the nearby lines.
  BloodGroup() : type("O"), rhFactor("+") {}

// This is a source comment that explains nearby code.
  // Parameterized constructor
// This creates or updates a value that the following logic uses.
  BloodGroup(string t, string rh) : type(t), rhFactor(rh) {}

// This is a source comment that explains nearby code.
  // Getters (Encapsulation)
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getType() const { return type; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getRhFactor() const { return rhFactor; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getFullType() const { return type + rhFactor; }

// This is a source comment that explains nearby code.
  // Setters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setType(string t) { type = t; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setRhFactor(string rh) { rhFactor = rh; }

// This is a source comment that explains nearby code.
  // Check if this blood group is a universal donor
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  bool isUniversalDonor() const { return (type == "O" && rhFactor == "-"); }

// This is a source comment that explains nearby code.
  // Check if this blood group is a universal recipient
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  bool isUniversalRecipient() const {
// This sends a result back to the code that called this function.
    return (type == "AB" && rhFactor == "+");
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Get list of blood groups this type can donate to
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  vector<string> canDonateTo() const {
// This creates or updates a value that the following logic uses.
    vector<string> compatible;
// This creates or updates a value that the following logic uses.
    string full = getFullType();

// This controls which part of the program runs next, based on the current condition or result.
    if (full == "O-") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"O-", "O+", "A-", "A+", "B-", "B+", "AB-", "AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else if (full == "O+") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"O+", "A+", "B+", "AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else if (full == "A-") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"A-", "A+", "AB-", "AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else if (full == "A+") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"A+", "AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else if (full == "B-") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"B-", "B+", "AB-", "AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else if (full == "B+") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"B+", "AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else if (full == "AB-") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"AB-", "AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else if (full == "AB+") {
// This performs one step of the current operation, then continues to the next statement.
      compatible = {"AB+"};
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This sends a result back to the code that called this function.
    return compatible;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Operator overloading: << for display
// This creates or updates a value that the following logic uses.
  friend ostream &operator<<(ostream &os, const BloodGroup &bg);
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// Friend function: overloaded << operator
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
ostream &operator<<(ostream &os, const BloodGroup &bg) {
// This performs one step of the current operation, then continues to the next statement.
  os << bg.type << bg.rhFactor;
// This controls which part of the program runs next, based on the current condition or result.
  if (bg.isUniversalDonor())
// This performs one step of the current operation, then continues to the next statement.
    os << " (Universal Donor)";
// This controls which part of the program runs next, based on the current condition or result.
  if (bg.isUniversalRecipient())
// This performs one step of the current operation, then continues to the next statement.
    os << " (Universal Recipient)";
// This sends a result back to the code that called this function.
  return os;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Abstract Base Class: Person
// This is a source comment that explains nearby code.
//  Demonstrates: Abstraction, Pure Virtual Fn
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class Person {
// The following members are shared with this class and its child classes.
protected:
// This creates or updates a value that the following logic uses.
  string name;
// This creates or updates a value that the following logic uses.
  int age;
// This creates or updates a value that the following logic uses.
  string gender;
// This creates or updates a value that the following logic uses.
  string phone;
// This creates or updates a value that the following logic uses.
  string address;

// The following members are available to other parts of the program.
public:
// This is a source comment that explains nearby code.
  // Constructor
// This is part of the current expression or block; read it together with the nearby lines.
  Person() : name(""), age(0), gender(""), phone(""), address("") {}
// This creates or updates a value that the following logic uses.
  Person(string n, int a, string g, string ph, string addr)
// This is part of the current expression or block; read it together with the nearby lines.
      : name(n), age(a), gender(g), phone(ph), address(addr) {}

// This is a source comment that explains nearby code.
  // Virtual destructor (important for polymorphism)
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  virtual ~Person() {
// This is a source comment that explains nearby code.
    // Destructor called
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Getters (Encapsulation)
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getName() const { return name; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  int getAge() const { return age; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getGender() const { return gender; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getPhone() const { return phone; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getAddress() const { return address; }

// This is a source comment that explains nearby code.
  // Setters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setName(string n) { name = n; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setAge(int a) { age = a; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setGender(string g) { gender = g; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setPhone(string ph) { phone = ph; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setAddress(string addr) { address = addr; }

// This is a source comment that explains nearby code.
  // Pure virtual function - makes Person abstract
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  virtual void displayInfo() const = 0;

// This is a source comment that explains nearby code.
  // Virtual function for polymorphism
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  virtual string getRole() const { return "Person"; }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: Donor (inherits from Person)
// This is a source comment that explains nearby code.
//  Demonstrates: Inheritance, Polymorphism
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class Donor : public Person {
// The following members are kept inside this class.
private:
// This creates or updates a value that the following logic uses.
  string donorID;
// This is part of the current expression or block; read it together with the nearby lines.
  BloodGroup bloodGroup; // Composition
// This performs one step of the current operation, then continues to the next statement.
  float weight;
// This performs one step of the current operation, then continues to the next statement.
  float hemoglobin;
// This creates or updates a value that the following logic uses.
  string lastDonationDate;
// This creates or updates a value that the following logic uses.
  bool consentToShowRecord;
// This creates or updates a value that the following logic uses.
  string email;

// This creates or updates a value that the following logic uses.
  static int donorCount; // Static member

// The following members are available to other parts of the program.
public:
// This is a source comment that explains nearby code.
  // Default constructor
// This is part of the current expression or block; read it together with the nearby lines.
  Donor()
// This is part of the current expression or block; read it together with the nearby lines.
      : Person(), weight(0), hemoglobin(0), lastDonationDate("N/A"),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        consentToShowRecord(true), email("") {
// This performs one step of the current operation, then continues to the next statement.
    donorCount++;
// This performs one step of the current operation, then continues to the next statement.
    donorID = "D-" + to_string(1000 + donorCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Parameterized constructor
// This creates or updates a value that the following logic uses.
  Donor(string n, int a, string g, string ph, string addr, BloodGroup bg,
// This creates or updates a value that the following logic uses.
        float w, float hb, string email_addr)
// This is part of the current expression or block; read it together with the nearby lines.
      : Person(n, a, g, ph, addr), bloodGroup(bg), weight(w), hemoglobin(hb),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        lastDonationDate("N/A"), consentToShowRecord(true), email(email_addr) {
// This performs one step of the current operation, then continues to the next statement.
    donorCount++;
// This performs one step of the current operation, then continues to the next statement.
    donorID = "D-" + to_string(1000 + donorCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Copy constructor
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  Donor(const Donor &other) : Person(other) {
// This performs one step of the current operation, then continues to the next statement.
    donorID = other.donorID;
// This performs one step of the current operation, then continues to the next statement.
    bloodGroup = other.bloodGroup;
// This performs one step of the current operation, then continues to the next statement.
    weight = other.weight;
// This performs one step of the current operation, then continues to the next statement.
    hemoglobin = other.hemoglobin;
// This performs one step of the current operation, then continues to the next statement.
    lastDonationDate = other.lastDonationDate;
// This performs one step of the current operation, then continues to the next statement.
    consentToShowRecord = other.consentToShowRecord;
// This performs one step of the current operation, then continues to the next statement.
    email = other.email;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Destructor
// This is part of the current expression or block; read it together with the nearby lines.
  ~Donor() override {}

// This is a source comment that explains nearby code.
  // Getters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getDonorID() const { return donorID; }
// This creates or updates a value that the following logic uses.
  BloodGroup getBloodGroup() const { return bloodGroup; }
// This creates or updates a value that the following logic uses.
  float getWeight() const { return weight; }
// This creates or updates a value that the following logic uses.
  float getHemoglobin() const { return hemoglobin; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getLastDonationDate() const { return lastDonationDate; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  bool getConsent() const { return consentToShowRecord; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getEmail() const { return email; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static int getDonorCount() { return donorCount; }

// This is a source comment that explains nearby code.
  // Setters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setDonorID(string id) { donorID = id; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setBloodGroup(BloodGroup bg) { bloodGroup = bg; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setWeight(float w) { weight = w; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setHemoglobin(float hb) { hemoglobin = hb; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setLastDonationDate(string d) { lastDonationDate = d; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setConsent(bool c) { consentToShowRecord = c; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setEmail(string e) { email = e; }

// This is a source comment that explains nearby code.
  // Overriding pure virtual function (Polymorphism)
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayInfo() const override {
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ┌─────────────────────────────────────┐\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  DONOR CARD                         │\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ├─────────────────────────────────────┤\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  ID      : " << left << setw(25) << donorID << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Name    : " << left << setw(25) << name << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Age     : " << left << setw(25) << age << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Gender  : " << left << setw(25) << gender << "│\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  │  Blood   : " << left << setw(25) << bloodGroup.getFullType()
// This performs one step of the current operation, then continues to the next statement.
         << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Weight  : " << left << setw(22) << weight << "kg │\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Hb      : " << left << setw(20) << hemoglobin << "g/dL │\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Phone   : " << left << setw(25) << phone << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Email   : " << left << setw(25) << email << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Last Don: " << left << setw(25) << lastDonationDate << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  └─────────────────────────────────────┘\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Override getRole (Polymorphism)
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getRole() const override { return "Donor"; }

// This is a source comment that explains nearby code.
  // Operator overloading: << for quick display
// This creates or updates a value that the following logic uses.
  friend ostream &operator<<(ostream &os, const Donor &d);

// This is a source comment that explains nearby code.
  // Save to file
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void saveToFile(ofstream &file) const {
// This is part of the current expression or block; read it together with the nearby lines.
    file << donorID << "|" << name << "|" << age << "|" << gender << "|"
// This is part of the current expression or block; read it together with the nearby lines.
         << phone << "|" << address << "|" << bloodGroup.getType() << "|"
// This is part of the current expression or block; read it together with the nearby lines.
         << bloodGroup.getRhFactor() << "|" << weight << "|" << hemoglobin
// This is part of the current expression or block; read it together with the nearby lines.
         << "|" << lastDonationDate << "|" << consentToShowRecord << "|"
// This performs one step of the current operation, then continues to the next statement.
         << email << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Load from file
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static Donor loadFromFile(const string &line) {
// This performs one step of the current operation, then continues to the next statement.
    Donor d;
// This performs one step of the current operation, then continues to the next statement.
    istringstream iss(line);
// This creates or updates a value that the following logic uses.
    string token;
// This creates or updates a value that the following logic uses.
    string bgType, bgRh;
// This creates or updates a value that the following logic uses.
    int consent;

// This performs one step of the current operation, then continues to the next statement.
    getline(iss, d.donorID, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, d.name, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, token, '|');
// This performs one step of the current operation, then continues to the next statement.
    d.age = stoi(token);
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, d.gender, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, d.phone, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, d.address, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, bgType, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, bgRh, '|');
// This performs one step of the current operation, then continues to the next statement.
    d.bloodGroup = BloodGroup(bgType, bgRh);
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, token, '|');
// This performs one step of the current operation, then continues to the next statement.
    d.weight = stof(token);
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, token, '|');
// This performs one step of the current operation, then continues to the next statement.
    d.hemoglobin = stof(token);
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, d.lastDonationDate, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, token, '|');
// This performs one step of the current operation, then continues to the next statement.
    d.consentToShowRecord = (stoi(token) == 1);
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, d.email, '|');

// This is part of the current expression or block; read it together with the nearby lines.
    donorCount--; // compensate for constructor increment
// This sends a result back to the code that called this function.
    return d;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This creates or updates a value that the following logic uses.
int Donor::donorCount = 0; // Static member initialization

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
ostream &operator<<(ostream &os, const Donor &d) {
// This is part of the current expression or block; read it together with the nearby lines.
  os << d.donorID << " | " << d.name << " | " << d.bloodGroup
// This performs one step of the current operation, then continues to the next statement.
     << " | Age: " << d.age;
// This sends a result back to the code that called this function.
  return os;
// This closes the block opened above, such as a function, class, or control-flow branch.
}

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: Recipient (inherits from Person)
// This is a source comment that explains nearby code.
//  Demonstrates: Inheritance, Polymorphism
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class Recipient : public Person {
// The following members are kept inside this class.
private:
// This creates or updates a value that the following logic uses.
  string recipientID;
// This performs one step of the current operation, then continues to the next statement.
  BloodGroup requiredBloodGroup;
// This creates or updates a value that the following logic uses.
  string hospital;
// This creates or updates a value that the following logic uses.
  string urgencyLevel; // Low, Medium, High, Critical

// This creates or updates a value that the following logic uses.
  static int recipientCount;

// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  Recipient() : Person(), hospital(""), urgencyLevel("Medium") {
// This performs one step of the current operation, then continues to the next statement.
    recipientCount++;
// This performs one step of the current operation, then continues to the next statement.
    recipientID = "R-" + to_string(2000 + recipientCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This creates or updates a value that the following logic uses.
  Recipient(string n, int a, string g, string ph, string addr, BloodGroup bg,
// This creates or updates a value that the following logic uses.
            string hosp, string urgency)
// This is part of the current expression or block; read it together with the nearby lines.
      : Person(n, a, g, ph, addr), requiredBloodGroup(bg), hospital(hosp),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        urgencyLevel(urgency) {
// This performs one step of the current operation, then continues to the next statement.
    recipientCount++;
// This performs one step of the current operation, then continues to the next statement.
    recipientID = "R-" + to_string(2000 + recipientCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is part of the current expression or block; read it together with the nearby lines.
  ~Recipient() override {}

// This is a source comment that explains nearby code.
  // Overriding displayInfo (Polymorphism)
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayInfo() const override {
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ┌─────────────────────────────────────┐\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  RECIPIENT CARD                      │\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ├─────────────────────────────────────┤\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  ID       : " << left << setw(24) << recipientID << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Name     : " << left << setw(24) << name << "│\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  │  Needs    : " << left << setw(24)
// This performs one step of the current operation, then continues to the next statement.
         << requiredBloodGroup.getFullType() << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Hospital : " << left << setw(24) << hospital << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Urgency  : " << left << setw(24) << urgencyLevel << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  └─────────────────────────────────────┘\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getRole() const override { return "Recipient"; }

// This is a source comment that explains nearby code.
  // Getters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getRecipientID() const { return recipientID; }
// This creates or updates a value that the following logic uses.
  BloodGroup getRequiredBloodGroup() const { return requiredBloodGroup; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getHospital() const { return hospital; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getUrgencyLevel() const { return urgencyLevel; }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This creates or updates a value that the following logic uses.
int Recipient::recipientCount = 0;

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: EligibilityChecker
// This is a source comment that explains nearby code.
//  Checks if a donor is eligible to donate
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class EligibilityChecker {
// The following members are kept inside this class.
private:
// This creates or updates a value that the following logic uses.
  static const int MIN_AGE = 18;
// This creates or updates a value that the following logic uses.
  static const int MAX_AGE = 65;
// This creates or updates a value that the following logic uses.
  constexpr static const float MIN_WEIGHT = 50.0f;
// This creates or updates a value that the following logic uses.
  constexpr static const float MIN_HEMOGLOBIN = 12.5f;
// This creates or updates a value that the following logic uses.
  static const int MIN_GAP_DAYS = 90;

// The following members are available to other parts of the program.
public:
// This begins a reusable type or template: a blueprint for related data and behavior.
  struct Result {
// This creates or updates a value that the following logic uses.
    bool eligible;
// This creates or updates a value that the following logic uses.
    vector<string> passedChecks;
// This creates or updates a value that the following logic uses.
    vector<string> failedChecks;
// This closes the block opened above, such as a function, class, or control-flow branch.
  };

// This is a source comment that explains nearby code.
  // Function overloading: check with different params
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static Result check(const Donor &donor) {
// This performs one step of the current operation, then continues to the next statement.
    Result result;
// This performs one step of the current operation, then continues to the next statement.
    result.eligible = true;

// This is a source comment that explains nearby code.
    // Age check
// This controls which part of the program runs next, based on the current condition or result.
    if (donor.getAge() >= MIN_AGE && donor.getAge() <= MAX_AGE) {
// This is part of the current expression or block; read it together with the nearby lines.
      result.passedChecks.push_back("Age: " + to_string(donor.getAge()) +
// This performs one step of the current operation, then continues to the next statement.
                                    " (within 18-65 range)");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else {
// This is part of the current expression or block; read it together with the nearby lines.
      result.failedChecks.push_back("Age: " + to_string(donor.getAge()) +
// This performs one step of the current operation, then continues to the next statement.
                                    " (must be 18-65)");
// This performs one step of the current operation, then continues to the next statement.
      result.eligible = false;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Weight check
// This controls which part of the program runs next, based on the current condition or result.
    if (donor.getWeight() >= MIN_WEIGHT) {
// This is part of the current expression or block; read it together with the nearby lines.
      result.passedChecks.push_back(
// This creates or updates a value that the following logic uses.
          "Weight: " + to_string((int)donor.getWeight()) +
// This performs one step of the current operation, then continues to the next statement.
          " kg (above 50 kg minimum)");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else {
// This is part of the current expression or block; read it together with the nearby lines.
      result.failedChecks.push_back(
// This creates or updates a value that the following logic uses.
          "Weight: " + to_string((int)donor.getWeight()) +
// This performs one step of the current operation, then continues to the next statement.
          " kg (below 50 kg minimum)");
// This performs one step of the current operation, then continues to the next statement.
      result.eligible = false;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Hemoglobin check
// This controls which part of the program runs next, based on the current condition or result.
    if (donor.getHemoglobin() >= MIN_HEMOGLOBIN) {
// This performs one step of the current operation, then continues to the next statement.
      ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
      oss << fixed << setprecision(1) << donor.getHemoglobin();
// This is part of the current expression or block; read it together with the nearby lines.
      result.passedChecks.push_back("Hemoglobin: " + oss.str() +
// This performs one step of the current operation, then continues to the next statement.
                                    " g/dL (above 12.5 threshold)");
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else {
// This performs one step of the current operation, then continues to the next statement.
      ostringstream oss;
// This performs one step of the current operation, then continues to the next statement.
      oss << fixed << setprecision(1) << donor.getHemoglobin();
// This is part of the current expression or block; read it together with the nearby lines.
      result.failedChecks.push_back("Hemoglobin: " + oss.str() +
// This performs one step of the current operation, then continues to the next statement.
                                    " g/dL (below 12.5 threshold)");
// This performs one step of the current operation, then continues to the next statement.
      result.eligible = false;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This sends a result back to the code that called this function.
    return result;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Overloaded: check with additional hemoglobin param
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static Result check(const Donor &donor, float currentHemoglobin) {
// This is part of the current expression or block; read it together with the nearby lines.
    Donor temp = donor; // Copy constructor used here
// This performs one step of the current operation, then continues to the next statement.
    temp.setHemoglobin(currentHemoglobin);
// This sends a result back to the code that called this function.
    return check(temp);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static void displayResult(const Result &result) {
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║     ELIGIBILITY CHECK RESULT          ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╠═══════════════════════════════════════╣\n";

// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &check : result.passedChecks) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  ║  ✓ " << left << setw(35) << check << "║\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &check : result.failedChecks) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  ║  ✗ " << left << setw(35) << check << "║\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╠═══════════════════════════════════════╣\n";
// This controls which part of the program runs next, based on the current condition or result.
    if (result.eligible) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  ║  RESULT: ELIGIBLE TO DONATE           ║\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  ║  RESULT: NOT ELIGIBLE (DEFERRED)      ║\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: Camp
// This is a source comment that explains nearby code.
//  Represents a blood donation camp event
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class Camp {
// The following members are kept inside this class.
private:
// This creates or updates a value that the following logic uses.
  string campID;
// This creates or updates a value that the following logic uses.
  string location;
// This creates or updates a value that the following logic uses.
  string date;
// This creates or updates a value that the following logic uses.
  int targetUnits;
// This creates or updates a value that the following logic uses.
  int collectedUnits;
// This creates or updates a value that the following logic uses.
  vector<string> registeredDonorIDs;

// This creates or updates a value that the following logic uses.
  static int campCount;

// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  Camp() : location(""), date(""), targetUnits(0), collectedUnits(0) {
// This performs one step of the current operation, then continues to the next statement.
    campCount++;
// This performs one step of the current operation, then continues to the next statement.
    campID = "C-" + to_string(100 + campCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This creates or updates a value that the following logic uses.
  Camp(string loc, string d, int target)
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
      : location(loc), date(d), targetUnits(target), collectedUnits(0) {
// This performs one step of the current operation, then continues to the next statement.
    campCount++;
// This performs one step of the current operation, then continues to the next statement.
    campID = "C-" + to_string(100 + campCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Getters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getCampID() const { return campID; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getLocation() const { return location; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getDate() const { return date; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  int getTargetUnits() const { return targetUnits; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  int getCollectedUnits() const { return collectedUnits; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  int getRegistrationCount() const { return registeredDonorIDs.size(); }

// This is a source comment that explains nearby code.
  // Setters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setCampID(string id) { campID = id; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setLocation(string loc) { location = loc; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setDate(string d) { date = d; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setTargetUnits(int t) { targetUnits = t; }

// This is a source comment that explains nearby code.
  // Register a donor to this camp
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void registerDonor(string donorID) { registeredDonorIDs.push_back(donorID); }

// This is a source comment that explains nearby code.
  // Record a collected unit
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void recordCollection() { collectedUnits++; }

// This is a source comment that explains nearby code.
  // Get progress percentage
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  float getProgress() const {
// This controls which part of the program runs next, based on the current condition or result.
    if (targetUnits == 0)
// This sends a result back to the code that called this function.
      return 0;
// This sends a result back to the code that called this function.
    return (float)collectedUnits / targetUnits * 100;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayInfo() const {
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ┌─────────────────────────────────────┐\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  CAMP: " << left << setw(29) << location << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ├─────────────────────────────────────┤\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  ID        : " << left << setw(23) << campID << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Date      : " << left << setw(23) << date << "│\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  │  Target    : " << left << setw(20) << targetUnits
// This performs one step of the current operation, then continues to the next statement.
         << "units│\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  │  Collected : " << left << setw(20) << collectedUnits
// This performs one step of the current operation, then continues to the next statement.
         << "units│\n";

// This is a source comment that explains nearby code.
    // Progress bar
// This creates or updates a value that the following logic uses.
    int barWidth = 20;
// This creates or updates a value that the following logic uses.
    int filled = (int)(getProgress() / 100 * barWidth);
// This performs one step of the current operation, then continues to the next statement.
    cout << "  │  Progress  : [";
// This controls which part of the program runs next, based on the current condition or result.
    for (int i = 0; i < barWidth; i++) {
// This performs one step of the current operation, then continues to the next statement.
      cout << (i < filled ? "█" : "░");
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This creates or updates a value that the following logic uses.
    cout << "] " << setw(3) << (int)getProgress() << "% │\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  │  Donors    : " << left << setw(23) << registeredDonorIDs.size()
// This performs one step of the current operation, then continues to the next statement.
         << "│\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  └─────────────────────────────────────┘\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Save to file
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void saveToFile(ofstream &file) const {
// This is part of the current expression or block; read it together with the nearby lines.
    file << campID << "|" << location << "|" << date << "|" << targetUnits
// This performs one step of the current operation, then continues to the next statement.
         << "|" << collectedUnits << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This creates or updates a value that the following logic uses.
int Camp::campCount = 0;

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: DonationRecord
// This is a source comment that explains nearby code.
//  Composition: has-a Donor reference, has-a Camp
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class DonationRecord {
// The following members are kept inside this class.
private:
// This creates or updates a value that the following logic uses.
  string recordID;
// This creates or updates a value that the following logic uses.
  string donorID;
// This creates or updates a value that the following logic uses.
  string donorName;
// This creates or updates a value that the following logic uses.
  string bloodGroupStr;
// This creates or updates a value that the following logic uses.
  string campLocation;
// This creates or updates a value that the following logic uses.
  string donationDate;
// This creates or updates a value that the following logic uses.
  string status; // Completed, Deferred, Pending

// This creates or updates a value that the following logic uses.
  static int recordCount;

// The following members are available to other parts of the program.
public:
// This is part of the current expression or block; read it together with the nearby lines.
  DonationRecord()
// This is part of the current expression or block; read it together with the nearby lines.
      : donorID(""), donorName(""), bloodGroupStr(""), campLocation(""),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        donationDate(""), status("Pending") {
// This performs one step of the current operation, then continues to the next statement.
    recordCount++;
// This performs one step of the current operation, then continues to the next statement.
    recordID = "R-" + to_string(2200 + recordCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This creates or updates a value that the following logic uses.
  DonationRecord(string dID, string dName, string bg, string campLoc,
// This creates or updates a value that the following logic uses.
                 string date, string st)
// This is part of the current expression or block; read it together with the nearby lines.
      : donorID(dID), donorName(dName), bloodGroupStr(bg),
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
        campLocation(campLoc), donationDate(date), status(st) {
// This performs one step of the current operation, then continues to the next statement.
    recordCount++;
// This performs one step of the current operation, then continues to the next statement.
    recordID = "R-" + to_string(2200 + recordCount);
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Getters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getRecordID() const { return recordID; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getDonorID() const { return donorID; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getDonorName() const { return donorName; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getBloodGroup() const { return bloodGroupStr; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getCampLocation() const { return campLocation; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getDate() const { return donationDate; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getStatus() const { return status; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static int getRecordCount() { return recordCount; }

// This is a source comment that explains nearby code.
  // Setters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setRecordID(string id) { recordID = id; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void setStatus(string s) { status = s; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayInfo() const {
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  " << left << setw(10) << recordID << setw(10) << donorID
// This is part of the current expression or block; read it together with the nearby lines.
         << setw(20) << donorName << setw(8) << bloodGroupStr << setw(24)
// This performs one step of the current operation, then continues to the next statement.
         << campLocation << setw(12) << donationDate << status << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Save to file
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void saveToFile(ofstream &file) const {
// This is part of the current expression or block; read it together with the nearby lines.
    file << recordID << "|" << donorID << "|" << donorName << "|"
// This is part of the current expression or block; read it together with the nearby lines.
         << bloodGroupStr << "|" << campLocation << "|" << donationDate << "|"
// This performs one step of the current operation, then continues to the next statement.
         << status << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Load from file
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static DonationRecord loadFromFile(const string &line) {
// This performs one step of the current operation, then continues to the next statement.
    DonationRecord r;
// This performs one step of the current operation, then continues to the next statement.
    istringstream iss(line);

// This performs one step of the current operation, then continues to the next statement.
    getline(iss, r.recordID, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, r.donorID, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, r.donorName, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, r.bloodGroupStr, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, r.campLocation, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, r.donationDate, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, r.status, '|');

// This is part of the current expression or block; read it together with the nearby lines.
    recordCount--; // compensate
// This sends a result back to the code that called this function.
    return r;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This creates or updates a value that the following logic uses.
int DonationRecord::recordCount = 0;

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: Account
// This is a source comment that explains nearby code.
//  User account for login/registration
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class Account {
// The following members are kept inside this class.
private:
// This creates or updates a value that the following logic uses.
  string username;
// This creates or updates a value that the following logic uses.
  string password;
// This creates or updates a value that the following logic uses.
  string fullName;
// This creates or updates a value that the following logic uses.
  string role; // admin, donor, volunteer

// The following members are available to other parts of the program.
public:
// This is part of the current expression or block; read it together with the nearby lines.
  Account() : username(""), password(""), fullName(""), role("donor") {}
// This creates or updates a value that the following logic uses.
  Account(string user, string pass, string name, string r)
// This is part of the current expression or block; read it together with the nearby lines.
      : username(user), password(pass), fullName(name), role(r) {}

// This is a source comment that explains nearby code.
  // Getters
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getUsername() const { return username; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getFullName() const { return fullName; }
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  string getRole() const { return role; }

// This is a source comment that explains nearby code.
  // Verify password
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  bool verifyPassword(const string &pass) const { return password == pass; }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayInfo() const {
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  Username: " << username << " | Name: " << fullName
// This performs one step of the current operation, then continues to the next statement.
         << " | Role: " << role << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Save to file
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void saveToFile(ofstream &file) const {
// This is part of the current expression or block; read it together with the nearby lines.
    file << username << "|" << password << "|" << fullName << "|" << role
// This performs one step of the current operation, then continues to the next statement.
         << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // Load from file
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  static Account loadFromFile(const string &line) {
// This performs one step of the current operation, then continues to the next statement.
    Account a;
// This performs one step of the current operation, then continues to the next statement.
    istringstream iss(line);
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, a.username, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, a.password, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, a.fullName, '|');
// This performs one step of the current operation, then continues to the next statement.
    getline(iss, a.role, '|');
// This sends a result back to the code that called this function.
    return a;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Class: BloodLineSystem
// This is a source comment that explains nearby code.
//  The main manager - ties everything together
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This begins a reusable type or template: a blueprint for related data and behavior.
class BloodLineSystem {
// The following members are kept inside this class.
private:
// This performs one step of the current operation, then continues to the next statement.
  vector<Donor> donors;
// This performs one step of the current operation, then continues to the next statement.
  vector<Recipient> recipients;
// This performs one step of the current operation, then continues to the next statement.
  vector<Camp> camps;
// This performs one step of the current operation, then continues to the next statement.
  vector<DonationRecord> records;
// This performs one step of the current operation, then continues to the next statement.
  vector<Account> accounts;
// This performs one step of the current operation, then continues to the next statement.
  Account *currentUser;

// This is a source comment that explains nearby code.
  // File paths
// This creates or updates a value that the following logic uses.
  const string DONORS_FILE = "donors.txt";
// This creates or updates a value that the following logic uses.
  const string RECORDS_FILE = "records.txt";
// This creates or updates a value that the following logic uses.
  const string ACCOUNTS_FILE = "accounts.txt";
// This creates or updates a value that the following logic uses.
  const string CAMPS_FILE = "camps.txt";

// The following members are available to other parts of the program.
public:
// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  BloodLineSystem() : currentUser(nullptr) {
// This performs one step of the current operation, then continues to the next statement.
    loadData();
// This is a source comment that explains nearby code.
    // Add default camps if none exist
// This controls which part of the program runs next, based on the current condition or result.
    if (camps.empty()) {
// This performs one step of the current operation, then continues to the next statement.
      camps.push_back(Camp("RamSay Hospital, Nainital", "12/08/2026", 80));
// This performs one step of the current operation, then continues to the next statement.
      camps.push_back(Camp("GEHU Bhimtal Campus", "19/08/2026", 150));
// This is part of the current expression or block; read it together with the nearby lines.
      camps.push_back(Camp("Central Himalayan Environment Association(CHEA)",
// This performs one step of the current operation, then continues to the next statement.
                           "02/09/2026", 60));
// This is part of the current expression or block; read it together with the nearby lines.
      camps.push_back(
// This performs one step of the current operation, then continues to the next statement.
          Camp("Graphic Era, Deemed to be University", "14/09/2026", 45));
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  ~BloodLineSystem() {
// This performs one step of the current operation, then continues to the next statement.
    saveData();
// This controls which part of the program runs next, based on the current condition or result.
    if (currentUser)
// This performs one step of the current operation, then continues to the next statement.
      delete currentUser;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── File I/O ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void saveData() {
// This is a source comment that explains nearby code.
    // Save donors
// This performs one step of the current operation, then continues to the next statement.
    ofstream dFile(DONORS_FILE);
// This controls which part of the program runs next, based on the current condition or result.
    if (dFile.is_open()) {
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &d : donors) {
// This performs one step of the current operation, then continues to the next statement.
        d.saveToFile(dFile);
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      dFile.close();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Save records
// This performs one step of the current operation, then continues to the next statement.
    ofstream rFile(RECORDS_FILE);
// This controls which part of the program runs next, based on the current condition or result.
    if (rFile.is_open()) {
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &r : records) {
// This performs one step of the current operation, then continues to the next statement.
        r.saveToFile(rFile);
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      rFile.close();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Save accounts
// This performs one step of the current operation, then continues to the next statement.
    ofstream aFile(ACCOUNTS_FILE);
// This controls which part of the program runs next, based on the current condition or result.
    if (aFile.is_open()) {
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &a : accounts) {
// This performs one step of the current operation, then continues to the next statement.
        a.saveToFile(aFile);
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      aFile.close();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void loadData() {
// This is a source comment that explains nearby code.
    // Load donors
// This performs one step of the current operation, then continues to the next statement.
    ifstream dFile(DONORS_FILE);
// This controls which part of the program runs next, based on the current condition or result.
    if (dFile.is_open()) {
// This creates or updates a value that the following logic uses.
      string line;
// This controls which part of the program runs next, based on the current condition or result.
      while (getline(dFile, line)) {
// This controls which part of the program runs next, based on the current condition or result.
        if (!line.empty()) {
// This performs one step of the current operation, then continues to the next statement.
          donors.push_back(Donor::loadFromFile(line));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      dFile.close();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Load records
// This performs one step of the current operation, then continues to the next statement.
    ifstream rFile(RECORDS_FILE);
// This controls which part of the program runs next, based on the current condition or result.
    if (rFile.is_open()) {
// This creates or updates a value that the following logic uses.
      string line;
// This controls which part of the program runs next, based on the current condition or result.
      while (getline(rFile, line)) {
// This controls which part of the program runs next, based on the current condition or result.
        if (!line.empty()) {
// This performs one step of the current operation, then continues to the next statement.
          records.push_back(DonationRecord::loadFromFile(line));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      rFile.close();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Load accounts
// This performs one step of the current operation, then continues to the next statement.
    ifstream aFile(ACCOUNTS_FILE);
// This controls which part of the program runs next, based on the current condition or result.
    if (aFile.is_open()) {
// This creates or updates a value that the following logic uses.
      string line;
// This controls which part of the program runs next, based on the current condition or result.
      while (getline(aFile, line)) {
// This controls which part of the program runs next, based on the current condition or result.
        if (!line.empty()) {
// This performs one step of the current operation, then continues to the next statement.
          accounts.push_back(Account::loadFromFile(line));
// This closes the block opened above, such as a function, class, or control-flow branch.
        }
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      aFile.close();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Account Management ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void createAccount() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║        CREATE NEW ACCOUNT             ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This creates or updates a value that the following logic uses.
    string username, password, fullName, role;

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Enter username    : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, username);

// This is a source comment that explains nearby code.
    // Check if username already exists
// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &a : accounts) {
// This controls which part of the program runs next, based on the current condition or result.
      if (a.getUsername() == username) {
// This performs one step of the current operation, then continues to the next statement.
        cout << "\n  ✗ Username already taken. Try another.\n";
// This performs one step of the current operation, then continues to the next statement.
        pauseScreen();
// This sends a result back to the code that called this function.
        return;
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Enter password    : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, password);
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Enter full name   : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, fullName);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Select role:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "    1. Donor\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "    2. Volunteer\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "    3. Admin\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Choice: ";
// This creates or updates a value that the following logic uses.
    string choice;
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, choice);

// This controls which part of the program runs next, based on the current condition or result.
    if (choice == "1")
// This performs one step of the current operation, then continues to the next statement.
      role = "donor";
// This controls which part of the program runs next, based on the current condition or result.
    else if (choice == "2")
// This performs one step of the current operation, then continues to the next statement.
      role = "volunteer";
// This controls which part of the program runs next, based on the current condition or result.
    else if (choice == "3")
// This performs one step of the current operation, then continues to the next statement.
      role = "admin";
// This controls which part of the program runs next, based on the current condition or result.
    else
// This performs one step of the current operation, then continues to the next statement.
      role = "donor";

// This performs one step of the current operation, then continues to the next statement.
    accounts.push_back(Account(username, password, fullName, role));
// This performs one step of the current operation, then continues to the next statement.
    saveData();

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ✓ Account created successfully!\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Welcome, " << fullName << "!\n";
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  bool login() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║             LOGIN                     ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This creates or updates a value that the following logic uses.
    string username, password;
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Username: ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, username);
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Password: ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, password);

// This controls which part of the program runs next, based on the current condition or result.
    for (auto &a : accounts) {
// This controls which part of the program runs next, based on the current condition or result.
      if (a.getUsername() == username && a.verifyPassword(password)) {
// This performs one step of the current operation, then continues to the next statement.
        currentUser = new Account(a);
// This performs one step of the current operation, then continues to the next statement.
        cout << "\n  ✓ Welcome back, " << a.getFullName() << "!\n";
// This performs one step of the current operation, then continues to the next statement.
        pauseScreen();
// This sends a result back to the code that called this function.
        return true;
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ✗ Invalid credentials. Try again.\n";
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This sends a result back to the code that called this function.
    return false;
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Donor Registration ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void registerDonor() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║       REGISTER NEW DONOR              ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This creates or updates a value that the following logic uses.
    string name, gender, phone, address, email;
// This creates or updates a value that the following logic uses.
    int age;
// This performs one step of the current operation, then continues to the next statement.
    float weight, hemoglobin;
// This creates or updates a value that the following logic uses.
    string bgType, bgRh;

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Full name       : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, name);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Age             : ";
// This performs one step of the current operation, then continues to the next statement.
    cin >> age;
// This performs one step of the current operation, then continues to the next statement.
    cin.ignore();

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Gender (M/F/O)  : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, gender);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Phone           : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, phone);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Email           : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, email);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Address         : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, address);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Blood group type (A/B/AB/O): ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, bgType);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Rh factor (+/-) : ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, bgRh);

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Weight (kg)     : ";
// This performs one step of the current operation, then continues to the next statement.
    cin >> weight;

// This performs one step of the current operation, then continues to the next statement.
    cout << "  Hemoglobin (g/dL): ";
// This performs one step of the current operation, then continues to the next statement.
    cin >> hemoglobin;
// This performs one step of the current operation, then continues to the next statement.
    cin.ignore();

// This performs one step of the current operation, then continues to the next statement.
    BloodGroup bg(bgType, bgRh);
// This is part of the current expression or block; read it together with the nearby lines.
    Donor newDonor(name, age, gender, phone, address, bg, weight, hemoglobin,
// This performs one step of the current operation, then continues to the next statement.
                   email);

// This is a source comment that explains nearby code.
    // Consent
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Allow your donation records to be visible to others? (y/n): ";
// This creates or updates a value that the following logic uses.
    string consent;
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, consent);
// This performs one step of the current operation, then continues to the next statement.
    newDonor.setConsent(consent == "y" || consent == "Y");

// This performs one step of the current operation, then continues to the next statement.
    donors.push_back(newDonor);
// This performs one step of the current operation, then continues to the next statement.
    saveData();

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ✓ Donor registered successfully!\n";
// This performs one step of the current operation, then continues to the next statement.
    newDonor.displayInfo();
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Eligibility Check ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void checkEligibility() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║      CHECK ELIGIBILITY                ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This controls which part of the program runs next, based on the current condition or result.
    if (donors.empty()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  No donors registered yet. Register first.\n";
// This performs one step of the current operation, then continues to the next statement.
      pauseScreen();
// This sends a result back to the code that called this function.
      return;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // List donors
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Registered Donors:\n";
// This controls which part of the program runs next, based on the current condition or result.
    for (size_t i = 0; i < donors.size(); i++) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "    " << (i + 1) << ". " << donors[i] << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Select donor number: ";
// This creates or updates a value that the following logic uses.
    int choice;
// This performs one step of the current operation, then continues to the next statement.
    cin >> choice;
// This performs one step of the current operation, then continues to the next statement.
    cin.ignore();

// This controls which part of the program runs next, based on the current condition or result.
    if (choice < 1 || choice > (int)donors.size()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  Invalid selection.\n";
// This performs one step of the current operation, then continues to the next statement.
      pauseScreen();
// This sends a result back to the code that called this function.
      return;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Demonstrating polymorphism: calling through base pointer
// This performs one step of the current operation, then continues to the next statement.
    Person *personPtr = &donors[choice - 1];
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "\n  Checking eligibility for " << personPtr->getName()
// This performs one step of the current operation, then continues to the next statement.
         << " (Role: " << personPtr->getRole() << ")\n";

// This is part of the current expression or block; read it together with the nearby lines.
    EligibilityChecker::Result result =
// This performs one step of the current operation, then continues to the next statement.
        EligibilityChecker::check(donors[choice - 1]);
// This performs one step of the current operation, then continues to the next statement.
    EligibilityChecker::displayResult(result);
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Record a Donation ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void recordDonation() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║      RECORD A DONATION                ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This controls which part of the program runs next, based on the current condition or result.
    if (donors.empty()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  No donors registered. Register a donor first.\n";
// This performs one step of the current operation, then continues to the next statement.
      pauseScreen();
// This sends a result back to the code that called this function.
      return;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Select donor
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Select Donor:\n";
// This controls which part of the program runs next, based on the current condition or result.
    for (size_t i = 0; i < donors.size(); i++) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "    " << (i + 1) << ". " << donors[i] << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Choice: ";
// This creates or updates a value that the following logic uses.
    int dChoice;
// This performs one step of the current operation, then continues to the next statement.
    cin >> dChoice;
// This performs one step of the current operation, then continues to the next statement.
    cin.ignore();

// This controls which part of the program runs next, based on the current condition or result.
    if (dChoice < 1 || dChoice > (int)donors.size()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  Invalid selection.\n";
// This performs one step of the current operation, then continues to the next statement.
      pauseScreen();
// This sends a result back to the code that called this function.
      return;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Check eligibility first
// This is part of the current expression or block; read it together with the nearby lines.
    EligibilityChecker::Result result =
// This performs one step of the current operation, then continues to the next statement.
        EligibilityChecker::check(donors[dChoice - 1]);
// This performs one step of the current operation, then continues to the next statement.
    EligibilityChecker::displayResult(result);

// This creates or updates a value that the following logic uses.
    string status;
// This controls which part of the program runs next, based on the current condition or result.
    if (result.eligible) {
// This performs one step of the current operation, then continues to the next statement.
      status = "Completed";
// This performs one step of the current operation, then continues to the next statement.
      cout << "\n  ✓ Donor is eligible. Proceeding with donation.\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    } else {
// This performs one step of the current operation, then continues to the next statement.
      status = "Deferred";
// This performs one step of the current operation, then continues to the next statement.
      cout << "\n  ✗ Donor is deferred. Recording with deferred status.\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Select camp
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Select Camp:\n";
// This controls which part of the program runs next, based on the current condition or result.
    for (size_t i = 0; i < camps.size(); i++) {
// This is part of the current expression or block; read it together with the nearby lines.
      cout << "    " << (i + 1) << ". " << camps[i].getLocation() << " ("
// This performs one step of the current operation, then continues to the next statement.
           << camps[i].getDate() << ")\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Choice: ";
// This creates or updates a value that the following logic uses.
    int cChoice;
// This performs one step of the current operation, then continues to the next statement.
    cin >> cChoice;
// This performs one step of the current operation, then continues to the next statement.
    cin.ignore();

// This controls which part of the program runs next, based on the current condition or result.
    if (cChoice < 1 || cChoice > (int)camps.size()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  Invalid selection.\n";
// This performs one step of the current operation, then continues to the next statement.
      pauseScreen();
// This sends a result back to the code that called this function.
      return;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Create record
// This is part of the current expression or block; read it together with the nearby lines.
    DonationRecord record(
// This is part of the current expression or block; read it together with the nearby lines.
        donors[dChoice - 1].getDonorID(), donors[dChoice - 1].getName(),
// This is part of the current expression or block; read it together with the nearby lines.
        donors[dChoice - 1].getBloodGroup().getFullType(),
// This performs one step of the current operation, then continues to the next statement.
        camps[cChoice - 1].getLocation(), getCurrentDate(), status);

// This performs one step of the current operation, then continues to the next statement.
    records.push_back(record);

// This controls which part of the program runs next, based on the current condition or result.
    if (status == "Completed") {
// This performs one step of the current operation, then continues to the next statement.
      camps[cChoice - 1].recordCollection();
// This performs one step of the current operation, then continues to the next statement.
      camps[cChoice - 1].registerDonor(donors[dChoice - 1].getDonorID());
// This performs one step of the current operation, then continues to the next statement.
      donors[dChoice - 1].setLastDonationDate(getCurrentDate());
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    saveData();

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ✓ Donation record created: " << record.getRecordID() << "\n";
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── View Donor Records ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void viewRecords() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "\n  "
// This is part of the current expression or block; read it together with the nearby lines.
            "╔═════════════════════════════════════════════════════════════════"
// This performs one step of the current operation, then continues to the next statement.
            "══════════════════════╗\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  ║                              DONATION RECORDS                 "
// This performs one step of the current operation, then continues to the next statement.
            "                      ║\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  "
// This is part of the current expression or block; read it together with the nearby lines.
            "╚═════════════════════════════════════════════════════════════════"
// This performs one step of the current operation, then continues to the next statement.
            "══════════════════════╝\n\n";

// This controls which part of the program runs next, based on the current condition or result.
    if (records.empty()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  No records yet. Make a donation first.\n";
// This performs one step of the current operation, then continues to the next statement.
      pauseScreen();
// This sends a result back to the code that called this function.
      return;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  " << left << setw(10) << "Record" << setw(10) << "Donor"
// This is part of the current expression or block; read it together with the nearby lines.
         << setw(20) << "Name" << setw(8) << "Blood" << setw(24) << "Camp"
// This is part of the current expression or block; read it together with the nearby lines.
         << setw(12) << "Date"
// This performs one step of the current operation, then continues to the next statement.
         << "Status\n";
// This creates or updates a value that the following logic uses.
    cout << "  " << string(84, '-') << "\n";

// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &r : records) {
// This performs one step of the current operation, then continues to the next statement.
      r.displayInfo();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── View All Donors ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void viewDonors() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║         ALL REGISTERED DONORS         ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This controls which part of the program runs next, based on the current condition or result.
    if (donors.empty()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  No donors registered yet.\n";
// This performs one step of the current operation, then continues to the next statement.
      pauseScreen();
// This sends a result back to the code that called this function.
      return;
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &d : donors) {
// This controls which part of the program runs next, based on the current condition or result.
      if (d.getConsent()) {
// This performs one step of the current operation, then continues to the next statement.
        d.displayInfo();
// This closes the block opened above, such as a function, class, or control-flow branch.
      } else {
// This is part of the current expression or block; read it together with the nearby lines.
        cout << "\n  [" << d.getDonorID()
// This performs one step of the current operation, then continues to the next statement.
             << " - Record hidden by donor's request]\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Total registered donors: " << donors.size() << "\n";
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Search by Blood Group ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void searchByBloodGroup() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║     SEARCH DONORS BY BLOOD GROUP      ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This creates or updates a value that the following logic uses.
    string bgType, bgRh;
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Blood group type (A/B/AB/O): ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, bgType);
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Rh factor (+/-): ";
// This performs one step of the current operation, then continues to the next statement.
    getline(cin, bgRh);

// This creates or updates a value that the following logic uses.
    string searchGroup = bgType + bgRh;
// This creates or updates a value that the following logic uses.
    bool found = false;

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Donors with blood group " << searchGroup << ":\n";
// This creates or updates a value that the following logic uses.
    cout << "  " << string(40, '-') << "\n";

// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &d : donors) {
// This controls which part of the program runs next, based on the current condition or result.
      if (d.getBloodGroup().getFullType() == searchGroup && d.getConsent()) {
// This performs one step of the current operation, then continues to the next statement.
        cout << "  " << d << "\n";
// This performs one step of the current operation, then continues to the next statement.
        found = true;
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This controls which part of the program runs next, based on the current condition or result.
    if (!found) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  No donors found with blood group " << searchGroup << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This is a source comment that explains nearby code.
    // Show compatibility info
// This performs one step of the current operation, then continues to the next statement.
    BloodGroup bg(bgType, bgRh);
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  Compatibility info for " << bg << ":\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  Can donate to: ";
// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &c : bg.canDonateTo()) {
// This performs one step of the current operation, then continues to the next statement.
      cout << c << " ";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n";

// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── View Camp Info ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void viewCamps() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║       CAMP INFORMATION                ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n";

// This controls which part of the program runs next, based on the current condition or result.
    for (const auto &c : camps) {
// This performs one step of the current operation, then continues to the next statement.
      c.displayInfo();
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Display Blood Group Info ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayBloodGroupInfo() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║     BLOOD GROUP INFORMATION           ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This creates or updates a value that the following logic uses.
    string groups[] = {"O-", "O+", "A+", "A-", "B+", "B-", "AB+", "AB-"};
// This creates or updates a value that the following logic uses.
    string notes[] = {"Universal donor - can donate to all blood types",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Most common blood type globally",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Second most common blood type",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Rare - only 6% of the population",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Common in South Asian populations",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Rare - only 2% of the population",
// This is part of the current expression or block; read it together with the nearby lines.
                      "Universal recipient - can receive from all",
// This performs one step of the current operation, then continues to the next statement.
                      "Rarest blood type - less than 1%"};

// This controls which part of the program runs next, based on the current condition or result.
    for (int i = 0; i < 8; i++) {
// This performs one step of the current operation, then continues to the next statement.
      cout << "  " << left << setw(6) << groups[i] << " - " << notes[i] << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ── Compatibility Chart ──\n\n";
// This is part of the current expression or block; read it together with the nearby lines.
    cout << "  " << left << setw(8) << "Type"
// This performs one step of the current operation, then continues to the next statement.
         << "Can donate to\n";
// This creates or updates a value that the following logic uses.
    cout << "  " << string(50, '-') << "\n";

// This creates or updates a value that the following logic uses.
    string types[] = {"O", "O", "A", "A", "B", "B", "AB", "AB"};
// This creates or updates a value that the following logic uses.
    string rhs[] = {"-", "+", "-", "+", "-", "+", "-", "+"};

// This controls which part of the program runs next, based on the current condition or result.
    for (int i = 0; i < 8; i++) {
// This performs one step of the current operation, then continues to the next statement.
      BloodGroup bg(types[i], rhs[i]);
// This performs one step of the current operation, then continues to the next statement.
      cout << "  " << left << setw(8) << bg.getFullType();
// This controls which part of the program runs next, based on the current condition or result.
      for (const auto &c : bg.canDonateTo()) {
// This performs one step of the current operation, then continues to the next statement.
        cout << c << " ";
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This performs one step of the current operation, then continues to the next statement.
      cout << "\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
    }

// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Benefits of Blood Donation ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayBenefits() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║   BENEFITS OF BLOOD DONATION          ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  HEALTH BENEFITS:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ─────────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  1. Reduces harmful iron stores in the body\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  2. Lowers risk of heart disease and cancer\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  3. Burns approximately 650 calories per donation\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  4. Stimulates blood cell production\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  5. Free health screening with every donation\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  EMOTIONAL BENEFITS:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ─────────────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  1. Sense of purpose and fulfillment\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  2. Connection to community\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  3. Knowing you saved up to 3 lives\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  THINGS TO KNOW BEFORE DONATING:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ────────────────────────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Eat a healthy meal 2-3 hours before\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Drink plenty of water\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Get a good night's sleep\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Bring a valid photo ID\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - The process takes about 10-15 minutes\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - You can donate every 90 days (whole blood)\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  AFTER DONATION:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ────────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Rest for 10-15 minutes at the camp\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Drink extra fluids for the next 24-48 hours\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Avoid heavy lifting for 5 hours\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Keep the bandage on for at least 4 hours\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Eat iron-rich foods (spinach, red meat, beans)\n";

// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // ── Mission & Vision ──

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void displayMissionVision() {
// This performs one step of the current operation, then continues to the next statement.
    clearScreen();
// This performs one step of the current operation, then continues to the next statement.
    cout << "\n  ╔═══════════════════════════════════════╗\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ║       ABOUT BLOODLINE                 ║\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ╚═══════════════════════════════════════╝\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  OUR MISSION:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ─────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  To simplify blood donation camp management\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  and make the process transparent, efficient,\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  and accessible to every community.\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  OUR VISION:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  A world where no patient suffers due to a\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  shortage of blood. Every drop counts, and\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  every donor is a hero.\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  WHY BLOODLINE?\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ───────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Built by students, for real-world impact\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Tracks donors, camps, and records in one place\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Eligibility checking prevents unsafe donations\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Privacy-first: donors control record visibility\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Open and transparent system design\n\n";

// This performs one step of the current operation, then continues to the next statement.
    cout << "  OOP PRINCIPLES USED:\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  ────────────────────\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Classes & Objects (Donor, Camp, Record, etc.)\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Inheritance (Person -> Donor, Recipient)\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Polymorphism (Virtual displayInfo())\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Encapsulation (Private data + getters/setters)\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Abstraction (Abstract Person class)\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Operator Overloading (<< for display)\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - File Handling (save/load data)\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Composition (Record has Donor + Camp)\n";
// This performs one step of the current operation, then continues to the next statement.
    cout << "  - Static Members (counting instances)\n";

// This performs one step of the current operation, then continues to the next statement.
    pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
  }

// This is a source comment that explains nearby code.
  // - Main Menu -

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
  void showMainMenu() {
// This controls which part of the program runs next, based on the current condition or result.
    while (true) {
// This performs one step of the current operation, then continues to the next statement.
      clearScreen();
// This performs one step of the current operation, then continues to the next statement.
      cout << "\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  =======================================\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  |                                     |\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  |     ♥  B L O O D L I N E  ♥         |\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  |     Blood Donation Camp Manager     |\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  |                                     |\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  =======================================\n\n";

// This controls which part of the program runs next, based on the current condition or result.
      if (currentUser) {
// This is part of the current expression or block; read it together with the nearby lines.
        cout << "  Logged in as: " << currentUser->getFullName() << " ("
// This performs one step of the current operation, then continues to the next statement.
             << currentUser->getRole() << ")\n\n";
// This closes the block opened above, such as a function, class, or control-flow branch.
      }

// This performs one step of the current operation, then continues to the next statement.
      cout << "  - ACCOUNT ---------------------------\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   1.  Create Account\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   2.  Login\n\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  - DONOR MANAGEMENT ------------------\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   3.  Register as Donor\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   4.  Check Eligibility\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   5.  Record a Donation\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   6.  View All Donors\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   7.  Search Donors by Blood Group\n\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  - RECORDS & CAMPS -------------------\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   8.  View Donation Records\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   9.  View Camp Information\n\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  - INFORMATION -----------------------\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  10.  Blood Group Information\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  11.  Benefits of Blood Donation\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  12.  About BloodLine (Mission/Vision)\n\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "   0.  Exit\n\n";
// This performs one step of the current operation, then continues to the next statement.
      cout << "  Enter choice: ";

// This creates or updates a value that the following logic uses.
      int choice;
// This performs one step of the current operation, then continues to the next statement.
      cin >> choice;
// This performs one step of the current operation, then continues to the next statement.
      cin.ignore();

// This controls which part of the program runs next, based on the current condition or result.
      switch (choice) {
// This controls which part of the program runs next, based on the current condition or result.
      case 1:
// This performs one step of the current operation, then continues to the next statement.
        createAccount();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 2:
// This performs one step of the current operation, then continues to the next statement.
        login();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 3:
// This performs one step of the current operation, then continues to the next statement.
        registerDonor();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 4:
// This performs one step of the current operation, then continues to the next statement.
        checkEligibility();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 5:
// This performs one step of the current operation, then continues to the next statement.
        recordDonation();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 6:
// This performs one step of the current operation, then continues to the next statement.
        viewDonors();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 7:
// This performs one step of the current operation, then continues to the next statement.
        searchByBloodGroup();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 8:
// This performs one step of the current operation, then continues to the next statement.
        viewRecords();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 9:
// This performs one step of the current operation, then continues to the next statement.
        viewCamps();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 10:
// This performs one step of the current operation, then continues to the next statement.
        displayBloodGroupInfo();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 11:
// This performs one step of the current operation, then continues to the next statement.
        displayBenefits();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 12:
// This performs one step of the current operation, then continues to the next statement.
        displayMissionVision();
// This changes the current loop flow: stop this loop, or move to its next iteration.
        break;
// This controls which part of the program runs next, based on the current condition or result.
      case 0:
// This performs one step of the current operation, then continues to the next statement.
        saveData();
// This performs one step of the current operation, then continues to the next statement.
        cout << "\n  Thank you for using BloodLine!\n";
// This performs one step of the current operation, then continues to the next statement.
        cout << "  Every donor is a hero. ♥\n\n";
// This sends a result back to the code that called this function.
        return;
// This controls which part of the program runs next, based on the current condition or result.
      default:
// This performs one step of the current operation, then continues to the next statement.
        cout << "\n  Invalid choice. Try again.\n";
// This performs one step of the current operation, then continues to the next statement.
        pauseScreen();
// This closes the block opened above, such as a function, class, or control-flow branch.
      }
// This closes the block opened above, such as a function, class, or control-flow branch.
    }
// This closes the block opened above, such as a function, class, or control-flow branch.
  }
// This closes the block opened above, such as a function, class, or control-flow branch.
};

// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────
// This is a source comment that explains nearby code.
//  Main Function
// This is a source comment that explains nearby code.
// ─────────────────────────────────────────────

// This declares or begins a function. Its name describes the operation, and its parameters provide the input.
int main() {
// This performs one step of the current operation, then continues to the next statement.
  BloodLineSystem system;
// This performs one step of the current operation, then continues to the next statement.
  system.showMainMenu();
// This sends a result back to the code that called this function.
  return 0;
// This closes the block opened above, such as a function, class, or control-flow branch.
}
