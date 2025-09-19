# Console Bank: A Simple Banking System in C

## 📖 Overview

Console Bank is a simple, command-line-based banking application written entirely in C. It simulates basic banking operations, allowing users to create accounts, manage PINs, deposit, withdraw, and transfer funds. This project was developed as a comprehensive introduction to software development, focusing on core C programming concepts, data structures, and file handling for data persistence.

The application uses a **hash table** to efficiently store and manage user account data in memory, providing quick access and retrieval of account information.

## ✨ Features

- **Account Management**:
    - **Create Account**: Open a new bank account with a unique, randomly generated 4-digit account number.
    - **PIN Creation**: Secure your account by creating a 4-digit PIN. Transactions are locked until a PIN is set.
- **Core Banking Operations**:
    - **Deposit**: Add funds to an account.
    - **Withdrawal**: Withdraw funds from an account, with a check for sufficient balance.
    - **Check Balance**: View account holder details and the current balance.
- **Fund Transfers**:
    - **Send Money**: Securely transfer funds from one account to another.
- **Security**:
    - **PIN Authentication**: All transactions (deposit, withdrawal, balance check) are protected by PIN verification.
    - **Attempt Limiting**: Users get a maximum of three attempts to enter the correct PIN before the transaction is cancelled.
- **Data Persistence & Logging**:
    - **CSV Storage**: All account data is saved to `accounts.csv`, allowing information to persist between program runs.
    - **Transaction History**: Every transaction is recorded in a separate text file for each account (e.g., `account_1234_transactions.txt`), providing a complete transaction history.

## 🛠️ How to Compile and Run

### Prerequisites

You need a C compiler installed on your system. The most common one is **GCC**.

### Compilation

1.  Save the code in a file named `main.c` (or any other `.c` filename you prefer).
2.  Open your terminal or command prompt and navigate to the directory where you saved the file.
3.  Compile the program using the following command:

    ```bash
    gcc main.c -o bank_app
    ```

    This command compiles the `main.c` file and creates an executable file named `bank_app`.

### Running the Application

1.  After a successful compilation, run the application with this command:

    ```bash
    ./bank_app
    ```

2.  The application will start, and you can interact with it through the console menu.

## 📁 File Structure

When you run the application, it will create and interact with the following files in the same directory:

-   `accounts.csv`: This is the main database file. It stores the details of all created bank accounts in a comma-separated format.
    -   **Format**: `Account Holder,Account Number,Balance,Pin,HasPin`
-   `account_<ACCOUNT_NUMBER>_transactions.txt`: A separate transaction log is created for each account that has any activity. For example, an account with the number `4582` will have its history stored in `account_4582_transactions.txt`.
    -   **Format**: `Date: DD-MM-YYYY | Type: [Deposit/Withdrawal/...] | Amount: [Amount]`

---
