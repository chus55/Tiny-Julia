#include <stdio.h>
#include "ast.h"
#include "tokens.h"

extern Statement *input;
extern FILE *yyin;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Incorrect usage; usage: %s <input file>\n", argv[0]);
		return 1;
	}
	yyin  = fopen(argv[1], "rb");
	if (yyin == NULL) {
		fprintf(stderr, "Can't open file %s\n", argv[1]);
		return 2;
	}
	input = NULL;
	tempInit();
    yyparse();
    if (input != 0){
		//input->execute();
		//((BlockStatement *)input)->printStatement();
		((BlockStatement *)input)->GenerateFile();
	}
	//cout<<"/--------------------------------------------------------------------------/"<<endl;
}
