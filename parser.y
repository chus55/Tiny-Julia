%{
/* Expression part of this grammar based on https://www.lysator.liu.se/c/ANSI-C-grammar-y.html#inclusive-or-expression */

#include <stdio.h>
#include <string>
#include "ast.h"

using namespace std;

extern int yylineno;

#define TYPE_BOOL 0
#define TYPE_INT 1
#define TYPE_TRUE 1
#define TYPE_FALSE 0

int yylex();

void yyerror (const char *msg)
{
	printf("Error at line %d: %s\n", yylineno, msg);
}

#define YYERROR_VERBOSE 1

Statement *input;
%}

%union {
	Statement* statement_type;
    Expr* expr_type;
	ExpressionList *exprlist_type;
	ParamTypeMap* paramtypemap_type;
	pair<string, int>* pair_type;
    int int_type;
    string* string_type;
}

%token OP_EQUALS OP_NOT_EQUAL OP_LESS_THAN OP_GREATER_THAN OP_LESS_EQUAL OP_GREATER_EQUAL
%token STMT_SEP TK_ERROR TK_DOUBLE_COLON
%token LOGICAL_AND LOGICAL_OR SHIFT_LEFT SHIFT_RIGHT
%token RW_FUNCTION RW_END RW_RETURN RW_CONTINUE RW_BREAK RW_PRINT RW_PRINTLN RW_IF RW_ELSEIF RW_ELSE RW_WHILE RW_FOR RW_IN 
%token RW_TRUE RW_FALSE RW_INT RW_BOOL RW_ARRAY
%token<int_type> LIT_NUMBER 
%token<string_type> LIT_STR TK_VAR

%type<pair_type> param_element

%type<int_type> bool type type_assign

%type<expr_type> expression logical_or_expression logical_and_expression inclusive_or_expression exclusive_or_expression and_expression
%type<expr_type> equality_expression relational_expression shift_expression additive_expression multiplicative_expression exponential_expression
%type<expr_type> unary_expression postfix_expression print_arg

%type<exprlist_type> array_arguments print_arguments function_arguments array

%type<paramtypemap_type> function_params

%type<statement_type> input statement_list statement print_statement printline_statement for_statement while_statement if_statement opt_elses
%type<statement_type> assign_or_functionCall_statement assign_statement function_call function_declaration_statement return_statement break_statement 
%type<statement_type> continue_statement

%%

input: optional_separators statement_list optional_separators { input = $2; }
;

statement_separators:	statement_separators STMT_SEP
					|	STMT_SEP
;

optional_separators:	statement_separators
					|
;

statement_list:	statement_list statement_separators statement 	{ $$ = $1; ((BlockStatement *)$$)->addStatement($3); }
		|		statement										{ $$ = new BlockStatement; ((BlockStatement *)$$)->addStatement($1); }
;

statement:	print_statement 					{ $$ = $1; }
		|	printline_statement 				{ $$ = $1; }
		|	for_statement						{ $$ = $1; }
		|	while_statement						{ $$ = $1; }
		|	if_statement 						{ $$ = $1; }
		|	assign_or_functionCall_statement 	{ $$ = $1; }
		|	function_declaration_statement		{ $$ = $1; }
		|	return_statement					{ $$ = $1; }
		|	break_statement						{ $$ = $1; }
		|	continue_statement					{ $$ = $1; }
;

break_statement:	RW_BREAK	{ $$ = new BreakStatement(); }
;

continue_statement:	RW_CONTINUE	{ $$ = new ContinueStatement(); }
;

function_declaration_statement:	RW_FUNCTION TK_VAR '(' function_params ')' type_assign statement_separators statement_list statement_separators RW_END	{ 
	$$ = new FunctionDeclarationStatement(*$2, $4, $6, $8);
 }
;

function_params:	function_params ',' param_element	{ $$ = $1; $$->insert(*$3); }
				|	param_element						{ $$ = new ParamTypeMap; $$->insert(*$1); }
				|										{ $$ = new ParamTypeMap; }
;

param_element:	TK_VAR type_assign	{ $$ = new std::pair<string, int>(*$1, $2); }
;

type_assign:	TK_DOUBLE_COLON type	{ $$ = $2; }
;

type:	RW_INT	{ $$ = TYPE_INT; }
	|	RW_BOOL	{ $$ = TYPE_BOOL; }
;

return_statement:	RW_RETURN expression	{ $$ = new ReturnStatement($2); }
;

assign_or_functionCall_statement:	function_call		{ $$ = $1; }
								|	assign_statement	{ $$ = $1; }
;

assign_statement:	TK_VAR TK_DOUBLE_COLON type '=' expression				{ $$ = new DeclarationStatement(*$1, $3, $5); }
				|	TK_VAR TK_DOUBLE_COLON RW_ARRAY '{' type '}' '=' array	{ $$ = new ArrayDeclarationStatement(*$1, $5, *$8); }
				|	TK_VAR '=' expression									{ $$ = new AssignStatement(*$1, $3); }
				|	TK_VAR '[' expression ']' '=' expression				{ $$ = new ArrayAssignStatement(*$1, $3, $6); }
;

function_call:	TK_VAR '(' function_arguments ')'	{ $$ = new FunctionCallStatement(*$1, *$3); }
;

function_arguments:	function_arguments ',' expression	{ $$ = $1; $$->push_back($3); }
				|	expression							{ $$ = new ExpressionList; $$->push_back($1); }
				|										{ $$ = new ExpressionList; }
;

print_statement:	RW_PRINT '(' print_arguments ')'	{ $$ = new PrintStatement(*$3); }
;

