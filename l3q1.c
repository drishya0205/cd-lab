#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

int isKeyword(char *word) {
    char *keywords[] = {
        "int", "float", "char", "double", "long", "short", "if", "else", "while",
        "for", "return", "switch", "case", "break", "continue", "void", "do", "default",
        "sizeof", "struct", "typedef", "static", "extern", "const", "signed", "unsigned"
    };
    int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

    for(int i = 0; i < num_keywords; i++) {
        if(strcmp(word, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

int isSpecialSymbol(char c) {
    char symbols[] = {';', '(', ')', '{', '}', '[', ']', ',', '.'};
    for(int i = 0; i < sizeof(symbols) / sizeof(symbols[0]); i++) {
        if(c == symbols[i])
            return 1;
    }
    return 0;
}

int isNumericConstant(char *word) {
    int dot_count = 0;
    for(int i = 0; word[i] != '\0'; i++) {
        if(isdigit(word[i])) continue;
        if(word[i] == '.' && dot_count == 0) {
            dot_count++;
            continue;
        }
        return 0;
    }
    return 1;
}

int isIdentifier(char *word) {
    if (!isalpha(word[0]) && word[0] != '_') return 0;
    for(int i = 1; word[i] != '\0'; i++) {
        if (!isalnum(word[i]) && word[i] != '_') return 0;
    }
    return 1;
}

void checkOperator(char c, FILE *fp) {
    char next = fgetc(fp);

    if(c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
        printf("\nArithmetic operator: %c", c);
    } else if(c == '=' && next == '=') {
        printf("\nRelational operator: ==");
    } else if(c == '!' && next == '=') {
        printf("\nRelational operator: !=");
    } else if(c == '<' && next == '=') {
        printf("\nRelational operator: <=");
    } else if(c == '>' && next == '=') {
        printf("\nRelational operator: >=");
    } else if(c == '<') {
        printf("\nRelational operator: <");
        ungetc(next, fp);
    } else if(c == '>') {
        printf("\nRelational operator: >");
        ungetc(next, fp);
    } else if(c == '&' && next == '&') {
        printf("\nLogical operator: &&");
    } else if(c == '|' && next == '|') {
        printf("\nLogical operator: ||");
    } else if(c == '!') {
        printf("\nLogical operator: !");
        ungetc(next, fp);
    } else {
        ungetc(next, fp);
    }
}

int main() {
    char c, buf[10], word[50];
    int index = 0;
    FILE *fp = fopen("sample.c", "r");
    
    if(fp == NULL) {
        printf("Cannot open file\n");
        exit(0);
    }

    while((c = fgetc(fp)) != EOF) {
        if(isalnum(c) || c == '_') {
            word[index++] = c;
        } else {
            if(index > 0) {
                word[index] = '\0';
                index = 0;
                
                if(isKeyword(word))
                    printf("\nKeyword: %s", word);
                else if(isNumericConstant(word))
                    printf("\nNumeric constant: %s", word);
                else if(isIdentifier(word))
                    printf("\nIdentifier: %s", word);
            }

            if(c == '"') {
                printf("\nString literal: \"");
                while((c = fgetc(fp)) != '"' && c != EOF) {
                    printf("%c", c);
                }
                printf("\"");
            } else if(isSpecialSymbol(c)) {
                printf("\nSpecial symbol: %c", c);
            } else {
                checkOperator(c, fp);
            }
        }
    }

    fclose(fp);
    return 0;
}
