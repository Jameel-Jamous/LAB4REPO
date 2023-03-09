#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

class BankAccount {
public:

	bool setAccount(std::string theData);		//	Initializes a BankAccount from data in a std::string
	std::string printAcc(int printWidths[]);	//	Prints the data stored in the BankAccount as a formatted std::string

private:
	std::string firstName = "N/A";
	std::string lastName  = "N/A";
	int	bankAccNo = 0000000;
	float balAvailable = 0.00;
	float balPresent   = 0.00;
};

enum class MAINM_OP { NO_OP = 0, PRINT_REC, SEARCH, SORT, QUIT };
enum class SUBM_OP { NO_OP = 0, FIRST, LAST, ID, BAL_AVAIL, BAL_PRES, RETURN };

const unsigned int MAX_BANK_ACCS = 100U;
const unsigned int MAX_BANK_ACC_DIGITS = 7U;
const unsigned int MAX_BANK_ACC_CONTENT = 5U;
const char	THE_TEXT_DELIM = ',';
const std::string theInFilePath = "tf1.csv";
const std::string theErrorFilePath = "err.txt";

MAINM_OP promptMenu(int progIter);	//	Prompts the main menu to the user and returns the corresponding MAINM_OP
int putDataIn(BankAccount storeHere[]);	//	Stores data from theInFile and returns the number of recorded valid records
void printRecords(BankAccount theRecords[], int noOfRecords);	//	Print the records in a tabulated format
std::string& toProper(std::string& theString);	//	Transforms a string to proper case(ie. jOhN => John)
std::string toBANO(int theBaNo);	//	Transforms a string to proper case(ie. jOhN => John)
bool sortBy(SUBM_OP theOp, BankAccount theRecords[], int noOfRecords);
bool searchBy(SUBM_OP theOp, BankAccount theRecords[], int noOfRecords);

void sortMenuOptions(BankAccount theRecords[], int noOfRecords);
void searchMenuOptions(BankAccount theRecords[], int noOfRecords);


int main() {
	MAINM_OP userOp = MAINM_OP::NO_OP;
	BankAccount theContainer[MAX_BANK_ACCS] = {};
	int noOfRecords = putDataIn(theContainer);
	int progIter = 0;

	std::cout << "\nWelcome to the Program! Current File <" << theInFilePath << ">" << std::endl;
	do {
		userOp = promptMenu(progIter);
		switch (userOp) {
		case MAINM_OP::PRINT_REC:
			printRecords(theContainer, noOfRecords);
			break;
		case MAINM_OP::SORT:
			sortMenuOptions(theContainer, noOfRecords);
			break;
		case MAINM_OP::SEARCH:
			searchMenuOptions(theContainer, noOfRecords);
			break;
		case MAINM_OP::QUIT:
			std::cout << "\nQuiting..." << std::endl;
			break;
		default:
			std::cout << "\nInvalid Input! Please Enter Again!\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			break;
		}
		progIter++;
	} while (userOp != MAINM_OP::QUIT);

	return 0;
}

bool BankAccount::setAccount(std::string theData) {
	size_t found = theData.find(THE_TEXT_DELIM);
	std::string theEntry = "";
	bool valid = true;
	int idx = 1;

	while (theData.size() > 0) {
		if (!theData.empty()) {	
			if (found == std::string::npos) {	//	Determine if a delimiter was found
				theEntry = theData.substr(0, found);  //	Record the entry
				theData.clear();	//	Clear theData
			}
			else {
				theEntry = theData.substr(0, found);  //	Record the entry
				theData.erase(0, found + 1);	//	Remove the entry from the data container
			}

			if (theEntry.empty()) {	//	If any entry is empty it is either a COMPLETELY valid or invalid
				valid = (idx > MAX_BANK_ACC_CONTENT) ? true : false;
				idx = -1;	//	Enter the default case
			}		
		}

		switch (idx) {	//	Determine where to store the extracted information
		case 1:
			firstName = toProper(theEntry);
			break;
		case 2:
			lastName = toProper(theEntry);
			break;
		case 3:
			if (theEntry.size() == MAX_BANK_ACC_DIGITS) {	//	The bank account number must be of 7 digits
				bankAccNo = std::stoi(theEntry);
			}
			else {	//	Otherwise it is invalid
				valid = false;
				idx = -1;
			}
			break;
		case 4:
			balAvailable = std::stof(theEntry);
			break;
		case 5:
			balPresent= std::stof(theEntry);
			break;
		default:
			theData.clear();	//	Clear theData to leave loop
			break;
		}

		if (valid && idx <= MAX_BANK_ACC_CONTENT) {
			idx++;
			found = theData.find(THE_TEXT_DELIM);
		}
	}

	return valid;
}

