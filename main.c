#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#define MAX_ACCTS 100 // Maximum number of bank accounts
// Structure to represent Bank Account
struct bankAccount{
    char accountHolder[20]; // Name of the account holder
    float balance; // Balance in the account
    int accountNo; // Account number
    int pin; // PIN for transactions
    int hasPin; // Flag to indicate if PIN is set
    int exists; // Flag to indicate if account exists
};
struct Transaction{
    int accountNo;
    char type[20];
    float amount;
    char date[11];
};
// Hash Table Entry structure
typedef struct HashEntry{
    int key; // Key for the entry
    struct bankAccount data; // Data associated with the key
    struct HashEntry *next; // Pointer to the next entry in the chain
}HashEntry;
// Hash Table structure
typedef struct HashTable{
    HashEntry **table; // Array of pointers to HashEntry structures
}HashTable;
// Function to calculate the hash value for a key
int hashFunction(int key){
    return key%MAX_ACCTS; // Simple hash function
}
// Function to create a new hash table
HashTable *createHashTable(){
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable)); // Allocate memory for the hash table
    ht->table = (HashEntry **)malloc(MAX_ACCTS * sizeof(HashEntry *)); // Allocate memory for the table array
    for(int i=0;i<MAX_ACCTS;i++)
        ht->table[i]=NULL; // Initialize each entry to NULL
    return ht; // Return the newly created hash table
}
// // Function to insert a new bank account into the hash table
void insert(HashTable *ht, int key, struct bankAccount data){
    int index = hashFunction(key);
    HashEntry *newEntry = (HashEntry *)malloc(sizeof(HashEntry)); // Allocate memory for the new entry
    newEntry->key=key;
    newEntry->data=data;
    newEntry->next=NULL;
    if(ht->table[index]==NULL)
        ht->table[index]=newEntry;
    else{
        HashEntry *current = ht->table[index];
        while(current->next!=NULL)
            current=current->next;
        current->next=newEntry;
    }
}
// Function to update an existing bank account in the hash table
bool update(HashTable *ht, int key, struct bankAccount data){
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
// Function to save all bank accounts to a CSV file
void saveAccounts(HashTable *ht){
    FILE *file=fopen("accounts.csv","w");
    if(file==NULL){
        printf("Error opening file.\n");
        return;
    }
    fprintf(file,"Account Holder,Account Number,Balance,Pin,HasPin\n"); // Write the header row to the file
    for(int i = 0;i < MAX_ACCTS;i++){
        HashEntry *current = ht->table[i];
        while(current!=NULL){
            fprintf(file,"%s,%d,%.2f,%d,%d\n",current->data.accountHolder,current->data.accountNo,current->data.balance,current->data.pin,current->data.hasPin);
            current=current->next;
        }
    }
    fclose(file);
    printf("Account data saved to accounts.csv\n");
}
// Function to load bank accounts from a CSV file into the hash table
void loadAccounts(HashTable *ht){
    FILE *file = fopen("accounts.csv","r");
    if(file==NULL){
        return;
    }
    char line[100]; // Buffer to store each line of the file
    fgets(line,sizeof(line),file); // Read the header row and discard it
    while(fgets(line, 100, file)){
        struct bankAccount newAccount;
        char *token = strtok(line, ",");
        strcpy(newAccount.accountHolder, token);
        token = strtok(NULL, ",");
        newAccount.accountNo = atoi(token);
        token = strtok(NULL, ",");
        newAccount.balance = atof(token);
        token = strtok(NULL, ",");
        newAccount.pin = atoi(token);
        token = strtok(NULL, ",");
        newAccount.hasPin = atoi(token);
        insert(ht,newAccount.accountNo,newAccount);
    }
    fclose(file);
}
void getDate(char *date){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date,"%02d-%02d-%04d",tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900);
}
char* generateFilename(int accountNo){
    char *filename = malloc(50 * sizeof(char));
    sprintf(filename, "account_%d_transactions.txt", accountNo);
    return filename;
}
void recordTransaction(struct Transaction transaction){
    char *filename = generateFilename(transaction.accountNo);
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(file, "Date: %s | Type: %s | Amount: %.2f\n", transaction.date,transaction.type,transaction.amount);
    fclose(file);
    free(filename);
}
struct bankAccount *search(HashTable *ht,int key){
    int index = hashFunction(key);
    HashEntry *entry = ht->table[index];
    while(entry!=NULL){
        if(entry->key==key)
            return &(entry->data);
        entry=entry->next;
    }
    return NULL;
}
int min = 1000;
int max = 9999;
int numAccounts=0;
bool validAccountNumber(int accountNumber){
    if (accountNumber >= min && accountNumber <= max)
        return true;
    return false;
}
bool validAmount(float amount){
    if (amount >= 0.00 && amount <= 200000)
        return true;
    return false;
}
// Function to create a new bank account
void createAccount(HashTable *ht){
    if(numAccounts < MAX_ACCTS){
        struct bankAccount newAccount;
        printf("Enter Account holder name: ");
        scanf("%s",newAccount.accountHolder);
        srand(time(0));
        int num = rand() % (max - min + 1) + min;
        newAccount.accountNo = num;
        printf("Account Number: %d generated successfully.\n",newAccount.accountNo);
        // Initially Bank Balance set to zero (0.00)
        newAccount.balance = 0.00;
        newAccount.hasPin = 0;
        newAccount.exists = 1;
        numAccounts++;
        insert(ht,newAccount.accountNo, newAccount);
        printf("Account created successfully.\n");
    }
    else{
        printf("Cannot create more accounts.\n");
        printf("Maximum limit reached.\n");
        printf(" Please come after lunch time!!.");
    }
}
// Function for pin creation for deposits and withdrawals
void pinCreation(HashTable *ht){
    int key,pin,temp,accountno;
    printf("Enter Account Number: ");
    scanf("%d",&accountno);
    if(!validAccountNumber(accountno)){
        printf("Invalid Account Number. Exiting to Main Menu....\n");
        return;
    }
    struct bankAccount *account = search(ht,accountno);
    if(account==NULL){
        printf("Account Not Found!\n");
        printf("Try Again!\n");
        return;
    }
    printf("Account Found Successfully!\n");
    printf("Are you sure to create pin?\n");
    printf("Enter 0 for No and 1 for Yes\n");
    scanf("%d",&key);
    if(key){
        if (account->hasPin) {
            printf("PIN already exists for this account. Exiting to Main Menu....\n");
            return;
        }
        printf("Enter the 4 digit password\n");
        scanf("%d",&pin);
        while(pin < 1000 || pin > 9999){
            printf("Invalid PIN. Please enter a 4-digit number: ");
            scanf("%d",&pin);
        }
        printf("Please Enter the PIN Again!\n");
        scanf("%d",&temp);
        while(temp!=pin){
            printf("PINs do not match. Please Enter the PIN Again!\n");
            scanf("%d",&temp);
        }
        printf("PIN created successfully!\n");
        account->pin = temp;
        account->hasPin = 1;
    }
}

