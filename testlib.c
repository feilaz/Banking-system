#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAX_NAME_LEN 10
#define MAX_SURNAME_LEN 20
#define MAX_ADDRESS_LEN 30
#define PESEL_LEN 11
#define MAX_BALANCE_LEN 10
#define MAX_NUMBER_LEN 10
#define DATABASE_LOCATION "database.txt"
#define COPY_FILE_LOCATION "copy_file.txt"
#define MAX_TRANSACTION_VAlUE 10000000

FILE* fptr;
FILE* tfptr;

enum input_type{text, number, text_and_number};
enum criteria{account_number, name, surname, address, pesel, balance};
enum operation_type{withdrawal = -1, deposit = 1};
enum file_type{main_file, copy_file};

struct account{
    char account_number[MAX_NUMBER_LEN + 1];
    char name[MAX_NAME_LEN + 1];
    char surname[MAX_SURNAME_LEN + 1];
    char address[MAX_ADDRESS_LEN + 1];
    char pesel[PESEL_LEN + 1];
    long double balance;
};


int input(int lower_border, int upper_border);
int main_screen();
int search_function(enum criteria criteria, struct account* found_account, bool print);
char* safe_input(int min_len, int max_len, enum input_type input_type, char* input);
long double safe_balance_input();
void create_new_account();
void clean_buffer();
void list_all_accounts();
void search_for_an_account();
void clear_database_content(enum file_type type);
void rewrite_copied_database();
void rewrite(struct account modified_account);
void input_with_type_selection(struct account* account, enum criteria criteria);
void balance_change(enum operation_type operation_type);
void print_deposit_menu();
void print_withdrawal_menu();
void transfer();
void print_transfer_menu();
void find_next_free_account_number(struct account* found_account);
void copy_acocunt(struct account* from_account, struct account* to_account);
void print_account(struct account* account);
void open_file(FILE* t_ptr, enum file_type type, char mode[3]);
void close_file(FILE* t_ptr);
bool account_creation_confirmation(struct account account);
bool balance_change_confirmation(enum operation_type operation_type, struct account account, long double balance_change);
bool transfer_confirmation(struct account from_account, struct account to_account, long double balance_change);
bool is_cryteria_the_same(struct account* found_account, struct account* temp_data, enum criteria criteria);
bool is_deposit_possible(long double balance, long double deposit);
bool is_withdrawal_possible(long double balance, long double withdrawal);
bool is_transfer_possible(long double from_balance, long double to_balance, long double withdrawal);
bool letter_processing(char *letter, int i);
bool is_number(char letter);
bool terminal_sign_processing(char *letter, int min_len, int i);
bool space_processing(char* letter, bool* space_encountered, int *i);
bool terminal_sign_processing_for_balance(char* input, bool minus_encountered, long double *balance);
bool dot_sign_processing(int i, bool dot_encountered, bool minus_encountered);
void write_into_database(struct account account, enum file_type);
void print_address(char address[MAX_ADDRESS_LEN + 1]);

void clean_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int input(int lower_border, int upper_border) {
    int choice;
    if (lower_border < 0 || lower_border > 9 || upper_border > 9 || upper_border < 0){
        abort();
    }
    while (1) {
        choice = getchar();
        clean_buffer();
        if (choice >= '0' + lower_border && choice <= '0' + upper_border) {
            break;
        } else {
            printf("Wrong input, try again\n");
        }
    }
    return (int)choice - '0';
}

int main_screen() {
    printf("----------------------------------------------\n");
    printf("|            Banking system menu             |\n");
    printf("----------------------------------------------\n");
    printf("1. Create a new account\n");
    printf("2. List all accounts present in the database\n");
    printf("3. Search for an account\n");
    printf("4. Make a money transfer\n");
    printf("5. Make a deposit\n");
    printf("6. Make a withdrawal\n");
    printf("7. Exit\n");
    printf("\n");
    printf("Please input operation number: \n");
    return input(1, 7);
}

void print_deposit_menu(){
    printf("----------------------------------------------\n");
    printf("|                Deposit menu                |\n");
    printf("----------------------------------------------\n");
}

void print_withdrawal_menu(){
    printf("----------------------------------------------\n");
    printf("|              Withdrawal menu               |\n");
    printf("----------------------------------------------\n");
}

void print_transfer_menu(){
    printf("----------------------------------------------\n");
    printf("|               Transfer menu                |\n");
    printf("----------------------------------------------\n");
}

