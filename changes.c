New features added
1
List all accounts — list_accounts()
A new menu option (5) that rewinds the file and loops through all 100 records using fread(...) == 1. It prints only slots where acctNum != 0, and shows "no accounts found" when the file is empty. This is the most basic viewing feature and essential for demos.
2
Search and display single account — display_account()
A new menu option (6) that asks for an account number, uses fseek to jump directly to that record, reads it, and displays it with a formatted header. If acctNum == 0, it reports no information. Demonstrates correct random-access file indexing without modifying any data.
3
ATM-style debit transaction — debit_transaction()
A new menu option (7) that asks for an account number and a positive withdrawal amount. It checks two conditions before writing: the account must exist (acctNum != 0), and the balance after deduction must not go below zero. If either check fails, the transaction is denied and the file is left unchanged. Matches the faculty-specified "ATM debit" innovation requirement.
4
Sorted text file export — write_sorted_text_file()
A new menu option (8) that loads all active records into a local array (up to 100), sorts them alphabetically by last name then first name using qsort(), and writes the result to accounts_sorted.txt. The comparator qsort_compare_client_data() is a file-scope function (not nested) to ensure compatibility with Windows and standard C compilers. Satisfies the "sorting of records" rubric requirement.
File handling improvements
5
Auto-create file on first run — open_data_file()
The original code called fopen("credit.dat", "rb+") and crashed if the file didn't exist. The new open_data_file() first tries rb+, and if that fails it creates the file with wb+ and immediately calls initialize_file(). This means the program works on the very first run without any manual setup.
6
Pre-fill 100 blank records — initialize_file()
When a new file is created, this function writes 100 records with acctNum = 0, empty name strings, and balance = 0.0. Without this step, random reads into an empty file would return garbage or fail silently. Pre-filling ensures all 100 account slots physically exist and the sentinel value acctNum == 0 reliably means "empty".
Bug fixes
7
Removed the feof() bug in text_file()
The original loop was while (!feof(readPtr)) { fread(...); }. This is a classic C mistake — feof() only becomes true after a read attempts to go past the end of file, so the loop always runs one extra time, potentially printing a duplicate or garbage last record. Fixed to while (fread(...) == 1) { ... }, which only continues when a record was actually read.
8
Corrected scanf format: %d → %u for unsigned int
The original code used scanf("%d", &accountNum) where accountNum is declared as unsigned int. Using %d with an unsigned int* is undefined behavior in C — it may appear to work but is not guaranteed. All account number and menu choice reads now correctly use %u.
Input safety & validation
9
Input line flusher — clear_input_line()
When a user types letters where a number is expected, scanf fails and leaves the bad characters sitting in the input buffer. Every subsequent scanf then reads those leftovers and fails again, causing an infinite loop. clear_input_line() reads and discards all characters up to \n or EOF, clearing the buffer so the next prompt works correctly.
10
Validated account number input — get_account_number()
Replaces all raw scanf("%d", &account) calls scattered across the program. Loops with a re-prompt until the user enters a valid integer between 1 and 100. Prevents out-of-range values from generating a bad fseek offset that would read or write the wrong part of the file.
11
Validated numeric input — get_double() and get_positive_double()
get_double(prompt) loops until a valid floating-point number is entered. get_positive_double(prompt) additionally rejects zero and negative values — used by the debit transaction to ensure withdrawal amounts are always positive. Both call clear_input_line() after each read.
12
Negative balance guard in update_record()
Before applying a transaction, the code now checks client.balance + transaction < 0.0. If the result would be negative, it prints "Transaction denied: balance cannot go negative." and returns without touching the file. The account is never left in an invalid state.
13
Check return value of every fread()
The original code called fread() and assumed it succeeded. Every fread() call now checks that it returned 1 (exactly one record read). If it returns anything else — due to a corrupt file, disk error, or wrong offset — the function prints "Error reading the record." and returns early, so no uninitialized memory is ever used.
14
Flush writes to disk — fflush() after every fwrite()
fflush(fPtr) is called after every write in new_record, update_record, delete_record, debit_transaction, and initialize_file. Without it, changes sit in the C library's buffer and may not reach the disk immediately. This is especially important during demos, where reading right after writing must show the updated data.
Code quality & compatibility
15
Naming: camelCase → snake_case throughout
All function and variable names were renamed to snake_case to follow standard C conventions: clientData→client_data, enterChoice→enter_choice, textFile→text_file, updateRecord→update_record, newRecord→new_record, deleteRecord→delete_record. Makes the code consistent and easier to read.
16
Magic number 100 → enum { MAX_RECORDS = 100 }
The literal 100 appeared in loops, bounds checks, and prompts across multiple functions. Extracting it into a named constant means you change one line to resize the entire system. All loops, prompts, and array sizes now reference MAX_RECORDS.
17
Proper return values from main()
The original main used exit(-1) on failure and had no explicit return. The new version returns EXIT_FAILURE on file-open error and return 0 on success — both portable, standards-compliant, and compatible with automated test scripts. (void)argc is added to suppress unused-parameter compiler warnings.
18
Windows / strict-C compiler fixes
Variable declarations were moved to the tops of blocks in update_record and new_record to avoid "mixed declarations and code" errors on MSVC and C89 compilers. Reverse seeks now use fseek(fptr, -(long)sizeof(struct client_data), SEEK_CUR) with an explicit signed cast, and the return value is checked — if fseek fails, the function prints an error and returns without writing to the wrong location.