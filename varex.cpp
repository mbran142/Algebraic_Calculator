#include "calc.h"
#include "varex.h"

using namespace io;

//Operator
Operator::Operator(char c) {

	name = c;
	switch (c) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '^': args = 2; break;
		default: args = 0;
	}
}

Operator::Operator(const string& s, int argCount) {
	name = s;
	args = argCount;
}

Operator::Operator(const Operator& o) {
	name = o.name;
	args = o.args;
}

int Operator::instanceof() const {
	return OPERATOR;
}

string Operator::toStr() const {
	return name;
}

string Operator::toDetailedStr() const {

	//parenthesis
	if (is_paren(name[0]))
		return name;

	//standard operators (+, -, etc)
	else if (is_operator(name[0]))
		return '@' + name;

	//functions
	return '_' + name + '_' + to_string(args);
}


//Variable
Variable::Variable(char c) {
	name = c;
}

Variable::Variable(const string& s) {
	name = s;
}

int Variable::instanceof() const {
	return VARIABLE;
}

string Variable::toStr() const {
	return name;
}

string Variable::toDetailedStr() const {
	return '$' + name;
}

Variable& Variable::operator=(const Variable& source) {
	name = source.name;
	return *this;
}


//Expression
Expression::Expression() {
	postfix = NULL;
	infix = NULL;
}

Expression::Expression(const string& input) {

	postfix = infix = NULL;
	string processed;

	int size = this->processExpression(processed, input);
	this->createPostfix(processed, size);
	this->updateInfix(size);
}

Expression::Expression(const Expression& input) {

	postfix = new vector<Holder*>();
	postfix->reserve(input.postfix->size());

	infix = new vector<Holder*>();
	infix->reserve(input.infix->size());

	typename vector<Holder*>::iterator itr;

	for (itr = input.postfix->begin(); itr != input.postfix->end(); ++itr)
		postfix->push_back((*itr)->clone());

	for (itr = input.infix->begin(); itr != input.infix->end(); ++itr)
		infix->push_back((*itr)->clone());
}

Expression::~Expression() {
	this->freePostfix();
	this->freeInfix();
}

void Expression::freePostfix() {

	if (postfix != NULL) {

		for (typename vector<Holder*>::iterator itr = postfix->begin(); itr != postfix->end(); ++itr)
			delete *itr;

		delete postfix;
		postfix = NULL;
	}
}

void Expression::freeInfix() {

	if (infix != NULL) {

		for (typename vector<Holder*>::iterator itr = infix->begin(); itr != infix->end(); ++itr)
			delete *itr;

		delete infix;
		infix = NULL;
	}
}

