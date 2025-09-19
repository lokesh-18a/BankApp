#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

// --- Constants and Type Definitions ---

#define MAX_ACCTS 100            // Maximum number of bank accounts
#define ACCT_HOLDER_NAME_LEN 50  // Maximum length of account holder's name

// Structure to represent Bank Account
typedef struct BankAccount{
    char accountHolder[ACCT_HOLDER_NAME_LEN]; // Name of the account holder
    float balance;                            // Balance in the account
    int accountNo;                            // Account number
    int pin;                                  // PIN for transactions
    int hasPin;                               // Flag to indicate if PIN is set
} BankAccount;

// Structure to represent a Transaction
typedef struct Transaction{
    int accountNo;
    char type[20];
    float amount;
    char date[11];
} Transaction;

// Hash Table Entry structure
typedef struct HashEntry{
    int key;                // Key for the entry
    BankAccount data;       // Data associated with the key
    struct HashEntry *next; // Pointer to the next entry in the chain
} HashEntry;

// Hash Table structure
typedef struct HashTable{
    HashEntry **table; // Array of pointers to HashEntry structures
} HashTable;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

bool validAccountNumber(int accountNumber){
    return accountNumber >= 1000 && accountNumber <= 9999;
}

bool validAmount(float amount){
    return amount >= 0.00 && amount <= 200000.00;
}

// Function to calculate the hash value for a key
int hashFunction(int key){
    return key % MAX_ACCTS; // Simple modulo hash function
}

// Function to search for a bank account by key
BankAccount *search(HashTable *ht, int key){
    int index = hashFunction(key);
    HashEntry *entry = ht -> table[index];
    while(entry != NULL){
        if(entry -> key == key)
            return &(entry -> data);
        entry = entry->next;
    }
    return NULL;
}

BankAccount* authenticateUser(HashTable *ht, int accountNumber) {
    if (!validAccountNumber(accountNumber)) {
        printf("Invalid Account Number. Exiting to Main Menu....\n");
        return NULL;
    }

    BankAccount *account = search(ht, accountNumber);
    if (account == NULL) {
        printf("Account Not Found!\n");
        printf("Try Again!\n");
        return NULL;
    }

    if (!account->hasPin) {
        printf("PIN not set. Exiting to Main Menu....\n");
        return NULL;
    }

    int enteredPin, pinAttempts = 0;
    while (pinAttempts < 3) {
        printf("Enter your 4-digit PIN: ");
        if (scanf("%d", &enteredPin) != 1) {
            printf("Invalid input. Please enter a numeric PIN.\n");
            clearInputBuffer(); // Clear invalid input
            continue;
        }
        clearInputBuffer(); // Clear the newline character from the buffer

        if (enteredPin == account->pin) {
            return account; // Authentication successful
        } else {
            pinAttempts++;
            printf("Incorrect PIN. %d attempts remaining.\n", 3 - pinAttempts);
        }
    }

    printf("Too many incorrect PIN attempts. Exiting to Main Menu....\n");
    return NULL; // Authentication failed
}

// Function to create a new hash table
HashTable *createHashTable(){
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable)); // Allocate memory for the hash table
    if (ht == NULL) {
        perror("Memory allocation failed for hash table.");
        exit(EXIT_FAILURE);
    }
    ht->table = (HashEntry **)calloc(MAX_ACCTS, sizeof(HashEntry *)); // Allocate memory for the table array
    if (ht->table == NULL) {
        perror("Memory allocation failed for hash table entries.");
        free(ht); // Free previously allocated memory
        exit(EXIT_FAILURE);
    }
    return ht; // Return the newly created hash table
}

// Function to insert a new bank account into the hash table
void insert(HashTable *ht, int key, BankAccount data){
    int index = hashFunction(key);
    HashEntry *newEntry = (HashEntry *)malloc(sizeof(HashEntry)); // Allocate memory for the new entry
    if (newEntry == NULL) {
        perror("Memory allocation failed for new hash entry.");
        return;
    }
    newEntry -> key = key;
    newEntry -> data = data;
    newEntry -> next = NULL;
    
    newEntry -> next = ht -> table[index]; // Insert the new entry at the beginning of the chain
    ht -> table[index] = newEntry;       // Update the head of the chain to
}

