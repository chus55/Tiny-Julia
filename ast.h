#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <list>
#include <stdio.h>
#include <math.h>
#include <typeinfo>
#include <algorithm>
#include <vector>

using namespace std;

//Expr//

struct ExprContext {
    string code;
    string place;
	int numberValue;
	bool isConstant;
};

enum ExpressionKind {
    LOGICAL_OR_EXPRESSION,//0
    LOGICAL_AND_EXPRESSION,
    INCLUSIVE_OR_EXPRESSION,//2
    EXCLUSIVE_OR_EXPRESSION,
    AND_EXPRESSION,//4
    EQUALITY_EXPRESSION,
    EQUAL_EXPRESSION,//6
    NOT_EQUAL_EXPRESSION,
    RELATIONAL_EXPRESSION,//8
    LESS_THAN_EXPRESSION,
    GREATER_THAN_EXPRESSION,//10
    LESS_EQUAL_EXPRESSION,
    GREATER_EQUAL_EXPRESSION,//12
    SHIFT_EXPRESSION,
    SHIFT_LEFT_EXPRESSION,//14
    SHIFT_RIGHT_EXPRESSION,
    ADDITIVE_EXPRESSION,//16
    ADD_EXPRESSION,
    SUB_EXPRESSION,//18
    MULTIPLICATIVE_EXPRESSION,
    MUL_EXPRESSION,//20
    DIV_EXPRESSION,
    MOD_EXPRESSION,//22
    EXPONENTIAL_EXPRESSION,
    EXPO_EXPRESSION,//24
    BITWISE_NOT_EXPRESSION,
    NEGATION_EXPRESSION,//26
    POSFIX_EXPRESSION,
    ID_EXPRESSION,//28
    NUM_EXPRESSION,
    BOOL_EXPRESSION,//30
    ARRAY_ACCESS_EXPRESSION,
    FUNCTION_EXPRESSION,//32
    STRING_EXPRESSION
};

class Expr {
public:
    virtual void genCode(ExprContext &ctx) = 0;
    virtual int getKind() = 0;
};

typedef list<Expr*> ExpressionList;
typedef map<string, int> ParamTypeMap;

class BinaryExpr: public Expr {
protected:
	BinaryExpr(Expr *expr1, Expr *expr2) {
		this->expr1 = expr1;
		this->expr2 = expr2;
	}
public:
	Expr *expr1, *expr2;
};

class LogicalOrExpression: public BinaryExpr {
public:
	LogicalOrExpression(Expr *expr1, Expr *expr2): BinaryExpr(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return LOGICAL_OR_EXPRESSION; }
};

class LogicalAndExpression: public LogicalOrExpression {
public:
	LogicalAndExpression(Expr *expr1, Expr *expr2): LogicalOrExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return LOGICAL_AND_EXPRESSION; }
};

class InclusiveOrExpression: public LogicalAndExpression {
public:
	InclusiveOrExpression(Expr *expr1, Expr *expr2): LogicalAndExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return INCLUSIVE_OR_EXPRESSION; }
};

class ExclusiveOrExpression: public InclusiveOrExpression {
public:
	ExclusiveOrExpression(Expr *expr1, Expr *expr2): InclusiveOrExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return EXCLUSIVE_OR_EXPRESSION; }
};

class AndExpression: public ExclusiveOrExpression {
public:
	AndExpression(Expr *expr1, Expr *expr2): ExclusiveOrExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return AND_EXPRESSION; }
};

class EqualityExpression: public AndExpression {
public:
	EqualityExpression(Expr *expr1, Expr *expr2): AndExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return EQUALITY_EXPRESSION; }
};

class EqualExpression: public EqualityExpression {
public:
    EqualExpression(Expr *expr1, Expr *expr2): EqualityExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return EQUAL_EXPRESSION; }
};

class NotEqualExpression: public EqualityExpression {
public:
    NotEqualExpression(Expr *expr1, Expr *expr2): EqualityExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return NOT_EQUAL_EXPRESSION; }
};

