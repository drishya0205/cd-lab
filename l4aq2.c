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
    tokenTypes type;
    int rowNum;
    int colNum;
} SymbolTableEntry;

SymbolTableEntry symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

const char *keywords[] = {
    "def", "do", "end", "if", "else", "elsif", "while", "return", "puts", "each"
};

int isKeyword(const char *str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

void addToSymbolTable(const char *token, tokenTypes type, int rowNum, int colNum) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, token) == 0) return;
    }
    if (symbolCount < MAX_SYMBOLS) {
        strncpy(symbolTable[symbolCount].name, token, sizeof(symbolTable[symbolCount].name) - 1);
        symbolTable[symbolCount].name[sizeof(symbolTable[symbolCount].name) - 1] = '\0';
        symbolTable[symbolCount].type = type;
        symbolTable[symbolCount].rowNum = rowNum;
        symbolTable[symbolCount].colNum = colNum;
        symbolCount++;
    }
}

void getNextToken(FILE *fp) {
    char bufferFile[MAX_FILE_SIZE];
    size_t bytesRead = fread(bufferFile, 1, MAX_FILE_SIZE - 1, fp);
    bufferFile[bytesRead] = '\0';

    char *token = strtok(bufferFile, " \t\n{};(),[]");
    int rowNum = 1, colNum = 1;

    while (token) {
        int startCol = colNum;
        if (strncmp(token, "#!", 2) == 0) {
            token = strtok(NULL, "\n");
            rowNum++;
        } else if (token[0] == '#') {
            token = strtok(NULL, "\n");
            rowNum++;
        } else if (isKeyword(token)) {
            printf("<KEYWORD : %s>\n", token);
            addToSymbolTable(token, KEYWORD, rowNum, startCol);
        } else if (isalpha(token[0]) || token[0] == '_') {
            printf("<IDENTIFIER : %s>\n", token);
            addToSymbolTable(token, IDENTIFIER, rowNum, startCol);
        } else if (strchr(":()[],{}", token[0])) {
            printf("<SPECIAL_SYMBOL : %s>\n", token);
            addToSymbolTable(token, SPECIAL_SYMBOL, rowNum, startCol);
        } else if (strcmp(token, "=") == 0 || strcmp(token, "+=") == 0) {
            printf("<OPERATOR : %s>\n", token);
            addToSymbolTable(token, OPERATOR, rowNum, startCol);
        } else if (isdigit(token[0])) {
            printf("<NUMERIC_CONSTANT : %s>\n", token);
            addToSymbolTable(token, NUMERIC_CONSTANT, rowNum, startCol);
        } else if (token[0] == '"' || token[0] == '\'') {
            printf("<STRING_LITERAL : %s>\n", token);
            addToSymbolTable(token, STRING_LITERAL, rowNum, startCol);
        }

        colNum += strlen(token) + 1;
        if (strchr(token, '\n')) {
            rowNum++;
            colNum = 1;
        }

        token = strtok(NULL, " \t\n():,[]{}");
    }
}

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
        printf("%d\t%s\t\t%s\t\t%d\t%d\n", i, symbolTable[i].name, typeName, symbolTable[i].rowNum, symbolTable[i].colNum);
    }
}

int main() {
    FILE *fp = fopen("script.rb", "r");
    if (!fp) {
        printf("Error opening file.\n");
        return 1;
    }

    getNextToken(fp);
    fclose(fp);
    printSymbolTable();
    return 0;
}