void Expression::createPostfix(const string& input, int size) {

	this->freePostfix();
	postfix = new vector<Holder*>();
	if (size)
		postfix->reserve(size);

	Stack<Operator> o;
	string substr;
	bool negative;
	int temp, args;
	long temp_l;

	for (unsigned i = 0; i < input.length(); i++) {

		if (input[i] == ' ')
			continue;

		if (input[i] == ',') {

			while (!o.peek().leftParen())
				postfix->push_back(new Operator(o.pop()));

			continue;
		}

		negative = false;

		if (input[i] == '~') {
			negative = true;
			i++;
		}

		//function format is _[funcname]_[numargs]
		if (input[i] == '_') {

			i++;
			substr.clear();

			do {
				substr.push_back(input[i]);
				i++;
			} while (is_alpha(input[i]) || is_num(input[i]));
			i++;

			args = 0;
			while (is_num(input[i])) {
				args *= 10;
				args += input[i++] & 0xf;
			}
			i--;

			o.push(Operator(substr, args));
		}

		//Operators and parenthesis
		else if (is_operator(input[i]) || is_paren(input[i])) {

			if (left_paren(input[i]))
				o.push(Operator('('));

			else if (right_paren(input[i])) {

				while (!o.peek().leftParen())
					postfix->push_back(new Operator(o.pop()));

				o.pop();

				if (!o.isEmpty() && !(o.peek().isOperator()) && !(o.peek().isParen()))
					postfix->push_back(new Operator(o.pop()));
			}

			// Operators
			else {

				if (o.isEmpty() || precedence(input[i]) > precedence(o.peek().toStr()))
					o.push(Operator(input[i]));

				else {

					while (!o.isEmpty() && precedence(o.peek().toStr()) >= precedence(input[i])
						   && !left_paren(o.peek().toStr()[0]))
						postfix->push_back(new Operator(o.pop()));

					o.push(Operator(input[i]));
				}
			}
		}

		// Numbers
		else if (is_num(input[i])) {

			temp = 0;
			temp_l = 0;

			// Value component of the number
			do {
				temp_l *= 10;
				temp_l += input[i] & 0xf;
				i++;
			} while (input[i] != 'e');
			i++;

			if (negative)
				temp_l *= -1;

			// Power component of the mumber
			if (input[i] == '~') {
				negative = true;
				i++;
			}
			else negative = false;

			do {
				temp *= 10;
				temp += input[i] & 0xf;
				i++;
			} while (i != input.length() && is_num(input[i]));
			i--;

			if (negative)
				temp *= -1;

			postfix->push_back(new Number(temp_l, temp));
		}

		// Variables
		else if (is_alpha(input[i])) {

			substr.clear();

			do {
				substr.push_back(input[i]);
				i++;
			} while (i != input.length() && (is_alpha(input[i]) || is_num(input[i])));
			i--;

			postfix->push_back(new Variable(substr));
		}

		else {
			cout << "Unknown error in Expression::updatePostfix" << endl;
			exit(1);
		};

	} //endloop

	while (!o.isEmpty())
		postfix->push_back(new Operator(o.pop()));
}

