%option noyywrap

%{
    #include <stdio.h>
    #include <string.h>
%}

%%

.*amal.*    printf("amal detected: String Rejected\n");
.*          printf("String Accepted");

%%

int main(){
    printf("Enter the string: ");
    yylex();
    return 0;
}