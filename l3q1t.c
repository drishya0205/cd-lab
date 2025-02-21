#include <stdio.h>
#include <stdlib.h>

int main() {
    char c, buf[3];  // Small buffer, max 2 chars needed for operators (e.g., '==')
    FILE *fp = fopen("l3q1t.c", "r");

    if (fp == NULL) {
        printf("Cannot open file\n");
        return 1;
    }

    while ((c = fgetc(fp)) != EOF) {  
        int i = 0;

        // Check for '=' operator
        if (c == '=') { 
            buf[i++] = c;
            c = fgetc(fp);  // Read next character
            if (c == '=') {
                buf[i++] = c;  // It's '=='
                buf[i] = '\0';
                printf("\n Relational operator: %s", buf);
            } else {
                buf[i] = '\0';
                printf("\n Assignment operator: %s", buf);
                ungetc(c, fp);  // Put back character if it's not '='
            }
        }

        // Check for relational operators
        else if (c == '<' || c == '>' || c == '!') {
            buf[i++] = c;
            c = fgetc(fp);
            if (c == '=') {
                buf[i++] = c;  // It's '<=', '>=', or '!='
            } else {
                ungetc(c, fp);  // Put back character if not '='
            }
            buf[i] = '\0';
            printf("\n Relational operator: %s", buf);
        }
    }

    fclose(fp);
    return 0;
}