//this will be called after the constructor creates a postfix vector
void Expression::updateInfix(int size) {

	Stack<string> s;
	Stack<string> t;
	string buf, temp_s;
	bool negative;
	unsigned temp;
	char low_op;
	int paren;

	this->freeInfix();
	infix = new vector<Holder*>();
	if (size)
		infix->reserve(size);

	for (typename vector<Holder*>::iterator itr = postfix->begin(); itr != postfix->end(); ++itr) {

		buf = (*itr)->toDetailedStr();

		//operand
		if ((*itr)->isOperand())
			s.push(buf);

		//function
		else if (buf[0] == '_') {

			temp = 0;
			unsigned j = 1;

			while (buf[j] != '_')
				j++;
			
			do {
				j++;
				temp *= 10;
				temp += buf[j] & 0xf;
			} while (j < buf.length() - 1);

			for (j = 0; j < temp; j++)
				t.push(s.pop());

			buf.push_back('(');

			while (!t.isEmpty())
				buf += t.pop() + ',';

			buf.pop_back();
			buf.push_back(')');
			s.push(buf);
		}

		//operator
		else {

			//second expression
			low_op = '\0';
			temp = 0;
			temp_s = s.pop();
			paren = 0;

			while (temp < temp_s.length()) {

				if (is_paren(temp_s[temp]))
					left_paren(temp_s[temp]) ? paren++ : paren--;

				//get the lowest-precedence operator outside of parenthesis in the expression
				else if (!paren && is_operator(temp_s[temp]) && precedence(temp_s[temp]) < precedence(low_op))
					low_op = temp_s[temp];

				temp++;
			}

			if (precedence(low_op) < precedence(buf[1]) || ((buf[1] == '/' || buf[1] == '-' || buf[1] == '^') && precedence(low_op) <= precedence(buf[1])))
				temp_s = '(' + temp_s + ')';

			//first expression
			temp = 0;
			low_op = '\0';
			paren = 0;

			//first expression
			while (temp < s.peek().length()) {

				if (is_paren(s.peek()[temp]))
					left_paren(s.peek()[temp]) ? paren++ : paren--;

				//get the lowest-precedence operator outside of parenthesis in the expression
				else if (!paren && is_operator(s.peek()[temp]) && precedence(s.peek()[temp]) < precedence(low_op))
					low_op = s.peek()[temp];

				temp++;
			}

			if (precedence(low_op) < precedence(buf[1]))
				s.peek() = '(' + s.peek() + ')';

			buf += temp_s;
			temp_s = s.pop();
			temp_s += buf;

			s.push(temp_s);
		}
	}

	buf = s.pop();
	/* codes are
	@ -> standard operator (+, -, *, etc)
	# -> Numbers
	$ -> Varaible
	_ -> Function
	parenthesis and commas both have no prefix
	*/
	for (unsigned i = 0; i < buf.length(); i++) {

		switch (buf[i]) {

			case '(': case ')': case ',':
				infix->push_back(new Operator(buf[i]));
				break;

			case '@':
				infix->push_back(new Operator(buf[++i]));
				break;

			case '#': {

				long value = 0, power = 0;
				if (buf[++i] == '-') {
					negative = true;
					i++;
				}
				else negative = false;

				while (is_num(buf[i])) {
					value *= 10;
					value += buf[i++] & 0xf;
				}
				if (negative) value *= -1;

				//skip 'e'
				if (buf[++i] == '-') {
					negative = true;
					i++;
				}
				else negative = false;
				while (i < buf.length() && is_num(buf[i])) {
					power *= 10;
					power += buf[i++] & 0xf;
				}
				if (negative) power *= -1;

				i--;
				infix->push_back(new Number(value, power));
			}
			break;

			case '$':
				temp_s.clear();
				i++;
				while (i < buf.length() && (is_num(buf[i]) || is_alpha(buf[i])))
					temp_s.push_back(buf[i++]);
				i--;

				infix->push_back(new Variable(temp_s));
				break;

			case '_':
				temp = 0;
				temp_s.clear();

				i++;
				while (buf[i] != '_')
					temp_s.push_back(buf[i++]);
				i++;

				while (i < buf.length() && is_num(buf[i])) {
					temp *= 10;
					temp += buf[i++] & 0xf;
				}
				i--;

				infix->push_back(new Operator(temp_s, temp));
				break;

			default:
				cout << "Unknown error in Expression::updateInfix()" << endl;
				exit(1);
		}
	}
}

Expression& Expression::operator=(const Expression& input) {

	if (this != &input) {

		this->freePostfix();
		this->freeInfix();

		postfix = new vector<Holder*>();
		postfix->reserve(input.postfix->size());

		infix = new vector<Holder*>();
		infix->reserve(input.infix->size());

		typename vector<Holder*>::iterator itr;

		for (itr = input.postfix->begin(); itr != input.postfix->end(); ++itr)
			postfix->push_back((*itr)->clone());

		for (itr = input.infix->begin(); itr != input.infix->end(); ++itr)
			infix->push_back((*itr)->clone());
	}

	return *this;
}

ostream& operator<<(ostream& os, const Expression& e) {

	for (typename vector<Holder*>::iterator itr = e.infix->begin(); itr != e.infix->end(); ++itr)
		cout << (*itr)->toStr();

	return os;
}

void Expression::simplify() {

	this->divToMult();
	this->subToAdd();

	/*bool done = true;

	this->divToMult();
	this->subToAdd();

	for (int i = 0; i < 3; i++) {

		switch (i) {
			case 0:
				done = done && this->simplifyExponents();
				break;
			case 1:
				done = done && this->simplifyAddition();
				break;
			case 2:
				done = done && this->processArithmetic();
				if (!done) i = -1;
		}
	}

	this->revertNegatives();*/
}

string Expression::evaluate() {

	//uses the current postfix vector to evaluate this
	return string("");
}