// Function to update an existing bank account in the hash table
bool update(HashTable *ht, int key, BankAccount data){
    int index = hashFunction(key);
    HashEntry *current = ht->table[index];
    while(current!=NULL){
        if(current->key==key){
            current->data=data;
            return true;
        }
        current=current->next; // Move to the next entry in the chain
    }
    return false; // Return false to indicate that the key was not found
}

// --- File I/O and Transaction Logging ---

// Function to save all bank accounts to a CSV file
void saveAccounts(HashTable *ht){
    FILE *file = fopen("accounts.csv","w");
    if(file == NULL){
        perror("Error opening file.");
        return;
    }
    if (fprintf(file, "Account Holder,Account Number,Balance,Pin,HasPin") < 0) {
        perror("Error writing header to accounts.csv");
        fclose(file);
        return;
    }
    for(int i = 0; i < MAX_ACCTS; i++){
        HashEntry *current = ht -> table[i];
        while(current != NULL){
            if (fprintf(file, "\"%s\",%d,%.2f,%d,%d\n",
                        current->data.accountHolder,
                        current->data.accountNo,
                        current->data.balance,
                        current->data.pin,
                        current->data.hasPin) < 0) {
                perror("Error writing account to accounts.csv");
                fclose(file);
                return;
            }
            current=current->next;
        }
    }
    if (fclose(file) == EOF) {
        perror("Error closing accounts.csv");
        return;
    }
    printf("Account data saved to accounts.csv\n");
}

// Function to load bank accounts from a CSV file
int loadAccounts(HashTable *ht){
    FILE *file = fopen("accounts.csv","r");
    if(file == NULL){
        perror("Error opening accounts.csv for reading");
        return 0;
    }
    char line[1024]; // Buffer to store each line of the file
    int count = 0;
    if (fgets(line, sizeof(line), file) == NULL) {
        // empty file or read error
        fclose(file);
        return 0;
    }
    while(fgets(line, sizeof(line), file)){
        BankAccount newAccount;
        // Parse line with quoted account holder name
        int result = sscanf(line, "\"%19[^\"]\",%d,%f,%d,%d",
                            newAccount.accountHolder,
                            &newAccount.accountNo,
                            &newAccount.balance,
                            &newAccount.pin,
                            &newAccount.hasPin);
        if(result == 5){
            insert(ht, newAccount.accountNo, newAccount);
            count++;
        } else {
            fprintf(stderr, "Malformed line in accounts.csv: %s", line);
        }
    }
    if (fclose(file) == EOF) {
        perror("Error closing accounts.csv");
        return count;
    }
    printf("Loaded %d accounts from accounts.csv\n", count);
    return count;
}

// --- Transaction and Date Functions ---

// Fills the date string with the current date in DD-MM-YYYY format
void getDate(char *date){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date,"%02d-%02d-%04d",tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// Generates a transaction filename based on the account number
void generateFilename(int accountNo, char *filename, int size){
    snprintf(filename, size, "account_%d_transactions.txt", accountNo);
}

// Records a transaction to the corresponding account's transaction file
void recordTransaction(Transaction transaction){
    char filename[50];
    generateFilename(transaction.accountNo, filename, sizeof(filename));
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file.");
        return;
    }
    fprintf(file, "Date: %s | Type: %s | Amount: %.2f\n", transaction.date,
        transaction.type,
        transaction.amount);
    fclose(file);
}

// Function to create a new bank account
void createAccount(HashTable *ht, int *numAccounts){
    if (*numAccounts >= MAX_ACCTS) {
        fprintf(stderr, "Cannot create more accounts. Maximum limit reached.\n");
        return;
    }
    BankAccount newAccount;
    printf("Enter Account holder name: ");
    if (fgets(newAccount.accountHolder, ACCT_HOLDER_NAME_LEN, stdin) == NULL) {
        printf("Error reading account holder name. Exiting...\n");
        return;
    }
    newAccount.accountHolder[strcspn(newAccount.accountHolder, "\n")] = '\0';
    int newAccNo;
    do {
        newAccNo = (rand() % 9000) + 1000; // Generate a random 4-digit account number
    } while (search(ht, newAccNo) != NULL);
    
    newAccount.accountNo = newAccNo;
    newAccount.balance = 0.0;
    newAccount.hasPin = false;

    insert(ht, newAccount.accountNo, newAccount);
    (*numAccounts)++;
    printf("Account created successfully! Your Account Number is %d\n", newAccount.accountNo);
}

