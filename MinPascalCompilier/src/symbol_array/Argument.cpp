/*
 * Argument.cpp
 *
 *  Created on: 27 Απρ 2014
 *      Author: plaisio
 */

#include "Argument.h"

Argument::Argument(int parMode,int type) {
	this->parMode = parMode;
	this->type = type;
}

int Argument::getParMode() const {
	return parMode;
}

int Argument::getType() const {
	return type;
}

void Argument::print(){
	if(parMode == 1)
		cout << "arg: mode: CV";
	else
		cout << "arg: mode: REF";
}
Argument::~Argument() {
	// TODO Auto-generated destructor stub
}