class RelationalExpression: public EqualityExpression {
public:
	RelationalExpression(Expr *expr1, Expr *expr2): EqualityExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return RELATIONAL_EXPRESSION; }
};

class LessThanExpression: public RelationalExpression {
public:
    LessThanExpression(Expr *expr1, Expr *expr2): RelationalExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return LESS_THAN_EXPRESSION; }
};

class GreaterThanExpression: public RelationalExpression {
public:
    GreaterThanExpression(Expr *expr1, Expr *expr2): RelationalExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return GREATER_THAN_EXPRESSION; }
};

class LessEqualExpression: public RelationalExpression {
public:
    LessEqualExpression(Expr *expr1, Expr *expr2): RelationalExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return LESS_EQUAL_EXPRESSION; }
};

class GreaterEqualExpression: public RelationalExpression {
public:
    GreaterEqualExpression(Expr *expr1, Expr *expr2): RelationalExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return GREATER_EQUAL_EXPRESSION; }
};

class ShiftExpression: public RelationalExpression {
public:
	ShiftExpression(Expr *expr1, Expr *expr2): RelationalExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return SHIFT_EXPRESSION; }
};

class ShiftLeftExpression: public ShiftExpression {
public:
	ShiftLeftExpression(Expr *expr1, Expr *expr2): ShiftExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return SHIFT_LEFT_EXPRESSION; }
};

class ShiftRightExpression: public ShiftExpression {
public:
	ShiftRightExpression(Expr *expr1, Expr *expr2): ShiftExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return SHIFT_RIGHT_EXPRESSION; }
};

class AdditiveExpression: public ShiftExpression {
public:
	AdditiveExpression(Expr *expr1, Expr *expr2): ShiftExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return ADDITIVE_EXPRESSION; }
};

class AddExpression: public AdditiveExpression{
public:
	AddExpression(Expr *expr1, Expr *expr2): AdditiveExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return ADD_EXPRESSION; }
};

class SubExpression: public AdditiveExpression{
public:
	SubExpression(Expr *expr1, Expr *expr2): AdditiveExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return SUB_EXPRESSION; }
};

class MultiplicativeExpression: public AdditiveExpression{
public:
	MultiplicativeExpression(Expr *expr1, Expr *expr2): AdditiveExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return MULTIPLICATIVE_EXPRESSION; }
};

class MulExpression: public MultiplicativeExpression{
public:
	MulExpression(Expr *expr1, Expr *expr2): MultiplicativeExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return MUL_EXPRESSION; }
};

class DivExpression: public MultiplicativeExpression{
public:
	DivExpression(Expr *expr1, Expr *expr2): MultiplicativeExpression(expr1, expr2){}
	void genCode(ExprContext &ctx);
    int getKind() { return DIV_EXPRESSION; }
};

class ModExpression: public MultiplicativeExpression {
public:
    ModExpression(Expr *expr1, Expr *expr2): MultiplicativeExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return MOD_EXPRESSION; }
};

class ExponentialExpression: public MultiplicativeExpression {
public:
    ExponentialExpression(Expr *expr1, Expr *expr2): MultiplicativeExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return EXPONENTIAL_EXPRESSION; }
};

class ExpoExpression: public ExponentialExpression {
public:
    ExpoExpression(Expr *expr1, Expr *expr2): ExponentialExpression(expr1, expr2){}
    void genCode(ExprContext &ctx);
    int getKind() { return EXPO_EXPRESSION; }
};

class UnaryExpression: public Expr {
protected:
    UnaryExpression(Expr *expr) {
        this->expr = expr;
    }
    UnaryExpression(){}
public:
    Expr *expr;
};

class BitwiseNotExpression: public UnaryExpression {
public:
    BitwiseNotExpression(Expr *expr): UnaryExpression(expr){}
    void genCode(ExprContext &ctx);
    int getKind() { return BITWISE_NOT_EXPRESSION; }
};

class NegationExpression: public UnaryExpression {
public:
    NegationExpression(Expr *expr): UnaryExpression(expr){}
    void genCode(ExprContext &ctx);
    int getKind() { return NEGATION_EXPRESSION; }
};

