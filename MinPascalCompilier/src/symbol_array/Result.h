/*
 * Result.h
 *
 *  Created on: 16 Ìבת 2014
 *      Author: plaisio
 */
#include "Scope.h"
#ifndef RESULT_H_
#define RESULT_H_

 struct Result{
	Scope* scope;
	Entity* entity;
/*	Result(Scope* scope,Entity* entity){
		this->scope = scope;
		this->entity = entity;
	};*/
	~Result(){
		delete this->scope;
		delete this->entity;
	};
};


#endif /* RESULT_H_ */
