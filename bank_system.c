#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

struct Account
{
    char name[50];
    char account_number[20];
    float balance;
    int pin;
};

// Function to create a new account
// Function to create a new account
void createAccount()
{
    FILE *fp, *accNumFile;
    struct Account acc;
    int lastAccountNumber = 1000; // Starting account number
    char prefix[] = "AC";         // Prefix for account numbers
    int valid = 0;

    // Open the file to store the last account number
    accNumFile = fopen("account_number.txt", "r");
    if (accNumFile)
    {
        fscanf(accNumFile, "%d", &lastAccountNumber);
        fclose(accNumFile);
    }

    // Increment the account number for the new account
    lastAccountNumber++;
    sprintf(acc.account_number, "%sxxxx%d", prefix, lastAccountNumber);

    fp = fopen("bank.txt", "ab");
    if (!fp)
    {
        printf("Error opening file!\n");
        return;
    }

    // Validate name (only letters)
    do
    {
        printf("Enter Name (letters only): ");
        getchar(); // Clear buffer
        fgets(acc.name, sizeof(acc.name), stdin);
        acc.name[strcspn(acc.name, "\n")] = '\0'; // Remove newline

        valid = 1; // Assume valid until proven otherwise
        for (int i = 0; i < strlen(acc.name); i++)
        {
            if (!isalpha(acc.name[i]) && acc.name[i] != ' ') // Allow letters and spaces
            {
                valid = 0;
                printf("Invalid name. Please use letters only.\n");
                break;
            }
        }
    } while (!valid);

    // Validate PIN (exactly 4 digits)
    do
    {
        printf("Set a 4-digit PIN for your account: ");
        scanf("%d", &acc.pin);

        if (acc.pin < 1000 || acc.pin > 9999) // Ensure PIN is exactly 4 digits
        {
            printf("Invalid PIN. Please enter a 4-digit number.\n");
        }
        else
        {
            valid = 1;
        }
    } while (!valid);

    // Validate initial deposit amount
    do
    {
        printf("Enter Initial Deposit Amount: ");
        scanf("%f", &acc.balance);

        if (acc.balance < 0)
        {
            printf("Invalid amount. Initial deposit cannot be negative.\n");
        }
    } while (acc.balance < 0);

    fwrite(&acc, sizeof(acc), 1, fp);
    fclose(fp);

    // Save the updated account number back to the file
    accNumFile = fopen("account_number.txt", "w");
    if (accNumFile)
    {
        fprintf(accNumFile, "%d", lastAccountNumber);
        fclose(accNumFile);
    }

    printf("Account created successfully. Account Number: %s\n", acc.account_number);
}

// Function to view all accounts
void displayAllAccounts()
{
    FILE *fp;
    struct Account acc;
    int accountFound = 0;

    fp = fopen("bank.txt", "rb");
    if (!fp)
    {
        printf("No data found.\n");
        return;
    }

    printf("\n--- All Accounts ---\n");
    while (fread(&acc, sizeof(acc), 1, fp))
    {
        accountFound = 1;
        printf("Account Number: %s\n", acc.account_number);
        printf("Name: %s\n", acc.name);
        printf("Balance: %.2f\n\n", acc.balance);
    }

    if (!accountFound)
    {
        printf("No existing accounts found.\n");
    }

    fclose(fp);
}

// Function to delete an account
void deleteAccount()
{
    FILE *fp, *temp;
    struct Account acc;
    char accNum[20];

    fp = fopen("bank.txt", "rb");
    temp = fopen("temp.txt", "wb");
    if (!fp || !temp)
    {
        printf("Error opening files.\n");
        return;
    }

    printf("Enter Account Number to delete: ");
    scanf("%s", accNum);

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.account_number, accNum) != 0)
            fwrite(&acc, sizeof(acc), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("bank.txt");
    rename("temp.txt", "bank.txt");

    printf("Account deleted successfully.\n");
}

void searchAccount()
{
    FILE *fp;
    struct Account acc;
    char accNum[20];
    int found = 0;

    fp = fopen("bank.txt", "rb");
    if (!fp)
    {
        printf("No data found.\n");
        return;
    }

    printf("Enter Account Number to search: ");
    scanf("%s", accNum);

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.account_number, accNum) == 0)
        {
            found = 1;
            printf("\n--- Account Details ---\n");
            printf("Account Number: %s\n", acc.account_number);
            printf("Name: %s\n", acc.name);
            printf("Balance: %.2f\n", acc.balance);
            break;
        }
    }

    if (!found)
    {
        printf("Account not found.\n");
    }

    fclose(fp);
}