// Function for pin creation for deposits and withdrawals
void pinCreation(HashTable *ht){
    int pin, temp, accountno, oldPin, attempts = 0;
    printf("Enter Account Number: ");
    scanf("%d",&accountno);
    clearInputBuffer();
    if(!validAccountNumber(accountno)){
        printf("Invalid Account Number. Exiting to Main Menu....\n");
        return;
    }
    BankAccount *account = search(ht, accountno);
    if(account == NULL){
        printf("Account Not Found!\n");
        return;
    }
    if (account->hasPin) {
        printf("PIN already exists for this account.\n");
        while (attempts < 3) {
            printf("Enter current PIN to update: ");
            scanf("%d", &oldPin);
            clearInputBuffer();

            if (oldPin == account->pin) {
                break; // correct pin
            }

            attempts++;
            if (attempts < 3) {
                printf("Incorrect PIN. Try again (%d attempt(s) left).\n", 3 - attempts);
            }
        }

        if (attempts == 3) {
            printf("Too many failed attempts. Exiting to Main Menu....\n");
            return;
        }
    }
    printf("Enter a new 4-digit PIN: ");
    scanf("%d", &pin);
    clearInputBuffer();
    while(pin < 1000 || pin > 9999){
        printf("Invalid PIN. Please enter a 4-digit number: \n");
        scanf("%d", &pin);
        clearInputBuffer();
    }
    printf("Re-enter the PIN for confirmation: ");
    scanf("%d", &temp);
    clearInputBuffer();
    if(pin == temp){
        printf("PIN %s successfully!\n", (account->hasPin ? "updated" : "set"));
        account->pin = pin;
        account->hasPin = true;
    }
    else{
        printf("PINs do not match. PIN creation failed.\n");
    }
}

// --- Function to perform a deposit into Bank Account --- 
void deposit(HashTable *ht){
    int accountNumber;
    float amount;

    printf("Enter Account Number for deposit : ");
    scanf("%d",&accountNumber);
    clearInputBuffer();

    BankAccount *account = authenticateUser(ht, accountNumber);
    if (account) {
        printf("Enter Deposit amount : ");
        scanf("%f",&amount);
        clearInputBuffer();
        if(!validAmount(amount)){
            printf("Invalid Amount. Exiting to Main Menu....\n");
            return;
        }

        account->balance += amount;
        printf("Deposit Successful. New Balance = %.2f\n", account->balance);
        Transaction transaction;
        transaction.accountNo = accountNumber;
        strcpy(transaction.type, "Deposit");
        transaction.amount = amount;
        getDate(transaction.date);
        recordTransaction(transaction);
    }
}

/* Function to perform a withdrawal from a Bank Account*/
void withdrawal(HashTable *ht){
    int accountNumber;
    float amount;
    printf("Enter Account Number for withdrawal: ");
    scanf("%d", &accountNumber);
    clearInputBuffer();
    
    BankAccount *account = authenticateUser(ht, accountNumber);
    if (account) {
        printf("Enter withdrawal amount: ");
        scanf("%f", &amount);
        clearInputBuffer();

        if (!validAmount(amount)) {
            printf("Invalid withdrawal amount.\n");
            return;
        }   

        if (account->balance < amount) {
            printf("Insufficient Funds Poor Peasant \U0001F612\n");
            return;
        }
        account->balance -= amount;
        printf("Withdrawal Successful. New Balance = %.2f\n", account->balance);
        Transaction transaction;
        transaction.accountNo = accountNumber;
        strcpy(transaction.type, "Withdrawal");
        transaction.amount = amount;
        getDate(transaction.date);
        recordTransaction(transaction);
    }
}

