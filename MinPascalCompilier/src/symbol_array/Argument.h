/*
 * Argument.h
 *
 *  Created on: 27 Απρ 2014
 *      Author: plaisio
 */
#include "../tools/Utils.h"

#ifndef ARGUMENT_H_
#define ARGUMENT_H_

class Argument {
	int parMode;
	int type;
public:
	Argument(int,int);
	virtual ~Argument();
	int getParMode() const;
	int getType() const;
	void print();
};

#endif /* ARGUMENT_H_ */
