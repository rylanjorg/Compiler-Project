%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define YY_DECL int yylex()
#include "parser_with_included.tab.h"

int words = 0;
int chars = 0;
int lines = 0;

%}

LETTER      [a-zA-Z]
DIGIT       [0-9]
OPERATOR	[+\-*/%]
CONDITIONAL_OPERATOR	[<>!]=|==
BOOLEAN_LITERAL (true|false)
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
delim       [ \t\n]
INT_LITERAL {DIGIT}+
FLOAT_LITERAL {DIGIT}+(\.{DIGIT}+)?[fF]
CHAR_LITERAL \'[^\']\'

ws          {delim}+

%%
"/*"    				{
							int c;
							while((c = input()) != 0) {
								if(c == '*') {
									if((c = input()) == '/')
										break;
									else
										unput(c);
								}
							}
						}


"int"			{words++; chars += strlen(yytext);
					printf("%s : TYPE\n", yytext);
					yylval.string = strdup(yytext); 
					return TYPE;
				}

"void" 			{words++; chars += strlen(yytext);
    				printf("%s : VOID\n", yytext);
    				yylval.string = strdup(yytext); 
    				return VOID;
				}

"char"			{words++; chars += strlen(yytext);
					printf("%s : TYPE\n", yytext);
					yylval.string = strdup(yytext); 
					return TYPE;
				}

"float"			{words++; chars += strlen(yytext);
					printf("%s : TYPE\n", yytext);
					yylval.string = strdup(yytext); 
					return TYPE;
				}

"bool"			{words++; chars += strlen(yytext);
					printf("%s : TYPE\n", yytext);
					yylval.string = strdup(yytext); 
					return TYPE;
				}
				

"return" 		{words++; chars += strlen(yytext);
    				printf("%s : RETURN\n", yytext);
    				yylval.string = strdup(yytext); 
    				return RETURN;
				}

"write"			{words++; chars += strlen(yytext);
					printf("%s : KEYWORD\n", yytext);
					yylval.string = strdup(yytext); 
					return WRITE;
				}

"if"			{words++; chars += strlen(yytext);
					printf("%s : CONDITIONAL\n", yytext);
					yylval.string = strdup(yytext); 
					return IF;
				}

"else"			{words++; chars += strlen(yytext);
					printf("%s : CONDITIONAL\n", yytext);
					yylval.string = strdup(yytext); 
					return ELSE;
				}

"while"			{words++; chars += strlen(yytext);
					printf("%s : KEYWORD\n", yytext);
					yylval.string = strdup(yytext); 
					return WHILE;
				}

"[unroll]"		{words++; chars += strlen(yytext);
					printf("%s : FLAG\n", yytext);
					yylval.string = strdup(yytext); 
					return UNROLL;
				}

{CONDITIONAL_OPERATOR}	{words++; chars += strlen(yytext);
							printf("%s : CONDITIONAL_OPERATOR\n",yytext);
							yylval.string = strdup(yytext); 
							return CONDITIONAL_OPERATOR;
						}

{BOOLEAN_LITERAL}    {
                        words++;
                        chars += strlen(yytext);
                        printf("%s : BOOLEAN_LITERAL\n", yytext);
                        yylval.boolean = (strcmp(yytext, "true") == 0) ? 1 : 0;
                        return BOOLEAN_LITERAL;
                    }

{IDENTIFIER}	{words++; chars += strlen(yytext);
					printf("%s : IDENTIFIER\n",yytext);
					yylval.string = strdup(yytext); 
					return IDENTIFIER;
				}
			
{INT_LITERAL}		{words++; chars += strlen(yytext);
				printf("%s : INT_LITERAL\n",yytext);
				yylval.number = atoi(strdup(yytext)); 
				return INT_LITERAL;
				}

{FLOAT_LITERAL} {
					words++;
					chars += strlen(yytext);

					// Remove the trailing 'f'
					char* floatString = strdup(yytext);
					floatString[strlen(floatString)-1] = '\0';

					printf("%s : FLOAT_LITERAL\n", floatString);
					yylval.float_number = atof(floatString);
					free(floatString); // Remember to free the allocated memory
					return FLOAT_LITERAL;
				}


{CHAR_LITERAL}  { 
					words++; 
					chars += strlen(yytext);
					printf("%s : CHAR_LITERAL\n", yytext);
					yylval.character = yytext[1];  // Extract the character from between the single quotes
					return CHAR_LITERAL; 
				}

{OPERATOR}		{words++; chars += strlen(yytext);
					printf("%s : OPERATOR\n",yytext);
					yylval.string = strdup(yytext); 
					return OPERATOR;
				}

		
";"				{chars++;
					printf("%s : SEMICOLON\n", yytext);
					yylval.string = strdup(yytext); 
					return SEMICOLON;
				}
				
"{"				{chars++;
					printf("%s : LCBRA\n", yytext);
					yylval.string = strdup(yytext); 
					return LCBRA;
				}

"}"				{chars++;
					printf("%s : RCBRA\n", yytext);
					yylval.string = strdup(yytext); 
					return RCBRA;
				}				
"(" 			{chars++;
    				printf("%s : LPAR\n", yytext);
    				yylval.string = strdup(yytext); 
    				return LPAR;
				}

")" 			{chars++;
    				printf("%s : RPAR\n", yytext);
    				yylval.string = strdup(yytext); 
    				return RPAR;
				}

"[" 			{chars++;
    				printf("%s : LBRACKET\n", yytext);
    				yylval.string = strdup(yytext); 
    				return LBRACKET;
				}

"]" 			{chars++;
    				printf("%s : RBRACKET\n", yytext);
    				yylval.string = strdup(yytext); 
    				return RBRACKET;
				}

"="				{chars++;
					printf("%s : EQ\n", yytext);
					yylval.string = strdup(yytext); 
					return EQ;
				}

","				{chars++;
					printf("%s : COMMA\n", yytext);
					yylval.string = strdup(yytext);
					return COMMA;
				}




[\n]					 {lines++; chars=0;}
[\t ]+                   {chars++; } 
.		{	
			chars++;
        	//printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
		}
		
%%