/*
 * EndCodeEngine.cpp
 *
 *  Created on: 12 Ìבת 2014
 *      Author: plaisio
 */

//#include "EndCodeEngine.h"
#include "SyntaxAnalyzer.h"

EndCodeEngine::EndCodeEngine(SyntaxAnalyzer* syntax) {
	this->syntax = syntax;
	this->parametr_number = 0;
	this->progLabelNum = 0;
}

EndCodeEngine::~EndCodeEngine() {
	//this->assemblyStream.close();
}

void EndCodeEngine::gnlvcode(string var_name) {
	Result *res = syntax->findVar(var_name);
	this->assemblyStream << "movi R[255], M[4+R[0]]" <<endl;

	int currentScopeNestingLevel = syntax->current->getNestingLevel();
	int foundedScopeNestingLevel = res->scope->getNestingLevel();
	int foundedVariableOffset = res->entity->getOffset();

	while(currentScopeNestingLevel != foundedScopeNestingLevel){
		this->assemblyStream <<"movi R[255], M[4+R[255]]" <<endl;
		foundedScopeNestingLevel++;//While Guard
	}
	this->assemblyStream <<"movi R[254], "<<foundedVariableOffset <<endl;
	this->assemblyStream <<"addi R[255], R[254], R[255]" <<endl;

}

void EndCodeEngine::loadvr(string var_name,int reg) {

	if(var_name.c_str()[0]>='0' && var_name.c_str()[0]<='9'){
		this->assemblyStream <<"movi R["<<reg<<"] , "<<var_name<<endl;
		return;
	}


	Result *res = syntax->findVar(var_name);
	//If the variable is a const nnumber
	//then just register the number
	if(res->entity->getType() == CONST){
		this->assemblyStream <<"movi R["<<reg<<"] , "<<res->entity->getValue()<<endl;
		return;
	}
	int currentScopeNestingLevel = syntax->current->getNestingLevel();
	int foundedScopeNestingLevel = res->scope->getNestingLevel();
	int foundedVariableOffset = res->entity->getOffset();

	if(foundedScopeNestingLevel == MAIN_PROGRAM_NES_LEVEL){
		this->assemblyStream <<"movi R["<<reg<<"], M["<<600+foundedVariableOffset<<"]"<<endl;
	}
	else if(currentScopeNestingLevel == foundedScopeNestingLevel){
		if(res->entity->getParMode() != REF){
			this->assemblyStream <<"movi R["<<reg<<"], M[R[0]+"<<foundedVariableOffset<<"]"<<endl;
		}
		else if(res->entity->getParMode() == REF){
			this->assemblyStream << "movi R[255], M["<<foundedVariableOffset<<"+R[0]]"<<endl;
			this->assemblyStream <<"movi R["<<reg<<"], M[R[255]]"<<endl;
		}
	}
	else if(currentScopeNestingLevel > foundedScopeNestingLevel){
		this->gnlvcode(var_name);
		if(res->entity->getParMode() == REF){
			this->assemblyStream <<"movi R[255], M[R[255]]"<<endl;
		}
		this->assemblyStream <<"movi R["<<reg<<"], M[R[255]]"<<endl;
	}
}

void EndCodeEngine::storevr(int reg, string var_name) {
	Result *res = syntax->findVar(var_name);
		//If we try to assign number to a const
	//then throw error
		if(res->entity->getType() == CONST){
			cout << "FATAL ERROR: You tried to assign value to a Const Variable"<<endl;exit(0);
		}
		int currentScopeNestingLevel = syntax->current->getNestingLevel();
		int foundedScopeNestingLevel = res->scope->getNestingLevel();
		int foundedVariableOffset = res->entity->getOffset();

		if(foundedScopeNestingLevel == MAIN_PROGRAM_NES_LEVEL){
			this->assemblyStream <<"movi M["<<600+foundedVariableOffset<<"], R["<<reg<<"]"<<endl;
		}
		else if(currentScopeNestingLevel == foundedScopeNestingLevel){
			if(res->entity->getParMode() != REF){
				this->assemblyStream <<"movi M["<<foundedVariableOffset<<"+R[0]], R["<<reg<<"]"<<endl;
			}
			else if(res->entity->getParMode() == REF){
				this->assemblyStream << "movi R[255], M["<<foundedVariableOffset<<"+R[0]]"<<endl;
				this->assemblyStream <<"movi M[R[255]], R["<<reg<<"]"<<endl;
			}
		}
		else if(currentScopeNestingLevel > foundedScopeNestingLevel){
			this->gnlvcode(var_name);
			if(res->entity->getParMode() == REF){
				this->assemblyStream <<"movi R[255], M[R[255]]"<<endl;
			}
			this->assemblyStream <<"movi M[R[255]], R["<<reg<<"]"<<endl;
		}
}

