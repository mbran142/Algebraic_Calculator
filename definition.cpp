#include "definition.h"

DefinitionList::DefinitionList() {
	list = new vector<string>();
}

DefinitionList::~DefinitionList() {
	delete list;
}

void DefinitionList::tryDefining(const string& input) {

	string assignment;
	int status = this->validateInput(assignment, input);

	switch (status) {
		case VALID_VARIABLE: addVariableDef(assignment); break;
		case VALID_FUNCTION: addFunctionDef(assignment); break;
		case INVALID_ASSIGNEE: throw "Syntax - Assignment must be single variable or function name";
		case NO_ASSIGNMENT_OPERATOR: throw "Syntax - No assignment operator \":=\" found";
		case INVALID_CHARACTER: throw "Syntax - Invalid character detected";
		case COMMA_ERROR: throw "Syntax - Comma syntax error";
		case INVALID_ASSIGNEE_ARGUMENT: throw "Syntax - Invalid assignee function argument";
		case NO_ASIGNEE_ARGUMENTS: throw "Syntax - Function asignees must have arguments";
		default: throw "Unknown error in DefinitionList::tryDefining()";
	}
}

int DefinitionList::validateInput(string& output, const string& input) const {

	bool function = false;
	unsigned i = 1;

	//check for asignee validity
	{
		bool prevComma = true, hasRightParen = false, noArgs = true;
		char c;

		if (!is_alpha(input[0]))
			return INVALID_ASSIGNEE;

		while (input[i] != ':') {

			c = input[i];

			if (c == ' ') {
				if (++i == input.length())
					return NO_ASSIGNMENT_OPERATOR;
				continue;
			}

			//check function validity
			if (function) {

				if (hasRightParen && c != ' ')
					return INVALID_ASSIGNEE;

				else if (c == ')') {
					if (prevComma)
						return COMMA_ERROR;
					hasRightParen = true;
				}

				else if (c == ',') {
					if (prevComma)
						return COMMA_ERROR;
					prevComma = true;
				}

				else if (prevComma && !is_alpha(c))
					return INVALID_ASSIGNEE_ARGUMENT;

				else if (!is_alpha(c) && !is_num(c))
					return INVALID_ASSIGNEE;

				else {
					prevComma = false;
					noArgs = false;
				}
			}

			else if (!is_alpha(c) && !is_num(c)) {
				//if there's a left paren, prepare to parse a function
				if (c == '(' && !function)
					function = true;
				else return INVALID_ASSIGNEE;
			}

			if (++i == input.length())
				return NO_ASSIGNMENT_OPERATOR;
		}

		if (input[++i] != '=')
			return NO_ASSIGNMENT_OPERATOR;

		else if (function && noArgs)
			return NO_ASIGNEE_ARGUMENTS;
	}

	output.clear();
	Expression::processExpression(output, input.substr(i + 1, input.length() - 1));
	output = input.substr(0, i + 1) + output;

	return function ? VALID_FUNCTION : VALID_VARIABLE;
}

void DefinitionList::addVariableDef(const string& input) {
	cout << "Valid variable: " << input << endl;
}

void DefinitionList::addFunctionDef(const string& input) {
	cout << "Valid function: " << input << endl;
}