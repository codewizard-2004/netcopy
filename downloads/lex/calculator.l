%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>

int num1, num2;
char op;
%}

%%

[0-9]+ {
    num1 = atoi(yytext);
    printf("Enter operator: ");
    scanf(" %c", &op);

    printf("Enter second number: ");
    scanf("%d", &num2);

    switch(op)
    {
        case '+':
            printf("Result = %d\n", num1 + num2);
            break;

        case '-':
            printf("Result = %d\n", num1 - num2);
            break;

        case '*':
            printf("Result = %d\n", num1 * num2);
            break;

        case '/':
            if(num2 != 0)
                printf("Result = %d\n", num1 / num2);
            else
                printf("Division by zero is not allowed\n");
            break;

        default:
            printf("Invalid operator\n");
    }
}

[ \t\n]+    ;

. {
    printf("Invalid input\n");
}

%%

int main()
{
    printf("Enter first number: ");
    yylex();

    return 0;
}