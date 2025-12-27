#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
#include <cstdlib>
using namespace std;

// Structure to hold user credentials
struct User {
    string username;
    string password;
    string fullName;
    string streetNumber;
    string residentialArea;
    int numberOfMeters;
    string meter1Serial;
    string meter2Serial;
};

// Structure to hold billing record
struct BillingRecord {
    string username;
    string meterSerial;
    string month;
    int previousReading;
    int currentReading;
    int unitsConsumed;
    double totalBill;
};

// Function prototypes
void clearScreen();
void registerUser();
void loginUser();
bool checkUserExists(string username);
bool validateLogin(string username, string password);
void mainMenu(string username);
void enterMeterReading(string username);
double calculateBill(int units);
void saveBillingRecord(BillingRecord record);
void displayBillingHistory(string username);
void displayStatistics(string username);
string encryptPassword(string password);
string decryptPassword(string encrypted);
int getLastMeterReading(string username, string meterSerial);
string getLastBillingMonth(string username, string meterSerial);
int getValidInteger(string prompt);
bool isPasswordValid(string password);
string getNextMonth(string currentMonth);
User getUserDetails(string username);

int main() {
    int choice;
    
    while (true) {
        clearScreen();
        cout << "\n========================================\n";
        cout << "   ELECTRICITY BILLING SYSTEM\n";
        cout << "========================================\n\n";
        cout << "1. Register New User\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "\nEnter your choice: ";
        choice = getValidInteger("");
        
        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                clearScreen();
                cout << "\nThank you for using the system!\n";
                return 0;
            default:
                cout << "\nInvalid choice! Please enter 1, 2, or 3.\n";
                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
        }
    }
    
    return 0;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Function to get valid integer input
