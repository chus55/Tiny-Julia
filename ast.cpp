#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>
#include <list>
#include "ast.h"
#include <algorithm>
#include <vector>

using namespace std;

/*Utilities Start*/

#define TYPE_TRUE 1
#define TYPE_FALSE 0
#define BOOL_T 0
#define INT_T 1
#define ARRAY_T 2


static int printIndex = 0;
map<string, bool> tempInUse;
static int labelCount = 12;
string globalVariablesString;
map<string, int> globalVariables;
map<string, map<string, int> > functions;
map<string, int > functionTypes;
StatementContext *currentContext = NULL;


void registerFunction(string funcName, map<string, int> arguments, int type){
	functions[funcName] = arguments;
	functionTypes[funcName] = type;
}

void registerLocalVariable(string varName, int type) {
	if (globalVariables.count(varName)){
		cout<<"Error. Local variable declaration conflicts with global variable of the same name."<<endl;
		terminate();
	}
	if (currentContext->variables.count(varName)){
		cout<<"Error. Variable "<< varName << " declared more than once."<<endl;
		terminate();
	}
	if (currentContext->functionParams.count(varName)){
		cout<<"Error. Argument "<< varName << " declared more than once."<<endl;
		terminate();
	}
	/*int address = (currentContext->variables.size()+1)*4;
	currentContext->variables[varName] = make_pair(type, address);*/
	int address = 4;
	if (currentContext->variables.rbegin() != currentContext->variables.rend()){
		std::map<string, pair<int, int>>::reverse_iterator rit = currentContext->variables.rbegin();
		if (currentContext->localArraySizes.count(rit->first)){
			address = rit->second.second + (4 * currentContext->localArraySizes[rit->first]);
		}
		else
			address = rit->second.second + 4;
	}
	currentContext->variables[varName] = make_pair(type, address);
	//cout<<"Registered variable: "<<varName<<", of type: "<<type<<", in address: "<<address<<endl;
}

void registerLocalArray(string varName, int type, int size) {
	if (globalVariables.count(varName)){
		cout<<"Error. Local variable declaration conflicts with global variable of the same name."<<endl;
		terminate();
	}
	if (currentContext->variables.count(varName)){
		cout<<"Error. Variable "<< varName << " declared more than once."<<endl;
		terminate();
	}
	if (currentContext->functionParams.count(varName)){
		cout<<"Error. Argument "<< varName << " declared more than once."<<endl;
		terminate();
	}
	int address = 4;
	if (currentContext->variables.rbegin() != currentContext->variables.rend()){
		std::map<string, pair<int, int>>::reverse_iterator rit = currentContext->variables.rbegin();
		if (currentContext->localArraySizes.count(rit->first)){
			address = rit->second.second + (4 * currentContext->localArraySizes[rit->first]);
		}
		else
			address = rit->second.second + 4;
	}
	currentContext->variables[varName] = make_pair(type, address);
	currentContext->localArraySizes[varName] = size;
	//cout<<"Registered array: "<<varName<<", of size: " <<size<<", and of type: "<<type<<", in address: "<<address<<endl;
}

void registerParam(string varName, int type) {
	if (globalVariables.count(varName)){
		cout<<"Error. Argument declaration conflicts with global variable of the same name."<<endl;
		terminate();
	}
	if (currentContext->functionParams.count(varName)){
		cout<<"Error. Argument "<< varName << " declared more than once."<<endl;
		terminate();
	}
	int address = (currentContext->functionParams.size()+2)*4;
	currentContext->functionParams[varName] = make_pair(type, address);
}

void push_context(int statement_kind, list<string> labels, bool locked_context){
	StatementContext *new_statement_context = new StatementContext;

	if(!currentContext->variables.empty() && locked_context == false)
		new_statement_context->variables.insert(currentContext->variables.begin(), currentContext->variables.end()); 

	if(!currentContext->functionParams.empty() && locked_context == false)
		new_statement_context->functionParams.insert(currentContext->functionParams.begin(),currentContext->functionParams.end());

	if(!currentContext->localArraySizes.empty() && locked_context == false)
		new_statement_context->localArraySizes.insert(currentContext->localArraySizes.begin(),currentContext->localArraySizes.end());

	//new_statement_context->localVariableAddress = NULL;
	new_statement_context->isGlobal = false;
	new_statement_context->lockedContext = locked_context;
	new_statement_context->statement_type = statement_kind;
	new_statement_context->important_labels = labels;
	new_statement_context->previous_context = currentContext;
	new_statement_context->next_context = NULL;

	currentContext = new_statement_context;
}

void pop_context(){
	if (currentContext->statement_type == GLOBAL_CONTEXT)
	{
		cout<<"Error. Tried to pop global context."<<endl;
		terminate();
	}
	StatementContext *previous_context = currentContext->previous_context;
	//free(currentContext);
	delete currentContext;
	currentContext = previous_context;
}

int getReserveSizeForStatement(Statement* currentStatement){
	int totalVariables = 0;
	int statement_type = currentStatement->getKind();
	if (statement_type == BLOCK_STATEMENT){
		//cout<<"BLOCK_STATEMENT"<<endl;
		BlockStatement* st = ((BlockStatement *)currentStatement);
		for (list<Statement*>::iterator i = st->statementList.begin(); i != st->statementList.end(); i++) {
			totalVariables += getReserveSizeForStatement((*i));
		}
	}
	if (statement_type == IF_STATEMENT) {
		//cout<<"IF_STATEMENT"<<endl;
		IfStatement* st = ((IfStatement *)currentStatement);
		totalVariables += getReserveSizeForStatement(st->st1);
		if (st->st2 != NULL)
			totalVariables += getReserveSizeForStatement(st->st2);
	}
	if (statement_type == WHILE_STATEMENT) {
		//cout<<"WHILE_STATEMENT"<<endl;
		WhileStatement* st = ((WhileStatement *)currentStatement);
		totalVariables += getReserveSizeForStatement(st->blockstatement);
	}
	if (statement_type == FOR_STATEMENT) {
		//cout<<"FOR_STATEMENT"<<endl;
		ForStatement* st = ((ForStatement *)currentStatement);
		totalVariables += 1;
		totalVariables += getReserveSizeForStatement(st->blockstatement);
	}
	if (statement_type == DECLARATION_STATEMENT) {
		//cout<<"DECLARATION_STATEMENT"<<endl;
		totalVariables += 1;
	}
	if (statement_type == ARRAY_DECLARATION_STATEMENT) {
		//cout<<"ARRAY_DECLARATION_STATEMENT"<<endl;
		ArrayDeclarationStatement* st = ((ArrayDeclarationStatement *)currentStatement);
		totalVariables += st->exprList.size();
	}
	//cout<<totalVariables<<endl;
	return totalVariables;
}

