/*
 * SyntaxAnalyzer.h
 *
 *  Created on: 7 Ìבס 2014
 *      Author:  Marina Mpourdakh 2111 Nikos Zois 2054
 */

#ifndef SYNTAXANALYZER_H_
#define SYNTAXANALYZER_H_

#include "LexicalAnalyzer.h"
#include "tools/LQList.h"
#include "tools/tokens.h"
#include "symbol_Array/Scope.h"
#include "symbol_Array/Entity.h"
#include "symbol_Array/Result.h"

class SyntaxAnalyzer;

class EndCodeEngine {
private:
	SyntaxAnalyzer* syntax;
	stringstream assemblyStream;
	fstream assemblyFile;
	int parametr_number;
	int progLabelNum;
public:
	EndCodeEngine(SyntaxAnalyzer*);
	virtual ~EndCodeEngine();
	//TODO Create Symbol Array Class Means not SyntaxAnalyzer Usage
	void gnlvcode(string);
	void loadvr(string,int);
	void storevr(int,string);
	void generateEndCode(LabelQuad*);
	void generateParEndCode(LabelQuad*);
	void tryGenerateRealOperator(LabelQuad*);
	void tryGenerateOperator(LabelQuad*);
	void printFile(string);
	void printConsole();
};

class SyntaxAnalyzer {
public:
	list<Scope*> scopeList; //symbolArray
	LexicalAnalyzer* lex;
	Result* findVar(string);
	SyntaxAnalyzer(LexicalAnalyzer *);
	virtual ~SyntaxAnalyzer();
	void checkSyntax();
	void printList();
	string getProgramName();
	Scope *current; //symbolArray
private:
	//Gia ton endiameso Kwdika
	LQList mainList;
	int temp_num;
	list<Argument*> tempArgList; //symbolArray
	int offset; //symbolArray
	//Apo thn arxh ths stoivas 0
	int Main_Program_Framelength;
	int nesting_level;//symbolArray
	LQList* emptyList(); //MidCode
	LQList* makeList(LabelQuad*);//MidCode
	LQList* merge(LQList*,LQList*);//MidCode
	LabelQuad* genQuad(string,string,string,string);//MidCode
	string newTemp(); //MidCode
	bool backpatch(LQList*,int); //MidCode
	int nextQuad(); //MidCode
	//SyntaxAnalyzer
	int cuToken;
	int Asc_num_quad;
	EndCodeEngine* code_generator;
	string PROGRAM_NAME;
	void program();
	void programBllock(string);
	void block();
	void decleration();
	void constDecl();
	void assignList();
	void assignConst();
	void varDecl();
	void varList();
	void subPrograms(LabelQuad*);
	void procorFunc();
	void procoFuncBody();
	void formalPars();
	void formalParList();
	void formalParItem();
	void selectStat();
	void sequence();
	void blockOrStat();
	void statement();
	void assignmentStat();
	void ifStat();
	void elsePart();
	void whileStat();
	void forStat();
	void stepPart(string*);
	void callStat();
	void printStat();
	void inputStat();
	void returnStat();
	void actualPars();
	void actualParList();
	void actualParItem();
	void condition(LQList**,LQList**);
	void boolTerm(LQList**,LQList**);
	void boolFactor(LQList**,LQList**);
	void relationalOpen(string*);
	void expression(string*);
	void term(string*);
	void factor(string*);
	void optactualPars(string*);
	void addOper();
	void mulOper();
	void optionalSign();
};


#endif /* SYNTAXANALYZER_H_ */