int Expression::processExpression(string& output, const string& input) {

	Stack<Operator> p;
	Stack<bool> f;
	string buf;
	bool decimal, prev_operator, in_function, negative;
	int temp, temp2, size, func_loc;
	long temp_l;
	char prev = '\0';

	prev_operator = true;
	in_function = false;
	size = 0;
	output.clear();

	//calculating projected size of expression and checks it for validity
	for (unsigned i = 0; i < input.length(); i++) {

		if (input[i] == ' ')
			continue;

		if (!f.isEmpty())
			in_function = f.peek();
		else in_function = false;

		if (in_function) {

			in_function = func_loc != p.getSize();

			if (in_function && input[i] == ',') {

				if (output[output.length() - 1] == ',' || prev_operator)
					throw "Syntax - Adjacent operators";

				prev_operator = true;
				output.push_back(',');
				continue;
			}
		}

		//negative char
		if (input[i] == '~') {

			negative = true;
			i++;

			if (i == input.length())
				throw "Syntax - Cannot end with operator";
		}
		else negative = false;

		if (is_operator(input[i])) { 

			if (prev_operator || negative) 
				throw prev ? "Syntax - Adjacent operators" : "Syntax - Cannot begin with operator";

			output.push_back(input[i]);
			size++;
			prev_operator = true;
		}

		//iterate through a number input
		else if (is_num(input[i]) || input[i] == '.') {

			temp_l = 0;
			temp = 0;
			size++;
			decimal = false;

			//implicit multiplication (1)(2) situations
			if (!prev_operator) {

					if (left_paren(prev)) {
						output[output.length() - 1] = '*';
						output.push_back(prev);
					}

					else
						output.push_back('*');
					
					size++;
			}

			do {

				if (input[i] == '.') {
					if (decimal)
						throw "Syntax - Multiple decimal points";
					decimal = true;
				}

				else {

					if (temp_l < MAX_MULT) {
						temp_l *= 10;
						temp_l += input[i] & 0xf;
					}
					else
						temp++;

					if (decimal)
						temp--;
				}

				i++;

			} while (i != input.length() && (is_num(input[i]) || input[i] == '.'));

			if (input[i - 1] == '.' && (i < 2 || !is_num(input[i - 2])))
				throw "Syntax - Decimal with no number";
			
			if (i != input.length() && (input[i] | ' ') == 'e') {

				i++;
				temp2 = 0;

				while (i != input.length() && (is_num(input[i]))) {

					if (temp2 > (MAX_INT / 10))
						throw "Syntax - Number overflow";

					temp2 *= 10;
					temp2 += input[i] & 0xf;
					i++;

				}

				if ((input[i - 1] | ' ') == 'e')
					throw "Syntax - Integer must follow scientific \'e\'";

				temp += temp2;
			}
			i--;

			if (negative)
				output.push_back('~');

			buf.clear();

			//number component
			do {
				buf.push_back(static_cast<char>((temp_l % 10) + '0'));
				temp_l /= 10;
			} while (temp_l);

			for (unsigned j = 0, k = buf.length(); j < k; j++) {
				output.push_back(buf[buf.length() - 1]);
				buf.pop_back();
			}

			//power component
			output.push_back('e');

			if (temp < 0) {
				temp *= -1;
				output.push_back('~');
			}

			do {
				buf.push_back(static_cast<char>((temp % 10) + '0'));
				temp /= 10;
			} while (temp);

			for (unsigned j = 0, k = buf.length(); j < k; j++) {
				output.push_back(buf[buf.length() - 1]);
				buf.pop_back();
			}

			prev_operator = false;
		}

		//iterate through a function or variable
		else if (is_alpha(input[i])) {

			if (negative)
				throw "Syntax - '~' not allowed on variables or functions";

			size++;
			temp = i;

			do {
				temp++;
			} while (temp != static_cast<int>(input.length()) && (is_alpha(input[temp]) || is_num(input[temp])));

			//function notation
			if (left_paren(input[temp])) {

				prev_operator = true;

				if (!in_function) {
					in_function = true;
					func_loc = p.getSize();
				}

				//preceding _ character denotes a function
				output.push_back('_');
				do {
					output.push_back(input[i]);
					i++;
				} while (i != input.length() && (is_alpha(input[i]) || is_num(input[i])));
				output.push_back('_'); //extra paren

				p.push(Operator(input[i]));
				f.push(true);

				//count arguments of function
				temp = 1;
				temp2 = 1;
				for (unsigned j = i + 1; temp2; j++) {

					if (j >= input.length())
						throw "Syntax - Mismatching parenthesis";

					if (input[j] == ',' && temp2 == 1)
						temp++;

					else if (input[j] == ')')
						temp2--;

					else if (input[j] == '(')
						temp2++;
				}

				do {
					buf.push_back(static_cast<char>((temp % 10) + '0'));
					temp /= 10;
				} while (temp);

				for (unsigned j = 0, k = buf.length(); j < k; j++) {
					output.push_back(buf[buf.length() - 1]);
					buf.pop_back();
				}

				output.push_back('(');
			}

			//non-function
			else {

				//implicit multiplication (a)(b) situations
				if (!prev_operator) {

					if (left_paren(prev)) {
						output[output.length() - 1] = '*';
						output.push_back(prev);
					}

					else
						output.push_back('*');
					
					size++;
				}

				do {
					output.push_back(input[i]);
					i++;
				} while (i != input.length() && (is_alpha(input[i]) || is_num(input[i])));
				i--;

				prev_operator = false;
			}
		}
		
		//check for matching parenthesis
		else if (is_paren(input[i])) {

			if (left_paren(input[i])) {
				p.push(Operator(input[i]));
				f.push(false);

				if (negative) {
					output += "~1e0*";
					size += 2;
				}
			}

			else {

				if (p.isEmpty() || left_paren(p.pop().toStr().at(0)) != right_paren(input[i]))
					throw "Syntax - Mismatching parenthesis";

				else if (prev_operator)
					throw "Syntax - Adjacent operators";

				f.pop();
			}

			output.push_back(input[i]);
		}

		else if (input[i] == ',')
			throw "Syntax - Comma out of function"; 

		else throw "Syntax - Unknown character input";

		prev = input[i];
	}

	if (!p.isEmpty())
		throw "Syntax - Mismatching parenthesis";

	if (prev_operator)
		throw "Syntax - Cannot end with operator";

	return size;
}