string getVariableAddress(string varName) {
	if (globalVariables.count(varName)) {
		return varName;
	}
	if (currentContext->functionParams.count(varName)) {
		int address = currentContext->functionParams[varName].second;
		return "ebp + " + to_string(address);
	}
	if (currentContext->variables.count(varName)) {
		int address = currentContext->variables[varName].second;
		return "ebp - " + to_string(address);
	}
	cout<<"Error, "<<varName<<" has not been declared in this scope."<<endl;
	terminate();
}

int getVariableType(string varName) {
	if (globalVariables.count(varName)) {
		return globalVariables[varName];
	}
	if (currentContext->functionParams.count(varName)) {
		return currentContext->functionParams[varName].first;
	}
	if (currentContext->variables.count(varName)) {
		return currentContext->variables[varName].first;
	}
	cout<<"Error, "<<varName<<" has not been declared in this scope."<<endl;
	terminate();
}

string newLabel() {
	string label = ".L" + std::to_string(labelCount);
	labelCount++;

	return label;
}

string newDataLabel() {
	string label = "L" + std::to_string(labelCount);
	labelCount++;

	return label;
}

string generatePowerFunction() {
	stringstream ss;
	ss << "pow:" << endl
	<< "push ebp" << endl
	<< "mov ebp, esp" << endl
	<< "sub esp, 4" << endl
	<< "mov dword[ebp - 4], 1" << endl
	<< "mov eax, dword[ebp + 12]" << endl
	<< "mov ebx, 0" << endl
	<< "cmp eax, ebx" << endl
	<< "je .L0" << endl
	<< "mov eax, 0" << endl
	<< "jmp .L1" << endl
	<< ".L0:" << endl
	<< "mov eax, 1" << endl
	<< ".L1:" << endl
	<< "mov ebx, eax" << endl
	<< "cmp ebx, 0" << endl
	<< "je .L2" << endl
	<< "mov eax, 1" << endl
	<< "leave" << endl
	<< "ret" << endl
	<< "jmp .L3" << endl
	<< ".L2:" << endl
	<< ".L3:" << endl
	<< ".L6:" << endl
	<< "mov eax, dword[ebp + 12]" << endl
	<< "mov ebx, 1" << endl
	<< "cmp eax, ebx" << endl
	<< "jge .L4" << endl
	<< "mov eax, 0" << endl
	<< "jmp .L5" << endl
	<< ".L4:" << endl
	<< "mov eax, 1" << endl
	<< ".L5:" << endl
	<< "mov ebx, eax" << endl
	<< "cmp ebx, 0" << endl
	<< "je .L7" << endl
	<< "mov eax, dword[ebp - 4]" << endl
	<< "mov ecx, dword[ebp + 8]" << endl
	<< "imul eax, ecx" << endl
	<< "mov dword[ebp - 4], eax" << endl
	<< "mov eax, dword[ebp + 12]" << endl
	<< "sub eax, 1" << endl
	<< "mov dword[ebp + 12], eax" << endl
	<< "jmp .L6" << endl
	<< ".L7:" << endl
	<< "mov eax, dword[ebp - 4]" << endl
	<< "mov eax, eax" << endl
	<< "leave" << endl
	<< "ret" << endl; 

	return ss.str();
}

string generateModuloFunction() {
	stringstream ss;
	ss << "mod:" << endl
	<< "push ebp" << endl
	<< "mov ebp, esp" << endl
	<< "sub esp, 4" << endl
	<< "mov eax, dword[ebp + 8]" << endl
	<< "mov dword[ebp - 4], eax" << endl
	<< ".L10:" << endl
	<< "mov eax, dword[ebp - 4]" << endl
	<< "mov ebx, dword[ebp + 12]" << endl
	<< "cmp eax, ebx" << endl
	<< "jge .L8" << endl
	<< "mov eax, 0" << endl
	<< "jmp .L9" << endl
	<< ".L8:" << endl
	<< "mov eax, 1" << endl
	<< ".L9:" << endl
	<< "cmp eax, 0" << endl
	<< "je .L11" << endl
	<< "mov ebx, dword[ebp - 4]" << endl
	<< "mov ecx, dword[ebp + 12]" << endl
	<< "sub ebx, ecx" << endl
	<< "mov dword[ebp - 4], ebx" << endl
	<< "jmp .L10" << endl
	<< ".L11:" << endl
	<< "mov eax, dword[ebp - 4]" << endl
	<< "mov eax, eax" << endl
	<< "leave" << endl
	<< "ret" << endl;

	return ss.str();
}

string generatePrintBoolFunction(){
	string returnThis;
	string true_label = newLabel();
	string end_label = newLabel();
	returnThis = "boolPrint:\n";
	returnThis += "push ebp\n";
	returnThis += "mov ebp, esp\n";
	returnThis += "cmp dword[ebp + 8], 1\n";
	returnThis += "je " + true_label + "\n";
	returnThis += "push false\n";
	returnThis += "call printf\n";
	returnThis += "add esp, 4\n";
	returnThis += "jmp " + end_label + "\n";
	returnThis +=  true_label + ":\n";
	returnThis += "push true\n";
	returnThis += "call printf\n";
	returnThis += "add esp, 4\n";
	returnThis += end_label + ":\n";
	returnThis += "pop ebp\n";
	returnThis += "ret\n"; 

	return returnThis;
}

string genDataSection() {
	stringstream ss;

	ss << "extern printf" << endl;
	ss << "global main" << endl;
	ss << "section .data" << endl;
	ss << globalVariablesString;
	ss << "format db \"%d\", 0" << endl;
	ss << "true db \"true\", 0" << endl;
	ss << "false db \"false\", 0" << endl;
	ss << "newLine db `\\n`, 0" << endl << endl;
	ss << "section .text" << endl;
	ss << "call main" << endl;
	ss << "ret" << endl << endl;
	ss << generatePrintBoolFunction() << endl;
	ss << generatePowerFunction() << endl;
	ss << generateModuloFunction() << endl << endl;

	return ss.str(); 
}

void tempInit() {
	tempInUse["eax"] = false;
	tempInUse["ebx"] = false;
	tempInUse["ecx"] = false;
	tempInUse["edx"] = false;
}

static string newTemp() {
	map<string, bool>::iterator it = tempInUse.begin();
	while (it != tempInUse.end()) {
		pair<string, bool> itm = *it;

		if (!itm.second) {
			tempInUse[itm.first] = true;
			//cout<<"Giving out: "<<itm.first<<endl;
			return itm.first;
		}
		it++;
	}
	cout<<"No var available!"<<endl;
}

string getTemp(string temp_name) {
	if (tempInUse.count(temp_name)) {
		if (tempInUse[temp_name] == false) {
			tempInUse[temp_name] = true;
			//cout<<"Giving out: "<<temp_name<<endl;
			return temp_name;
		} else{
			cout<<"Var "<<temp_name<<" is in use!"<<endl;
			terminate();
		}
	} else {
		cout<<"Var "<<temp_name<< " doesn't exist!"<<endl;
		terminate();
	}
}

static void releaseTemp(string temp) {
	if (tempInUse.count(temp)){
		//cout<<"Releasing: " << temp << endl;
		tempInUse[temp] = false;
	}
}

