/*
 * SyntaxAnalyzer.cpp
 *
 *  Created on: 7 Ìבס 2014
 *      Author:  Marina Mpourdakh 2111 Nikos Zois 2054
 */

#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer* lex) {
	this->lex = lex;
	this->cuToken = 999;
	this->temp_num = 0;
	this->offset = 0;
	this->nesting_level = 0;
	this->Asc_num_quad = 0;
	//Apo thn arxh ths stoivas 0
	this->Main_Program_Framelength = 0;
}

SyntaxAnalyzer::~SyntaxAnalyzer() {
	delete lex;
}

void SyntaxAnalyzer::checkSyntax() {
	cuToken = this->lex->getToken();
	this->program();
}

void SyntaxAnalyzer::program() {
	if(cuToken == PROGRAM){
		cuToken = this->lex->getToken();
		if(cuToken == ID){
			string name = lex->getLexis();
			//SetUp EndCode Engine generator
			this->code_generator = new EndCodeEngine(this);
			//Create Main Program Scope
			this->PROGRAM_NAME =  name;
			current = new Scope(name);
			current->setNestingLevel(this->nesting_level);
			scopeList.push_back(current);
			this->programBllock(name);
			this->genQuad("halt","_","_","_");;
			current->setFrameLength(Main_Program_Framelength);
			//Generate EndCode for the Quads that produced
			list<LabelQuad*>::iterator it;
			it=mainList.m_list.begin();
			while(it != mainList.m_list.end()){
				this->code_generator->generateEndCode((*it));
				mainList.m_list.pop_front();
				it=mainList.m_list.begin();
				//free (*it);
			}

		}else{
			cout << "Syntax Error: Expected program ID-Name after program."<< " Line:" << this->lex->line <<endl;system("PAUSE");exit(0);
		}
	}else{
		cout << "Syntax error: Exprected program word."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::programBllock(string program_name) {
	this->decleration();

	LabelQuad* temp_quad = genQuad("jump","_","_","_");

	//Subprogram too Have allready the cuToken because they called after a while in decleration();
	this->subPrograms(temp_quad);

	//Dont include it to list
	//it doesnt produce end code is only problem
	this->genQuad("begin_block",program_name,"_","_");
	temp_quad->setZ(nextQuad());
	//Subprogram too Have allready the cuToken because they called after a while in subPrograms();
	this->block();

	this->genQuad("end_block",program_name,"_","_");

}

void SyntaxAnalyzer::block() {
	if(cuToken == BEGIN){
		this->sequence();
		//sequence use while so we dot need to get again the token , we already know it
		//VERSION 1.0.2 cuToken = this->lex->getToken();

		if(cuToken != END){
			cout << "Syntax Error: Expected end Word to close begin.";system("PAUSE");exit(0);
		}
	}else{
		cout << "Syntax Error: Expected begin."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);
	}
}

void SyntaxAnalyzer::decleration() {
	cuToken = this->lex->getToken();
	while (cuToken == CONST || cuToken==VARIABLE){
		if(cuToken ==CONST){
			this->constDecl();
			cuToken = this->lex->getToken();
		}
		else if(cuToken == VARIABLE){
			this->varDecl();
			cuToken = this->lex->getToken();
		}
	}
}

void SyntaxAnalyzer::constDecl() {
	//We already check in called function if const exists so dont need to check it again
	this->assignList();
	//cuToken will be valid because we call it before a while at assignList();
	if(cuToken != SEMI_COL){
		cout << "Syntax Error: Expected ; after const declaration."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::assignList() {
	this->assignConst();
	cuToken = this->lex->getToken();
	while (cuToken == COMMA){
		this->assignConst();
		cuToken = this->lex->getToken();
	}

}

void SyntaxAnalyzer::assignConst() {
	cuToken = this->lex->getToken();
	if(cuToken == ID){
	string term,op,value;
		term = this->lex->getLexis();
		cuToken = this->lex->getToken();
		if(cuToken == PUT){
//			op = this->lex->getLexis();
			cuToken = this->lex->getToken();
			if(cuToken != NUMERIC){
				cout << "Syntax Error: Exepted a constant numeric after := ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
			}
			value = this->lex->getLexis();
			current->add(new Entity(term,value));
		}else{
			cout << "Warning: A variable is not intiliazied."<<endl;;
		}
	}
	else{
		cout << "Expected variable name after const.";
	}
}

void SyntaxAnalyzer::varDecl() {
	//We already check in called function if var exists so dont need to check it again
	this->varList();
	//cuToken will be valid because we call it before a while at varList();
	if(cuToken != SEMI_COL){
		cout << "Syntax Error: Expected ; after var declaration."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}

}

void SyntaxAnalyzer::varList() {
	cuToken = this->lex->getToken();
	if(cuToken != ID){
		cout << "Syntax Error: Expected a varriable after var."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
	string var_name = this->lex->getLexis();
	//Increase Offset By 4
	offset += 4;
	//Create Entity and With the offset that we create it
	current->add(new Entity(var_name,offset));

	cuToken = this->lex->getToken();
	while (cuToken == COMMA){
		cuToken = this->lex->getToken();
		//Version 1.0.2
		if(cuToken == ID){
			var_name = this->lex->getLexis();
			//Increase Offset By 4
			offset += 4;
			//Create Entity and With the offset that we create it
			current->add(new Entity(var_name,offset));
			cuToken = this->lex->getToken();
		}
	}
}

void SyntaxAnalyzer::subPrograms(LabelQuad* temp_quad) {
	this->nesting_level++;
	while(cuToken == PROCEDURE || cuToken == FUNCTION){
		//Clean the temp list with arguments pointers
		tempArgList.clear();

		//Temporary we save the parent ofset in order to continue after finishing the Current subprogram
		int parent_offset = this->offset;
		this->offset = 12;

		int type = cuToken;
		cuToken = this->lex->getToken();
		string proc_name = this->lex->getLexis();

		int startQuad = nextQuad();
		current = new Scope(proc_name);
		current->setNestingLevel(this->nesting_level);
		scopeList.push_back(current);

		this->procorFunc();

		//Maybe need framelength+4 , offset have the last variable place of bytes
		int framelength = offset;

		current->print();

		current->setFrameLength(framelength);
		//we fill the temp_jump
		temp_quad->setZ(nextQuad());

		//Generate EndCode for the Quads that produced
		list<LabelQuad*>::iterator it;
		it=mainList.m_list.begin();
		while(it != mainList.m_list.end()){
			this->code_generator->generateEndCode((*it));
			mainList.m_list.pop_front();
			it=mainList.m_list.begin();
			//free (*it);
		}
		//we remove the function from our scopelist the generated scope
		scopeList.pop_back();
		//We recover the Parent offset to continue
		this->offset = parent_offset;
		current = scopeList.back();

		current->add(new Entity(proc_name, type, startQuad, tempArgList,
			framelength));

		cuToken = this->lex->getToken();


	}

}

void SyntaxAnalyzer::procorFunc() {
	if(cuToken == ID){
		this->procoFuncBody();
	}
	else{
		cout << "Syntax Error: Expected an id name for the Subprogram."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}

}

void SyntaxAnalyzer::procoFuncBody() {
	string proc_name = this->lex->getLexis();

	this->formalPars();
	this->programBllock(proc_name);
}

void SyntaxAnalyzer::formalPars() {
	cuToken = this->lex->getToken();
	if(cuToken == OPEN_PAREN){
		this->formalParList();
		//No need to get again the Token because we have it from formalPaList that uses while
		if(cuToken != CLOSE_PAREN){
			cout << "Syntax error: Close ) is missing."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
	}else{
		cout << "Syntax Error: Expected ( after subprogram name."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::formalParList() {
	this->formalParItem();
	if(cuToken != CLOSE_PAREN)
		cuToken = this->lex->getToken();
	while(cuToken == COMMA){
		this->formalParItem();
		cuToken = this->lex->getToken();
	}
}

void SyntaxAnalyzer::formalParItem(){
	cuToken = this->lex->getToken();
	if(cuToken == VARIABLE){
		cuToken = this->lex->getToken();
		if(cuToken != ID){
			cout << "Syntax Error: Expected variable name after var."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
		//New entity me REF
		string var_name = this->lex->getLexis();
		current->add(new Entity(var_name,PAR,REF));
		tempArgList.push_back(new Argument(REF,VARIABLE));
	}
	//VERSION 1.0.2
	else if(cuToken == ID){
		//New entity me CV
		string var_name = this->lex->getLexis();
		current->add(new Entity(var_name,PAR,CV));
		tempArgList.push_back(new Argument(CV,VARIABLE));
	}
}

void SyntaxAnalyzer::sequence() {
	//Version 1.0.2
	cuToken = this->lex->getToken();

	this->statement();
	//Version 1.0.2
	//Some cases we already read the SemiCol token so we dont ask i again here
	//But we ask for it into statements end
	while(cuToken == SEMI_COL){
		cuToken = this->lex->getToken();
		this->statement();
	}
}

void SyntaxAnalyzer::blockOrStat() {
	//cuToken = this->lex->getToken();
	if(cuToken == BEGIN){
		this->block();
	}
	else{
		this->statement();
	}
}

void SyntaxAnalyzer::statement() {

	switch (cuToken){
		case ID: this->assignmentStat(); break;
		case IF: this->ifStat(); break;
		case WHILE: this->whileStat(); break;
		case FOR: this->forStat(); break;
		case CALL: this->callStat(); break;
		case PRINT: this->printStat(); break;
		case INPUT: this->inputStat(); break;
		case RETURN: this->returnStat(); break;
		//TODO case SELECT: this->selectStat(); break;
	}

}

void SyntaxAnalyzer::assignmentStat() {
	string ass_value = this->lex->getLexis();
	cuToken = this->lex->getToken();
	if(cuToken == PUT){
		string term;
		cuToken = this->lex->getToken();
		this->expression(&term);
		genQuad(":=",term,"_",ass_value);
	}
	else{
		cout << "Syntax Error: Expected := after variable."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::ifStat() {
	LQList *E_true,*E_false;
	E_true = this->emptyList();
	E_false = this->emptyList();

	cuToken = this->lex->getToken();
	this->condition(&E_true,&E_false);

	if(cuToken == THEN){
		cuToken = this->lex->getToken();
		this->blockOrStat();

		//we dont need backpatch for E true cause the condition already did it
		backpatch(E_false,nextQuad());

		this->elsePart();
	}
	else{
		cout << "Syntax Error: Expected then after IF Statement."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::elsePart() {
	cuToken = this->lex->getToken();
	if(cuToken == ELSE){
		cuToken = this->lex->getToken();
		this->blockOrStat();
		//ASK AGAIN HERE TO READ SEMI COLON
		cuToken = this->lex->getToken();
	}
}

void SyntaxAnalyzer::whileStat() {
	LQList *E_true,*E_false;
	E_true = this->emptyList();
	E_false = this->emptyList();
	//We hold the Pos of quad the make the condition of while..
	//In order to jump before while
	int jump_pos = nextQuad();

	this->condition(&E_true,&E_false);

	if(cuToken == DO){
		cuToken = this->lex->getToken();

		this->blockOrStat();
		genQuad("jump","_","_","_")->setZ(jump_pos);
		///VERSION 1.0.2 After Block Or stat CUTOKEN == END SO we ask again to read ;
		cuToken = this->lex->getToken();
		//we dont need backpatch for E true cause the condition already did it
		backpatch(E_false,nextQuad());

	}
	else{
		cout <<"Syntax Error: Expected do after while statement."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::forStat() {

	cuToken = this->lex->getToken();//This get e.x i var
	string i_term = this->lex->getLexis();

	this->assignmentStat();
	// Version 1.0.2 cuToken = this->lex->getToken();
	if(cuToken == TO){
		string to_term;
		cuToken = this->lex->getToken();
		this->expression(&to_term);

		//Expression use while so we dont get the Token again into stepPart
		string step_term;
		this->stepPart(&step_term);

		//StepPart well if Step was the word before we need to get again the token if dont have to so
		if(cuToken == STEP)
			cuToken = this->lex->getToken();

		int jump_pos = nextQuad();
		LabelQuad *condQuad = genQuad("<",i_term,to_term,"_");
		LabelQuad *jumpQuad = genQuad("jump","_","_","_");
		condQuad->setZ(nextQuad());
		this->blockOrStat();
		genQuad("+",i_term,step_term,i_term);
		genQuad("jump","_","_","_")->setZ(jump_pos);
		jumpQuad->setZ(nextQuad());

		///VERSION 1.0.2 After Block Or stat CUTOKEN == END SO we ask again to read ;
		cuToken = this->lex->getToken();

	}else{
		cout << "Syntax Error: Expected to after for statement."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::stepPart(string *step_term) {
	if(cuToken == STEP){
		cuToken = this->lex->getToken();
		this->expression(step_term);
	}
	else{
		*step_term = "1";
	}
}

void SyntaxAnalyzer::callStat() {
	cuToken = this->lex->getToken();
	if(cuToken == ID){
		string proc_name = this->lex->getLexis();
		cuToken = this->lex->getToken();
		this->actualPars();
		genQuad("call",proc_name,"_","_");
		///VERSION 1.0.2 After Block Or stat CUTOKEN == END SO we ask again to read ;
		cuToken = this->lex->getToken();
	}
	else{
		cout << "Syntax Error: Expected subprogram name after call stat."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::printStat() {
	cuToken = this->lex->getToken();
	if(cuToken == OPEN_PAREN){
		string term;
		cuToken = this->lex->getToken();
		this->expression(&term);

		genQuad("out",term,"_","_");

		//expression uses hile so we dont need to take again the token
		if(cuToken != CLOSE_PAREN){
			cout << "Syntax Error: Close ) Expected after Open ( ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
		///VERSION 1.0.2 After Block Or stat CUTOKEN == END SO we ask again to read ;
		cuToken = this->lex->getToken();
	}else{
		cout << "Syntax Error: Expected Open ( after Print stat."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::inputStat() {
	cuToken = this->lex->getToken();
	if(cuToken == OPEN_PAREN){
		cuToken = this->lex->getToken();
		if(cuToken == ID){

			string term = this->lex->getLexis();
			genQuad("inp",term,"_","_");

			cuToken = this->lex->getToken();
			if(cuToken != CLOSE_PAREN){
			cout << "Syntax Error: Close ) Expected after Open ( ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
			}
			///VERSION 1.0.2 After Block Or stat CUTOKEN == END SO we ask again to read ;
			cuToken = this->lex->getToken();
		}else{
			cout << "Syntax Error: Expected a variable name into input(VARNAME) stat."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
	}else{
		cout << "Syntax Error: Expected Open ( after Input stat."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::returnStat() {
	cuToken = this->lex->getToken();
	if(cuToken == OPEN_PAREN){
		string term;
		cuToken = this->lex->getToken();
		this->expression(&term);

		genQuad("retv",term,"_","_");

		//expression uses hile so we dont need to take again the token
		if(cuToken != CLOSE_PAREN){
			cout << "Syntax Error: Close ) Expected after Open ( ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
		///VERSION 1.0.2 After Block Or stat CUTOKEN == END SO we ask again to read ;
		cuToken = this->lex->getToken();
	}else{
		cout << "Syntax Error: Expected Open ( after Return stat."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::actualPars() {
	//cuToken = this->lex->getToken();
	if(cuToken == OPEN_PAREN){
		this->actualParList();
		//actualParList uses hile so we dont need to take again the token
		if(cuToken != CLOSE_PAREN){
			cout << "Syntax Error: Close ) Expected after Open ( ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
	}else{
		cout << "Syntax Error: Expected Open ( after Print stat."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::actualParList() {
	this->actualParItem();
	/*if(cuToken == CLOSE_PAREN)
		cuToken = this->lex->getToken();*/
	while(cuToken == COMMA){
		this->actualParItem();
		cuToken = this->lex->getToken();
	}
}

void SyntaxAnalyzer::actualParItem() {
	cuToken = this->lex->getToken();
	if(cuToken == VARIABLE){
		cuToken = this->lex->getToken();
		string term = this->lex->getLexis();
		genQuad("par",term,"REF","_");
		if(cuToken != ID){
			cout << "Syntax Error: Expected variable name after var."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
	}
	else if(cuToken == ID){
		string term;
		this->expression(&term);
		genQuad("par",term,"CV","_");
	}
}

void SyntaxAnalyzer::condition(LQList **E_true,LQList **E_false) {
	LQList *B_true,*B_false;
	B_true = emptyList();
	B_false = emptyList();
	this->boolTerm(&B_true,&B_false);
	//Bool term uses while so we not have to ask again for token
	while(cuToken == OR){
		backpatch(B_false,nextQuad());
		cuToken = this->lex->getToken();
		this->boolTerm(&B_true,&B_false);
		//Bool term uses while so we not have to ask again for token
	}

	backpatch(B_true,nextQuad());

	*E_true = B_true;
	*E_false = B_false;
}

void SyntaxAnalyzer::boolTerm(LQList** B_true,LQList** B_false) {
	LQList *Q_true,*Q_false;
	Q_true = emptyList();
	Q_false = emptyList();

	this->boolFactor(&Q_true,&Q_false);
	//boolfactor if we find the not or [ we need to ask for token else we dont
	if(cuToken==NOT || cuToken==OPEN_AGG)
		cuToken = this->lex->getToken();
	while(cuToken == AND){
		backpatch(Q_true,nextQuad());
		this->boolFactor(&Q_true,&Q_false);
	}

	*B_true = merge(*B_true,Q_true);
	*B_false = merge(*B_false,Q_false);
	//O LOGOS pou den kanoume edw to backpatch gia tin B TRUE
	//einai thema aisthitikhs gia kaluterh katanonhsh tou provlimatos
}

//In Order to change the Pointer pointed value with have pass it as Value **
void SyntaxAnalyzer::boolFactor(LQList** Q_true,LQList** Q_false) {
	LQList *R_true,*R_false;
	//cuToken = this->lex->getToken();
	if(cuToken == NOT){
		cuToken = this->lex->getToken();
		if(cuToken == OPEN_AGG){
			//TODO NEED FIX NOT WORKS (BETA)
			cuToken = this->lex->getToken();
			R_true = emptyList();
			R_false = emptyList();
			this->condition(Q_true,Q_false);

			cuToken = this->lex->getToken();
			if(cuToken != CLOSE_AGG){
				cout << "Syntax Error: Expected Close ] after open [ ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
			}
			//Ask Token Here to read the next word as we closed 100% ]
			cuToken = this->lex->getToken();
		}
		else{
			cout << "Syntax Error: Expected Open [ after NOT  ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
	}
	else if(cuToken == OPEN_AGG){
		//TODO NEED FIX NOT WORKS (BETA)
		cuToken = this->lex->getToken();
		R_true = emptyList();
		R_false = emptyList();
		this->condition(Q_true,Q_false);
		//backpatch(R_true,nextQuad());
		if(cuToken != CLOSE_AGG){
			cout << "Syntax Error: Expected Close ] after open [ ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
		}
		//Ask Token Here to read the next word as we closed 100% ]
		cuToken = this->lex->getToken();
	}
	else{
		//Dont ask again for Token because the cuToken is not the NOT or The [

		string term_one;
		this->expression(&term_one);

		//Expresion use while so not ask for token again
		string rel_op;
		this->relationalOpen(&rel_op);

		string term_two;
		cuToken = this->lex->getToken();
		this->expression(&term_two);

		LabelQuad* next = genQuad(rel_op,term_one,term_two,"_");
		R_true = makeList(next);
		LabelQuad* jump = genQuad("jump","_","_","_");
		R_false = makeList(jump);
		*Q_true = R_true;
		*Q_false = merge(*Q_false,R_false);
	}

}

void SyntaxAnalyzer::relationalOpen(string* op) {

	if(cuToken == EQUAL || cuToken == LESS || cuToken == GREATER
			|| cuToken == GREATER_OR_EQ || cuToken == LESS_OR_EQ
			|| cuToken == DIFF)	{
		//Here we return what is our Operation
		*op = this->lex->getLexis();
	}
	else{
		cout << "Syntax Erro: In COndition place use a relationalOpen."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
	}
}

void SyntaxAnalyzer::expression(string* e) {

	//OptionalSign has already the Next cuToken
	this->optionalSign();

	//Optional sigh uses ו so we need to check if we had + or - we have to ask for token
	if(cuToken == PLUS || cuToken == MINUS)
		cuToken = this->lex->getToken();

	string Term_one = this->lex->getLexis();

	this->term(&Term_one);

	while(cuToken == PLUS || cuToken == MINUS){
		string symbol = this->lex->getLexis();
		string Term_two;
		this->term(&Term_two);
		string w = this->newTemp();

		offset += 4;
		current->add(new Entity(w,offset));

		this->genQuad(symbol,Term_two,Term_one,w);
		Term_one = w;
		//Term have a while so we dont need to ask again for token
		//cuToken = this->lex->getToken();
	}
	*e = Term_one;
}

void SyntaxAnalyzer::term(string *term) {
	if(cuToken != ID)
		cuToken = this->lex->getToken();

	//Version 1.0.2
	if(cuToken !=PLUS && cuToken != MINUS){
		//TODO IF != real Op
		this->factor(term);
	}

	//The factor have the case that we dont found ( but we asked for token so dont ask again
	if(cuToken == OPEN_PAREN || cuToken==ID){
		cuToken = this->lex->getToken();
	}
	while(cuToken == MULTIPLE || cuToken == DIVIDE){
		string symbol = this->lex->getLexis();
		cuToken = this->lex->getToken();
		string Term_two;
		this->factor(&Term_two);
		string w = this->newTemp();

		offset += 4;
		current->add(new Entity(w,offset));

		this->genQuad(symbol,Term_two,*term,w);
		*term = w;
		//The factor have the case that we dont found ( but we asked for token so dont ask again
		if(cuToken == OPEN_PAREN || cuToken==ID)
			cuToken = this->lex->getToken();
	}

}

void SyntaxAnalyzer::factor(string* term) {

	if(cuToken == OPEN_PAREN){
		this->expression(term);
		if(cuToken != CLOSE_PAREN){
			cout << "Syntax Error: Close ) Expected after Open ( ."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);
		}
		cuToken = this->lex->getToken();
	}
	else if(cuToken == ID){
		*term = this->lex->getLexis();
		cuToken = this->lex->getToken();
		this->optactualPars(term);
	}
	else if(cuToken ==NUMERIC){
		*term = this->lex->getLexis();
		cuToken = this->lex->getToken();
	}
	else if(cuToken == EQUAL || cuToken == LESS || cuToken == GREATER
			|| cuToken == GREATER_OR_EQ || cuToken == LESS_OR_EQ
			|| cuToken == DIFF || cuToken == THEN || cuToken == AND
			|| cuToken == OR || cuToken==CLOSE_AGG || DO){
		//cuToken = this->lex->getToken();
	}
	else{
		cout <<"Syntax Error: Use a numeric function or expresion."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);
	}
}

void SyntaxAnalyzer::optactualPars(string *term) {
	if(cuToken == OPEN_PAREN){
		this->actualPars();
		//Ask for token because we need to check ;
		cuToken = this->lex->getToken();

		string w = newTemp();

		offset += 4;
		current->add(new Entity(w,offset));

		genQuad("par",w,"RET","_");
		genQuad("call",*term,"_","_");
		//TODO MAYBE NEED AT EXPRESSION THE NAME OF
		//FUNCTION NOT THE RETURN VALUE OF IT
		//DELETE
		*term = w;
	}
}

void SyntaxAnalyzer::addOper() {
	//Version 1.0.2
	if(cuToken != PLUS && cuToken != MINUS)
		cout << "You must use + or - for this expresion."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);
}

void SyntaxAnalyzer::mulOper() {
	//Version 1.0.2
		if(cuToken != MULTIPLE && cuToken != DIVIDE)
			cout << "You must use * or * for this expresion."<<" Line:" << this->lex->line <<endl;system("PAUSE");exit(0);;
}

void SyntaxAnalyzer::selectStat() {
	//TODO Select stat
}

Result* SyntaxAnalyzer::findVar(string var_name) {
	//Search from Last elemnt to begin
	list<Scope*>::reverse_iterator rit;
	Entity* searchEnt;
	cout << "We Searching at Scope: "<<current->name<<endl;
	for(rit=this->scopeList.rbegin();rit != scopeList.rend();++rit){
		searchEnt = (*rit)->getEntity(var_name);
		if( searchEnt != NULL){
			Result *res = new Result();
			res->scope = (*rit);
			res->entity = searchEnt;
			return res;
		}
	}
	cout << "FATAL ERROR: The Variable '"<<var_name
			<<"' is not declared to any Scope.Please declare first."<<endl;exit(0);
}

string SyntaxAnalyzer::getProgramName() {
	return PROGRAM_NAME;
}

void SyntaxAnalyzer::optionalSign() {

	//cuToken = this->lex->getToken();
	if(cuToken == PLUS || cuToken == MINUS)
		this->addOper();

}

LabelQuad* SyntaxAnalyzer::genQuad(string op,string x,string y,string z){
	int pos=-99;
	if(op != "begin_block")
		pos = Asc_num_quad++;
	//Generate a new LabelQuad
	LabelQuad *item = new LabelQuad(pos,op,x,y,z);

	//Append it to Main list
	this->mainList.m_list.push_back(item);

	return item;
}

string SyntaxAnalyzer::newTemp(){
	//IN C++ WE cant do string + int but we can do it with string stream
	stringstream word;
	word << "T_"<<temp_num;
	this->temp_num++;

	return word.str();
}

int SyntaxAnalyzer::nextQuad(){
	//Elements Number is +1 because is counting from 1 and not from 0
	 return mainList.m_list.size();
}

LQList* SyntaxAnalyzer::emptyList(){
	return new LQList();
}

LQList* SyntaxAnalyzer::makeList(LabelQuad *item){
	LQList * temp = new LQList();
	temp->m_list.push_back(item);
	return temp;
}

bool SyntaxAnalyzer::backpatch(LQList* list_one,int item){
	list<LabelQuad*>::iterator it;

	for(it=list_one->m_list.begin();it != list_one->m_list.end();++it){
		(*it)->setZ(item);
	}
	list_one->m_list.clear();
	return true;
}

LQList* SyntaxAnalyzer::merge(LQList* list_one,LQList* list_two){

	list_one->m_list.merge(list_two->m_list);
	//TODO Delete list_two
	return list_one;
}

void SyntaxAnalyzer::printList(){
	list<LabelQuad*>::iterator it;
	for(it=mainList.m_list.begin();it != mainList.m_list.end();++it){
		//DELETE
		(*it)->print();
		cout << endl;
	}

	cout << "~~~Symbol Array~~~" <<endl;
	list<Scope*>::iterator it_1;
	for(it_1=scopeList.begin();it_1 != scopeList.end();++it_1){
		//DELETE
		(*it_1)->print();
		cout << endl;
	}

	this->code_generator->printConsole();
	this->code_generator->printFile(PROGRAM_NAME);
}