std::string BankAccount::printAcc(int printWithds[]) {	// Consider an alternative approach instead of passing widths from header
	std::stringstream retString("");
	int noOfDigit = 2;	//	Is this really needed?
	float pendingAmount = balPresent - balAvailable;
	std::string baNo = toBANO(bankAccNo);	// convert int to string 

	retString << firstName << std::setw(printWithds[1]) << lastName << std::setw(printWithds[0])
		<< baNo << std::setw(printWithds[0]) 
		<< std::setprecision(noOfDigit) << std::fixed << balAvailable << std::setw(printWithds[0])
		<< balPresent << std::setw(printWithds[0]); // Set precision on the float values
	if (pendingAmount == 0)
		retString << "-";
	else
		retString << pendingAmount;
	retString << std::endl;

	return retString.str();
}

MAINM_OP promptMenu(int progIter) {
	unsigned int userSel = 0U;
	int width = 5;

	std::cout << "\nMain Menu (Iteration:" << progIter << ")" << std::endl << std::setfill(' ')
		<< std::setw(width) << int(MAINM_OP::PRINT_REC) << " - Print" << std::endl
		<< std::setw(width) << int(MAINM_OP::SEARCH) << " - Search" << std::endl
		<< std::setw(width) << int(MAINM_OP::SORT) << " - Sort" << std::endl
		<< std::setw(width) << int(MAINM_OP::QUIT) << " - Quit" << std::endl 
		<< "Enter a number: " << std::endl;
	std::cin >> userSel;

	return MAINM_OP(userSel);
}

int putDataIn(BankAccount theRecords[]) {
	std::ifstream inFile(theInFilePath.c_str(), std::ios::in);
	std::ofstream errFile(theErrorFilePath.c_str(), std::ios::out);
	std::string temp;
	unsigned int idx = 0, e_idx = 0, noOfRecords = 0;

	errFile << "This error file corresponds to the following file <" << theInFilePath << ">\n";

	if (!inFile) {
		std::cout << "Input file not found..." << std::endl;
		noOfRecords = -1;
	}

	if (!errFile) {
		std::cout << "Error file could not be opened..." << std::endl;
		noOfRecords = -1;
	}

	while (idx < MAX_BANK_ACCS && !inFile.eof()) {					
		inFile >> temp;												// Aquire the bank account entry and store it in temp
		if (theRecords[idx].setAccount(temp)) {						// Set the BankAccount class to the contents of temp
			noOfRecords++;											// If the contents were valid, increment counters and idx
			idx++;
			e_idx++;
			temp.clear();
		}
		else {
			errFile << "Invalid Entry at line " << e_idx << std::endl;
			e_idx++;
		}
	}

	if (idx == 1) {
		std::cout << "The input file is empty..." << std::endl;
		noOfRecords = -1;
	}
	else if (idx == MAX_BANK_ACCS && !inFile.eof()) {
		std::cout << "***WARNING!***\nNot all numbers have been stored; only first "
			<< MAX_BANK_ACCS << " will be processed" << std::endl;
	}

	return noOfRecords;
}