string BlockStatement::genFunctions() {
	stringstream ss;
	for (list<Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++) {
		int statement_type = (*i)->getKind();
		if (statement_type == FUNCTION_DECLARATION_STATEMENT){
			ss << (*i)->genCode() << endl;
		}
	}
	return ss.str();
}

void genLiteralStringsPerStatement(Statement* currentStatement){
	int statement_type = currentStatement->getKind();
	if (statement_type == BLOCK_STATEMENT){
		//cout<<"Found a block statement, gonna pull it's string now..."<<endl;
		BlockStatement* st = ((BlockStatement *)currentStatement);
		for (list<Statement*>::iterator i = st->statementList.begin(); i != st->statementList.end(); i++) {
			genLiteralStringsPerStatement((*i));
		}
	}
	if (statement_type == PRINT_STATEMENT) {
		PrintStatement* st = ((PrintStatement *)currentStatement);
		for (list<Expr*>::iterator i = st->exprList.begin(); i != st->exprList.end(); i++) {
			int expr_kind = (*i)->getKind();
			if (expr_kind == STRING_EXPRESSION){
				//cout<<"Found a string!"<<endl;
				StringExpression* strExp = ((StringExpression *)(*i));
				string newPlace = newDataLabel();
				globalVariablesString += newPlace + " db `" + strExp->str + "`, 0\n";
				strExp->str = newPlace;
			}
		}
	}
	if (statement_type == PRINTLINE_STATEMENT) {
		PrintLineStatement* st = ((PrintLineStatement *)currentStatement);
		for (list<Expr*>::iterator i = st->exprList.begin(); i != st->exprList.end(); i++) {
			int expr_kind = (*i)->getKind();
			if (expr_kind == STRING_EXPRESSION){
				//cout<<"Found a string!"<<endl;
				StringExpression* strExp = ((StringExpression *)(*i));
				string newPlace = newDataLabel();
				globalVariablesString += newPlace + " db `" + strExp->str + "`, 0\n";
				strExp->str = newPlace;
			}
		}
	}
	if (statement_type == IF_STATEMENT) {
		//cout<<"Found an if statement, gonna pull it's string now..."<<endl;
		IfStatement* st = ((IfStatement *)currentStatement);
		genLiteralStringsPerStatement(st->st1);
		if (st->st2 != NULL)
			genLiteralStringsPerStatement(st->st2);
	}
	if (statement_type == WHILE_STATEMENT) {
		//cout<<"Found a while statement, gonna pull it's string now..."<<endl;
		WhileStatement* st = ((WhileStatement *)currentStatement);
		genLiteralStringsPerStatement(st->blockstatement);
	}
	if (statement_type == FOR_STATEMENT) {
		//cout<<"Found a for statement, gonna pull it's string now..."<<endl;
		ForStatement* st = ((ForStatement *)currentStatement);
		genLiteralStringsPerStatement(st->blockstatement);
	}
	if (statement_type == FUNCTION_DECLARATION_STATEMENT) {
		//cout<<"Found a function declaration, gonna pull it's string now..."<<endl;
		FunctionDeclarationStatement* st = ((FunctionDeclarationStatement *)currentStatement);
		genLiteralStringsPerStatement(st->blockstatement);
	}
}

void BlockStatement::genLiteralStrings() {
	for (list<Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++) {
		genLiteralStringsPerStatement((*i));
	}
}

void setGlobalContext() {
	StatementContext *new_statement_context = new StatementContext;
	//new_statement_context->variables = globalVariables;
	//new_statement_context->functionParams = NULL;
	//new_statement_context->localVariableAddress = NULL;
	new_statement_context->isGlobal = true;
	new_statement_context->lockedContext = true;
	new_statement_context->statement_type = GLOBAL_CONTEXT;
	//new_statement_context->important_labels = NULL;
	new_statement_context->previous_context = NULL;
	new_statement_context->next_context = NULL;

	currentContext = new_statement_context;
}

void BlockStatement::getGlobalVariables(){
	for (list<Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++) {
		int statement_type = (*i)->getKind();
		if (statement_type == DECLARATION_STATEMENT){
			DeclarationStatement* st = ((DeclarationStatement *)(*i));
			if (globalVariables.count(st->varName)){
				cout<<"Error, "<<st->varName<<" has already been declared."<<endl;
				terminate();
			}
			globalVariables[st->varName] = st->type;
			globalVariablesString += st->varName + ": dd 0\n";
		}
		if (statement_type == ARRAY_DECLARATION_STATEMENT){
			ArrayDeclarationStatement* st = ((ArrayDeclarationStatement *)(*i));
			if (globalVariables.count(st->varName)){
				cout<<"Error, "<<st->varName<<" has already been declared."<<endl;
				terminate();
			}
			globalVariables[st->varName] = st->type;
			string inserGlobal = st->varName + ": dd ";
			int array_size = st->exprList.size();
			while(array_size > 0){
				inserGlobal += "0";
				if ((array_size-1) > 0)
					inserGlobal += ", ";
				array_size--;
			}
			inserGlobal += "\n";
			globalVariablesString += inserGlobal;
		}
	}
}

int BlockStatement::getGlobalLocalVariables(){
	int totalVariables = 0;
	for (list<Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++) {
		int statement_kind = (*i)->getKind();
		if (statement_kind == BLOCK_STATEMENT || statement_kind == IF_STATEMENT || statement_kind == WHILE_STATEMENT || statement_kind == FOR_STATEMENT){
			totalVariables +=getReserveSizeForStatement((*i));
			//cout<<totalVariables<<endl;
		}
	}
	return totalVariables;
}

void BlockStatement::GenerateFile()
{
	getGlobalVariables();
	setGlobalContext();
	int global_local_declarations = getGlobalLocalVariables();
	//cout<<"Amount of non-global declarations: "<<global_local_declarations<<endl;
	stringstream ss;
	genLiteralStrings();
	string generatedFunctions = genFunctions();
	string generated = genCode();
	ss << genDataSection() << endl;
	ss << generatedFunctions << endl;
	ss << "main:" << endl;
	ss << "push ebp" << endl;
	ss << "mov ebp, esp" << endl;
	if (global_local_declarations > 0)
		ss << "sub esp, " << (global_local_declarations * 4) << endl;
	ss << generated << endl;
	//ss << "pop ebp" << endl;
	ss << "mov eax, 0" << endl;
	//if (global_local_declarations > 0)
		ss << "leave" << endl;
	//ss << "pop ebp" << endl;
	ss << "ret" << endl;
	/*genLiteralStrings();
	ss << genDataSection() << endl;*/
	cout<<ss.str();
    std::ofstream out("result.S"); 
    out << ss.str(); 
    out.close();
}

/*Utilities End*/

/*Start Expressions*/

void LogicalOrExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	/*if (ctx2.isConstant) {
		string tempPlace = newTemp();
		ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
		ctx2.place = tempPlace;
	}
	if (ctx1.isConstant) {
		string tempPlace = newTemp();
		ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
		ctx1.place = tempPlace;
	}*/
	releaseTemp(ctx1.place);
	releaseTemp(ctx2.place);
	ctx.place = newTemp();
	//cout<<"code1: \n" << ctx1.code<<endl;
	//cout<<"code2: \n" << ctx2.code<<endl;

	string newcode;
	string end_label = newLabel();
	string true_label = newLabel();
    newcode = ctx1.code + "\n" + ctx2.code + "\n";
    newcode += "cmp " + ctx1.place + ", 1\n";
	newcode += "je " + true_label + "\n";
	newcode += "cmp " + ctx2.place + ", 1\n";
	newcode += "je " + true_label + "\n";
	newcode += "mov " + ctx.place + ", 0\n";
	newcode += "jmp " + end_label + "\n";
	newcode += true_label + ":\n";
	newcode += "mov " + ctx.place + ", 1\n";
	newcode += end_label + ":\n";
	ctx.code += newcode;
	ctx.isConstant = false;
}

void LogicalAndExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	/*if (ctx2.isConstant) {
		string tempPlace = newTemp();
		ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
		ctx2.place = tempPlace;
	}
	if (ctx1.isConstant) {
		string tempPlace = newTemp();
		ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
		ctx1.place = tempPlace;
	}*/
	releaseTemp(ctx1.place);
	releaseTemp(ctx2.place);
	ctx.place = newTemp();
	//cout<<"code1: \n" << ctx1.code<<endl;
	//cout<<"code2: \n" << ctx2.code<<endl;

	string newcode;
	string end_label = newLabel();
	string false_label = newLabel();
    newcode = ctx1.code + "\n" + ctx2.code + "\n";
    newcode += "cmp " + ctx1.place + ", 0\n";
	newcode += "je " + false_label + "\n";
	newcode += "cmp " + ctx2.place + ", 0\n";
	newcode += "je " + false_label + "\n";
	newcode += "mov " + ctx.place + ", 1\n";
	newcode += "jmp " + end_label + "\n";
	newcode += false_label + ":\n";
	newcode += "mov " + ctx.place + ", 0\n";
	newcode += end_label + ":\n";
	ctx.code += newcode;
	ctx.isConstant = false;
}

void InclusiveOrExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue | ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx2.place);
	    ctx.place = ctx1.place;
	    ctx.code += "or " + ctx1.place + ", " + ctx2.place + "\n";
		ctx.isConstant = false;
	}
}

void ExclusiveOrExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue ^ ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx2.place);
	    ctx.place = ctx1.place;
	    ctx.code += "xor " + ctx1.place + ", " + ctx2.place + "\n";
		ctx.isConstant = false;
	}
}

void AndExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue & ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx2.place);
	    ctx.place = ctx1.place;
	    ctx.code += "and " + ctx1.place + ", " + ctx2.place + "\n";
		ctx.isConstant = false;
	}
}

void compareRelational(ExprContext &ctx, Expr *expr1, Expr *expr2, string comparison){
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);
	if (ctx1.isConstant){
		ctx1.place = newTemp();
		ctx1.code = "mov " + ctx1.place + ", " + to_string(ctx1.numberValue) + "\n";
	}

	if (ctx2.isConstant){
		ctx2.place = newTemp();
		ctx2.code = "mov " + ctx2.place + ", " + to_string(ctx2.numberValue) + "\n";
	}
	
	releaseTemp(ctx1.place);
	releaseTemp(ctx2.place);
	
    string newcode;
	string newplace = newTemp();
	string true_label = newLabel();
	string end_label = newLabel();
    newcode = ctx1.code + "\n" + ctx2.code + "\n";
    newcode += "cmp " + ctx1.place + ", " + ctx2.place + "\n";
	newcode += comparison + true_label + "\n";
	newcode += "mov " + newplace + ", 0\n";
	newcode += "jmp " + end_label + "\n";
	newcode += true_label + ":\n";
	newcode += "mov " + newplace + ", 1\n";
	newcode += end_label + ":\n";
	//cout<<"ctx1.code: \n"<<ctx1.code<<endl;
	//cout<<"ctx2.code: \n"<<ctx2.code<<endl;

	ctx.code = newcode;
    ctx.place = newplace;
}

void EqualityExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void EqualExpression::genCode(ExprContext &ctx) {
	compareRelational(ctx, expr1, expr2, "je ");
}

void NotEqualExpression::genCode(ExprContext &ctx) {
	compareRelational(ctx, expr1, expr2, "jne ");
}

void RelationalExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void LessThanExpression::genCode(ExprContext &ctx) {
	compareRelational(ctx, expr1, expr2, "jl ");
}

void GreaterThanExpression::genCode(ExprContext &ctx) {
	compareRelational(ctx, expr1, expr2, "jg ");
}

void LessEqualExpression::genCode(ExprContext &ctx) {
	compareRelational(ctx, expr1, expr2, "jle ");
}

void GreaterEqualExpression::genCode(ExprContext &ctx) {
	compareRelational(ctx, expr1, expr2, "jge ");
}

void ShiftExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ShiftLeftExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue << ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx2.place);
	    ctx.place = ctx1.place;
	    ctx.code += "sal " + ctx1.place + ", " + ctx2.place + "\n";
		ctx.isConstant = false;
	}
}

void ShiftRightExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue >> ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx2.place);
	    ctx.place = ctx1.place;
	    ctx.code += "sar " + ctx1.place + ", " + ctx2.place + "\n";
		ctx.isConstant = false;
	}
}

void AdditiveExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void AddExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
    expr2->genCode(ctx2);

	ctx.isConstant = false;
	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue + ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else if (ctx2.isConstant) {
		ctx.code = ctx1.code + "\n";
		ctx.place = ctx1.place;
		ctx.code += "add " + ctx.place + ", " + ctx2.place;
	} else if (ctx1.isConstant) {
		ctx.code = ctx2.code + "\n";
		ctx.place = ctx2.place;
		ctx.code += "add " + ctx.place + ", " + ctx1.place;
	} else {
		ctx.code = ctx1.code + "\n" + ctx2. code + "\n";
		releaseTemp(ctx2.place);
		ctx.place = ctx1.place;
		ctx.code += "add " + ctx.place + ", " + ctx2.place;
	}
}

void SubExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
    expr2->genCode(ctx2);

	ctx.isConstant = false;
	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue - ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else if (ctx2.isConstant) {
		ctx.code = ctx1.code + "\n";
		ctx.place = ctx1.place;
		ctx.code += "sub " + ctx.place + ", " + ctx2.place;
	} else if (ctx1.isConstant) {
		string tempPlace = newTemp();
		releaseTemp(ctx2.place);
		ctx.code = ctx2.code + "\n" +
				   "mov " + tempPlace + ", " + ctx1.place + "\n";
		ctx.place = tempPlace;
		ctx.code += "sub "+ ctx.place + ", " + ctx2.place;
	} else {
		ctx.code = ctx1.code + "\n" + ctx2. code + "\n";
		releaseTemp(ctx2.place);
	    ctx.place = ctx1.place;
	    ctx.code += "sub "+ ctx.place + ", " + ctx2.place;
	}
}

void MultiplicativeExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void MulExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
	expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue * ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx2.place);
	    ctx.place = ctx1.place;
	    ctx.code += "imul " + ctx1.place + ", " + ctx2.place + "\n";
		ctx.isConstant = false;
	}
}

void DivExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
    expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = ctx1.numberValue / ctx2.numberValue;
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";

		string previous = ctx2.place;
		if (ctx2.place == "eax"){
			string tempPlace = newTemp();
			
			ctx.code += "mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
			releaseTemp(ctx2.place);
		}
	    releaseTemp(ctx1.place);
	    releaseTemp(previous);
	    ctx.place = getTemp("eax");		
		ctx.code += "mov " + ctx.place + ", " + ctx1.place + "\n";
		ctx.code += "cdq\n";
	    ctx.code += "idiv " + ctx2.place + "\n";
		ctx.isConstant = false;
	}
}

void ModExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
    expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = (ctx1.numberValue % ctx2.numberValue);
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx1.place);
	    releaseTemp(ctx2.place);
	    ctx.place = getTemp("eax");		
		ctx.code += "push " + ctx2.place + "\n";
		ctx.code += "push " + ctx1.place + "\n";
		ctx.code += "call mod\n";
		ctx.code += "add esp, 8\n";
		ctx.isConstant = false;
	}
}

void ExponentialExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void ExpoExpression::genCode(ExprContext &ctx) {
	ExprContext ctx1;
    ExprContext ctx2;

    expr1->genCode(ctx1);
    expr2->genCode(ctx2);

	if (ctx1.isConstant && ctx2.isConstant) {
		ctx.numberValue = pow(ctx1.numberValue, ctx2.numberValue);
		ctx.place = to_string(ctx.numberValue);
		ctx.isConstant = true;
	} else {
		if (ctx2.isConstant) {
			string tempPlace = newTemp();
			ctx2.code ="mov " + tempPlace + ", " + ctx2.place + "\n";
			ctx2.place = tempPlace;
		}
		if (ctx1.isConstant) {
			string tempPlace = newTemp();
			ctx1.code = "mov " + tempPlace + ", " + ctx1.place + "\n";
			ctx1.place = tempPlace;
		}
		ctx.code = ctx1.code + "\n" + ctx2.code + "\n";
	    releaseTemp(ctx1.place);
	    releaseTemp(ctx2.place);
	    ctx.place = getTemp("eax");		
		ctx.code += "push " + ctx2.place + "\n";
		ctx.code += "push " + ctx1.place + "\n";
		ctx.code += "call pow\n";
		ctx.code += "add esp, 8\n";
		ctx.isConstant = false;
	}
}

void BitwiseNotExpression::genCode(ExprContext &ctx) {
	stringstream ss;
	ExprContext ctx1;
	expr->genCode(ctx1);
	if (ctx1.isConstant) {
		string tempPlace = newTemp();
		ctx1.code ="mov " + tempPlace + ", " + ctx1.place + "\n";
		ctx1.place = tempPlace;
	}
	ctx.code = ctx1.code + "\n";
	releaseTemp(ctx1.place);
	ctx.place = newTemp();
	int expr_kind = expr->getKind();
	//cout<<expr_kind<<endl;
	if (expr_kind == BOOL_EXPRESSION || expr_kind == NEGATION_EXPRESSION || expr_kind == EQUAL_EXPRESSION || expr_kind == NOT_EQUAL_EXPRESSION
		|| expr_kind == LESS_THAN_EXPRESSION || expr_kind == GREATER_THAN_EXPRESSION || expr_kind == LESS_EQUAL_EXPRESSION
		|| expr_kind == GREATER_EQUAL_EXPRESSION){
			string negate_false = newLabel();
			string end_label = newLabel();
			ss << "cmp " << ctx1.place << ", 0" << endl
			   << "je " << negate_false << endl
			   << "mov " << ctx.place << ", 0" << endl
			   << "jmp " << end_label << endl
			   << negate_false << ":" << endl
			   << "mov " << ctx.place << ", 1" << endl
			   << end_label << ":" << endl;
	}
	else if(expr_kind == ID_EXPRESSION){
		IdExpression* idExpr = ((IdExpression *)(expr));
		int expr_type = getVariableType(idExpr->varName);
		if (expr_type == BOOL_T){
			string negate_false = newLabel();
			string end_label = newLabel();
			ss << "cmp " << ctx1.place << ", 0" << endl
			   << "je " << negate_false << endl
			   << "mov " << ctx.place << ", 0" << endl
			   << "jmp " << end_label << endl
			   << negate_false << ":" << endl
			   << "mov " << ctx.place << ", 1" << endl
			   << end_label << ":" << endl;
		}
		else{
			ss << "mov " << ctx.place << ", " << ctx1.place << endl
			<< "not " << ctx.place << endl;
		}
	}
	else{
		ss << "mov " << ctx.place << ", " << ctx1.place << endl
		   << "not " << ctx.place << endl;
	}
	ctx.code += ss.str();
	ctx.isConstant = false;
}

void NegationExpression::genCode(ExprContext &ctx) {
	stringstream ss;
	ExprContext ctx1;
	int expr_kind = expr->getKind();
	//cout<<expr_kind<<endl;
	if (expr_kind != BOOL_EXPRESSION && expr_kind != GREATER_EQUAL_EXPRESSION && expr_kind != LESS_EQUAL_EXPRESSION && expr_kind != GREATER_THAN_EXPRESSION && 
		expr_kind != LESS_THAN_EXPRESSION && expr_kind != NOT_EQUAL_EXPRESSION && expr_kind != EQUAL_EXPRESSION && expr_kind != ID_EXPRESSION &&
		expr_kind != ARRAY_ACCESS_EXPRESSION && expr_kind != FUNCTION_EXPRESSION) {
			cout<<"Error, negation called on non boolean expression."<<endl;
			terminate();
	}
	if (expr_kind == ID_EXPRESSION) {
		IdExpression* idExpr = ((IdExpression *)(expr));
		int expr_type = getVariableType(idExpr->varName);
		if (expr_type != BOOL_T){
			cout<<"Error, negation called on non boolean variable."<<endl;
			terminate();
		}
	}
	if (expr_kind == ARRAY_ACCESS_EXPRESSION) {
		ArrayAccessExpression* idExpr = ((ArrayAccessExpression *)(expr));
		int expr_type = getVariableType(idExpr->varName);
		if (expr_type != BOOL_T){
			cout<<"Error, negation called on non boolean variable."<<endl;
			terminate();
		}
	}
	if (expr_kind == FUNCTION_EXPRESSION) {
		FunctionExpression* funcExpr = ((FunctionExpression *)(expr));
		int expr_type = functionTypes[funcExpr->varName];
		if (expr_type != BOOL_T){
			cout<<"Error, negation called on non boolean function."<<endl;
			terminate();
		}
	}
	expr->genCode(ctx1);
	if (ctx1.isConstant) {
		string tempPlace = newTemp();
		ctx1.code ="mov " + tempPlace + ", " + ctx1.place + "\n";
		ctx1.place = tempPlace;
	}
	ctx.code = ctx1.code + "\n";
	releaseTemp(ctx1.place);
	ctx.place = newTemp();
	string negate_false = newLabel();
	string end_label = newLabel();
	ss << "cmp " << ctx1.place << ", 0" << endl
	   << "je " << negate_false << endl
	   << "mov " << ctx.place << ", 0" << endl
	   << "jmp " << end_label << endl
	   << negate_false << ":" << endl
	   << "mov " << ctx.place << ", 1" << endl
	   << end_label << ":" << endl;
	ctx.code += ss.str();
	ctx.isConstant = false;
}