void create_new_account() {
    struct account account;
    system("clear");
    printf("----------------------------------------------\n");
    printf("|            Create a new account            |\n");
    printf("----------------------------------------------\n");
    find_next_free_account_number(&account);
    printf("Input the name: ");
    input_with_type_selection(&account, name);
    printf("\nInput the surname: ");
    input_with_type_selection(&account, surname);
    printf("\nInput the address: ");
    input_with_type_selection(&account, address);
    printf("\nInput the PESEL: ");
    input_with_type_selection(&account, pesel);
    printf("\nInput the account balance: ");
    input_with_type_selection(&account, balance);
    printf("\n");
    if (account_creation_confirmation(account)) {
        fseek ( fptr , 0 , SEEK_END);
        write_into_database(account, main_file);
    }
}

void write_into_database(struct account account, enum file_type file_type){
    if (file_type == main_file){
        if (fprintf(fptr, "%s %s %s %s %s %.2Lf\n", account.account_number ,account.name, account.surname, account.address, account.pesel, account.balance) < 0) {
            perror("Error writing data to file");
            exit(2);
        }
    }
    else {
        if (fprintf(tfptr, "%s %s %s %s %s %.2Lf\n", account.account_number, account.name, account.surname,
                    account.address, account.pesel, account.balance) < 0) {
            perror("Error writing data to file");
            exit(2);
        }
    }
}

void search_for_an_account(){
    enum criteria criteria;
    struct account temp_for_searching;
    printf("----------------------------------------------\n");
    printf("|               Searching menu               |\n");
    printf("----------------------------------------------\n");
    printf("1. Search for account number\n");
    printf("2. Search for name\n");
    printf("3. Search for surname\n");
    printf("4. Search for address\n");
    printf("5. Search for pesel\n");
    printf("6. Search for balance\n");
    printf("Please input operation number: \n\n");
    criteria = input(1, 6) - 1;
    printf("Input search criteria:");
    input_with_type_selection(&temp_for_searching, criteria);
    printf("Accounts found: \n\n");
    printf("INDEX NAME SURNAME ADDRESS PESEL BALANCE\n");
    search_function(criteria, &temp_for_searching, true);
    printf("\nInput any key to continue\n");
    clean_buffer();
}

void list_all_accounts(){
    struct account account;
    rewind(fptr);
    printf("----------------------------------------------\n");
    printf("|              Existing accounts             |\n");
    printf("----------------------------------------------\n\n");
    printf("ACCOUNT_NUMBER NAME SURNAME ADDRESS PESEL BALANCE\n\n");
    while(fscanf(fptr, "%s %s %s %s %s %Lf", account.account_number ,account.name, account.surname, account.address, account.pesel, &account.balance) != EOF) {
        print_account(&account);
    }
    printf("\nPress any button to continue\n");
    clean_buffer();
}

void print_account(struct account* account){
    printf("%s %s %s ", account->account_number, account->name, account->surname);
    print_address(account->address);
    printf(" %s %Lf\n", account->pesel, account->balance);
}

void print_address(char address[MAX_ADDRESS_LEN + 1]){
	int i = 0;
	while (address[i] != '\0'){
			if (address[i] == '_'){
				printf(" ");
				i++;
			}
			printf("%c", address[i]);
			i++;
	}
}

bool account_creation_confirmation(struct account account){
    printf("Please confirm creation of an account:\n");
    printf("Account number: %s\n", account.account_number);
    printf("Name: %s\n", account.name);
    printf("Surname: %s\n", account.surname);
    printf("Address: %s\n",account.address);
    printf("Pesel: %s\n",account.pesel);
    printf("Balance: %.2Lf\n", account.balance);
    printf("Input '1' to accept or '0' to dismiss\n");
    if (input(0, 1)){
        return true;
    }
    return false;
}

bool is_withdrawal_possible(long double balance, long double withdrawal){
    if(balance - withdrawal >= 0 && withdrawal <= MAX_TRANSACTION_VAlUE){
        return true;
    }
    return false;
}

bool is_deposit_possible(long double balance, long double deposit){
    long double max_balance = 9;
    for(int i = 0; i < MAX_BALANCE_LEN; i++){
        max_balance *= 10;
        max_balance += 9;
    }
    if (balance + deposit <= max_balance && deposit <= MAX_TRANSACTION_VAlUE){
        return true;
    }
    return false;
}

