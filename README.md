# Transaction Processing System  
### Mini Project – 24UCS271 Lab

---
## Tasks to be Performed

1. Compile and run the given executable code.
2. Perform your own testing of the program.
3. Add a new account detail and regenerate the `accounts.txt` file.
   - Inspect the contents of the file.
   - Verify whether the output matches your expectations.
4. Identify and fix any logical errors in the program.
5. Add new functionality  
   - Example: Listing all account information.
6. Optimize the program for better performance and efficiency.

---
## Issues Fixed:

# Critical issues fixed:
1. File Handle Crash (Line 27): Fixed the  crash when "credit.dat" is missing by adding an initializeFile function that creates a new data file with 100 blank records if it doesn't exist.
2. Buffer & Bounds Protection (Lines 108, 147, 176): Implemented strict account number validation (must be 1–100) across all functions (updateRecord,deleteRecord,newRecord) to prevent invalid memory access and file pointer manipulation.
# Input Validation:
- Added checks for non-numeric input in the menu to prevent infinite loops.
- Added validation for transaction amounts to ensure data integrity.
# Logical Error Fix: 
- Improved textFile logic by replacing the unreliable feof() check with a direct fread() status check, ensuring accurate exports to "accounts.txt".

## New Features Implemented:

# Authentication Engine: 
- Added a secure entry point that requires a password (admin) before the program can be accessed.
# Transaction Logging (Audit Trail): 
- Created a logTransaction function that appends every CREATE, UPDATE, and DELETE action to audit_trail.txt,providing a persistent history of all activities.
# List Accounts to Screen: 
- Added a new menu option (Option 5) to print all active account details directly to the console for quick reference.

## Performance & Optimization:

# Efficient File Seeking: 
- Optimized fseek operations to correctly position the file pointer before every read/write action.
