//this code is made for a sample ruby code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILE_SIZE 10000
#define MAX_SYMBOLS 500

typedef enum {
    KEYWORD,
    IDENTIFIER,
    NUMERIC_CONSTANT,
    SPECIAL_SYMBOL,
    OPERATOR,
    STRING_LITERAL
} tokenTypes;

typedef struct {
    char name[50];
    tokenTypes type;
    int row, col;
} SymbolTableEntry;

SymbolTableEntry symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
int row = 1, col = 1;

const char *keywords[] = {"def", "for", "in", "return", "end", "puts"};
const char *operators[] = {"=>", "+=", "..", "="};

// Function to check if a token is a keyword
int isKeyword(const char *str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

// Function to check if a token is an operator
int isOperator(const char *str) {
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
        if (strcmp(str, operators[i]) == 0) return 1;
    }
    return 0;
}

// Function to add token to symbol table
void addToSymbolTable(const char *token, tokenTypes type, int row, int col) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, token) == 0) return;
    }
    if (symbolCount < MAX_SYMBOLS) {
        strncpy(symbolTable[symbolCount].name, token, sizeof(symbolTable[symbolCount].name) - 1);
        symbolTable[symbolCount].type = type;
        symbolTable[symbolCount].row = row;
        symbolTable[symbolCount].col = col;
        symbolCount++;
    }
}

// Tokenizer function
void getNextToken(FILE *fp) {
    char buffer[MAX_FILE_SIZE];
    size_t bytesRead = fread(buffer, 1, MAX_FILE_SIZE - 1, fp);
    buffer[bytesRead] = '\0'; // Null-terminate the buffer

    char *token = strtok(buffer, " \t\n{};(),[]");

    while (token) {
        int tokenRow = row, tokenCol = col;

        // Handle new lines
        if (strchr(token, '\n')) {
            row++;
            col = 1;
        } else {
            col += strlen(token) + 1;
        }

        // Ignore comments
        if (strncmp(token, "#", 1) == 0) {
            token = strtok(NULL, "\n");
            continue;
        }

        // Check for keywords
        if (isKeyword(token)) {
            printf("<KEYWORD : %s>\n", token);
            addToSymbolTable(token, KEYWORD, tokenRow, tokenCol);
        }
        // Check for operators
        else if (isOperator(token)) {
            printf("<OPERATOR : %s>\n", token);
            addToSymbolTable(token, OPERATOR, tokenRow, tokenCol);
        }
        // Check for numeric constants
        else if (isdigit(token[0])) {
            printf("<NUMERIC_CONSTANT : %s>\n", token);
            addToSymbolTable(token, NUMERIC_CONSTANT, tokenRow, tokenCol);
        }
        // Check for string literals
        else if (token[0] == '"' || (token[0] == 'f' && token[1] == '"')) {
            printf("<STRING_LITERAL : %s>\n", token);
            addToSymbolTable(token, STRING_LITERAL, tokenRow, tokenCol);
        }
        // Check for identifiers
        else if (isalpha(token[0]) || token[0] == '_') {
            printf("<IDENTIFIER : %s>\n", token);
            addToSymbolTable(token, IDENTIFIER, tokenRow, tokenCol);
        }
        // Check for special symbols
        else if (strchr(":()[],{}", token[0])) {
            printf("<SPECIAL_SYMBOL: %s>\n", token);
            addToSymbolTable(token, SPECIAL_SYMBOL, tokenRow, tokenCol);
        }

        token = strtok(NULL, " \t\n():,[]{}");
    }
}

// Print the symbol table
void printSymbolTable() {
    printf("\n===== SYMBOL TABLE =====\n");
    printf("Index\tToken\t\tType\t\tRow\tCol\n");
    printf("-------------------------------------------------\n");
    for (int i = 0; i < symbolCount; i++) {
        const char *typeName;
        switch (symbolTable[i].type) {
            case KEYWORD: typeName = "KEYWORD"; break;
            case IDENTIFIER: typeName = "IDENTIFIER"; break;
            case NUMERIC_CONSTANT: typeName = "NUMERIC_CONSTANT"; break;
            case SPECIAL_SYMBOL: typeName = "SPECIAL_SYMBOL"; break;
            case OPERATOR: typeName = "OPERATOR"; break;
            case STRING_LITERAL: typeName = "STRING_LITERAL"; break;
            default: typeName = "UNKNOWN"; break;
        }
        printf("%d\t%s\t\t%s\t\t%d\t%d\n", i, symbolTable[i].name, typeName, symbolTable[i].row, symbolTable[i].col);
    }
}

int main() {
    FILE *fp = fopen("sample.rb", "r");
    if (!fp) {
        printf("Error opening file.\n");
        return 1;
    }

    getNextToken(fp);
    fclose(fp);
    printSymbolTable();
    return 0;
}
