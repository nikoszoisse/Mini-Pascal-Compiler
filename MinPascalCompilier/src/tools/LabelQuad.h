/*
 * LabelQuad.h
 *
 *  Created on: 3 Απρ 2014
 *      Author: plaisio
 */

#ifndef LABELQUAD_H_
#define LABELQUAD_H_

#include "Utils.h"

class LabelQuad {

private:
	int pos;
	string op,x,y,z;
public:
	LabelQuad(int,string,string,string,string);
	virtual ~LabelQuad();
	int getPos();
	void setZ(int);
	void print();
	const string& getOp() const;
	void setOp(const string& op);
	const string& getX() const;
	void setX(const string& x);
	const string& getY() const;
	void setY(const string& y);
	const string& getZ() const;
};

#endif /* LABELQUAD_H_ */