void logTransaction(const char *accountNumber, const char *type, float amount, float newBalance)
{
    FILE *historyFile;
    char filename[50];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[30];

    // Format the timestamp
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    // Create a unique file for the account's transaction history
    sprintf(filename, "%s_history.txt", accountNumber);

    historyFile = fopen(filename, "a"); // Open in append mode
    if (!historyFile)
    {
        printf("Error: Could not open transaction history file.\n");
        return;
    }

    // Log the transaction
    fprintf(historyFile, "%s | %s | %.2f | New Balance: %.2f\n", timestamp, type, amount, newBalance);
    fclose(historyFile);
}

void transferMoney()
{
    FILE *fp;
    struct Account acc, sender, receiver;
    char senderAccNum[20], receiverAccNum[20];
    int senderFound = 0, receiverFound = 0, enteredPin;
    float transferAmount;

    fp = fopen("bank.txt", "rb+");
    if (!fp)
    {
        printf("File not found.\n");
        return;
    }

    printf("Enter Sender's Account Number: ");
    scanf("%s", senderAccNum);

    // Find the sender's account
    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.account_number, senderAccNum) == 0)
        {
            sender = acc;
            senderFound = 1;
            break;
        }
    }

    if (!senderFound)
    {
        printf("Sender's account not found.\n");
        fclose(fp);
        return;
    }

    printf("Enter Sender's 4-digit PIN: ");
    scanf("%d", &enteredPin);

    if (enteredPin != sender.pin)
    {
        printf("Incorrect PIN. Transaction denied.\n");
        fclose(fp);
        return;
    }

    printf("Enter Receiver's Account Number: ");
    scanf("%s", receiverAccNum);

    // Find the receiver's account
    rewind(fp); // Reset file pointer to the beginning
    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.account_number, receiverAccNum) == 0)
        {
            receiver = acc;
            receiverFound = 1;
            break;
        }
    }

    if (!receiverFound)
    {
        printf("Receiver's account not found.\n");
        fclose(fp);
        return;
    }

    printf("Enter Amount to Transfer: ");
    scanf("%f", &transferAmount);

    if (transferAmount <= 0)
    {
        printf("Invalid transfer amount.\n");
        fclose(fp);
        return;
    }

    if (transferAmount > sender.balance)
    {
        printf("Insufficient funds in sender's account.\n");
        fclose(fp);
        return;
    }

    // Update balances
    sender.balance -= transferAmount;
    receiver.balance += transferAmount;

    // Update sender's account in the file
    rewind(fp);
    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.account_number, senderAccNum) == 0)
        {
            fseek(fp, -((long)sizeof(acc)), SEEK_CUR);
            fwrite(&sender, sizeof(sender), 1, fp);
            break;
        }
    }

    // Update receiver's account in the file
    rewind(fp);
    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.account_number, receiverAccNum) == 0)
        {
            fseek(fp, -((long)sizeof(acc)), SEEK_CUR);
            fwrite(&receiver, sizeof(receiver), 1, fp);
            break;
        }
    }

    fclose(fp);

    // Log the transactions
    logTransaction(sender.account_number, "Transfer Out", transferAmount, sender.balance);
    logTransaction(receiver.account_number, "Transfer In", transferAmount, receiver.balance);

    printf("Transfer successful! %.2f transferred from %s to %s.\n", transferAmount, sender.account_number, receiver.account_number);
}

// Function to update balance (Deposit or Withdraw)
void updateBalance(int isDeposit)
{
    FILE *fp;
    struct Account acc;
    char accNum[20];
    int found = 0, enteredPin;
    float amount;

    fp = fopen("bank.txt", "rb+");
    if (!fp)
    {
        printf("File not found.\n");
        return;
    }

    printf("Enter Account Number: ");
    scanf("%s", accNum);

    while (fread(&acc, sizeof(acc), 1, fp))
    {
        if (strcmp(acc.account_number, accNum) == 0)
        {
            printf("Enter your 4-digit PIN: ");
            scanf("%d", &enteredPin);

            if (enteredPin != acc.pin)
            {
                printf("Incorrect PIN. Transaction denied.\n");
                fclose(fp);
                return;
            }

            found = 1;
            printf("Current Balance: %.2f\n", acc.balance);
            printf("Enter amount: ");
            scanf("%f", &amount);

            if (!isDeposit && amount > acc.balance)
            {
                printf("Insufficient funds!\n");
                fclose(fp);
                return;
            }

            acc.balance += (isDeposit ? amount : -amount);
            fseek(fp, -((long)sizeof(acc)), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, fp);

            // Log the transaction
            logTransaction(acc.account_number, isDeposit ? "Deposit" : "Withdrawal", amount, acc.balance);

            printf("Transaction successful.\n");
            break;
        }
    }

    if (!found)
        printf("Account not found.\n");

    fclose(fp);
}