void PosfixExpression::genCode(ExprContext &ctx) {
	/*Do some generating, baby!*/
}

void IdExpression::genCode(ExprContext &ctx) {
	ctx.place = newTemp();
	ctx.code = "mov " + ctx.place + ", dword[" + getVariableAddress(varName) + "]\n";
	ctx.isConstant = false;
}

void NumExpression::genCode(ExprContext &ctx) {
	ctx.numberValue = number;
	ctx.place = to_string(number);
	ctx.isConstant = true;
    ctx.code = "";
}

void BoolExpression::genCode(ExprContext &ctx) {
	ctx.place = to_string(boolInt);
	ctx.code = "";
	ctx.isConstant = true;
}

void ArrayAccessExpression::genCode(ExprContext &ctx) {
	stringstream ss;
	ExprContext position_ctx;
	position->genCode(position_ctx);
	ctx.place = newTemp();
	if (position_ctx.isConstant) {
		ss << "mov " << ctx.place << ", dword[" << getVariableAddress(varName) << " + (4 * " << to_string(stoi(position_ctx.place)-1) << ")]" << endl;
	}
	else {
		ss << position_ctx.code << endl;
		string tempPlace = newTemp();
		ss << "mov " + tempPlace + ", 4\n";
		releaseTemp(tempPlace);
		ss <<  "sub " << position_ctx.place + ", 1" << endl;
		ss <<  "imul " << position_ctx.place + ", " << tempPlace << endl;
		ss << "mov " << ctx.place << ", dword[" << getVariableAddress(varName) << " + " << position_ctx.place << "]" << endl;
	}
	releaseTemp(position_ctx.place);	
	ctx.code = ss.str();
	ctx.isConstant = false;
}

void FunctionExpression::genCode(ExprContext &ctx) {
	/*if (exprList.size() != functionArguments[varName]){
		printf("Error: Supplied arguments do not match quantity needed by function.\n"); 
		exit(0);
		}*/
	/*TODO: check arguments vs function*/
	for (list<Expr*>::reverse_iterator it=exprList.rbegin(); it!=exprList.rend(); ++it){
		ExprContext currentCtx;
		(*it)->genCode(currentCtx);
		releaseTemp(currentCtx.place);
		ctx.code += currentCtx.code + "\n";
		ctx.code += "push " + currentCtx.place + "\n";
	}
	ctx.code += "call " + varName + "\n";
	if(exprList.size() > 0)
		ctx.code += "add esp, " + to_string(exprList.size() * 4) + "\n";
	ctx.place = getTemp("eax");
	ctx.isConstant = false;
}

void StringExpression::genCode(ExprContext &ctx) {
	ctx.place = str;
	ctx.code = "";
}

/*End Expressions*/

/*Start Statements*/

string ArrayDeclarationStatement::genCode() {
	stringstream ss;
	if (currentContext->isGlobal){
		/*Do nothing cause it's been declared already*/
	}
	else {
		registerLocalArray(varName, type, exprList.size());
	}
	int index = 0;
	for (list<Expr*>::iterator i = exprList.begin(); i != exprList.end(); i++){
		ExprContext ctx;
		(*i)->genCode(ctx);
		if (ctx.isConstant) {
			ss << "mov dword[" << getVariableAddress(varName) << " + (4 * " << index << ")], " << ctx.place << endl;
		} else{
			ss << ctx.code << endl;
			ss << "mov dword[" << getVariableAddress(varName) << " + (4 * " << index << ")], " << ctx.place << endl;
		}
		releaseTemp(ctx.place);
		index++;
	}
	return ss.str();
}

string ArrayAssignStatement::genCode() {
	stringstream ss;
	ExprContext position_ctx;
	ExprContext ctx;

	expr->genCode(ctx);
	position->genCode(position_ctx);
	if (ctx.isConstant && position_ctx.isConstant) {
		ss << "mov dword[" << getVariableAddress(varName) << " + (4 * " << to_string(stoi(position_ctx.place)-1) << ")], " << ctx.place << endl;
	} else if (position_ctx.isConstant){
		ss << ctx.code << endl;
		ss << "mov dword[" << getVariableAddress(varName) << " + (4 * " << to_string(stoi(position_ctx.place)-1) << ")], " << ctx.place << endl;
	} else {
		ss << position_ctx.code << endl;
		string tempPlace = newTemp();
		ss << "mov " + tempPlace + ", 4\n";
		releaseTemp(tempPlace);
		ss <<  "sub " << position_ctx.place + ", 1" << endl;
		ss <<  "imul " << position_ctx.place + ", " << tempPlace << endl;
		if (ctx.isConstant) {
			ss << "mov dword[" << getVariableAddress(varName) << " + " << position_ctx.place << "], " << ctx.place << endl;
		}
		else {
			ss << ctx.code << endl;
			ss << "mov dword[" << getVariableAddress(varName) << " + " << position_ctx.place << "], " << ctx.place << endl;
		}
	}
	releaseTemp(ctx.place);
	releaseTemp(position_ctx.place);
	return ss.str();
}

string DeclarationStatement::genCode() {
	stringstream ss;
	ExprContext ctx;

	expr->genCode(ctx);

	if (currentContext->isGlobal){
		/*Do nothing cause it's been declared already*/
	}
	else {
		if(currentContext->variables.count(varName)) {
			cout<<"Error, "<<varName<<" has already been declared."<<endl;
			terminate();
		}
		else{
			registerLocalVariable(varName, type);
		}
	}
	if (ctx.isConstant) {
		ss << "mov dword[" << getVariableAddress(varName) << "], " << ctx.place;
	} else{
		ss << ctx.code << endl;
		ss << "mov dword[" << getVariableAddress(varName) << "], " << ctx.place;
	}
	releaseTemp(ctx.place);
	return ss.str();
}

string AssignStatement::genCode() {
	stringstream ss;
	ExprContext ctx;

	expr->genCode(ctx);
	if (ctx.isConstant) {
		ss << "mov dword[" << getVariableAddress(varName) << "], " << ctx.place;
	} else{
		ss << ctx.code << endl;
		ss << "mov dword[" << getVariableAddress(varName) << "], " << ctx.place;
	}
	releaseTemp(ctx.place);
	return ss.str();
}

