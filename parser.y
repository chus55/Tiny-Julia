%{
/* Expression part of this grammar based on https://www.lysator.liu.se/c/ANSI-C-grammar-y.html#inclusive-or-expression */

#include <stdio.h>
#include <string>
#include "ast.h"

using namespace std;

extern int yylineno;
int yylex();

void yyerror (const char *msg)
{
	printf("Error at line %d: %s\n", yylineno, msg);
}

#define YYERROR_VERBOSE 1

%}

%union {
    int int_type;
    string* string_type;
}

%token OP_EQUALS OP_NOT_EQUAL OP_LESS_THAN OP_GREATER_THAN OP_LESS_EQUAL OP_GREATER_EQUAL
%token STMT_SEP TK_ERROR TK_DOUBLE_COLON
%token LOGICAL_AND LOGICAL_OR SHIFT_LEFT SHIFT_RIGHT
%token RW_FUNCTION RW_END RW_RETURN RW_CONTINUE RW_BREAK RW_PRINT RW_PRINTLN RW_IF RW_ELSEIF RW_ELSE RW_WHILE RW_FOR RW_IN RW_TRUE RW_FALSE RW_INT RW_BOOL RW_ARRAY
%token<int_type> LIT_NUMBER
%token<string_type> LIT_STR TK_VAR

%%

input: optional_separators statement_list optional_separators { printf("Success!\n"); }
;

statement_separators:	statement_separators STMT_SEP	{  }
					|	STMT_SEP						{  }
;

optional_separators:	statement_separators	{  }
					|
;

statement_list:	statement_list statement_separators statement 	{  }
		|		statement										{  }
;

statement:	print_statement 					{  }
		|	printline_statement 				{  }
		|	for_statement						{  }
		|	while_statement						{  }
		|	if_statement 						{  }
		|	assign_or_functionCall_statement 	{  }
		|	function_declaration_statement		{  }
		|	return_statement					{  }
		|	break_statement						{  }
		|	continue_statement					{  }
;

break_statement:	RW_BREAK	{  }
;

continue_statement:	RW_CONTINUE	{  }
;

function_declaration_statement:	RW_FUNCTION TK_VAR '(' function_params ')' type_assign statement_separators statement_list statement_separators RW_END	{  }
;

function_params:	function_params ',' param_element	{  }
				|	param_element						{  }
				|
;

param_element:	TK_VAR type_assign	{  }
;

type_assign:	TK_DOUBLE_COLON type	{  }
;

type:	RW_INT	{  }
	|	RW_BOOL	{  }
;

return_statement:	RW_RETURN expression	{  }
;

assign_or_functionCall_statement:	function_call		{  }
								|	assign_statement	{  }
;

assign_statement:	TK_VAR TK_DOUBLE_COLON RW_INT '=' expression			{  }
				|	TK_VAR TK_DOUBLE_COLON RW_BOOL '=' expression			{  }
				|	TK_VAR TK_DOUBLE_COLON RW_ARRAY '{' type '}' '=' array	{  }
				|	TK_VAR '=' expression_array								{  }
				|	TK_VAR '[' expression ']' '=' expression				{  }
;

function_call:	TK_VAR '(' function_arguments ')'	{  }
;

function_arguments:	function_arguments ',' expression	{  }
				|	expression							{  }
				|
;

expression_array:	expression	{  }
				|	array		{  }
;

print_statement:	RW_PRINT '(' print_arguments ')'	{  }
;

printline_statement:	RW_PRINTLN '(' print_arguments ')'	{  }
;

print_arguments:	print_arguments ',' print_arg	{  }
				|	print_arg						{  }
;

print_arg:	expression_array	{  }
		|	LIT_STR				{  }
;

if_statement:	RW_IF expression statement_separators statement_list statement_separators opt_elses RW_END	{  }
;

opt_elses:	RW_ELSEIF expression statement_separators statement_list statement_separators opt_elses	{  }
		|	RW_ELSE statement_separators statement_list statement_separators						{  }
		|
;

while_statement:	RW_WHILE expression statement_separators statement_list statement_separators RW_END	{  }
;

for_statement:	RW_FOR TK_VAR in_equal expression ':' expression statement_separators statement_list statement_separators RW_END	{  }
;

in_equal:	"in"	{  }
		|	'='		{  }
;

array:	'[' array_arguments ']'	{  }
;

array_arguments:	array_arguments ',' expression	{  }
				|	expression						{  }
;

bool:	RW_TRUE		{  }
	|	RW_FALSE	{  }
;

expression:	logical_or_expression	{  }
;

logical_or_expression:	logical_and_expression									{  }
					|	logical_or_expression LOGICAL_OR logical_and_expression {  }
;

logical_and_expression:	inclusive_or_expression										{  }
					|	logical_and_expression LOGICAL_AND inclusive_or_expression 	{  }
;

inclusive_or_expression:	exclusive_or_expression								{  }
						|	inclusive_or_expression '|' exclusive_or_expression {  }
;

exclusive_or_expression:	and_expression								{  }
						|	exclusive_or_expression '$' and_expression 	{  }
;

and_expression:	equality_expression						{  }
			|	and_expression '&' equality_expression 	{  }
;

equality_expression:	relational_expression									{  }
					|	equality_expression OP_EQUALS relational_expression		{  }
					|	equality_expression OP_NOT_EQUAL relational_expression 	{  }
;

relational_expression:	shift_expression										{  }
					|	relational_expression OP_LESS_THAN shift_expression 	{  }
					|	relational_expression OP_GREATER_THAN shift_expression	{  }
					|	relational_expression OP_LESS_EQUAL shift_expression	{  }
					|	relational_expression OP_GREATER_EQUAL shift_expression	{  }
;

shift_expression:	additive_expression									{  }
				|	shift_expression SHIFT_LEFT additive_expression		{  }
				|	shift_expression SHIFT_RIGHT additive_expression	{  }
;

additive_expression:	multiplicative_expression							{  }
					|	additive_expression '+' multiplicative_expression	{  }
					|	additive_expression '-' multiplicative_expression	{  }
;

multiplicative_expression:	exponential_expression									{  }
						|	multiplicative_expression '*' exponential_expression	{  }
						|	multiplicative_expression '/' exponential_expression	{  }
						|	multiplicative_expression '%' exponential_expression	{  }
;

exponential_expression:	unary_expression							{  }
					|	exponential_expression '^' unary_expression	{  }
;

unary_expression:	postfix_expression			{  }
				|	'-' unary_expression		{  }
				|	'~' unary_expression		{  }
				|	'!' unary_expression		{  }
;

postfix_expression:	TK_VAR						{  }
				|	LIT_NUMBER					{  }
				|	'(' expression ')'			{  }
				|	bool						{  }
				|	TK_VAR '[' expression ']'	{  }
				|	function_call				{  }
;