string Expression::toStr() {

	string output;

	for (typename vector<Holder*>::iterator itr = infix->begin(); itr != infix->end(); ++itr)
		output += (*itr)->toStr();

	return output;
}

string Expression::getPostfixString() {

	string output;

	for (typename vector<Holder*>::iterator itr = postfix->begin(); itr != postfix->end(); ++itr) {

		if (itr != postfix->begin())
			output.push_back(',');

		output += (*itr)->toStr();
	}

	return output;
}

// //checking if something is a function
// /*else if (is_alpha(processed[i])) {

// 	substr = string();

// 	do {
// 		substr.push_back(processed[i]);
// 		i++;
// 	} while (is_alpha(processed[i]));

// 	if (!in_funclist(substr))
// 		throw "Syntax - Function not recognized";
// }*/

// Expression::~Expression() {
// 	this->clearPostfix();
// 	this->clearInfix();
// 	delete postfix;
// 	delete infix;
// }


// void Expression::simplify() {

// 	this->div_to_mult();
// 	this->sub_to_add();
// 	this->simplify_exponents();
// 	this->simplify_addition();
// 	this->process_arithmetic();
// 	this->revert_negatives();
// }

// ostream& operator<<(ostream& os, const Expression& e) {

// 	for (typename vector<Holder*>::iterator itr = e.infix->begin(); itr != e.infix->end(); ++itr)
// 		cout << (*itr)->toStr();

// 	return os;
// }

