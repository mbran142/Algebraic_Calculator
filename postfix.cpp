#include "calc.h"

void parse_expression(string input, string out) {

	out.clear();
	bool prev_operator = true;
	Stack<Operator> p;
	char prev_c, c;
	prev_c = '\0';

	for (unsigned i = 0; i < input.length(); i++) {

		c = input.at(i);

		if (c == ' ') {
			out.push_back(' ');
			continue;
		}

		//TODO: include variables here later
		if (is_num(c)) {

			//(a)b situation
			if (right_paren(prev_c)) {
				out.push_back('*');
				prev_operator = true;
			}

			if (!prev_operator) {
				out.clear();
				return;
			}

			while (is_num(input.at(i))) {

				out.push_back(input.at(i));
				i++;

				if (i == input.length())
					break;
			}
			i--;

			prev_operator = false;
		}

		else if (precedence(c) && !is_paren(c)) {

			if (prev_operator) {
				out.clear();
				return;
			}

			prev_operator = true;
		}

		else if (left_paren(c)) {

			//a(b) sitation and (a)(b) situation
			if (is_num(prev_c) || right_paren(prev_c)) {
				out.push_back('*');
				prev_operator = true;
			}

			p.push(Operator(c));
		}
		
		else if (right_paren(c)) {

			if (p.isEmpty() || left_paren(p.pop().oper) != right_paren(c) //make sure parens match
				|| prev_operator) { //make sure the thing before the right paren is an operand
				out.clear();
				return;
			}
		}

		else {
			out.clear();
			return;
		}

		if (!is_num(c))
			out.push_back(c);

		prev_c = c;
	}

	if (!p.isEmpty()) {
		out.clear();
		return;
	}
}

void to_postfix(string input, string out) {

	out.clear();
	Stack<Operator> o;

	for (unsigned i = 0; i < input.length(); i++) {

		if (input.at(i) == ' ')
			continue;

		else if (precedence(input.at(i))) {

			if (left_paren(input.at(i)))
				o.push(Operator('('));

			else if (right_paren(input.at(i))) {

				while (!left_paren(o.peek().oper)) {
					out.push_back(o.pop().oper);
					out.push_back(' ');
				}
			
				o.pop();
			}

			else {

				if (o.isEmpty() || precedence(input.at(i)) > precedence(o.peek().oper))
					o.push(Operator(input.at(i)));

				else {

					while (!o.isEmpty() && precedence(o.peek().oper) >= precedence(input.at(i)) && !left_paren(o.peek().oper)) {
						out.push_back(o.pop().oper);
						out.push_back(' ');
					}

					o.push(Operator(input.at(i)));
				}
			}
		}

		else if (is_num(input.at(i))) {

			while (is_num(input.at(i))) {

				out.push_back(((char)input.at(i)));
				i++;

				if (i == input.length())
					break;
			}

			i--;
			out.push_back(' ');
		}
	}

	while (!o.isEmpty()) {
		out.push_back(o.pop().oper);
		out.push_back(' ');
	}
}

inline int precedence(char c) {

	switch (c) {
		case '+': return 1;
		case '-': return 1;
		case '*': return 2;
		case '/': return 2;
		case '^': return 3;
		case '(': case ')':
		case '[': case ']':
		case '{': case '}': return 4;
		default : return 0;
	}
}

inline int is_paren(char c) {
	return left_paren(c) ? left_paren(c) : right_paren(c);
}

inline int left_paren(char c) {
	
	switch (c) {
		case '(': return 1;
		case '[': return 2;
		case '{': return 3;
		default : return 0;
	}
}

inline int right_paren(char c) {
	
	switch (c) {
		case ')': return 1;
		case ']': return 2;
		case '}': return 3;
		default : return 0;
	}
}