class PosfixExpression: public UnaryExpression {
public:
    PosfixExpression(Expr *expr): UnaryExpression(expr){}
    PosfixExpression(){}
    void genCode(ExprContext &ctx);
    int getKind() { return POSFIX_EXPRESSION; }
};

class IdExpression: public PosfixExpression {
public:
	IdExpression(string varName){
		this->varName = varName;
	}

	void genCode(ExprContext &ctx);
    int getKind() { return ID_EXPRESSION; }

	string varName;
};

class NumExpression: public PosfixExpression {
public:
	NumExpression(int number) {
		this->number = number;
	}

    void genCode(ExprContext &ctx);
    int getKind() { return NUM_EXPRESSION; }

	int number;
};

class BoolExpression: public PosfixExpression {
public:
	BoolExpression(int boolInt) {
		this->boolInt = boolInt;
	}

    void genCode(ExprContext &ctx);
    int getKind() { return BOOL_EXPRESSION; }

	int boolInt;
};

class ArrayAccessExpression: public PosfixExpression {
public:
	ArrayAccessExpression(string varName, Expr *position) {
        this->varName = varName;
        this->position = position;
	}

    void genCode(ExprContext &ctx);
    int getKind() { return ARRAY_ACCESS_EXPRESSION; }

    string varName;
    Expr *position;
};

class FunctionExpression: public PosfixExpression {
public:
	FunctionExpression(string varName, ExpressionList exprList) {
		this->varName = varName;
		this->exprList = exprList;
	}

	void genCode(ExprContext &ctx);
    int getKind() { return FUNCTION_EXPRESSION; }

	string varName;
	ExpressionList exprList;
};

class StringExpression: public Expr {
public:
    StringExpression(string str) {
		this->str = str;
	}
    void genCode(ExprContext &ctx);
    int getKind() { return STRING_EXPRESSION; }
    string str;
};

//End of expr//

//Statements//

enum StatementKind {
    GLOBAL_CONTEXT,
    ARRAY_DECLARATION_STATEMENT,
    ARRAY_ASSIGN_STATEMENT,
    DECLARATION_STATEMENT,
    ASSIGN_STATEMENT,
    BLOCK_STATEMENT,
    PRINT_STATEMENT,
    PRINTLINE_STATEMENT,
    IF_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    FUNCTION_DECLARATION_STATEMENT,
    FUNCTION_CALL_STATEMENT,
    RETURN_STATEMENT
};

struct StatementContext {
    map<string, pair<int, int>> variables;
    map<string, pair<int, int>> functionParams;
    map<string, int> localArraySizes;
	bool isGlobal;
    bool lockedContext;
    int statement_type;
    list<string> important_labels;
	struct StatementContext *previous_context;
	struct StatementContext *next_context;
};

class Statement {
public:
    virtual string genCode() = 0;
    virtual int getKind() = 0;
    virtual void printStatement() = 0;
};

class ArrayDeclarationStatement: public Statement {
public:
	ArrayDeclarationStatement(string varName, int type, ExpressionList exprList) {
        this->varName = varName;
        this->type = type;
        this->exprList = exprList;
    }
    int getKind() { return ARRAY_DECLARATION_STATEMENT; }
    void printStatement();

	string genCode();

    string varName;
    int type;
	ExpressionList exprList;
};

class ArrayAssignStatement: public Statement {
public:
	ArrayAssignStatement(string varName,Expr *position, Expr *expr) {
        this->varName = varName;
        this->position = position;
		this->expr = expr;
	}
    int getKind() { return ARRAY_ASSIGN_STATEMENT; }
    void printStatement();

	string genCode();

    string varName;
    Expr *position;
	Expr *expr;
};

class DeclarationStatement: public Statement {
public:
	DeclarationStatement(string varName, int type, Expr *expr) {
        this->varName = varName;
        this->type = type;
        this->expr = expr;
	}
    int getKind() { return DECLARATION_STATEMENT; }
    void printStatement();

