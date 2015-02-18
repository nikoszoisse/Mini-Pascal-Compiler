/*
 * Entity.cpp
 *
 *  Created on: 27 Απρ 2014
 *      Author: plaisio
 */

#include "Entity.h"

Entity::Entity(string name, int type, int offset) {
	this->name = name;
	this->type = type;
	if(type == PAR){
		this->parMode = offset;
	}else if(type == VARIABLE){
		this->offset = offset;
	}
}

Entity::Entity(string name, int type, int startQuad, list<Argument*> args,
		int framelength) {
	this->name = name;
	this->type = type;
	this->startQuad = startQuad;
	this->argList = args;
	this->frameLength = framelength;
}

Entity::Entity(string name, string value) {
	this->name = name;
	this->value = value;
	this->type = CONST;
}

Entity::Entity(string name, int offset) {
	this->name = name;
	this->offset = offset;
}

string Entity::getName(){
	return this->name;
}

int Entity::getOffset(){
	return this->offset;
}

Entity::~Entity() {
	// TODO Auto-generated destructor stub
}

void Entity::print(){
	cout << "Entity{ ";
	if(type == FUNCTION || type == PROCEDURE){
		cout << "Type: Function Name: "<<name<<"FrameLength: "<<frameLength
				<<"StartQuad: "<<startQuad<<" Args:";
		list<Argument*>::iterator it;
		for(it=argList.begin();it != argList.end();++it){
			//DELETE
			(*it)->print();
			cout << endl;
		}
	}
	else{
		cout <<"Type: VAR,PAR Name: "<<name;
		if(offset)
			cout <<" Offset: "<<offset;
	}
	cout <<"}"<<endl;
}

const list<Argument*>& Entity::getArgList() const {
	return argList;
}

int Entity::getFrameLength() const {
	return frameLength;
}

int Entity::getParMode() const {
	return parMode;
}

int Entity::getStartQuad() const {
	return startQuad;
}

int Entity::getType() const {
	return type;
}

const string& Entity::getValue() const {
	return value;
}
