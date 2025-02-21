#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 100

// Token types
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_SYMBOL,
    TOKEN_STRING,
    TOKEN_UNKNOWN
} TokenType;

// Structure to store token details
typedef struct {
    char lexeme[MAX_TOKEN_LENGTH];
    int row, col;
    TokenType type;
} Token;

// List of C keywords
const char *keywords[] = {
    "int", "float", "char", "double", "if", "else", "while", "for",
    "return", "void", "switch", "case", "break", "continue", "struct", "typedef"
};
#define KEYWORD_COUNT (sizeof(keywords) / sizeof(keywords[0]))

// Function to check if a string is a keyword
int isKeyword(const char *str) {
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// Function to get the next token
Token getNextToken(FILE *fp, int *row, int *col) {
    Token token;
    char ch;
    int index = 0;
    token.lexeme[0] = '\0';

    while ((ch = fgetc(fp)) != EOF) {
        (*col)++;

        // Skip whitespace and update row & col
        if (ch == ' ' || ch == '\t') continue;
        if (ch == '\n') { (*row)++; *col = 0; continue; }

        // Ignore preprocessor directives
        if (ch == '#') {
            while ((ch = fgetc(fp)) != EOF && ch != '\n');  
            (*row)++; *col = 0;
            continue;
        }

        // Handle single-line comments
        if (ch == '/' && (ch = fgetc(fp)) == '/') {
            while ((ch = fgetc(fp)) != EOF && ch != '\n');  
            (*row)++; *col = 0;
            continue;
        }

        // Handle multi-line comments
        if (ch == '/' && (ch = fgetc(fp)) == '*') {
            while ((ch = fgetc(fp)) != EOF) {
                if (ch == '*' && (ch = fgetc(fp)) == '/') break;
                if (ch == '\n') (*row)++;
            }
            continue;
        }

        // Handle string literals
        if (ch == '"') {
            while ((ch = fgetc(fp)) != EOF && ch != '"') {
                if (ch == '\n') (*row)++;
            }
            continue;
        }

        // Handle numbers
        if (isdigit(ch)) {
            token.type = TOKEN_NUMBER;
            token.lexeme[index++] = ch;
            while (isdigit(ch = fgetc(fp))) {
                token.lexeme[index++] = ch;
                (*col)++;
            }
            ungetc(ch, fp);
        }
        // Handle identifiers & keywords
        else if (isalpha(ch) || ch == '_') {
            token.type = TOKEN_IDENTIFIER;
            token.lexeme[index++] = ch;
            while (isalnum(ch = fgetc(fp)) || ch == '_') {
                token.lexeme[index++] = ch;
                (*col)++;
            }
            ungetc(ch, fp);

            // Check if it is a keyword
            if (isKeyword(token.lexeme))
                token.type = TOKEN_KEYWORD;
        }
        // Handle operators
        else if (strchr("+-*/%=!&|<>", ch)) {
            token.type = TOKEN_OPERATOR;
            token.lexeme[index++] = ch;
            char nextChar = fgetc(fp);
            if (strchr("=|&<>", nextChar)) {
                token.lexeme[index++] = nextChar;
                (*col)++;
            } else {
                ungetc(nextChar, fp);
            }
        }
        // Handle symbols (punctuation)
        else if (strchr("(){}[],;:", ch)) {
            token.type = TOKEN_SYMBOL;
            token.lexeme[index++] = ch;
        }
        // Unknown token
        else {
            token.type = TOKEN_UNKNOWN;
            token.lexeme[index++] = ch;
        }

        // Null-terminate the lexeme
        token.lexeme[index] = '\0';
        token.row = *row;
        token.col = *col - strlen(token.lexeme) + 1;

        return token;
    }

    // If EOF is reached
    token.type = TOKEN_UNKNOWN;
    return token;
}

// Function to print token details
void printToken(Token token) {
    if (token.type == TOKEN_UNKNOWN) return;

    printf("Token: %-15s Type: ", token.lexeme);
    switch (token.type) {
        case TOKEN_KEYWORD: printf("Keyword"); break;
        case TOKEN_IDENTIFIER: printf("Identifier"); break;
        case TOKEN_NUMBER: printf("Number"); break;
        case TOKEN_OPERATOR: printf("Operator"); break;
        case TOKEN_SYMBOL: printf("Symbol"); break;
        default: printf("Unknown");
    }
    printf(" (Row: %d, Col: %d)\n", token.row, token.col);
}

// Main function
int main() {
    FILE *fp = fopen("sample.c", "r");
    if (!fp) {
        printf("Error opening file!\n");
        return 1;
    }

    Token token;
    int row = 1, col = 0;

    printf("Lexical Analysis:\n");
    while ((token = getNextToken(fp, &row, &col)).type != TOKEN_UNKNOWN) {
        printToken(token);
    }

    fclose(fp);
    return 0;
}