int getValidInteger(string prompt) {
    int value;
    
    while (true) {
        if (!prompt.empty()) {
            cout << prompt;
        }
        
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << "\nInvalid input! Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Password validation function
bool isPasswordValid(string password) {
    if (password.length() < 8) {
        cout << "\nPassword must be at least 8 characters long!\n";
        return false;
    }
    
    bool hasCapital = false;
    bool hasSpecial = false;
    
    for (int i = 0; i < password.length(); i++) {
        if (password[i] >= 'A' && password[i] <= 'Z') {
            hasCapital = true;
        }
        if (password[i] == '!' || password[i] == '@' || password[i] == '#' || 
            password[i] == '$' || password[i] == '%' || password[i] == '&' || 
            password[i] == '*') {
            hasSpecial = true;
        }
    }
    
    if (!hasCapital) {
        cout << "\nPassword must contain at least one capital letter!\n";
        return false;
    }
    
    if (!hasSpecial) {
        cout << "\nPassword must contain at least one special character (!@#$%&*)!\n";
        return false;
    }
    
    return true;
}

// Simple encryption - shifts each character by 3
string encryptPassword(string password) {
    string encrypted = "";
    for (int i = 0; i < password.length(); i++) {
        encrypted += char(password[i] + 3);
    }
    return encrypted;
}

// Simple decryption - shifts each character back by 3
string decryptPassword(string encrypted) {
    string decrypted = "";
    for (int i = 0; i < encrypted.length(); i++) {
        decrypted += char(encrypted[i] - 3);
    }
    return decrypted;
}

// Get next month
string getNextMonth(string currentMonth) {
    string months[] = {"January", "February", "March", "April", "May", "June", 
                       "July", "August", "September", "October", "November", "December"};
    
    for (int i = 0; i < 12; i++) {
        if (currentMonth == months[i]) {
            if (i == 11) {
                return months[0]; // After December comes January
            } else {
                return months[i + 1];
            }
        }
    }
    return "January"; // Default
}

// Get user details from file
User getUserDetails(string username) {
    User user;
    ifstream inFile("users.txt");
    
    if (inFile.is_open()) {
        string storedUsername, encryptedPassword;
        while (inFile >> storedUsername) {
            if (storedUsername == username) {
                user.username = storedUsername;
                inFile >> encryptedPassword;
                inFile.ignore();
                getline(inFile, user.fullName);
                getline(inFile, user.streetNumber);
                getline(inFile, user.residentialArea);
                inFile >> user.numberOfMeters;
                inFile.ignore();
                getline(inFile, user.meter1Serial);
                if (user.numberOfMeters == 2) {
                    getline(inFile, user.meter2Serial);
                }
                break;
            } else {
                // Skip this user's data
                string temp;
                inFile >> temp;
                inFile.ignore();
                for (int i = 0; i < 5; i++) {
                    getline(inFile, temp);
                }
            }
        }
        inFile.close();
    }
    
    return user;
}

// Get last meter reading for a specific meter
int getLastMeterReading(string username, string meterSerial) {
    ifstream inFile("records.txt");
    BillingRecord record;
    int lastReading = 0;
    
    if (inFile.is_open()) {
        while (inFile >> record.username) {
            inFile >> record.meterSerial;
            inFile.ignore();
            getline(inFile, record.month);
            inFile >> record.previousReading >> record.currentReading 
                   >> record.unitsConsumed >> record.totalBill;
            inFile.ignore();
            
            if (record.username == username && record.meterSerial == meterSerial) {
                lastReading = record.currentReading;
            }
        }
        inFile.close();
    }
    
    return lastReading;
}

// Get last billing month for a specific meter
string getLastBillingMonth(string username, string meterSerial) {
    ifstream inFile("records.txt");
    BillingRecord record;
    string lastMonth = "";
    
    if (inFile.is_open()) {
        while (inFile >> record.username) {
            inFile >> record.meterSerial;
            inFile.ignore();
            getline(inFile, record.month);
            inFile >> record.previousReading >> record.currentReading 
                   >> record.unitsConsumed >> record.totalBill;
            inFile.ignore();
            
            if (record.username == username && record.meterSerial == meterSerial) {
                lastMonth = record.month;
            }
        }
        inFile.close();
    }
    
    return lastMonth;
}

void registerUser() {
    User newUser;
    
    clearScreen();
    cout << "\n========================================\n";
    cout << "         USER REGISTRATION\n";
    cout << "========================================\n\n";
    
    // Username input
    cout << "Enter Username: ";
    cin >> newUser.username;
    cin.ignore();
    
    // Check if username already exists
    if (checkUserExists(newUser.username)) {
        cout << "\nUsername already exists! Please choose a different username.\n";
        cout << "Press Enter to continue...";
        cin.get();
        return;
    }
    
    // Full Name input
    cout << "Enter Full Name: ";
    getline(cin, newUser.fullName);
    
    // Password input with validation
    while (true) {
        cout << "\nPassword Requirements:\n";
        cout << "- At least 8 characters long\n";
        cout << "- At least one capital letter (A-Z)\n";
        cout << "- At least one special character (!@#$%&*)\n\n";
        cout << "Enter Password: ";
        getline(cin, newUser.password);
        
        if (isPasswordValid(newUser.password)) {
            break;
        }
    }
    
    // Street Number
    cout << "\nEnter Street Number: ";
    getline(cin, newUser.streetNumber);
    
    // Residential Area
    cout << "Enter Residential Area Name: ";
    getline(cin, newUser.residentialArea);
    
    // Number of Meters
    while (true) {
        newUser.numberOfMeters = getValidInteger("\nHow many meters do you have? (Maximum 2): ");
        
        if (newUser.numberOfMeters >= 1 && newUser.numberOfMeters <= 2) {
            break;
        } else {
            cout << "Invalid! Please enter 1 or 2 only.\n";
        }
    }
    
    // Meter Serial Numbers
    cout << "\nEnter Serial Number for Meter 1: ";
    getline(cin, newUser.meter1Serial);
    
    if (newUser.numberOfMeters == 2) {
        cout << "Enter Serial Number for Meter 2: ";
        getline(cin, newUser.meter2Serial);
    }
    
    // Encrypt password before saving
    string encryptedPassword = encryptPassword(newUser.password);
    
    // Save user to file
    ofstream outFile("users.txt", ios::app);
    if (outFile.is_open()) {
        outFile << newUser.username << " " << encryptedPassword << endl;
        outFile << newUser.fullName << endl;
        outFile << newUser.streetNumber << endl;
        outFile << newUser.residentialArea << endl;
        outFile << newUser.numberOfMeters << endl;
        outFile << newUser.meter1Serial << endl;
        if (newUser.numberOfMeters == 2) {
            outFile << newUser.meter2Serial << endl;
        }
        outFile.close();
        
        cout << "\n========================================\n";
        cout << "Registration successful!\n";
        cout << "Your password is encrypted and stored securely.\n";
        cout << "========================================\n";
        cout << "Press Enter to continue...";
        cin.get();
    } else {
        cout << "\nError: Unable to open file!\n";
        cout << "Press Enter to continue...";
        cin.get();
    }
}

bool checkUserExists(string username) {
    ifstream inFile("users.txt");
    string storedUsername, storedPassword;
    
    if (inFile.is_open()) {
        while (inFile >> storedUsername >> storedPassword) {
            if (storedUsername == username) {
                inFile.close();
                return true;
            }
            // Skip rest of user data
            string temp;
            inFile.ignore();
            for (int i = 0; i < 5; i++) {
                getline(inFile, temp);
            }
        }
        inFile.close();
    }
    return false;
}

void loginUser() {
    User loginUser;
    
    clearScreen();
    cout << "\n========================================\n";
    cout << "            USER LOGIN\n";
    cout << "========================================\n\n";
    
    cout << "Enter Username: ";
    cin >> loginUser.username;
    cout << "Enter Password: ";
    cin >> loginUser.password;
    
    if (validateLogin(loginUser.username, loginUser.password)) {
        cout << "\nLogin successful! Welcome, " << loginUser.username << "!\n";
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
        mainMenu(loginUser.username);
    } else {
        cout << "\nInvalid username or password!\n";
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

bool validateLogin(string username, string password) {
    ifstream inFile("users.txt");
    string storedUsername, storedEncryptedPassword;
    
    if (inFile.is_open()) {
        while (inFile >> storedUsername >> storedEncryptedPassword) {
            if (storedUsername == username) {
                string storedPassword = decryptPassword(storedEncryptedPassword);
                if (storedPassword == password) {
                    inFile.close();
                    return true;
                }
            }
            // Skip rest of user data
            string temp;
            inFile.ignore();
            for (int i = 0; i < 5; i++) {
                getline(inFile, temp);
            }
        }
        inFile.close();
    }
    return false;
}

void mainMenu(string username) {
    int choice;
    
    while (true) {
        clearScreen();
        cout << "\n========================================\n";
        cout << "         MAIN MENU - " << username << "\n";
        cout << "========================================\n\n";
        cout << "1. Enter Meter Reading\n";
        cout << "2. View Billing History\n";
        cout << "3. View Usage Statistics\n";
        cout << "4. Logout\n";
        cout << "\nEnter your choice: ";
        choice = getValidInteger("");
        
        switch (choice) {
            case 1:
                enterMeterReading(username);
                break;
            case 2:
                displayBillingHistory(username);
                break;
            case 3:
                displayStatistics(username);
                break;
            case 4:
                cout << "\nLogging out...\n";
                return;
            default:
                cout << "\nInvalid choice! Please enter 1, 2, 3, or 4.\n";
                cout << "Press Enter to continue...";
                cin.get();
        }
    }
}

void enterMeterReading(string username) {
    clearScreen();
    
    User user = getUserDetails(username);
    
    cout << "\n========================================\n";
    cout << "       MONTHLY METER READING\n";
    cout << "========================================\n";
    cout << "User: " << user.fullName << "\n";
    cout << "Address: " << user.streetNumber << ", " << user.residentialArea << "\n";
    cout << "========================================\n\n";
    
    // Select which meter to enter reading for
    int meterChoice;
    string selectedMeter;
    
    if (user.numberOfMeters == 1) {
        cout << "Meter Serial: " << user.meter1Serial << "\n\n";
        selectedMeter = user.meter1Serial;
    } else {
        cout << "Select Meter:\n";
        cout << "1. Meter 1 (Serial: " << user.meter1Serial << ")\n";
        cout << "2. Meter 2 (Serial: " << user.meter2Serial << ")\n\n";
        meterChoice = getValidInteger("Enter choice (1 or 2): ");
        
        if (meterChoice == 1) {
            selectedMeter = user.meter1Serial;
        } else if (meterChoice == 2) {
            selectedMeter = user.meter2Serial;
        } else {
            cout << "\nInvalid choice!\n";
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
    }
    
    BillingRecord record;
    record.username = username;
    record.meterSerial = selectedMeter;
    
    // Get or input month
    string lastMonth = getLastBillingMonth(username, selectedMeter);
    
    if (lastMonth.empty()) {
        // First time billing for this meter
        cout << "\nAvailable months:\n";
        string months[] = {"January", "February", "March", "April", "May", "June", 
                          "July", "August", "September", "October", "November", "December"};
        for (int i = 0; i < 12; i++) {
            cout << (i + 1) << ". " << months[i] << "\n";
        }
        int monthChoice = getValidInteger("\nSelect month (1-12): ");
        if (monthChoice >= 1 && monthChoice <= 12) {
            record.month = months[monthChoice - 1];
        } else {
            record.month = "January";
        }
    } else {
        // Auto-generate next month
        record.month = getNextMonth(lastMonth);
        cout << "\nBilling Month (Auto-Generated): " << record.month << "\n";
    }
    
    // Auto-fetch previous reading
    int lastReading = getLastMeterReading(username, selectedMeter);
    
    if (lastReading > 0) {
        cout << "Previous Meter Reading (Auto-Fetched): " << lastReading << " units\n";
        record.previousReading = lastReading;
    } else {
        cout << "No previous record found for this meter.\n";
        record.previousReading = getValidInteger("Enter Previous Meter Reading: ");
    }
    
    // Get current reading with validation
    while (true) {
        record.currentReading = getValidInteger("Enter Current Meter Reading: ");
        
        if (record.currentReading < record.previousReading) {
            cout << "\nError: Current reading (" << record.currentReading 
                 << ") cannot be less than previous reading (" 
                 << record.previousReading << ")!\n";
            cout << "Please enter current reading again.\n\n";
        } else {
            break;
        }
    }
    
    // Calculate units consumed
    record.unitsConsumed = record.currentReading - record.previousReading;
    
    // Calculate bill
    record.totalBill = calculateBill(record.unitsConsumed);
    
    // Display bill details with calculation
    clearScreen();
    cout << "\n========================================\n";
    cout << "         BILL SUMMARY\n";
    cout << "========================================\n";
    cout << "User           : " << user.fullName << "\n";
    cout << "Meter Serial   : " << record.meterSerial << "\n";
    cout << "Billing Month  : " << record.month << "\n";
    cout << "========================================\n";
    cout << fixed << setprecision(2);
    cout << "Previous Reading : " << record.previousReading << " units\n";
    cout << "Current Reading  : " << record.currentReading << " units\n";
    cout << "Units Consumed   : " << record.unitsConsumed << " units\n";
    cout << "========================================\n";
    
    // Show calculation breakdown
    cout << "\nBill Calculation:\n";
    cout << "----------------------------------------\n";
    
    if (record.unitsConsumed <= 100) {
        cout << record.unitsConsumed << " units x Rs. 5 = Rs. " << (record.unitsConsumed * 5.0) << "\n";
    } else if (record.unitsConsumed <= 300) {
        cout << "First 100 units x Rs. 5 = Rs. " << (100 * 5.0) << "\n";
        cout << "Next " << (record.unitsConsumed - 100) << " units x Rs. 8 = Rs. " << ((record.unitsConsumed - 100) * 8.0) << "\n";
    } else {
        cout << "First 100 units x Rs. 5  = Rs. " << (100 * 5.0) << "\n";
        cout << "Next 200 units x Rs. 8   = Rs. " << (200 * 8.0) << "\n";
        cout << "Remaining " << (record.unitsConsumed - 300) << " units x Rs. 10 = Rs. " << ((record.unitsConsumed - 300) * 10.0) << "\n";
    }
    
    cout << "========================================\n";
    cout << "TOTAL BILL AMOUNT: Rs. " << record.totalBill << "\n";
    cout << "========================================\n";
    
    // Save record to file
    saveBillingRecord(record);
    
    cout << "\nBilling record saved successfully!\n";
    cout << "Press Enter to continue...";
    cin.get();
}

double calculateBill(int units) {
    double bill = 0.0;
    
    if (units <= 100) {
        bill = units * 5.0;
    } else if (units <= 300) {
        bill = (100 * 5.0) + ((units - 100) * 8.0);
    } else {
        bill = (100 * 5.0) + (200 * 8.0) + ((units - 300) * 10.0);
    }
    
    return bill;
}

void saveBillingRecord(BillingRecord record) {
    ofstream outFile("records.txt", ios::app);
    
    if (outFile.is_open()) {
        outFile << record.username << " "
                << record.meterSerial << " "
                << record.month << " "
                << record.previousReading << " "
                << record.currentReading << " "
                << record.unitsConsumed << " "
                << fixed << setprecision(2) << record.totalBill << endl;
        outFile.close();
    }
}

void displayBillingHistory(string username) {
    clearScreen();
    cout << "\n========================================\n";
    cout << "       BILLING HISTORY - " << username << "\n";
    cout << "========================================\n\n";
    
    ifstream inFile("records.txt");
    BillingRecord record;
    bool recordFound = false;
    int recordNumber = 1;
    
    if (inFile.is_open()) {
        cout << fixed << setprecision(2);
        
        while (inFile >> record.username) {
            inFile >> record.meterSerial;
            inFile.ignore();
            getline(inFile, record.month);
            inFile >> record.previousReading >> record.currentReading 
                   >> record.unitsConsumed >> record.totalBill;
            inFile.ignore();
            
            if (record.username == username) {
                recordFound = true;
                
                cout << "Record #" << recordNumber++ << "\n";
                cout << "----------------------------------------\n";
                cout << "Meter Serial     : " << record.meterSerial << "\n";
                cout << "Month            : " << record.month << "\n";
                cout << "Previous Reading : " << record.previousReading << " units\n";
                cout << "Current Reading  : " << record.currentReading << " units\n";
                cout << "Units Consumed   : " << record.unitsConsumed << " units\n";
                cout << "Total Bill       : Rs. " << record.totalBill << "\n";
                cout << "----------------------------------------\n\n";
            }
        }
        inFile.close();
        
        if (!recordFound) {
            cout << "No billing records found for this user.\n";
        }
    } else {
        cout << "Unable to open records file!\n";
    }
    
    cout << "\nPress Enter to continue...";
    cin.get();
}

void displayStatistics(string username) {
    clearScreen();
    cout << "\n========================================\n";
    cout << "   USAGE STATISTICS - " << username << "\n";
    cout << "========================================\n\n";
    
    ifstream inFile("records.txt");
    BillingRecord record;
    int totalRecords = 0;
    int totalUnits = 0;
    double totalAmount = 0.0;
    int maxUnits = 0;
    int minUnits = 999999;
    
    if (inFile.is_open()) {
        while (inFile >> record.username) {
            inFile >> record.meterSerial;
            inFile.ignore();
            getline(inFile, record.month);
            inFile >> record.previousReading >> record.currentReading 
                   >> record.unitsConsumed >> record.totalBill;
            inFile.ignore();
            
            if (record.username == username) {
                totalRecords++;
                totalUnits += record.unitsConsumed;
                totalAmount += record.totalBill;
                
                if (record.unitsConsumed > maxUnits) {
                    maxUnits = record.unitsConsumed;
                }
                if (record.unitsConsumed < minUnits) {
                    minUnits = record.unitsConsumed;
                }
            }
        }
        inFile.close();
        
        if (totalRecords > 0) {
            cout << fixed << setprecision(2);
            cout << "Total Bills Generated : " << totalRecords << "\n";
            cout << "Total Units Consumed  : " << totalUnits << " units\n";
            cout << "Total Amount Paid     : Rs. " << totalAmount << "\n";
            cout << "Average Units/Month   : " << (totalUnits / totalRecords) << " units\n";
            cout << "Average Bill/Month    : Rs. " << (totalAmount / totalRecords) << "\n";
            cout << "Highest Consumption   : " << maxUnits << " units\n";
            cout << "Lowest Consumption    : " << minUnits << " units\n";
            cout << "========================================\n";
        } else {
            cout << "No statistics available yet.\n";
        }
    } else {
        cout << "Unable to open records file!\n";
    }
    
    cout << "\nPress Enter to continue...";
    cin.get();
}