bool is_transfer_possible(long double from_balance, long double to_balance, long double withdrawal){
    if(is_deposit_possible(to_balance, withdrawal) && is_withdrawal_possible(from_balance, withdrawal)){
        return true;
    }
    return false;
}

bool balance_change_confirmation(enum operation_type operation_type, struct account account, long double balance_change){
    system("clear");
    if (operation_type == deposit){
        print_deposit_menu();
        printf("Do You confirm deposit of %.2Lf from account:\n%s %s %s %s %s %.2Lf\n\n", balance_change,account.account_number, account.name, account.surname, account.address, account.pesel, account.balance);
    }
    else{
        print_withdrawal_menu();
        printf("Do You confirm withdrawal of %.2Lf from account:\n%s %s %s %s %s %.2Lf\n\n", balance_change,account.account_number, account.name, account.surname, account.address, account.pesel, account.balance);
    }
    printf("input '1' to accept or '0' to dismiss\n");
    if (input(0, 1)){
        return true;
    }
    return false;
}

bool transfer_confirmation(struct account from_account, struct account to_account, long double balance_change){
    system("clear");
    print_transfer_menu();
    printf("Do You confirm transfer of %.2Lf from account:\n%s %s %s %s %s %.2Lf\nto account:\n%s %s %s %s %s %.2Lf\n", balance_change,from_account.account_number, from_account.name, from_account.surname, from_account.address, from_account.pesel, from_account.balance, to_account.account_number, to_account.name, to_account.surname, to_account.address, to_account.pesel, to_account.balance);
    printf("input '1' to accept or '0' to dismiss\n");
    if (input(0, 1)){
        return true;
    }
    return false;
}

bool space_processing(char* letter, bool* space_encountered, int *i){
    if(i == 0 || *space_encountered == true){
        return false;
    }
    *space_encountered = true;
    *letter = '_';
    (*i)++;
    return true;
}

bool terminal_sign_processing(char *letter, int min_len, int i){
    if(i <= min_len - 1){
    }
    else{
        if (*letter == '\n'){
            *letter = '\0';
        }
        return true;
    }
    return false;
}

bool is_number(char letter){
    if (letter >= '0' && letter <= '9'){
        return true;
    }
    return false;
}

bool letter_processing(char *letter, int i){
    if (!((*letter >= 'a' && *letter <= 'z') || (*letter >= 'A' && *letter <= 'Z'))){
        return false;
    }
    else{
        if (i == 0){
            *letter = toupper(*letter);
        }
        else{
            *letter = tolower(*letter);
        }
    }
    return true;
}

bool terminal_sign_processing_for_balance(char* input, bool minus_encountered, long double *balance) {
    *balance = strtof(input, NULL);
    if (*balance == 0 && !(input[0] == '0' || (minus_encountered && input[1] == '0'))) {
    } else {
        return true;
    }
    return false;
}

bool dot_sign_processing(int i, bool dot_encountered, bool minus_encountered){
    if(i == 0 || dot_encountered == true || (i == 1 && minus_encountered)){
        return false;
    }
    return true;
}

bool is_cryteria_the_same(struct account* found_account, struct account* temp_data, enum criteria criteria){
    switch (criteria + 1) {
        case 1:
            if (!strcmp(found_account->account_number, temp_data->account_number)) {
                return true;
            }
            break;
        case 2:
            if (!strcmp(found_account->name, temp_data->name)) {
                return true;
            }
            break;
        case 3:
            if (!strcmp(found_account->surname, temp_data->surname)) {
                return true;
            }
            break;
        case 4:
            if (!strcmp(found_account->address, temp_data->address)) {
                return true;
            }
            break;
        case 5:
            if (!strcmp(found_account->pesel, temp_data->pesel)) {
                return true;
            }
            break;
        case 6:
            if (found_account->balance == temp_data->balance) {
                return true;
            }
            break;
    }
    return false;
}

char* safe_input(int min_len, int max_len, enum input_type input_type, char* input){
    bool running = true;
    bool space_encountered = false;
    if (min_len > max_len){
        abort();
    }
    while(running) {
        fgets(input, max_len + 2, stdin);
        for (int i = 0; i <= max_len; i++) {
            if (input[i] == '\0' || input[i] == '\n'){
                if(terminal_sign_processing(&input[i], min_len, i)){
                    running = false;
                }
                break;
            }
            if (input[i] == ' ' && input_type == text_and_number){
                if(!space_processing(&input[i], &space_encountered, &i)) {
                    break;
                }
            }
                if (input_type == text || (input_type == text_and_number && !space_encountered)) {
                    if(!letter_processing(&input[i], i)){
                        break;
                    }
                }
                else{
                    if (!is_number(input[i])){
                        break;
                    }
                }
        }
        if (strlen(input) > max_len){
            clean_buffer();
            running = true;
        }
        if (running){
            printf("\nWrong input, try again: ");
            space_encountered = false;
        }
    }
    return input;
}

