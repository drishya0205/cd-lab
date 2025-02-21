//ok so chatgpt is useless, deepseek ftw
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

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
    tokenTypes type; // whatever category my token falls under
    int row;        // Row number in the source file
    int col;        // Column number in the source file
} SymbolTableEntry;

SymbolTableEntry symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// Check if a string is a Ruby keyword
int isKeyword(const char *str) {
    const char *keywords[] = {
        "def", "end", "if", "else", "elsif", "while", "for", "in", "return", "do", "class", "module", "begin", "rescue", "ensure", "nil", "true", "false"
    };
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

// Add a token to the symbol table
void addToSymbolTable(const char *token, tokenTypes type, int row, int col) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, token) == 0) return;
    }
    if (symbolCount < MAX_SYMBOLS) {
        strncpy(symbolTable[symbolCount].name, token, sizeof(symbolTable[symbolCount].name) - 1);
        symbolTable[symbolCount].name[sizeof(symbolTable[symbolCount].name) - 1] = '\0';
        symbolTable[symbolCount].type = type;
        symbolTable[symbolCount].row = row;
        symbolTable[symbolCount].col = col;
        symbolCount++;
    }
}

// Tokenize the input file
void getNextToken(FILE *fp) {
    char bufferFile[MAX_FILE_SIZE];
    size_t bytesRead = fread(bufferFile, 1, MAX_FILE_SIZE - 1, fp);
    bufferFile[bytesRead] = '\0';

    int row = 1, col = 1;
    char *token = strtok(bufferFile, " \t\n{};(),[]");

    while (token) {
        // Calculate row and column numbers
        int position = token - bufferFile;
        for (int i = 0; i < position; i++) {
            if (bufferFile[i] == '\n') {
                row++;
                col = 1;
            } else {
                col++;
            }
        }

        if (token[0] == '#') {
            // Skip comments
            token = strtok(NULL, "\n");
        } else if (isKeyword(token)) {
            printf("<KEYWORD : %s> (Row: %d, Col: %d)\n", token, row, col);
            addToSymbolTable(token, KEYWORD, row, col);
        } else if (isalpha(token[0]) || token[0] == '_') {
            printf("<IDENTIFIER : %s> (Row: %d, Col: %d)\n", token, row, col);
            addToSymbolTable(token, IDENTIFIER, row, col);
        } else if (strchr(":()[],{}", token[0])) {
            printf("<SPECIAL_SYMBOL: %s> (Row: %d, Col: %d)\n", token, row, col);
            addToSymbolTable(token, SPECIAL_SYMBOL, row, col);
        } else if (strcmp(token, "=") == 0 || strcmp(token, "*=") == 0 || strcmp(token, "==") == 0) {
            printf("<OPERATOR : %s> (Row: %d, Col: %d)\n", token, row, col);
            addToSymbolTable(token, OPERATOR, row, col);
        } else if (isdigit(token[0])) {
            printf("<NUMERIC_CONSTANT : %s> (Row: %d, Col: %d)\n", token, row, col);
            addToSymbolTable(token, NUMERIC_CONSTANT, row, col);
        } else if (token[0] == '"' || (token[0] == '\'' && token[1] != '\0')) {
            printf("<STRING_LITERAL : %s> (Row: %d, Col: %d)\n", token, row, col);
            addToSymbolTable(token, STRING_LITERAL, row, col);
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