	string genCode();

    string varName;
    int type;
	Expr *expr;
};

class AssignStatement: public Statement {
public:
	AssignStatement(string varName, Expr *expr) {
		this->varName = varName;
		this->expr = expr;
	}
    int getKind() { return ASSIGN_STATEMENT; }
    void printStatement();

	string genCode();

	string varName;
	Expr *expr;
};

class BlockStatement: public Statement {
public:
	BlockStatement() {}

	void addStatement(Statement *statement) {
		statementList.push_back(statement);
    }
	void GenerateFile();
    int getKind() { return BLOCK_STATEMENT; }
    void printStatement();
    string genFunctions();
    void genLiteralStrings();
    void getGlobalVariables();
    int getGlobalLocalVariables();

	string genCode();

	list<Statement*> statementList;
};

class PrintStatement: public Statement {
public:
	PrintStatement(ExpressionList exprList) {
		this->exprList = exprList;
	}
    int getKind() { return PRINT_STATEMENT; }
    void printStatement();

	string genCode();

	ExpressionList exprList;
};

class PrintLineStatement: public Statement {
public:
	PrintLineStatement(ExpressionList exprList) {
		this->exprList = exprList;
	}
    int getKind() { return PRINTLINE_STATEMENT; }
    void printStatement();

	string genCode();

	ExpressionList exprList;
};

class IfStatement: public Statement {
public:
	IfStatement(Expr *expr, Statement *st_true, Statement *st_false) {
		this->expr = expr;
		this->st1 = st_true;
		this->st2 = st_false;
	}
    int getKind() { return IF_STATEMENT; }
    void printStatement();

	string genCode();

	Expr *expr;
	Statement *st1;
	Statement *st2;
};

class BreakStatement: public Statement {
public:
    BreakStatement() {}
    int getKind() { return BREAK_STATEMENT; }
    void printStatement();
    string genCode();
};

class ContinueStatement: public Statement {
public:
    ContinueStatement() {}
    int getKind() { return CONTINUE_STATEMENT; }
    void printStatement();
    string genCode();
};

class WhileStatement: public Statement {
public:
	WhileStatement(Expr *expr, Statement *blockstatement) {
		this->expr = expr;
		this->blockstatement = blockstatement;
	}
    int getKind() { return WHILE_STATEMENT; }
    void printStatement();

	string genCode();

	Expr *expr;
	Statement *blockstatement;
};

class ForStatement: public Statement {
public:
    ForStatement(string id, Expr *startingExpr, Expr *endingExpr, Statement *blockstatement) {
        this->id = id;
        this->startingExpr = startingExpr;
        this->endingExpr = endingExpr;
        this->blockstatement = blockstatement;
    }
    int getKind() { return FOR_STATEMENT; }
    void printStatement();
    string genCode();
    string id;
    Expr *startingExpr;
    Expr *endingExpr;
    Statement *blockstatement;
};

class FunctionDeclarationStatement: public Statement {
public:
	FunctionDeclarationStatement(string varName, ParamTypeMap *varList, int type, Statement *blockstatement) {
		this->varName = varName;
        this->varList = varList;
        this->type = type;
		this->blockstatement = blockstatement;
	}
    int getKind() { return FUNCTION_DECLARATION_STATEMENT; }
    void printStatement();

	string genCode();

    string varName;
    ParamTypeMap *varList;
    int type;
	Statement *blockstatement;
};

class FunctionCallStatement: public Statement {
public:
	FunctionCallStatement(string varName, ExpressionList exprList) {
		this->varName = varName;
		this->exprList = exprList;
	}
    int getKind() { return FUNCTION_CALL_STATEMENT; }
    void printStatement();

	string genCode();

	string varName;
	ExpressionList exprList;
};

class ReturnStatement: public Statement {
public:
	ReturnStatement(Expr *expr) {
		this->expr = expr;
	}
    int getKind() { return RETURN_STATEMENT; }
    void printStatement();

	string genCode();

	Expr *expr;
};

//End of statements//

void tempInit();
string genDataSection();

#endif