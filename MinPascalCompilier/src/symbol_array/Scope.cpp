/*
 * Scope.cpp
 *
 *  Created on: 27 Απρ 2014
 *      Author: plaisio
 */

#include "Scope.h"

Scope::Scope(string name) {
	// TODO Auto-generated constructor stub
	this->name = name;
	this->frameLength = 0;
}

Scope* Scope::getEnclosingScope() {
	return enclosingScope;
}

void Scope::setEnclosingScope(Scope* enclosingScope) {
	this->enclosingScope = enclosingScope;
}

int Scope::getNestingLevel() {
	return nesting_level;
}

void Scope::setNestingLevel(int nestingLevel) {
	nesting_level = nestingLevel;
}

Scope::~Scope() {
	// TODO Auto-generated destructor stub
}

void Scope::add(Entity* Ent) {
	this->entList.push_back(Ent);
}

Entity* Scope::getEntity(string entity_name){
	list<Entity*>::iterator it;
	for(it=entList.begin();it != entList.end();++it){
		if( (*it)->getName() == entity_name ){
			return (*it);
		}
	}

	return NULL;
}

void Scope::print(){
	cout <<" Scope { Name: "<<name<<" Entities: "<<endl;
	list<Entity*>::iterator it;
	for(it=entList.begin();it != entList.end();++it){
		//DELETE
		(*it)->print();
		cout << endl;
	}
	cout <<"}"<<endl;
}
