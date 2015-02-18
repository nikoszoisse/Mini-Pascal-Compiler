/*
 * Scope.h
 *
 *  Created on: 27 Απρ 2014
 *      Author: plaisio
 */
#include "../tools/Utils.h"
#include "Entity.h"

#ifndef SCOPE_H_
#define SCOPE_H_

class Scope {
private:
	list<Entity*> entList;
	int nesting_level;
	Scope *enclosingScope;
	int frameLength;
public:
	string name;
	Scope(string);
	virtual ~Scope();
	Scope* getEnclosingScope();
	void setEnclosingScope(Scope* enclosingScope);
	int getNestingLevel();
	void setNestingLevel(int nestingLevel);
	void add(Entity*);
	Entity* getEntity(string);
	void print();

	int getFrameLength() const {
		return frameLength;
	}

	void setFrameLength(int frameLength) {
		this->frameLength = frameLength;
	}
};

#endif /* SCOPE_H_ */
