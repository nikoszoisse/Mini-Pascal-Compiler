/*
 * LQList.h
 *
 *  Created on: 3 Απρ 2014
 *      Author: plaisio
 */
#include "Utils.h"

#ifndef LQLIST_H_
#define LQLIST_H_
#include "LabelQuad.h"

class LQList {
public  :
	list<LabelQuad*> m_list;
	LQList();
	virtual ~LQList();
};

#endif /* LQLIST_H_ */
