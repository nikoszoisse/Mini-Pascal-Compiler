/*
 * LexicalAnalyzer.cpp
 *
 *  Created on: 7 Ìבס 2014
 *      Author:  Marina Mpourdakh 2111 Nikos Zois 2054
 */

#include "LexicalAnalyzer.h"
#include "tools/tokens.h"

bool isLetter(char ch)
{
 if( (ch>='a'&&ch<='z') || (ch>='A'&&ch<='Z') || ch=='_')
  return true;
 return false;
}

bool isDigit(char ch)
{
  if(ch>='0'&&ch<='9')
                      return true;
  return false;
}

bool isOperator(char ch)
{
 if(ch=='{'||ch=='}'||ch==':'||ch=='='||ch=='+'||ch=='/'||ch=='-'||ch=='*'||ch=='<'||ch=='>'||ch=='('||ch==')'||ch=='['||ch==']'||ch=='.'||ch==','||ch==';')
     return true;
 return false;
}

bool isBannedChar(char ch){
	if(ch=='!' || ch=='_' || ch=='\\'){
		return true;
	}
	return false;
}

bool isWhiteChar(char ch){
	return isspace(ch);
}

LexicalAnalyzer::LexicalAnalyzer(fstream &fileInputa) {
	this->fileInput = &fileInputa;
	this->lexis = "";
	this->line = 1;
}

LexicalAnalyzer::~LexicalAnalyzer() {
	delete fileInput;
}

int LexicalAnalyzer::getTokenID(){
	//Check if the lexis is an operator
	if(lexis.size() == 1 && isOperator(lexis.c_str()[0])){
		if(lexis == "+") return PLUS;
		else if(lexis == "-") return MINUS;
		else if(lexis == "*") return MULTIPLE;
		else if(lexis == "/") return DIVIDE;
		else if(lexis == "<") return LESS;
		else if(lexis == ">") return GREATER;
		else if(lexis == "=") return EQUAL;
		else if(lexis == "{") return OPEN_BRACKET;
		else if(lexis == "}") return CLOSE_BRACKET;
		else if(lexis == "(") return OPEN_PAREN;
		else if(lexis == ")") return CLOSE_PAREN;
		else if(lexis == "[") return OPEN_AGG;
		else if(lexis == "]") return CLOSE_AGG;
		else if(lexis == ",") return COMMA;
		else if(lexis == ";") return SEMI_COL;
	}//Check if is numeric
	else if(isDigit(lexis.c_str()[0])){
		return NUMERIC;
	}
	else{//check if is lexis
		if (lexis=="EOF") return EOF;
		else if(lexis == "program") return PROGRAM;
		else if(lexis == "function") return FUNCTION;
		else if(lexis == "procedure") return PROCEDURE;
		else if(lexis == "begin") return BEGIN;
		else if(lexis == "end") return END;
		else if(lexis == "if") return IF;
		else if(lexis == "then") return THEN;
		else if(lexis == "else") return ELSE;
		else if(lexis == "select") return SELECT;
		else if(lexis == "while") return WHILE;
		else if(lexis == "do") return DO;
		else if(lexis == "for") return FOR;
		else if(lexis == "to") return TO;
		else if(lexis == "step") return STEP;
		else if(lexis == "call") return CALL;
		else if(lexis == "return") return RETURN;
		else if(lexis == "input") return INPUT;
		else if(lexis == "print") return PRINT;
		else if(lexis == "and") return AND;
		else if(lexis == "or") return OR;
		else if(lexis == "not") return NOT;
		else if(lexis == "<=") return LESS_OR_EQ;
		else if(lexis == ">=") return GREATER_OR_EQ;
		else if(lexis == "<>") return DIFF;
		else if(lexis == ":=") return PUT;
		else if(lexis == "var") return VARIABLE;
		else if(lexis == "const") return CONST;
		else {			return ID;}
	}
	return 999;
}

bool LexicalAnalyzer::parseToken(){
	int state = state0;
	lexis = "";
	//As we are not at end state we continue reading char
	while(state!=OK && state !=ERROR){
		char c = fileInput->get();
		char temp = fileInput->peek(); //The after char of 'c' WITHOUT extracting him from buffer

		if(c == '\n'){
			this->line++;
		}
		if(state==state0 && fileInput->eof()){state=OK;lexis="EOF";}
		//VERSION 1.0.1 Changes from else if(isWhiteChar(c) &&){state=state0;} ->
		//else if(isWhiteChar(c) && state==state0){state=state0;}
		else if(isWhiteChar(c) && state==state0){state=state0;}
		else if(isBannedChar(c)){state=ERROR;this->errors << "Error: Unrecognized word : "<< (char)c<<" Line: " <<line;}
		else if(state==state0 && isLetter(c)){state=state1; lexis += c;	}
		else if(state==state1 && (isLetter(c) || isDigit(c) ) ){
			state=state1;lexis+=c;	}
		//Could be state==state0 || state==state2
		else if(state==state0 && isDigit(c)){state=state2;lexis+=c;	}
		else if(state==state2 && isDigit(c)){state=state2;lexis+=c;	}
		else if(state==state0 && c=='<'){state=state3;lexis+=c;	}
		else if(state==state3 && c=='>'){state=OK;lexis+=c;	}
		else if(state==state3 && c=='='){state=OK;lexis+=c;	}
		else if(state==state0 && c=='>'){state=state4;lexis+=c;	}
		else if(state==state4 && c=='='){state=OK;lexis+=c;	}
		//Tip: As we checked that temp is Ok we have to remove it from Buffer/File
		//we use get again
		//VERSION 1.0.2
		else if(state==state0 && c==':' && temp=='='){state=OK;lexis =":=";fileInput->get();	}
		else if(state==state0 && c==':' && temp!='='){
			state=ERROR;fileInput->get();
			errors << "FATAL ERROR : Expected ':=' and not ':' line: "<< line;}
		else if(state==state0 && c=='/'){state=state6;lexis+=c;}
		else if(state==state6 && c=='*'){state=state7;lexis="";}
		else if(state==state7 && c!='*' && temp!='/'){state=state7;fileInput->get();}
		//VERSION 1.0.2
		else if(state==state7 && c=='*' && temp=='/'){state=state0;fileInput->get();}
		//Yes we already checked for '/'  for this reason we check at this point if
		//the char is operator->->
		else if(state==state0 && isOperator(c)){state = OK;lexis+=c;}
		//If we dont found a recognized character then throw error
		else {
			//TODO VERSION 1.0.2 Maybe ned delete
			fileInput->putback(c);
			state=OK;
		}
	}

	if(state==ERROR)
		return false;
	else return true;
}

int LexicalAnalyzer::getToken(){
	//parseToken return false if we have an error
	if(!parseToken()){
		cout << errors.str() << " Line:" << line <<endl;system("PAUSE");exit(0);
	}
	else{
		if(lexis.size()>30 && isLetter(lexis.c_str()[0]))
			lexis = lexis.substr(0,30);
		else if(isDigit(lexis.c_str()[0])){
			int numb;
			istringstream ( lexis ) >> numb;
			int pows = pow(2,31);
			if((numb >= pows-1) || (numb <= -pows-1)){
				cout << "FATAL ERROR: Mini pascal accepts only numbers between -(2^31)-1 and (2^31)-1" << " Line:" << line <<endl;system("PAUSE");exit(0);
			}
		}

		return getTokenID();
	}
	return 999;
}

string LexicalAnalyzer::getLexis(){
	return this->lexis;
}