long double safe_balance_input(){
    long double balance;
    bool running = true;
    bool minus_encountered = false;
    bool dot_encountered = false;
    int numbers_after_dot = 0;
    char input[MAX_BALANCE_LEN + 5];
    while(running) {
        fgets(input, MAX_BALANCE_LEN + 5 , stdin);
        if (input[0] == '-'){
            minus_encountered = true;
        }
        for (int i = 0 + minus_encountered; i <= MAX_BALANCE_LEN + minus_encountered + dot_encountered; i++) {
            if (input[i] == '\0' || input[i] == '\n'){
                if(terminal_sign_processing_for_balance(input, minus_encountered, &balance)){
                    running = false;
                }
                break;
            }
            else if (input[i] == '.'){
                if(dot_sign_processing(i, dot_encountered, minus_encountered)){
                    dot_encountered = true;
                    i++;
                }
                else{
                    break;
                }
            }
            if (!is_number(input[i])){
                break;
            }
            else if(dot_encountered){
                if (++numbers_after_dot > 2){
                    break;
                }
            }
        }
        if (strlen(input) > MAX_BALANCE_LEN + minus_encountered + dot_encountered + 2){
            clean_buffer();
            running = true;
        }
        if (running){
            printf("\nWrong input, try again: ");
            minus_encountered = false;
            dot_encountered = false;
            numbers_after_dot = 0;
        }
    }
    return balance;
}


void find_next_free_account_number(struct account* found_account){
    found_account->account_number[0] = '1';
    found_account->account_number[1] = '\0';
    int shift = 0;
    while(search_function(account_number, found_account, false)){
        while(found_account->account_number[strlen(found_account->account_number) - 1 - shift] == '9'){
            found_account->account_number[strlen(found_account->account_number) - 1 - shift] = '0';
            shift++;
            if(shift > strlen(found_account->account_number) - 1){
                printf("\nNo more places in database\n");
                exit(4);
            }
        }
        found_account->account_number[strlen(found_account->account_number) - 1 - shift] += 1;
    }
}


void input_with_type_selection(struct account* account, enum criteria criteria){
    switch(criteria + 1){
        case 1:
            safe_input(1, MAX_NUMBER_LEN, number, account->account_number);
            break;
        case 2:
            safe_input(1, MAX_NAME_LEN, text, account->name);
            break;
        case 3:
            safe_input(1, MAX_SURNAME_LEN, text, account->surname);
            break;
        case 4:
            safe_input(1, MAX_ADDRESS_LEN, text_and_number, account->address);
            break;
        case 5:
            safe_input(PESEL_LEN, PESEL_LEN, number, account->pesel);
            break;
        case 6:
            account->balance = safe_balance_input();
            break;
    }
}

int search_function(enum criteria criteria, struct account* found_account, bool print){
    rewind (fptr);
    int found_accounts = 0;
    bool new_found = false;
    struct account temp_data;
    while (fscanf(fptr, "%s %s %s %s %s %Lf\n", temp_data.account_number, temp_data.name, temp_data.surname, temp_data.address, temp_data.pesel, &temp_data.balance) != EOF) {
        if(is_cryteria_the_same(found_account, &temp_data, criteria)){
            new_found = true;
            found_accounts++;
        }
        if (found_accounts > 0 && new_found) {
            copy_acocunt(&temp_data, found_account);
        }
        if (new_found && print){
            print_account(found_account);
            new_found=false;
        }
    }
    return found_accounts;
}

void balance_change(enum operation_type operation_type){
	struct account modified_account;
    long double balance_change;
    if (operation_type == deposit){
        print_deposit_menu();
    }
    else{
        print_withdrawal_menu();
    }
    printf("Please input the account number\n");
    input_with_type_selection(&modified_account, account_number);
    search_function(account_number, &modified_account, true);
    printf("\nPlease input the transaction value\n");
    while(1) {
        balance_change = safe_balance_input();
        if((operation_type == deposit && is_deposit_possible(modified_account.balance, balance_change)) || (operation_type == withdrawal && is_withdrawal_possible(modified_account.balance, balance_change))){
            break;
        }
        printf("Incorrect transaction value, try again\n");
        }
    if(balance_change_confirmation(operation_type, modified_account, balance_change)){
        modified_account.balance += balance_change * operation_type;
        rewrite(modified_account);
    }
}