// int Expression::process_expression(string& output, const string& input) {

// 	Stack<Operator> p;
// 	Stack<Operator> f;
// 	string buf;
// 	bool decimal, prev_operator, in_function, negative;
// 	int temp, temp2, size, func_loc;
// 	long temp_l;
// 	char prev = '\0';

// 	prev_operator = true;
// 	in_function = false;
// 	size = 0;
// 	output.clear();

// 	//calculating projected size of expression and checks it for validity
// 	for (unsigned i = 0; i < input.length(); i++) {

// 		if (input[i] == ' ')
// 			continue;

// 		if (!f.isEmpty())
// 			in_function = f.peek().toStr()[0] == 'f';
// 		else in_function = false;

// 		if (in_function) {

// 			in_function = func_loc != p.getSize();

// 			if (in_function && input[i] == ',') {

// 				if (output[output.length() - 1] == ',' || prev_operator)
// 					throw "Syntax - Adjacent operators";

// 				prev_operator = true;
// 				output.push_back(',');
// 				continue;
// 			}
// 		}

// 		//negative char
// 		if (input[i] == '~') {

// 			negative = true;
// 			i++;

// 			if (i == input.length())
// 				throw "Syntax - Cannot end with operator";
// 		}
// 		else negative = false;

// 		if (isOperator(input[i])) { 

// 			if (prev_operator || negative) 
// 				throw prev ? "Syntax - Adjacent operators" : "Syntax - Cannot begin with operator";

// 			output.push_back(input[i]);
// 			size++;
// 			prev_operator = true;
// 		}

// 		//iterate through a number input
// 		else if (is_num(input[i]) || input[i] == '.') {

// 			temp_l = 0;
// 			temp = 0;
// 			size++;
// 			decimal = false;

// 			//implicit multiplication (1)(2) situations
// 			if (!prev_operator) {

// 					if (left_paren(prev)) {
// 						output[output.length() - 1] = '*';
// 						output.push_back(prev);
// 					}

// 					else
// 						output.push_back('*');
					
// 					size++;
// 			}

// 			do {

// 				if (input[i] == '.') {
// 					if (decimal)
// 						throw "Syntax - Multiple decimal points";
// 					decimal = true;
// 				}

// 				else {

// 					if (temp_l < MAX_MULT) {
// 						temp_l *= 10;
// 						temp_l += input[i] & 0xf;
// 					}
// 					else
// 						temp++;

// 					if (decimal)
// 						temp--;
// 				}

// 				i++;

// 			} while (i != input.length() && (is_num(input[i]) || input[i] == '.'));

// 			if (input[i - 1] == '.' && (i < 2 || !is_num(input[i - 2])))
// 				throw "Syntax - Decimal with no number";
			
// 			if (i != input.length() && (input[i] | ' ') == 'e') {

// 				i++;
// 				temp2 = 0;

// 				while (i != input.length() && (is_num(input[i]))) {

// 					if (temp2 > (MAX_INT / 10))
// 						throw "Syntax - Number overflow";

// 					temp2 *= 10;
// 					temp2 += input[i] & 0xf;
// 					i++;

// 				}

// 				if ((input[i - 1] | ' ') == 'e')
// 					throw "Syntax - Integer must follow scientific \'e\'";

// 				temp += temp2;
// 			}
// 			i--;

// 			if (negative)
// 				output.push_back('~');

// 			buf.clear();

// 			//number component
// 			do {
// 				buf.push_back(static_cast<char>((temp_l % 10) + '0'));
// 				temp_l /= 10;
// 			} while (temp_l);

// 			for (unsigned j = 0, k = buf.length(); j < k; j++) {
// 				output.push_back(buf[buf.length() - 1]);
// 				buf.pop_back();
// 			}

// 			//power component
// 			output.push_back('e');

// 			if (temp < 0) {
// 				temp *= -1;
// 				output.push_back('~');
// 			}

