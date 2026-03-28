// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Required for strcmp
// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void listRecords(FILE *fPtr); // new function to list to screen
void logTransaction(const char *action, unsigned int acct, double amount);
int authenticate(void); // authentication engine
void initializeFile(const char *filename);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // Authentication Engine
    if (!authenticate())
    {
        puts("Authentication failed. Exiting.");
        exit(1);
    }

    // Try to open file for update (rb+). 
    // If it fails, the file likely doesn't exist, so we initialize it.
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File 'credit.dat' not found. Initializing new file...\n");
        initializeFile("credit.dat");
        
        // Re-open in rb+ mode after initialization
        if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
        {
            perror("Error opening file after initialization");
            exit(-1);
        }
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 6)
    {
        switch (choice)
        {
        case 1: // create text file from record file
            textFile(cfPtr);
            break;
        case 2: // update record
            updateRecord(cfPtr);
            break;
        case 3: // create record
            newRecord(cfPtr);
            break;
        case 4: // delete existing record
            deleteRecord(cfPtr);
            break;
        case 5: // list all accounts to screen
            listRecords(cfPtr);
            break;
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
    return 0;
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            // write single record to text file
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
        printf("Accounts exported to 'accounts.txt' successfully.\n");
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    printf("%s", "Enter account to update ( 1 - 100 ): ");
    if (scanf("%d", &account) != 1 || account < 1 || account > 100)
    {
        puts("Invalid account number. Must be between 1 and 100.");
        while (getchar() != '\n'); // clear buffer
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    { // update record
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        if (scanf("%lf", &transaction) != 1)
        {
            puts("Invalid transaction amount.");
            while (getchar() != '\n');
            return;
        }
        
        double oldBalance = client.balance;
        client.balance += transaction; // update record balance

        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer back to correct record
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        
        logTransaction("UPDATE", client.acctNum, transaction);
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       
    struct clientData blankClient = {0, "", "", 0}; 
    unsigned int accountNum;                        

    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    if (scanf("%d", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        while (getchar() != '\n');
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    } 
    else
    { 
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        logTransaction("DELETE", accountNum, 0.0);
        printf("Account %d deleted.\n", accountNum);
    } 
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; 

    printf("%s", "Enter new account number ( 1 - 100 ): ");
    if (scanf("%d", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        while (getchar() != '\n');
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    } 
    else
    { 
        printf("%s", "Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

        client.acctNum = accountNum;
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        logTransaction("CREATE", client.acctNum, client.balance);
    } 
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; 
    printf("%s", "\nEnter your choice\n"
                 "1 - Store a formatted text file of accounts (accounts.txt)\n"
                 "2 - Update an account balance\n"
                 "3 - Add a new account\n"
                 "4 - Delete an account\n"
                 "5 - List all accounts on screen\n"
                 "6 - End program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        while (getchar() != '\n'); // clear buffer on invalid input
        return 0; // return dummy choice to trigger default case
    }
    return menuChoice;
} // end function enterChoice

// Authenticate user with a simple password
int authenticate(void)
{
    char password[20];
    const char *correctPassword = "admin";
    printf("Enter Security Password: ");
    scanf("%19s", password);
    return (strcmp(password, correctPassword) == 0);
}

// Initialize file with 100 blank records
void initializeFile(const char *filename)
{
    FILE *fPtr = fopen(filename, "wb");
    if (fPtr == NULL)
    {
        puts("Error creating data file.");
        exit(1);
    }
    struct clientData blankClient = {0, "", "", 0.0};
    for (int i = 0; i < 100; ++i)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
    fclose(fPtr);
}

// Log transactions to audit_trail.txt
void logTransaction(const char *action, unsigned int acct, double amount)
{
    FILE *logPtr = fopen("audit_trail.txt", "a");
    if (logPtr != NULL)
    {
        fprintf(logPtr, "Action: %-10s | Acct: %-5u | Value: %10.2f\n", action, acct, amount);
        fclose(logPtr);
    }
}

// List all active records to the screen
void listRecords(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    rewind(fPtr);
    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        }
    }
}