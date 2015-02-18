/*
 * LexicalAnalyzer.h
 *
 *  Created on: 7 Ìבס 2014
 *      Author:  Marina Mpourdakh 2111 Nikos Zois 2054
 */

#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

#include "tools/Utils.h"

class LexicalAnalyzer {

private:
	fstream* fileInput;
	string lexis;
	stringstream errors;
	bool parseToken();
	int getTokenID();
public:
	LexicalAnalyzer(fstream&);
	virtual ~LexicalAnalyzer();
	int getToken();
	string getLexis();
	int line;

};
#endif /* LEXICALANALYZER_H_ */
