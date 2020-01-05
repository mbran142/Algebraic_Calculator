#include "varex.h"
#include "definition.h"
#include "calc.h"

extern DefinitionList defList;

struct intBool {
	int integer;
	bool boolean;
	intBool(int i, bool b) : integer(i), boolean(b) {};
};

void Expression::divToMult() {

	int index = 0, paren, count;
	bool extraParen;
	char op;
	Stack<int> exp;
	Stack<intBool> mult;
	vector<Holder*>* newInfix = new vector<Holder*>();
	newInfix->reserve(infix->size());
	typename vector<Holder*>::iterator itr, temp;

	for (itr = infix->begin(); itr < infix->end(); ++itr, ++index) {

		//change all division to multiplication ^~1
		if ((*itr)->toStr() == "/") {

			newInfix->push_back(new Operator('*'));

			//find where the next ^~1 should go
			temp = itr + 1;
			count = paren = 0;
			do {
				if ((*temp)->isFunction()) {
					++temp; ++count;
				}
				if ((*temp)->isParen())
					(*temp)->leftParen() ? paren++ : paren--;
				++temp; ++count;
			} while (paren);

			exp.push(count + index);
		}

		//copy the old infix into the new infix
		else newInfix->push_back((*itr)->clone());

		//if a negative exponent needs to be added...
		if (!exp.isEmpty() && exp.peek() == index) {

			++itr; ++index;
			exp.pop();
			newInfix->push_back(new Operator('^'));

			//simple case when we just add ^~1
			if (itr >= infix->end() || (*itr)->toStr() != "^") {
				newInfix->push_back(new Number(-1));
				--itr; --index;
			}

			//cases where the exponent exists and must be negated
			else {

				//figure out where the )*~1 needs to be added
				temp = itr + 1;
				count = paren = 0;
				extraParen = false;
				do {

					if ((*temp)->isFunction()) {
						++temp; ++count;
					}

					else  {
						op = (*temp)->toStr()[0];

						if (is_paren(op))
							left_paren(op) ? paren++ : paren--;

						else if (paren == 0 && (op == '+' || op == '-'))
							extraParen = true;
					}

					++temp; ++count;

				} while (temp != infix->end() &&
					(paren || (*temp)->isFunction() || (*temp)->isOperand() || (*temp)->toStr() == "^" || (*temp)->leftParen()));

				if (extraParen)
					newInfix->push_back(new Operator('('));
				mult.push(intBool(count + index, extraParen));
				newInfix->push_back(new Operator('('));
			}
		}

		//if the end of an existing exponent that needs to be negated is here, negate it
		if (!mult.isEmpty() && mult.peek().integer == index) {
			if (mult.pop().boolean)
				newInfix->push_back(new Operator(')'));
			newInfix->push_back(new Operator('*'));
			newInfix->push_back(new Number(-1));
			newInfix->push_back(new Operator(')'));
		}

		//cout << vprint(newInfix) << endl;
	}

	this->freeInfix();
	infix = newInfix;
}

void Expression::subToAdd() {

	int index = 0, paren, count, end;
	char op;
	bool done;
	Stack<int> mult;
	vector<Holder*>* newInfix = new vector<Holder*>();
	newInfix->reserve(infix->size());
	typename vector<Holder*>::iterator itr, temp;

	for (itr = infix->begin(); itr < infix->end(); ++itr, ++index) {

		//if a negative should be placed, place it
		if (!mult.isEmpty() && mult.peek() == index) {
			mult.pop();
			newInfix->push_back(new Operator('*'));
			newInfix->push_back(new Number(-1));
			newInfix->push_back(new Operator(')'));
		}

		//change all substraction to addition *-1
		if ((*itr)->toStr() == "-") {

			newInfix->push_back(new Operator('+'));
			newInfix->push_back(new Operator('('));

			//decide where to place the *-1
			end = count = paren = 0;
			done = false;

			for (temp = itr + 1; temp != infix->end() && !done; ++temp, ++count) {

				if (!(*temp)->isOperand() && !(*temp)->isFunction()) {

					op = (*temp)->toStr()[0];

					if (op == '(') paren++;
					else if (op == ')') {
						paren--;
						if (!paren) end = count;
						else if (paren == -1) done = true; //if paren is -1, the scope has ended
					}

					done = done || (!paren && (op == '+' || op == '-'));
				}

				else if ((*temp)->isOperand() && !paren)
					end = count;
			}

			mult.push(end + index + 2);
		}

		else newInfix->push_back((*itr)->clone());

		//cout << vprint(newInfix) << endl;
	}

	while (!mult.isEmpty()) {
		mult.pop();
		newInfix->push_back(new Operator('*'));
		newInfix->push_back(new Number(-1));
		newInfix->push_back(new Operator(')'));
	}

	this->freeInfix();
	infix = newInfix;
}

bool Expression::simplifyExponents() {

	return true;
}

bool Expression::simplifyAddition() {

	return true;
}

bool Expression::processArithmetic() {

	return true;
}

void Expression::revertNegatives() {

}

string vprint(vector<Holder*>* v) {

	string out;
	for (typename vector<Holder*>::iterator itr = v->begin(); itr != v->end(); ++itr)
		out += (*itr)->toStr();

	return out;
}