/*
 * LabelQuad.cpp
 *
 *  Created on: 3 Απρ 2014
 *      Author: plaisio
 */

#include "LabelQuad.h"

LabelQuad::LabelQuad(int pos,string op,string x,string y,string z) {
		this->pos = pos;
		this->op = op;
		this->x = x;
		this->y = y;
		this->z = z;
}

LabelQuad::~LabelQuad() {
	// TODO Auto-generated destructor stub
}

int LabelQuad::getPos(){
	return this->pos;
}

void LabelQuad::setZ(int value){
	stringstream val;
	val	<< value;
	this->z = val.str();
}

const string& LabelQuad::getOp() const {
	return op;
}

void LabelQuad::setOp(const string& op) {
	this->op = op;
}

const string& LabelQuad::getX() const {
	return x;
}

void LabelQuad::setX(const string& x) {
	this->x = x;
}

const string& LabelQuad::getY() const {
	return y;
}

void LabelQuad::setY(const string& y) {
	this->y = y;
}

const string& LabelQuad::getZ() const {
	return z;
}

void LabelQuad::print(){
	cout <<pos<<": "<<"("<<op<<", "<<x<<", "<<y<<", "<<z<<")"<<endl;
}