// Structure for loan details
struct Loan
{
    char account_number[20];
    float principal;
    float rate;
    int tenure;        // in months
    struct Loan *next; // For linked list
};

// Head of the loan linked list
struct Loan *loanHead = NULL;

// Function to apply for a loan
void applyForLoan()
{
    struct Loan *newLoan = (struct Loan *)malloc(sizeof(struct Loan));
    printf("Enter Account Number: ");
    scanf("%s", newLoan->account_number);
    printf("Enter Loan Amount (Principal): ");
    scanf("%f", &newLoan->principal);
    printf("Enter Annual Interest Rate (in %%): ");
    scanf("%f", &newLoan->rate);
    printf("Enter Loan Tenure (in months): ");
    scanf("%d", &newLoan->tenure);

    newLoan->next = loanHead;
    loanHead = newLoan;

    printf("Loan application submitted successfully!\n");
}

// Function to calculate EMI
void calculateEMI(float principal, float rate, int tenure)
{
    float emi;
    rate = rate / (12 * 100); // Monthly interest rate
    emi = (principal * rate * pow(1 + rate, tenure)) / (pow(1 + rate, tenure) - 1);
    printf("Your EMI is: %.2f\n", emi);
}

// Function to view all loans
void viewAllLoans()
{
    struct Loan *current = loanHead;
    if (current == NULL)
    {
        printf("No loans found.\n");
        return;
    }

    printf("\n--- Loan Details ---\n");
    while (current != NULL)
    {
        printf("Account Number: %s\n", current->account_number);
        printf("Principal: %.2f\n", current->principal);
        printf("Rate: %.2f%%\n", current->rate);
        printf("Tenure: %d months\n", current->tenure);
        calculateEMI(current->principal, current->rate, current->tenure);
        printf("\n");
        current = current->next;
    }
}

void viewTransactionHistory()
{
    FILE *historyFile;
    char accNum[20], filename[50], line[200];

    printf("Enter Account Number: ");
    scanf("%s", accNum);

    sprintf(filename, "%s_history.txt", accNum);

    historyFile = fopen(filename, "r");
    if (!historyFile)
    {
        printf("No transaction history found for this account.\n");
        return;
    }

    printf("\n--- Transaction History for Account %s ---\n", accNum);
    while (fgets(line, sizeof(line), historyFile))
    {
        printf("%s", line);
    }

    fclose(historyFile);
}

// Structure for audit logs
struct AuditLog
{
    char action[100];
    char timestamp[30];
    struct AuditLog *next;
};

// Head of the audit log linked list
struct AuditLog *logHead = NULL;

// Function to add an audit log
void addAuditLog(const char *action)
{
    struct AuditLog *newLog = (struct AuditLog *)malloc(sizeof(struct AuditLog));
    strcpy(newLog->action, action);

    // Get the current timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(newLog->timestamp, sizeof(newLog->timestamp), "%Y-%m-%d %H:%M:%S", t);

    newLog->next = logHead;
    logHead = newLog;
}

// Function to view all audit logs
void viewAuditLogs()
{
    struct AuditLog *current = logHead;
    if (current == NULL)
    {
        printf("No audit logs found.\n");
        return;
    }

    printf("\n--- Audit Logs ---\n");
    while (current != NULL)
    {
        printf("Action: %s\n", current->action);
        printf("Timestamp: %s\n", current->timestamp);
        printf("\n");
        current = current->next;
    }
}

