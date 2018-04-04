%option noyywrap
%option nodefault
%option yylineno

%x comment

%{
	#include <stdio.h>
	#include <string>
	#include "ast.h"
	#include "tokens.h"

	using namespace std;
%}

%%
[ \t]					{ /* Do nothing */ }
"\n"					{ return STMT_SEP; }
";"						{ return STMT_SEP; }
"#="					{ BEGIN(comment); }
"#"[^=][^\n]*\n			{ /* Do nothing */ }
"+"						{ return '+'; }
"-"						{ return '-'; }
"*"						{ return '*'; }
"/"						{ return '/'; }
"%"						{ return '%'; }
"^"						{ return '^'; }
"="						{ return '='; }
"==" 					{ return OP_EQUALS; }
"!=" 					{ return OP_NOT_EQUAL; }
"<" 					{ return OP_LESS_THAN; }
">" 					{ return OP_GREATER_THAN; }
"<=" 					{ return OP_LESS_EQUAL; }
">=" 					{ return OP_GREATER_EQUAL; }
"&&"					{ return LOGICAL_AND; }
"||"					{ return LOGICAL_OR; }
"&"						{ return '&'; }
"|"						{ return '|'; }
"$"						{ return '$'; }
"~"						{ return '~'; }
"<<"					{ return SHIFT_LEFT; }
">>"					{ return SHIFT_RIGHT; }
"!"						{ return '!'; }
"("						{ return '('; }
")"						{ return ')'; }
"["						{ return '['; }
"]"						{ return ']'; }
"{"						{ return '{'; }
"}"						{ return '}'; }
","						{ return ','; }
":"						{ return ':'; }
"::"					{ return TK_DOUBLE_COLON; }
"function"				{ return RW_FUNCTION; }
"end"					{ return RW_END; }
"return"				{ return RW_RETURN; }
"continue"				{ return RW_CONTINUE; }
"break"					{ return RW_BREAK; }
"print"					{ return RW_PRINT; }
"println"				{ return RW_PRINTLN; }
"if"					{ return RW_IF; }
"elseif"				{ return RW_ELSEIF; }
"else"					{ return RW_ELSE; }
"while"					{ return RW_WHILE; }
"for"					{ return RW_FOR; }
"in"					{ return RW_IN; }
"true"					{ return RW_TRUE; }
"false"					{ return RW_FALSE; }
"Int"					{ return RW_INT; }
"Bool"					{ return RW_BOOL; }
"Array"					{ return RW_ARRAY; }
[0-9]+					{ yylval.int_type = atoi(yytext); return LIT_NUMBER; }
\"(\\.|[^\\"])*"\"" 	{ yytext[yyleng-1] = '\0'; string *str = new string(&yytext[1]); yylval.string_type = str; return LIT_STR; }
[_a-zA-Z][_a-zA-Z0-9]*	{ string *str = new string(yytext); yylval.string_type = str; return TK_VAR; }
<INITIAL><<EOF>>		{ return 0; }
.						{ return TK_ERROR; }

<comment>"=#"		{ BEGIN(INITIAL); }
<comment>.|\n		{/* Do nothing */}
<comment><<EOF>>	{ printf("Error, block comment was never closed.\n"); terminate(); }

%%