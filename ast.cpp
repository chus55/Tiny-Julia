#include "ast.h"

using namespace std;

static int printIndex = 0;

/*Start Expressions*/

void LogicalOrExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void LogicalAndExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void InclusiveOrExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ExclusiveOrExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void AndExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void EqualityExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void EqualExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void NotEqualExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void RelationalExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void LessThanExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void GreaterThanExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void LessEqualExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void GreaterEqualExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ShiftExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ShiftLeftExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ShiftRightExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void AdditiveExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void AddExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void SubExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void MultiplicativeExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void MulExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void DivExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ModExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ExponentialExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ExpoExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void BitwiseNotExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void NegationExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void PosfixExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void IdExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void NumExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void BoolExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ArrayAccessExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void FunctionExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void StringExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

/*End Expressions*/

/*Start Statements*/

string ArrayDeclarationStatement::genCode() {
	/*Do some generating, baby!*/
}

string ArrayAssignStatement::genCode() {
	/*Do some generating, baby!*/
}

string DeclarationStatement::genCode() {
	/*Do some generating, baby!*/
}

string AssignStatement::genCode() {
	/*Do some generating, baby!*/
}

string BlockStatement::genCode() {
	/*Do some generating, baby!*/
}

string PrintStatement::genCode() {
	/*Do some generating, baby!*/
}

string PrintLineStatement::genCode() {
	/*Do some generating, baby!*/
}

string IfStatement::genCode() {
	/*Do some generating, baby!*/
}

string BreakStatement::genCode() {
	/*Do some generating, baby!*/
}

string ContinueStatement::genCode() {
	/*Do some generating, baby!*/
}

string WhileStatement::genCode() {
	/*Do some generating, baby!*/
}

string ForStatement::genCode() {
	/*Do some generating, baby!*/
}

string FunctionDeclarationStatement::genCode() {
	/*Do some generating, baby!*/
}

string FunctionCallStatement::genCode() {
	/*Do some generating, baby!*/
}

string ReturnStatement::genCode() {
	/*Do some generating, baby!*/
}

/*End Statements*/

/*Print Statements*/

void printWithTabs(string str){
	int tempIndex = printIndex;
	while(tempIndex > 0){
		cout<<"\t";
		tempIndex--;
	}
	cout<<str<<endl;
}

void ArrayDeclarationStatement::printStatement() {
	printWithTabs("Array Declaration Statement");
}

void ArrayAssignStatement::printStatement() {
	printWithTabs("Array Assign Statement");
}

void DeclarationStatement::printStatement() {
	printWithTabs("Declaration Statement");
}

void AssignStatement::printStatement() {
	printWithTabs("Assign Statement");
}

void BlockStatement::printStatement() {
	printIndex++;
	for (list<Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++) {
		(*i)->printStatement();
	}
	printIndex--;
}

void PrintStatement::printStatement() {
	printWithTabs("Print Statement");
}

void PrintLineStatement::printStatement() {
	printWithTabs("PrintLine Statement");
}

void IfStatement::printStatement() {
	printWithTabs("If Statement");
	st1->printStatement();
	if (st2 != NULL){
		printWithTabs("Else Statement");
		st2->printStatement();
	}
}

void BreakStatement::printStatement() {
	printWithTabs("Break Statement");
}

void ContinueStatement::printStatement() {
	printWithTabs("Continue Statement");
}

void WhileStatement::printStatement() {
	printWithTabs("While Statement");
	blockstatement->printStatement();
}

void ForStatement::printStatement() {
	printWithTabs("For Statement");
	blockstatement->printStatement();
}

void FunctionDeclarationStatement::printStatement() {
	printWithTabs("Function Declaration Statement");
	blockstatement->printStatement();
}

void FunctionCallStatement::printStatement() {
	printWithTabs("Function Call Statement");
}

void ReturnStatement::printStatement() {
	printWithTabs("Return Statement");
}