string BlockStatement::genCode() {
	stringstream ss;
	for (list<Statement*>::iterator i = statementList.begin(); i != statementList.end(); i++) {
		if ((*i)->getKind() != FUNCTION_DECLARATION_STATEMENT){
			ss << (*i)->genCode() << endl;
		}
	}
	return ss.str();
}

string genPrintCode(ExpressionList exprList) {
	stringstream ss;
	for (list<Expr*>::iterator i = exprList.begin(); i != exprList.end(); i++) {
		ExprContext ctx;
		int expr_kind = (*i)->getKind();
		(*i)->genCode(ctx);
		if (expr_kind == STRING_EXPRESSION){
			ss << ctx.code << endl
			<< "mov eax, " << ctx.place << endl
			<< "push eax" << endl
			<< "call printf" << endl
			<< "add esp, 4" << endl;
		} 
		else if (expr_kind == BOOL_EXPRESSION || expr_kind == NEGATION_EXPRESSION || expr_kind == EQUAL_EXPRESSION || expr_kind == NOT_EQUAL_EXPRESSION
		|| expr_kind == LESS_THAN_EXPRESSION || expr_kind == GREATER_THAN_EXPRESSION || expr_kind == LESS_EQUAL_EXPRESSION
		|| expr_kind == GREATER_EQUAL_EXPRESSION || expr_kind == LOGICAL_OR_EXPRESSION || expr_kind == LOGICAL_AND_EXPRESSION){
			ss << ctx.code << endl
			<< "mov eax, " << ctx.place << endl
			<< "push eax" << endl
			<< "call boolPrint" << endl
			<< "add esp, 4" << endl;
		}
		else if(expr_kind == BITWISE_NOT_EXPRESSION){
			BitwiseNotExpression* bitwiseExpr = ((BitwiseNotExpression *)(*i));
			int internal_kind = bitwiseExpr->expr->getKind();
			if (internal_kind == BOOL_EXPRESSION || internal_kind == NEGATION_EXPRESSION || internal_kind == EQUAL_EXPRESSION || internal_kind == NOT_EQUAL_EXPRESSION
				|| internal_kind == LESS_THAN_EXPRESSION || internal_kind == GREATER_THAN_EXPRESSION || internal_kind == LESS_EQUAL_EXPRESSION
				|| internal_kind == GREATER_EQUAL_EXPRESSION ){
					ss << ctx.code << endl
					<< "mov eax, " << ctx.place << endl
					<< "push eax" << endl
					<< "call boolPrint" << endl
					<< "add esp, 4" << endl;
			} 
			else if (internal_kind == ID_EXPRESSION){
				IdExpression* idExpr = ((IdExpression *)(bitwiseExpr->expr));
				int expr_type = getVariableType(idExpr->varName);
				if (expr_type == BOOL_T){
					ss << ctx.code << endl
					<< "mov eax, " << ctx.place << endl
					<< "push eax" << endl
					<< "call boolPrint" << endl
					<< "add esp, 4" << endl;
				}
				else {
					ss << ctx.code << endl
					<< "mov eax, " << ctx.place << endl
					<< "push eax" << endl
					<< "push format" << endl
					<< "call printf" << endl
					<< "add esp, 8" << endl;
				}
			}
			else {
				ss << ctx.code << endl
				<< "mov eax, " << ctx.place << endl
				<< "push eax" << endl
				<< "push format" << endl
				<< "call printf" << endl
				<< "add esp, 8" << endl;
			}
		}
		else if(expr_kind == ID_EXPRESSION) {
			IdExpression* idExpr = ((IdExpression *)(*i));
			int expr_type = getVariableType(idExpr->varName);
			if (expr_type == BOOL_T){
				ss << ctx.code << endl
				<< "mov eax, " << ctx.place << endl
				<< "push eax" << endl
				<< "call boolPrint" << endl
				<< "add esp, 4" << endl;
			}else{
				ss << ctx.code << endl
				<< "mov eax, " << ctx.place << endl
				<< "push eax" << endl
				<< "push format" << endl
				<< "call printf" << endl
				<< "add esp, 8" << endl;
			}
		}
		/*TODO: validate array access and function return for booleans*/
		else if (expr_kind == ARRAY_ACCESS_EXPRESSION) {
			ArrayAccessExpression* arrayExpr = ((ArrayAccessExpression *)(*i));
			int expr_type = getVariableType(arrayExpr->varName);
			if (expr_type == BOOL_T){
				ss << ctx.code << endl
				<< "mov eax, " << ctx.place << endl
				<< "push eax" << endl
				<< "call boolPrint" << endl
				<< "add esp, 4" << endl;
			}else{
				ss << ctx.code << endl
				<< "mov eax, " << ctx.place << endl
				<< "push eax" << endl
				<< "push format" << endl
				<< "call printf" << endl
				<< "add esp, 8" << endl;
			}
		}
		else if (expr_kind == FUNCTION_EXPRESSION) {
			FunctionExpression* funcExpr = ((FunctionExpression *)(*i));
			int func_type = functionTypes[funcExpr->varName];
			if (func_type == BOOL_T){
				ss << ctx.code << endl
				<< "mov eax, " << ctx.place << endl
				<< "push eax" << endl
				<< "call boolPrint" << endl
				<< "add esp, 4" << endl;
			}else{
				ss << ctx.code << endl
				<< "mov eax, " << ctx.place << endl
				<< "push eax" << endl
				<< "push format" << endl
				<< "call printf" << endl
				<< "add esp, 8" << endl;
			}
		}
		else {
			ss << ctx.code << endl
			<< "mov eax, " << ctx.place << endl
			<< "push eax" << endl
			<< "push format" << endl
			<< "call printf" << endl
			<< "add esp, 8" << endl;
		}
		releaseTemp(ctx.place);
	}
	return ss.str();
}

string PrintStatement::genCode() {
	return genPrintCode(exprList);
}

string PrintLineStatement::genCode() {
	string generatedCode = genPrintCode(exprList);
	generatedCode += "push newLine\n";
	generatedCode += "call printf\n";
	generatedCode += "add esp, 4\n";
	return generatedCode;
}

string IfStatement::genCode() {
	stringstream ss;
	ExprContext ctx;

	/*TODO: check if expr returns a bool, not an int*/
	expr->genCode(ctx);
	//cout<< "Expr kind: "<< expr->getKind() <<endl;

	string labelFalse = newLabel();
	string labelEndIf = newLabel();

	if (ctx.isConstant){
		string newPlace = newTemp();
		ctx.code += "mov " + newPlace + ", " + ctx.place;
		releaseTemp(ctx.place);
		ctx.place = newPlace;
	}

	list<string> import_labels;
	import_labels.push_back(labelFalse);
	import_labels.push_back(labelEndIf);

	ss << ctx.code << endl
	   << "cmp " << ctx.place << ", 0" << endl;
	   releaseTemp(ctx.place);
	ss << "je " << labelFalse << endl;
	   push_context(IF_STATEMENT, import_labels, false);
	ss << st1->genCode() << endl;
	   pop_context();
	ss << "jmp " << labelEndIf << endl
	   << labelFalse << ":" << endl;

	if (st2 != NULL) {
		push_context(IF_STATEMENT, import_labels, false);
		ss << st2->genCode();
		pop_context();
	}
	ss << labelEndIf << ":\n";
	return ss.str();
}