// 			do {
// 				buf.push_back(static_cast<char>((temp % 10) + '0'));
// 				temp /= 10;
// 			} while (temp);

// 			for (unsigned j = 0, k = buf.length(); j < k; j++) {
// 				output.push_back(buf[buf.length() - 1]);
// 				buf.pop_back();
// 			}

// 			prev_operator = false;
// 		}

// 		//iterate through a function or variable
// 		else if (is_alpha(input[i])) {

// 			if (negative) {
// 				output += "~1e0*";
// 				size += 2;
// 			}

// 			size++;
// 			temp = i;

// 			do {
// 				temp++;
// 			} while (temp != static_cast<int>(input.length()) && (is_alpha(input[temp]) || is_num(input[temp])));

// 			//function notation
// 			if (left_paren(input[temp])) {

// 				prev_operator = true;

// 				if (!in_function) {
// 					in_function = true;
// 					func_loc = p.getSize();
// 				}

// 				//preceding _ character denotes a function
// 				output.push_back('_');
// 				do {
// 					output.push_back(input[i]);
// 					i++;
// 				} while (i != input.length() && (is_alpha(input[i]) || is_num(input[i])));
// 				output.push_back(input[i]); //extra paren

// 				p.push(Operator(input[i]));
// 				f.push(Operator('f'));
// 			}

// 			//non-function
// 			else {

// 				//implicit multiplication (a)(b) situations
// 				if (!prev_operator) {

// 					if (left_paren(prev)) {
// 						output[output.length() - 1] = '*';
// 						output.push_back(prev);
// 					}

// 					else
// 						output.push_back('*');
					
// 					size++;
// 				}

// 				do {
// 					output.push_back(input[i]);
// 					i++;
// 				} while (i != input.length() && (is_alpha(input[i]) || is_num(input[i])));
// 				i--;

// 				prev_operator = false;
// 			}
// 		}
		
// 		//check for matching parenthesis
// 		else if (is_paren(input[i])) {

// 			if (left_paren(input[i])) {
// 				p.push(Operator(input[i]));
// 				f.push(Operator('n'));

// 				if (negative) {
// 					output += "~1e0*";
// 					size += 2;
// 				}
// 			}

// 			else {

// 				if (p.isEmpty() || left_paren(p.pop().toStr().at(0)) != right_paren(input[i]))
// 					throw "Syntax - Mismatching parenthesis";

// 				else if (prev_operator)
// 					throw "Syntax - Adjacent operators";

// 				f.pop();
// 			}

// 			output.push_back(input[i]);
// 		}

// 		else if (input[i] == ',')
// 			throw "Syntax - Comma out of function"; 

// 		else throw "Syntax - Unknown character input";

// 		prev = input[i];
// 	}

// 	if (!p.isEmpty())
// 		throw "Syntax - Mismatching parenthesis";

// 	if (prev_operator)
// 		throw "Syntax - Cannot end with operator";

// 	return size;
// }

// void Expression::clearPostfix() {

// 	for (typename vector<Holder*>::iterator itr = postfix->begin(); itr != postfix->end(); ++itr)
// 		delete *itr;

// 	postfix->clear();
// }

// void Expression::clearInfix() {

// 	for (typename vector<Holder*>::iterator itr = infix->begin(); itr != infix->end(); ++itr)
// 		delete *itr;

// 	infix->clear();
// }

// int in_funclist(const string& input) {

// 	int matches = 0;
// 	int id = 0;

// 	for (unsigned i = 0; i < LIST_SIZE; i++) {

// 		for (unsigned j = 0; j < input.length(); j++) {

// 			if (j == funclist[i].length() || funclist[i][j] != (input[j] | ' '))
// 				break;

// 			if (j == input.length() - 1) {
// 				matches++;
// 				id = i;
// 			}
// 		}

// 		if (matches > 1)
// 			break;
// 	}

// 	if (matches != 1)
// 		return 0;

// 	return id;
// }