void EndCodeEngine::tryGenerateRealOperator(LabelQuad* quad){
	string op = quad->getOp();

	if(op == "=" || op == "<" || op == "<=" || op == ">" ||
			op == ">="){
		loadvr(quad->getX(),1);
		this->assemblyStream <<"";
		loadvr(quad->getY(),2);
		this->assemblyStream <<"cmpi R[1],R[2]"<<endl;
	}
	if(op == "="){
		this->assemblyStream <<"je L"<<quad->getZ()<<endl;
	}
	else if(op == "<"){
		this->assemblyStream <<"ja L"<<quad->getZ()<<endl;
	}
	else if(op == "<="){
		this->assemblyStream <<"jae L"<<quad->getZ()<<endl;
	}
	else if(op == ">"){
		this->assemblyStream <<"jb L"<<quad->getZ()<<endl;
	}
	else if(op == ">="){
		this->assemblyStream <<"jbe L"<<quad->getZ()<<endl;
	}
}

void EndCodeEngine::tryGenerateOperator(LabelQuad* quad){
	string op = quad->getOp();
	if(op == "+" || op == "-" || op == "*" || op == "/"){
		loadvr(quad->getX(),1);
		this->assemblyStream <<"";
		loadvr(quad->getY(),2);
		if(op == "+"){
			this->assemblyStream <<"addi R[2], R[1], R[2]"<<endl;
		}
		else if(op == "-"){
			this->assemblyStream <<"subi R[2], R[1], R[2]"<<endl;
		}
		else if(op == "*"){
			this->assemblyStream <<"muli R[2], R[1], R[2]"<<endl;
		}
		else if(op == "/"){
			this->assemblyStream <<"divi R[2], R[1], R[2]"<<endl;
		}
		this->assemblyStream<<"";
		storevr(2,quad->getZ());
	}
}

void EndCodeEngine::generateParEndCode(LabelQuad* quad){
	int i = ++(this->parametr_number);
	int framelength = syntax->current->getFrameLength();
	int d = framelength + 12 +4*i;
	if (quad->getY() == "CV"){
		loadvr(quad->getX(),255);
		this->assemblyStream <<"movi M["<<d<<"+R[0]], R[255]"<<endl;
	}
	else if(quad->getY() == "REF"){
		Result *res = syntax->findVar(quad->getX());
		int currentScopeNestingLevel = syntax->current->getNestingLevel();
		int foundedScopeNestingLevel = res->scope->getNestingLevel();
		if(currentScopeNestingLevel == foundedScopeNestingLevel){
			this->assemblyStream << "movi R[255],R[0]"<<endl;
			this->assemblyStream <<"movi R[254],"<<res->entity->getOffset()<<endl;
			this->assemblyStream <<"addi R[255], R[254],R[255]"<<endl;

			if(res->entity->getType() == PAR && res->entity->getParMode() == REF){
				this->assemblyStream <<"movi R[1], Ì[R[255]]"<<endl;
				this->assemblyStream <<"movi M["<< d <<"+R[0]], R[1]"<<endl;				}
			else{	//Is a normal variable or PAR With CV
				this->assemblyStream <<"movi M["<< d <<"+R[0]], R[255]"<<endl;
			}
		}
		else {
			gnlvcode(quad->getX());

			if(res->entity->getType() == PAR && res->entity->getParMode() == REF){
				this->assemblyStream <<"movi R[1], Ì[R[255]]"<<endl;
				this->assemblyStream <<"movi M["<< d <<"+R[0]], R[1]"<<endl;				}
			else{	//Is a normal variable or PAR With CV
				this->assemblyStream <<"movi M["<< d <<"+R[0]], R[255]"<<endl;
			}
		}
	}
	else if(quad->getY() == "RET"){
		Result *res = syntax->findVar(quad->getX());
		int foundedVariableOffset = res->entity->getOffset();
		int framelength = syntax->current->getFrameLength();

		this->assemblyStream << "movi R[255], R[0]"<<endl;
		this->assemblyStream <<"movi R[254], "<<foundedVariableOffset<<endl;
		this->assemblyStream <<"addi R[255],R[254], R[255]"<<endl;
		this->assemblyStream <<"movi M["<<framelength+8<<"+R[0]], R[255]"<<endl;
	}
}

