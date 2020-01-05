#ifndef _DEFINITION_H
#define _DEFINITION_H

#include "varex.h"

const int VALID_VARIABLE = 0;
const int VALID_FUNCTION = 1;
const int INVALID_ASSIGNEE = 2;
const int NO_ASSIGNMENT_OPERATOR = 3;
const int INVALID_CHARACTER = 4;
const int COMMA_ERROR = 5;
const int INVALID_ASSIGNEE_ARGUMENT = 6;
const int NO_ASIGNEE_ARGUMENTS = 7;

using std::string;

class DefinitionList {
public:
	DefinitionList();
	~DefinitionList();
	void tryDefining(const string&);
private:
	vector<string>* list;
	int validateInput(string&, const string&) const;
	void addVariableDef(const string&);
	void addFunctionDef(const string&);
};

#endif