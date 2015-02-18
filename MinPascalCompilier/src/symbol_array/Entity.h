/*
 * Entity.h
 *
 *  Created on: 27 Απρ 2014
 *      Author: plaisio
 */
#include "../tools/Utils.h"
#include "Argument.h"
#include "../tools/tokens.h"

#ifndef ENTITY_H_
#define ENTITY_H_

class Entity {
public:
	//Variable,Parametr set
	Entity(string,int,int);
	//Fnction
	Entity(string,int,int,list<Argument*>,int);
	//Const
	Entity(string,string);
	//Temp var
	Entity(string,int);
	void print();
	string getName();
	int getOffset();
	virtual ~Entity();
	const list<Argument*>& getArgList() const;
	int getFrameLength() const;
	int getParMode() const;
	int getStartQuad() const;
	int getType() const;
	const string& getValue() const;

private:
	string name;
	//Variable,Function
	int type;
	//Variable,Function,Parametres,tempVAr
	int offset;
	//Function
	int startQuad;
	list<Argument*> argList;
	int frameLength;
	//Const Var
	string value;
	//Parametres
	int parMode;

};

#endif /* ENTITY_H_ */