void printRecords(BankAccount theRecords[], int noOfRecords) {
	int width = 20;	
	int theWidths[4] = { width , width - 5, width - 2, (5 * width)}; 

	std::cout << "\nFirst Name" << std::setw(theWidths[1]) << "Last Name" << std::setw(theWidths[0])
		<< "Account Number" << std::setw(theWidths[0]) << "Available Balance" << std::setw(theWidths[0])
		<< "Present Balance" << std::setw(theWidths[0]) << "Pending Amount\n" << std::setfill('-')
		<< std::setw(theWidths[3]) << "-" << std::endl;	//	Print the header 

	if (noOfRecords > 0) {	//	Print the records
		for (int i = 0; i < noOfRecords-1; i++) {
			std::cout << theRecords[i].printAcc(theWidths) << std::endl;
		}
	}
	else {
		std::cout << "\nNo Records to Print...\n";
	}
}

std::string& toProper(std::string& aString) {
	int idx = 0;
	
	// Clear junk
	if (aString[0] < 0) {
		while (aString[idx] < 0) {
			idx++;
		}
		aString.erase(0, idx);
	}

	// Lower Case the whole string
	for (int i = 0; i < aString.size(); i++) {
		aString[i] = tolower(aString[i]);
	}

	// Upper Case the first letter
	aString[0] = toupper(aString[0]);

	return aString;
}

std::string toBANO(int theBaNo) {
	std::string baNoStr = std::to_string(theBaNo);

	if (baNoStr.size() < MAX_BANK_ACC_DIGITS) {	// Check size 
		for (int i = baNoStr.size(); i < (MAX_BANK_ACC_DIGITS); i++) {		
			baNoStr.insert(0, "0"); // insert 0s at the beginning of the string
		}
	}
	return baNoStr;
}

void sortMenuOptions(BankAccount theRecords[], int noOfRecords){
	unsigned int userSel = 0U;
	int width = 5;
	SUBM_OP userOp = SUBM_OP::NO_OP;
	do {
		std::cout << "\nSort Menu" << std::endl << std::setfill(' ')
			<< std::setw(width) << int(SUBM_OP::FIRST) << " - First" << std::endl 
			<< std::setw(width) << int(SUBM_OP::LAST) << " - Last" << std::endl
			<< std::setw(width) << int(SUBM_OP::ID) << " - ID" << std::endl
			<< std::setw(width) << int(SUBM_OP::BAL_AVAIL) << " - Balance Available" << std::endl 
			<< std::setw(width) << int(SUBM_OP::BAL_PRES) << " - Balance Present" << std::endl 
			<< std::setw(width) << int(SUBM_OP::RETURN) << " - Return to Main Menu" << std::endl 
			<< "Enter a number: " << std::endl;
		std::cin >> userSel;
		userOp = SUBM_OP(userOp);
	} while(userOp != SUBM_OP::RETURN);
}

void searchMenuOptions(BankAccount theRecords[], int noOfRecords){
	unsigned int userSel = 0U;
	int width = 5;
	SUBM_OP userOp = SUBM_OP::NO_OP;
	do {
		std::cout << "\nSearch Menu" << std::endl << std::setfill(' ')
			<< std::setw(width) << int(SUBM_OP::FIRST) << " - First" << std::endl
			<< std::setw(width) << int(SUBM_OP::LAST) << " - Last" << std::endl
			<< std::setw(width) << int(SUBM_OP::ID) << " - ID" << std::endl
			<< std::setw(width) << int(SUBM_OP::RETURN) << " - Return to Main Menu" << std::endl 
			<< "Enter a number: " << std::endl;
		std::cin >> userSel;
		userOp = SUBM_OP(userSel);
		if(userOp > SUBM_OP::NO_OP && userOp < SUBM_OP::BAL_AVAIL){
			//sortBy(userOp, theRecords, noOfRecords);
		}
		else if (userOp == SUBM_OP::RETURN) {
			std::cout << "\nReturning to Main Menu...\n";
		}
		else{
			std::cout << "\nInvalid Input! Please Enter Again!\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	} while(userOp != SUBM_OP::RETURN);
}


/*****************************************  TEST RUNS BEGIN HERE **********************************************************

******************************************  TEST RUNS END HERE   ************************************************************/








































/*******************************************TEST RUNS ENDS HERE*************************************************************/