// Function to back up data
void backupData()
{
    FILE *src, *dest;
    char ch;

    src = fopen("bank.txt", "r");
    if (!src)
    {
        printf("Error: Could not open source file for backup.\n");
        return;
    }

    dest = fopen("backup_bank.txt", "w");
    if (!dest)
    {
        printf("Error: Could not create backup file.\n");
        fclose(src);
        return;
    }

    while ((ch = fgetc(src)) != EOF)
    {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    printf("Backup completed successfully.\n");
}

// Function to restore data from backup
void restoreData()
{
    FILE *src, *dest;
    char ch;

    src = fopen("backup_bank.txt", "r");
    if (!src)
    {
        printf("Error: Could not open backup file for restoration.\n");
        return;
    }

    dest = fopen("bank.txt", "w");
    if (!dest)
    {
        printf("Error: Could not restore data to the main file.\n");
        fclose(src);
        return;
    }

    while ((ch = fgetc(src)) != EOF)
    {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    printf("Data restored successfully.\n");
}

// Function to handle login
// Function to handle login
int login(int *isAdmin)
{
    FILE *fp;
    char username[50], password[50];
    char storedUsername[50], storedPassword[50];
    int loginSuccess = 0;

    fp = fopen("credentials.txt", "r"); // Open file in read mode
    if (!fp)
    {
        printf("Error: Could not open credentials file.\n");
        return 0;
    }

    printf("----- Login Page -----\n");
    printf("\nEnter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    // Check credentials against the stored ones
    while (fscanf(fp, "%s %s", storedUsername, storedPassword) != EOF)
    {
        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0)
        {
            loginSuccess = 1;
            if (strcmp(username, "admin") == 0) // Check if the user is the admin
            {
                *isAdmin = 1;
            }
            break;
        }
    }

    fclose(fp);

    if (loginSuccess)
    {
        printf("Login successful!\n");
        return 1; // Login successful
    }
    else
    {
        printf("Invalid username or password. Access denied.\n");
        return 0; // Login failed
    }
}

// Function to add a new user (admin only)
void addUser()
{
    FILE *fp;
    char username[50], password[50];

    fp = fopen("credentials.txt", "a"); // Open file in append mode
    if (!fp)
    {
        printf("Error: Could not open credentials file.\n");
        return;
    }

    printf("----- Add New User -----\n");
    printf("Enter New Username: ");
    scanf("%s", username);
    printf("Enter New Password: ");
    scanf("%s", password);

    // Write the new username and password to the file
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);

    printf("New user added successfully!\n");
}

int main()
{
    int choice;
    int isAdmin = 0; // Flag to check if the logged-in user is an admin

    if (!login(&isAdmin))
        return 0;

    do
    {
        printf("\n--- Banking Management System ---\n\n");
        printf("1. Create Account\n");
        printf("2. Display All Accounts\n");
        printf("3. Deposit Money\n");
        printf("4. Withdraw Money\n");
        printf("5. Transfer Money\n");
        printf("6. View Transaction History\n");
        printf("7. Search Account\n");
        printf("8. Delete Account\n");
        printf("9. Apply for Loan\n");
        printf("10. View All Loans\n");
        printf("11. Backup Data\n");
        printf("12. Restore Data\n");
        printf("13. View Audit Logs\n");

        // Admin-only option to add a new user
        if (isAdmin)
        {
            printf("14. Add New Official User\n");
        }
        printf("15. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createAccount();
            addAuditLog("Created a new account");
            break;
        case 2:
            displayAllAccounts();
            addAuditLog("Viewed all accounts");
            break;
        case 3:
            updateBalance(1); // Deposit
            addAuditLog("Deposited money");
            break;
        case 4:
            updateBalance(0); // Withdraw
            addAuditLog("Withdrew money");
            break;
        case 5:
            transferMoney();
            addAuditLog("Transferred money between accounts");
            break;
        case 6:
            viewTransactionHistory();
            addAuditLog("Viewed transaction history");
            break;
        case 7:
            searchAccount();
            addAuditLog("Searched for an account");
            break;
        case 8:
            deleteAccount();
            addAuditLog("Deleted an account");
            break;
        case 9:
            applyForLoan();
            addAuditLog("Applied for a loan");
            break;
        case 10:
            viewAllLoans();
            addAuditLog("Viewed all loans");
            break;
        case 11:
            backupData();
            addAuditLog("Backed up data");
            break;
        case 12:
            restoreData();
            addAuditLog("Restored data from backup");
            break;
        case 13:
            viewAuditLogs();
            break;
        // Admin-only option to add a new user
        case 14:
            if (isAdmin)
            {
                addUser();
                addAuditLog("Added a new user");
            }
            else
            {
                printf("Invalid choice! Please try again.\n");
            }
            break;
        case 15:
            printf("Exited...\n");
            break;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 15);

    return 0;
}