/* Function to Check Bank Balance*/
void checkBalance(HashTable *ht){
    int accountNumber;
    printf("Enter Account Number to check Balance : ");
    scanf("%d",&accountNumber);
    clearInputBuffer();
    BankAccount *account = authenticateUser(ht, accountNumber);
    if (account) {
        printf("\n--- Account Details ---\n");
        printf("Account Holder: %s\n", account->accountHolder);
        printf("Account Number: %d\n", account->accountNo);
        printf("Current Balance: %.2f\n", account->balance);
        printf("-----------------------\n");
    }
}

// Function to Send Money from one account to another
void sendMoney(HashTable *ht){
    int senderAccNo, receiverAccNo;
    float amount;
    BankAccount *senderAccount, *receiverAccount;

    printf("Enter your Account Number: ");
    scanf("%d", &senderAccNo);
    clearInputBuffer();
    senderAccount = authenticateUser(ht, senderAccNo);
    if (senderAccount == NULL) {
        printf("Sender authentication failed. Transaction cancelled.\n");
        return;
    }

    printf("Enter Receiver's Account Number: ");
    scanf("%d", &receiverAccNo);
    clearInputBuffer();

    if (!validAccountNumber(receiverAccNo)) {
        printf("Invalid Receiver Account Number format.\n");
        return;
    }

    receiverAccount = search(ht, receiverAccNo);
    if (receiverAccount == NULL) {
        printf("Receiver Account Not Found!\n");
        return;
    }

    if (senderAccNo == receiverAccNo) {
        printf("Sender and receiver accounts cannot be the same.\n");
        return;
    }
    printf("Enter Amount to Send: ");
    scanf("%f", &amount);
    clearInputBuffer();

    if (!validAmount(amount)) {
        printf("Invalid Amount. Exiting to Main Menu....\n");
        return;
    }

    if (senderAccount->balance < amount) {
        printf("Insufficient Funds. Transaction cancelled.\n");
        return;
    }
    
    senderAccount->balance -= amount;
    receiverAccount->balance += amount;

    Transaction transaction;
    getDate(transaction.date);
    transaction.amount = amount;
    
    strcpy(transaction.type, "Transfer Out");
    transaction.accountNo = senderAccNo;
    recordTransaction(transaction);
    
    strcpy(transaction.type, "Transfer In");
    transaction.accountNo = receiverAccNo;
    recordTransaction(transaction);

    printf("Transfer Successful.\n");
    printf("Your new balance is: %.2f\n", senderAccount->balance);
}

void freeHashTable(HashTable *ht) {
    for (int i = 0; i < MAX_ACCTS; i++) {
        HashEntry *entry = ht->table[i];
        while (entry != NULL) {
            HashEntry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}

int main(){
    srand(time(0));
    HashTable *ht = createHashTable();
    int numAccounts = loadAccounts(ht);
    printf("Welcome to Console Bank\n");
    int choice;
    do{
        printf("\nConsole Bank\n");
        printf("1. Create a New Account\n");
        printf("2. Create / Update PIN\n");
        printf("3. Deposit\n");
        printf("4. Withdrawal\n");
        printf("5. Check Balance\n");
        printf("6. Send Money\n");
        printf("7. Exit\n");
        printf("-------------------------\n");
        printf("Enter your choice: ");
        scanf("%d",&choice);
        clearInputBuffer();
        switch(choice){
            case 1:
                   createAccount(ht, &numAccounts);
                   break;
            case 2:
                   pinCreation(ht);
                   break;
            case 3:
                   deposit(ht);
                   break;
            case 4:
                   withdrawal(ht);
                   break;
            case 5:
                   checkBalance(ht);
                   break;
            case 6:
                   sendMoney(ht);
                    break;
            case 7:
                   printf("Exiting the Bank. Thank You!\n");
                   break;
            default:
                    printf("Invalid choice. Please enter a valid option.\n");
        }
    }
    while(choice!=7);
    saveAccounts(ht);
    freeHashTable(ht);
    return 0;
}