void EndCodeEngine::printConsole(){
	cout << "~~~~~~~~~~Assembly File~~~~~~~~~"<<endl;
	cout << this->assemblyStream.str();
}
void EndCodeEngine::printFile(string file_name){
	file_name.append(".pascAssembly");
	this->assemblyFile.open(file_name.c_str(),ios::out);
	if(assemblyFile.is_open()){
		assemblyFile << assemblyStream.str();
		assemblyFile.close();
	}else{
		cout <<"Error: Couldnt create a file to the directory,check permisions"<<endl;system("pause");}
}
void EndCodeEngine::generateEndCode(LabelQuad* quad){
	quad->print();

	string op = quad->getOp();
//I prwth entoli einai h arxh ths quad mas// wste na kanoume swwsto jump kai na to upologisoume
	if(op != "begin_block")
		this->assemblyStream << "L"<<quad->getPos()<<": ";

	tryGenerateRealOperator(quad);
	tryGenerateOperator(quad);
	if(op == "jump"){
		this->assemblyStream << "jmp L"<<quad->getZ()<<endl;
	}
	else if(op == ":="){
		loadvr(quad->getX(),1);
		this->assemblyStream <<"";
		storevr(1,quad->getZ());
	}
	else if(op == "inp"){
		this->assemblyStream <<"ini R[1]"<<endl;
		storevr(1,quad->getX());
	}
	else if(op == "out"){
		loadvr(quad->getX(),1);
		this->assemblyStream <<"outi R[1]"<<endl;
	}
	else if(op == "retv"){
		loadvr(quad->getX(),1);
		this->assemblyStream <<"movi R[255],M[8+R[0]"<<endl;
		this->assemblyStream <<"movi M[R[255]],R[1]"<<endl;
	}
	else if(op == "par"){
		generateParEndCode(quad);
	}
	else if(op == "call"){
		Result *res = syntax->findVar(quad->getX());
		if(res->entity->getType() == FUNCTION){
			int currentScopeNestingLevel = syntax->current->getNestingLevel();
			int foundedScopeNestingLevel = res->scope->getNestingLevel();
			int framelength = syntax->current->getFrameLength();
			if(currentScopeNestingLevel == foundedScopeNestingLevel){
				this->assemblyStream <<"movi R[255], M[4+R[0]]"<<endl;
				this->assemblyStream <<"movi M["<<framelength+4<<"+R[0]], R[255]"<<endl;
			}
			else if(currentScopeNestingLevel < foundedScopeNestingLevel){
				this->assemblyStream <<"movi M["<<framelength+4<<"+R[0]], R[0]"<<endl;
			}
			this->assemblyStream <<"movi R[255], "<<framelength<<endl;
			this->assemblyStream <<"addi R[0], R[255], R[0]"<<endl;
			this->assemblyStream <<"movi R[255], $"<<endl;
			this->assemblyStream <<"movi R[254], 15 "<<endl;
			this->assemblyStream <<"addi R[255], R[255], R[254]"<<endl;
			this->assemblyStream <<"movi M[R[0]], R[255]"<<endl;
			this->assemblyStream <<"jmp L"<<res->entity->getStartQuad()<<endl;
			this->assemblyStream <<"movi R[255],"<<framelength<<endl;
			this->assemblyStream <<"subi R[0], R[0], R[255]"<<endl;
		}
	}
	else if(op == "end_block" && (quad->getX() != syntax->getProgramName())){
		this->assemblyStream <<"jmp M[R[0]]"<<endl;
	}
	else if(op == "end_block" && (quad->getX() == syntax->getProgramName())){
		this->assemblyStream <<"jmp L"<<quad->getPos()+1<<endl;//Jump to nextQuad
	}
	else if(op == "halt"){
		this->assemblyStream <<"halt"<<endl;
	}


	if(op != "PAR"){
		this->parametr_number = 0;
	}

}
