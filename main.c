#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#define MAX_ACCTS 100
// Structure to represent Bank Account
struct bankAccount{
    char accountHolder[20];
    float balance;
    int accountNo;
    int pin;
    int hasPin;
};
typedef struct HashEntry{
    int key;
    struct bankAccount data;
    struct HashEntry *next;
}HashEntry;
typedef struct HashTable{
    HashEntry **table;
}HashTable;
int hashFunction(int key){
    return key%MAX_ACCTS;
}
HashTable *createHashTable(){
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    ht->table = (HashEntry **)malloc(MAX_ACCTS * sizeof(HashEntry *));
    for(int i=0;i<MAX_ACCTS;i++)
        ht->table[i]=NULL;
    return ht;
}
void insert(HashTable *ht, int key, struct bankAccount data){
    int index = hashFunction(key);
    HashEntry *newEntry = (HashEntry *)malloc(sizeof(HashEntry));
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
void pinCreation(HashTable *ht, int numAccounts){
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
                   pinCreation(ht, numAccounts);
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
    return 0;
}