void transfer(){
    struct account from_account;
    struct account to_account;
    long double balance_change;
    print_transfer_menu();
    printf("Please input the account from which the money will be taken from\n");
    input_with_type_selection(&from_account, account_number);
    while(search_function(account_number, &from_account, true) != 1){
        printf("Account not found, try again\n");
    }
    printf("\nPlease input the account to which the money will sent\n");
    input_with_type_selection(&to_account, account_number);
    while(search_function(account_number, &to_account, true) != 1){
    printf("Account not found, try again\n");
}
    printf("\nPlease input the value of transfer\n");
    while(1) {
        balance_change = safe_balance_input();
        if(is_transfer_possible(from_account.balance, to_account.balance, balance_change)){
            break;
        }
        printf("Incorrect transfer value, try again\n");
    }
    if(transfer_confirmation(from_account, to_account, balance_change)){
        from_account.balance -= balance_change;
        rewrite(from_account);
        to_account.balance += balance_change;
        rewrite(to_account);
    }
}

void copy_acocunt(struct account* from_account, struct account* to_account){
    strcpy(to_account->account_number, from_account->account_number);
    strcpy(to_account->name, from_account->name);
    strcpy(to_account->surname, from_account->surname);
    strcpy(to_account->address, from_account->address);
    strcpy(to_account->pesel, from_account->pesel);
    to_account->balance = from_account->balance;
}

void rewrite(struct account modified_account){
    rewind (fptr);
    clear_database_content(copy_file);
    rewind(tfptr);
    struct account temp_data;
    while (fscanf(fptr, "%s %s %s %s %s %Lf\n", temp_data.account_number, temp_data.name, temp_data.surname, temp_data.address, temp_data.pesel, &temp_data.balance) != EOF)
    {
        if (!strcmp(temp_data.account_number, modified_account.account_number)) {
            write_into_database(modified_account, copy_file);
        } else {
            write_into_database(temp_data, copy_file);
        }
    }
    clear_database_content(main_file);
    rewrite_copied_database();
    clear_database_content(copy_file);
}

void rewrite_copied_database() {
    rewind(tfptr);
    struct account temp;
    while (fscanf(tfptr, "%s %s %s %s %s %Lf", temp.account_number, temp.name, temp.surname, temp.address, temp.pesel,
                  &temp.balance) != EOF) {
        write_into_database(temp, main_file);
    }
}

void close_file(FILE* t_ptr){
    if(fclose(t_ptr)){
        perror("Error");
        exit(3);
    }
}

void open_file(FILE* t_ptr, enum file_type type, char mode[3]){
    if(type == main_file){
        t_ptr = fopen(DATABASE_LOCATION, mode);
    }
    else {
        t_ptr = fopen(COPY_FILE_LOCATION, mode);
    }
    if (t_ptr == NULL) {
        printf("Error occurred, while trying to open the file!");
        exit(1);
    }
    if(type == main_file){
        fptr = t_ptr;
    }
    else {
        tfptr = t_ptr;
    }
}

void clear_database_content(enum file_type type){
    FILE* temp_p;
    if(type == main_file){
        temp_p = fptr;
    }
    else{
        temp_p = tfptr;
    }
    close_file(temp_p);
    if(type == main_file){
        open_file(temp_p, main_file, "w+");
    }
    else{
        tfptr = fopen(COPY_FILE_LOCATION, "w+");
    }
}

int main(){
    bool running = true;
    open_file(fptr, main_file, "r+");
    open_file(tfptr, copy_file, "w+");
    while (running) {
        system("clear");
        switch(main_screen()){
            case 1:
            system("clear");
                create_new_account();
                break;
            case 2:
            system("clear");
                list_all_accounts();
                break;
            case 3:
            system("clear");
                search_for_an_account();
                break;
            case 4:
            system("clear");
                transfer();
                break;
            case 5:
            system("clear");
                balance_change(deposit);
                break;
            case 6:
            system("clear");
                balance_change(withdrawal);
                break;
            case 7:
                running = false;
                break;
        }
    }
    close_file(fptr);
    close_file(tfptr);
    return 0;
}