/* Function to perform a deposit into Bank Account*/
void deposit(HashTable *ht, int accountNumber,float amount){
    int enteredPin,pinAttempts = 0, correctPin = 0;
    struct bankAccount *account = search(ht,accountNumber);
    if(account==NULL){
        printf("Account Not Found!\n");
        printf("Deposit Failed!.\n");
        return;
    }
    if(account->hasPin == 0){
        printf("PIN not set. Deposit Failed!.\n");
        return;
    }
    while(pinAttempts < 3 && !correctPin){
        printf("Enter your 4-digit PIN: ");
        scanf("%d",&enteredPin);
        if(enteredPin == account->pin){
            correctPin = 1;
            account->balance += amount;
            printf("Deposit Successful. New Balance = %.2f\n",account->balance);
            struct Transaction transaction;
            transaction.accountNo=accountNumber;
            strcpy(transaction.type,"Deposit");
            transaction.amount=amount;
            getDate(transaction.date);
            recordTransaction(transaction);
        }
        else{
            pinAttempts++;
            printf("Incorrect PIN. %d attempts remaining. \n",3 - pinAttempts);
        }
    }
    if(!correctPin){
        printf("Too many incorrect PIN attempts. Deposit failed.\n");
        return;
    }
}

/* Function to perform a withdrawal from a Bank Account*/
void withdrawal(HashTable *ht, int accountNumber,float amount){
    int enteredPin,pinAttempts = 0, correctPin = 0;
    struct bankAccount *account = search(ht,accountNumber);
    if(account==NULL){
        printf("Account Not Found!\n");
        printf("Withdrawal Failed!.\n");
        return;
    }
    if(account->hasPin){
        while(pinAttempts < 3 && !correctPin){
            printf("Enter your 4-digit PIN: ");
            scanf("%d",&enteredPin);
            if(enteredPin == account->pin){
                correctPin = 1;
                if(amount <= account->balance){
                    account->balance -= amount;
                    printf("Withdrawal Successful. New Balance = %.2f\n",account->balance);
                    struct Transaction transaction;
                    transaction.accountNo=accountNumber;
                    strcpy(transaction.type,"Withdrawal");
                    transaction.amount=amount;
                    getDate(transaction.date);
                    recordTransaction(transaction);
                }
                else
                    printf("Insufficient Funds Poor Peasant \U0001F612\n");
            }
            else{
                pinAttempts++;
                printf("Incorrect PIN. %d attempts remaining. \n",3 - pinAttempts);
            }
        }
        if(!correctPin){
            printf("Too many incorrect PIN attempts. Withdrawal failed.\n");
            return;
        }
    }
    else{
        printf("PIN not set. Withdrawal Failed!.\n");
        return;
    }
}