printline_statement:	RW_PRINTLN '(' print_arguments ')'	{ $$ = new PrintLineStatement(*$3); }
;

print_arguments:	print_arguments ',' print_arg	{ $$ = $1; $$->push_back($3); }
				|	print_arg						{ $$ = new ExpressionList; $$->push_back($1); }
;

print_arg:	expression			{ $$ = $1; }
		|	LIT_STR				{ $$ = new StringExpression(*$1); }
;

if_statement:	RW_IF expression statement_separators statement_list statement_separators opt_elses RW_END	{ $$ = new IfStatement($2, $4, $6); }
;

opt_elses:	RW_ELSEIF expression statement_separators statement_list statement_separators opt_elses	{ $$ = new IfStatement($2, $4, $6); }
		|	RW_ELSE statement_separators statement_list statement_separators						{ $$ = $3; }
		|																							{ $$ = NULL; }
;

while_statement:	RW_WHILE expression statement_separators statement_list statement_separators RW_END	{ $$ = new WhileStatement($2, $4); }
;

for_statement:	RW_FOR TK_VAR in_equal expression ':' expression statement_separators statement_list statement_separators RW_END	{ 
	$$ = new ForStatement(*$2, $4, $6, $8);
 }
;

in_equal:	"in"
		|	'='
;

array:	'[' array_arguments ']'	{ $$ = $2; }
;

array_arguments:	array_arguments ',' expression	{ $$ = $1; $$->push_back($3); }
				|	expression						{ $$ = new ExpressionList; $$->push_back($1); }
;

bool:	RW_TRUE		{ $$ = TYPE_TRUE; }
	|	RW_FALSE	{ $$ = TYPE_FALSE; }
;

expression:	logical_or_expression	{ $$ = $1; }
;

logical_or_expression:	logical_and_expression									{ $$ = $1; }
					|	logical_or_expression LOGICAL_OR logical_and_expression { $$ = new LogicalOrExpression($1, $3); }
;

logical_and_expression:	inclusive_or_expression										{ $$ = $1; }
					|	logical_and_expression LOGICAL_AND inclusive_or_expression 	{ $$ = new LogicalAndExpression($1, $3); }
;

inclusive_or_expression:	exclusive_or_expression								{ $$ = $1; }
						|	inclusive_or_expression '|' exclusive_or_expression { $$ = new InclusiveOrExpression($1, $3); }
;

exclusive_or_expression:	and_expression								{ $$ = $1; }
						|	exclusive_or_expression '$' and_expression 	{ $$ = new ExclusiveOrExpression($1, $3); }
;

and_expression:	equality_expression						{ $$ = $1; }
			|	and_expression '&' equality_expression 	{ $$ = new AndExpression($1, $3); }
;

equality_expression:	relational_expression									{ $$ = $1; }
					|	equality_expression OP_EQUALS relational_expression		{ $$ = new EqualExpression($1, $3); }
					|	equality_expression OP_NOT_EQUAL relational_expression 	{ $$ = new NotEqualExpression($1, $3); }
;

relational_expression:	shift_expression										{ $$ = $1; }
					|	relational_expression OP_LESS_THAN shift_expression 	{ $$ = new LessThanExpression($1, $3); }
					|	relational_expression OP_GREATER_THAN shift_expression	{ $$ = new GreaterThanExpression($1, $3); }
					|	relational_expression OP_LESS_EQUAL shift_expression	{ $$ = new LessEqualExpression($1, $3); }
					|	relational_expression OP_GREATER_EQUAL shift_expression	{ $$ = new GreaterEqualExpression($1, $3); }
;

shift_expression:	additive_expression									{ $$ = $1; }
				|	shift_expression SHIFT_LEFT additive_expression		{ $$ = new ShiftLeftExpression($1, $3); }
				|	shift_expression SHIFT_RIGHT additive_expression	{ $$ = new ShiftRightExpression($1, $3); }
;

additive_expression:	multiplicative_expression							{ $$ = $1; }
					|	additive_expression '+' multiplicative_expression	{ $$ = new AddExpression($1, $3); }
					|	additive_expression '-' multiplicative_expression	{ $$ = new SubExpression($1, $3); }
;

multiplicative_expression:	exponential_expression									{ $$ = $1; }
						|	multiplicative_expression '*' exponential_expression	{ $$ = new MulExpression($1, $3); }
						|	multiplicative_expression '/' exponential_expression	{ $$ = new DivExpression($1, $3); }
						|	multiplicative_expression '%' exponential_expression	{ $$ = new ModExpression($1, $3); }
;

exponential_expression:	unary_expression							{ $$ = $1; }
					|	exponential_expression '^' unary_expression	{ $$ = new ExpoExpression($1, $3); }
;

unary_expression:	postfix_expression			{ $$ = $1; }
				|	'-' unary_expression		{ $$ = new MulExpression(new NumExpression(-1), $2); }
				|	'~' unary_expression		{ $$ = new BitwiseNotExpression($2); }
				|	'!' unary_expression		{ $$ = new NegationExpression($2); }
;

postfix_expression:	TK_VAR								{ $$ = new IdExpression(*$1); }
				|	LIT_NUMBER							{ $$ = new NumExpression($1); }
				|	'(' expression ')'					{ $$ = $2; }
				|	bool								{ $$ = new BoolExpression($1); }
				|	TK_VAR '[' expression ']'			{ $$ = new ArrayAccessExpression(*$1, $3); }
				|	TK_VAR '(' function_arguments ')'	{ $$ = new FunctionExpression(*$1, *$3); }
;