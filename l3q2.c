// this code takes your sample code and generates tokens(hence the function name gettoken), but it also prints out row num and colnum in these neatly described boxes
#include <stdio.h>
#include <stdlib.h>

struct token {
    char lexeme[64];
    int row, col;
    char type[30];
};

char keywords[][10] = {"for", "if", "else", "while", "do", "break", "continue", "return", 
                        "int", "double", "float", "char", "long", "short", "sizeof", "typedef", 
                        "switch", "case", "struct", "const", "void", "exit"};

char specialSymbols[] = {'?', ';', ':', ',', '(', ')', '{', '}', '.'};
char arithmeticSymbols[] = {'+', '-', '*', '/', '%'};

int row = 1, col = 1;

int isKeyword(char *str) {
    for (int i = 0; i < 22; i++) {
        int j = 0;
        while (str[j] == keywords[i][j]) {
            if (str[j] == '\0' && keywords[i][j] == '\0') return 1;
            j++;
        }
    }
    return 0;
}

int belongsTo(char c, char *arr, int len) {
    for (int i = 0; i < len; i++) {
        if (c == arr[i]) return 1;
    }
    return 0;
}

struct token getNextToken(FILE *file) {
    struct token tkn;
    tkn.row = -1; // Default: No token found
    char c;
    
    while ((c = fgetc(file)) != EOF) {
        // Skip whitespace
        if (c == ' ' || c == '\t') {
            col++;
            continue;
        }

        // Handle new lines
        if (c == '\n') {
            row++;
            col = 1;
            continue;
        }

        // Skip single-line comments //
        if (c == '/') {
            char next = fgetc(file);
            if (next == '/') {
                while ((c = fgetc(file)) != EOF && c != '\n');
                row++;
                col = 1;
                continue;
            } else {
                ungetc(next, file);  // Put the character back
            }
        }

        // Handle special symbols
        if (belongsTo(c, specialSymbols, sizeof(specialSymbols))) {
            tkn.row = row;
            tkn.col = col++;
            tkn.lexeme[0] = c;
            tkn.lexeme[1] = '\0';
            return tkn;
        }

        // Handle arithmetic operators
        if (belongsTo(c, arithmeticSymbols, sizeof(arithmeticSymbols))) {
            tkn.row = row;
            tkn.col = col++;
            tkn.lexeme[0] = c;
            tkn.lexeme[1] = '\0';
            return tkn;
        }

        // Handle numbers
        if (c >= '0' && c <= '9') {
            tkn.row = row;
            tkn.col = col;
            int k = 0;
            while (c >= '0' && c <= '9') {
                tkn.lexeme[k++] = c;
                c = fgetc(file);
                col++;
            }
            tkn.lexeme[k] = '\0';
            ungetc(c, file);
            return tkn;
        }

        // Handle identifiers and keywords
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
            tkn.row = row;
            tkn.col = col;
            int k = 0;
            while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_') {
                tkn.lexeme[k++] = c;
                c = fgetc(file);
                col++;
            }
            tkn.lexeme[k] = '\0';
            ungetc(c, file);  // Put back the extra character read
            return tkn;
        }

        // Handle string literals
        if (c == '"') {
            tkn.row = row;
            tkn.col = col;
            int k = 0;
            tkn.lexeme[k++] = c;
            while ((c = fgetc(file)) != '"' && c != EOF) {
                tkn.lexeme[k++] = c;
                col++;
            }
            tkn.lexeme[k++] = '"';
            tkn.lexeme[k] = '\0';
            return tkn;
        }

        col++;
    }

    return tkn; // No token found
}

int main() {
    FILE *file = fopen("test.c", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    struct token tkn;
    while ((tkn = getNextToken(file)).row != -1) {
        printf("<%s, %d, %d>\n", tkn.lexeme, tkn.row, tkn.col);
    }

    fclose(file);
    return 0;
}