/* Function to Check Bank Balance*/
void check(HashTable *ht, int accountNumber){
    int enteredPin,pinAttempts = 0, correctPin = 0;
    struct bankAccount *account = search(ht,accountNumber);
    if(account==NULL){
        printf("Account Not Found!\n");
        printf("Balance Check Failed!.\n");
        return;
    }
    if(account->hasPin == 0){
        printf("PIN not set. Deposit Failed!.\n");
        return;
    }
    while(pinAttempts < 3 && !correctPin){
        printf("Enter your 4-digit PIN: ");
        scanf("%d",&enteredPin);
        if(enteredPin == account->pin){
            correctPin = 1;
            printf("Account Found. Getting Details....\n");
            printf("Account Holder : %s\nAccount Number : %ld\nBalance : %.2f\n",account->accountHolder,account->accountNo,account->balance);
            return;
        }
        else{
            pinAttempts++;
            printf("Incorrect PIN. %d attempts remaining. \n",3 - pinAttempts);
        }
    }
    if(!correctPin){
        printf("Too many incorrect PIN attempts. Balance check failed.\n");
        return;
    }
}
int main(){
    HashTable *ht = createHashTable();
    loadAccounts(ht);
    printf("Welcome to Console Bank\n");
    int key,choice,accountNumber;
    float amount;
    bool val;
    do{
        printf("\nConsole Bank\n");
        printf("1. Create a New Account\n");
        printf("2. PIN Creation\n");
        printf("3. Deposit\n");
        printf("4. Withdrawal\n");
        printf("5. Check Balance\n");
        printf("6. Exit\n");
        printf("Enter my(your) choice: ");
        scanf("%d",&choice);
        switch(choice){
            case 1:
                   createAccount(ht);
                   break;
            case 2:
                   pinCreation(ht);
                   break;
            case 3:
                   printf("Enter Account Number for deposit : ");
                   scanf("%d",&accountNumber);
                   val = validAccountNumber(accountNumber);
                   if(val){
                        printf("Enter Deposit amount : ");
                        scanf("%f",&amount);
                        bool val2 = validAmount(amount);
                        if(val2){
                            deposit(ht,accountNumber,amount);
                        }
                        else
                            printf("Invalid Amount. Exiting to Main Menu....\n");
                   }
                   else
                        printf("Invalid Account Number. Exiting to Main Menu....\n");
                   break;
            case 4:
                   printf("Enter Account Number for withdrawal : ");
                   scanf("%d",&accountNumber);
                   val = validAccountNumber(accountNumber);
                   if(val){
                        printf("Enter Withdrawal amount : ");
                        scanf("%f",&amount);
                        bool val2 = validAmount(amount);
                        if(val2){
                            withdrawal(ht,accountNumber,amount);
                        }
                        else
                            printf("Invalid Amount. Exiting to Main Menu....\n");
                   }
                   else
                        printf("Invalid Account Number. Exiting to Main Menu....\n");
                   break;
            case 5:
                   printf("Enter Account Number to check Balance : ");
                   scanf("%d",&accountNumber);
                   val = validAccountNumber(accountNumber);
                   if(val){
                        check(ht,accountNumber);
                   }
                   else
                        printf("Exiting to Main Menu....\n");
                   break;
            case 6:
                   printf("Exiting the Bank. Thank You!\n");
                   break;
            default:
                    printf("Invalid choice. Please enter a valid option.\n");
        }
    }
    while(choice!=6);
    saveAccounts(ht);
    return 0;
}