string BreakStatement::genCode() {
	stringstream ss;
	StatementContext* iterator_context = currentContext;
	while(iterator_context->statement_type != GLOBAL_CONTEXT) {
		//cout<<iterator_context->statement_type<<endl;
		if(iterator_context->statement_type == WHILE_STATEMENT || iterator_context->statement_type == FOR_STATEMENT){
			auto it = std::next(iterator_context->important_labels.begin(), 1);
			ss << "jmp " << *it << endl;
			break;
		}
		iterator_context = iterator_context->previous_context;
	}
	if (iterator_context->statement_type == GLOBAL_CONTEXT){
		cout<<"Error. Break statement outside of loop."<<endl;
		terminate();
	}
	return ss.str();
}

string ContinueStatement::genCode() {
	stringstream ss;
	StatementContext* iterator_context = currentContext;
	while(iterator_context->statement_type != GLOBAL_CONTEXT) {
		//cout<<iterator_context->statement_type<<endl;
		if(iterator_context->statement_type == WHILE_STATEMENT || iterator_context->statement_type == FOR_STATEMENT){
			ss << "jmp " << iterator_context->important_labels.front() << endl;
			break;
		}
		iterator_context = iterator_context->previous_context;
	}
	if (iterator_context->statement_type == GLOBAL_CONTEXT){
		cout<<"Error. Continue statement outside of loop."<<endl;
		terminate();
	}
	return ss.str();
}

string WhileStatement::genCode() {
	stringstream ss;
	ExprContext ctx;

	expr->genCode(ctx);

	string labelWhile = newLabel();
	string labelEndW = newLabel();
	list<string> import_labels;
	import_labels.push_back(labelWhile);
	import_labels.push_back(labelEndW);
	if (ctx.isConstant){
		string newPlace = newTemp();
		ctx.code += "mov " + newPlace + ", " + ctx.place;
		releaseTemp(ctx.place);
		ctx.place = newPlace;
	}

	ss << labelWhile << ":" << endl
	   << ctx.code << endl
	   << "cmp " << ctx.place << ", 0" << endl;;
	   releaseTemp(ctx.place);
	ss << "je " << labelEndW << endl;
	push_context(WHILE_STATEMENT, import_labels, false);
	ss << blockstatement->genCode() << endl;
	pop_context();
	ss << "jmp " << labelWhile << endl
	   << labelEndW << ":" << endl;
	
	return ss.str();
}

string ForStatement::genCode() {
	stringstream ss;
	ExprContext ctx1;
	ExprContext ctx2;

	startingExpr->genCode(ctx1);
	endingExpr->genCode(ctx2);

	string labelFor = newLabel();
	string labelEndF = newLabel();
	list<string> import_labels;
	import_labels.push_back(labelFor);
	import_labels.push_back(labelEndF);
	if (ctx1.isConstant){
		string newPlace = newTemp();
		ctx1.code += "mov " + newPlace + ", " + ctx1.place;
		releaseTemp(ctx1.place);
		ctx1.place = newPlace;
	}
	if (ctx2.isConstant){
		string newPlace = newTemp();
		ctx2.code += "mov " + newPlace + ", " + ctx2.place;
		releaseTemp(ctx2.place);
		ctx2.place = newPlace;
	}
	push_context(FOR_STATEMENT, import_labels, false);
	registerLocalVariable(id, INT_T);
	ss << ctx1.code << endl;
	ss << "mov dword[" << getVariableAddress(id) << "], " << ctx1.place << endl;
	ss << "sub dword[" << getVariableAddress(id) << "], 1" << endl;
	ss << labelFor << ":" << endl
	   << "add dword[" << getVariableAddress(id) << "], 1" << endl
	   << ctx2.code << endl
	   << "cmp dword[" << getVariableAddress(id) << "], " << ctx2.place << endl
	   << "jg " << labelEndF << endl;
	releaseTemp(ctx1.place);
	releaseTemp(ctx2.place);
	ss << blockstatement->genCode() << endl;
	//ss << "add dword[" << getVariableAddress(id) << "], 1" << endl;
	pop_context();
	ss << "jmp " << labelFor << endl
	   << labelEndF << ":" << endl;
	
	return ss.str();
}

string FunctionDeclarationStatement::genCode() {
	/*TODO: Override*/
	stringstream ss;
	BlockStatement* blockStmt = (BlockStatement*)blockstatement;
	bool foundReturn = false;
	for (Statement *st : blockStmt->statementList) {
		if ((*st).getKind() == RETURN_STATEMENT){
			//cout<<"Found return statement!" << endl;
			foundReturn = true;
		}
	}
	if (!foundReturn){
		//cout<<"No return statement found, adding one...." << endl; 
		blockStmt->addStatement(new ReturnStatement(new NumExpression(0)));
	}
	registerFunction(varName, *varList, type);
	list<string> import_labels;
	push_context(FUNCTION_DECLARATION_STATEMENT, import_labels, true);
	for (std::map<string, int>::iterator it = varList->begin(); it != varList->end(); ++it)
	{
	  registerParam(it->first, it->second);
	}
	int totalVariables = getReserveSizeForStatement(blockStmt);
	ss << varName << ":" << endl;
	ss << "push ebp" << endl;
	ss << "mov ebp, esp" << endl;
	if (totalVariables > 0)
		ss << "sub esp, " << (totalVariables * 4) << endl;
	ss << blockStmt->genCode();
	pop_context();
	return ss.str();
}

string FunctionCallStatement::genCode() {
	stringstream ss;
	/*if (exprList.size() != functionArguments[varName]){
		printf("Error: Supplied arguments do not match quantity needed by function.\n"); 
		exit(0);
		}*/
	/*TODO: check arguments vs function*/
	for (list<Expr*>::reverse_iterator it=exprList.rbegin(); it!=exprList.rend(); ++it){
		ExprContext currentCtx;
		(*it)->genCode(currentCtx);
		releaseTemp(currentCtx.place);
		ss << currentCtx.code << endl
		   << "push " << currentCtx.place << endl;
	}
	ss << "call " << varName << endl;
	if(exprList.size() > 0)
		ss << "add esp, " << to_string(exprList.size() * 4) << endl;
	
	return ss.str();
}

string ReturnStatement::genCode() {
	/*TODO: check that return is only in functions*/
	stringstream ss;
	ExprContext ctx;
	expr->genCode(ctx);
	releaseTemp(ctx.place);
	ss << ctx.code << endl;
	ss << "mov eax, " << ctx.place << endl;
	ss << "leave"<<endl;
	ss << "ret"<<endl;

	return ss.str();
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