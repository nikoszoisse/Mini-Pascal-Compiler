//============================================================================
// Name        : MinPascalCompilier.cpp
// Author      : Nikos Zois 2054 Marina Bourdaki 2111
// Version     :
// Copyright   : @Nz Cs Uoi
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include "tools/Utils.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"

using namespace std;

int main(int argc, const char *argv[]) {

	//Check if we choosed a file for compile
	if(argc < 2){
		cout << "Please Select a file to compile" << endl;
	}
	else{
		fstream fileToCompile;

		fileToCompile.open(argv[1],ios::in);

		if(fileToCompile.is_open()){
			//Create Lexical Instance and then Create Syntax Instance
			LexicalAnalyzer *lex = new LexicalAnalyzer(fileToCompile);
			SyntaxAnalyzer *syntax = new SyntaxAnalyzer(lex);
			syntax->checkSyntax();
			cout <<"~~~~~~~~~~~~END LIST~~~~~~~~~~"<<endl;
			syntax->printList();
		}
		else{
			cout << "FATAL ERROR: Cant open file: " << argv[1]<<endl; ;
			cout << "\t --> Check if you gave a file" << endl;
			cout << "\t --> Be sure thats is the correct path" << endl;
		}
		fileToCompile.close();
	}
	return 0;
}
