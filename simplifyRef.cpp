#include "calc.h"
#include "varex.h"

void remove_constants(string&, const string&);
void extract_constants(string&, const string&);
void add_nodup(Lnode<string>*&, const string&, int&);
void add_dup(Lnode<string>*&, const string&, int&);

void div_to_mult(string& exp) {

	//strategy: take advantage of the following exponnent simplifications;
	//			just great all division at multiplication times exp^-1

	Stack<Operator> s;
	string buf;
	bool no_paren;

	for (unsigned i = 0; i < exp.length(); i++) {

		no_paren = false;

		if (exp[i] == '/') {

			i++;
			buf.push_back('*');

			if (!left_paren(exp[i]))
				no_paren = true;

			else {
				buf.push_back('(');
				s.push(Operator('y'));
				continue;
			}
		}

		else buf.push_back(exp[i]);

		if (no_paren || is_paren(exp[i])) {

			if (left_paren(exp[i]))
				s.push(Operator('n'));

			else if (no_paren || s.pop().to_str()[0] == 'y') {

				if (!no_paren)
					i++;

				while (i < exp.length() && (is_alpha(exp[i]) || is_num(exp[i]))) {
					buf.push_back(exp[i]);
					i++;
				}
			
				if (i < exp.length() && exp[i] == '^') {

					buf.push_back('^');
					i++;

					if (exp[i] != '~')
						buf.push_back('~');
					else i++;

					do {
						buf.push_back(exp[i]);
						i++;
					} while (i < exp.length() && (is_alpha(exp[i]) || is_num(exp[i])));

				}

				else buf += "^~1";
				i--;
			}
		}
	}

	exp = buf;
}

void sub_to_add(string& exp) {

	//strategy: similar to the div_to_mult(), we're just changing all
	//			subtraction to addition *~1

	Stack<Operator> s;
	string buf;
	bool no_paren;

	for (unsigned i = 0; i < exp.length(); i++) {

		no_paren = false;

		if (exp[i] == '-') {

			i++;
			buf.push_back('+');

			if (!left_paren(exp[i]))
				no_paren = true;

			else {
				buf.push_back('(');
				s.push(Operator('y'));
				continue;
			}
		}

		else buf.push_back(exp[i]);

		if (no_paren || is_paren(exp[i])) {

			if (left_paren(exp[i]))
				s.push(Operator('n'));

			else if (no_paren || s.pop().to_str()[0] == 'y') {

				if (!no_paren)
					i++;

				while (i < exp.length() && (is_alpha(exp[i]) || is_num(exp[i]))) {
					buf.push_back(exp[i]);
					i++;
				}
			
				if (i < exp.length() && exp[i] == '*') {

					buf.push_back('*');
					i++;

					if (exp[i] != '~')
						buf.push_back('~');
					else i++;

					do {
						buf.push_back(exp[i]);
						i++;
					} while (i < exp.length() && (is_alpha(exp[i]) || is_num(exp[i])));

				}

				else buf += "*~1";
				i--;
			}
		}
	}

	exp = buf;
}

void simplify_exponents(string& exp) {

	//strategy: add up all the exponents of like terms
	//			and simplify notation if extra 1's exist

	string buf, temp, temp_exp, *var_arr, *power_arr;
	int size, temp2, start, paren, *neg_arr;
	unsigned register i;
	bool negative;
	Lnode<string> *list, *p;
	Stack<Integer> start_point;

	list = NULL;
	start_point.push(Integer(0));
	temp_exp = exp;

	//get starting points
	for (i = 0; i < exp.length(); i++) {

		if (left_paren(exp[i]) || exp[i] == ',') {

			i++;
			paren = 0;

			for (unsigned j = i; paren || (!right_paren(exp[j]) && exp[j] != ','); j++) {

				if (is_paren(exp[j]))
					left_paren(exp[j]) ? paren++ : paren--;

				else if (!paren && exp[j] == '*') {
					start_point.push(Integer(i));
					break;
				}
			}
		}
	}

	while (!start_point.isEmpty()) {

		start = start_point.pop().data;
		size = 0;
		i = start;

		for (unsigned j = i; j < exp.length() && !is_paren(exp[j]) && exp[j] != '+' && exp[j] != '-' && exp[j] != ','; j++) {

			//skip over anything in parenthesis
			if (exp[j] == '^') {

				bool paren = false;
				temp2 = 0;

				do {
					j++;

					if (is_paren(exp[j])) {
						paren = true;
						left_paren(exp[j]) ? temp2++ : temp2--;
					}

				} while (j < exp.length() && (temp2 || is_alpha(exp[j]) || is_num(exp[j]) || exp[j] == '^' || exp[j] == '~'));
				
				if (!paren)
					j--;

				continue;
			}

			//if there's a variable in a term, add it to the list
			if (is_alpha(exp[j]) || is_num(exp[j])) {

				temp.clear();
				bool var = is_alpha(exp[j]);

				do {
					temp.push_back(exp[j]);
					j++;
				} while (j < exp.length() && (is_alpha(exp[j]) || is_num(exp[j])));
				j--;

				//ignore functions
				if (var && j + 1 < exp.length() && left_paren(exp[j + 1]))
					continue;

				add_nodup(list, temp, size);
			}
		}

		if (!size)
			continue;

		//put the contents of the list into a static array
		var_arr = new string[size];
		power_arr = new string[size];
		neg_arr = new int[size];

		for (int j = 0; j < size; j++) {
			p = list->next;
			var_arr[j] = list->data;
			power_arr[j].clear();
			neg_arr[j] = 0;
			delete list;
			list = p;
		}

		//add up the powers of the variables
		while (i < temp_exp.length() && !right_paren(temp_exp[i]) && temp_exp[i] != '+' && temp_exp[i] != '-' && temp_exp[i] != ',') {

			if (temp_exp[i] == '~') {
				negative = true;
				i++;
			}

			if (is_alpha(temp_exp[i]) || is_num(temp_exp[i])) {

				temp.clear();

				do {
					temp.push_back(temp_exp[i]);
					i++;
				} while (i < temp_exp.length() && (is_alpha(temp_exp[i]) || is_num(temp_exp[i])));

				temp2 = 0;
				while (temp != var_arr[temp2])
					temp2++;

				if (negative)
					neg_arr[temp2]++;

				negative = i < temp_exp.length() && temp_exp[i] == '^' && temp_exp[i + 1] == '~';

				if (negative)
					power_arr[temp2].push_back(power_arr[temp2].length() ? '-' : '~');
				else if (power_arr[temp2].length())
					power_arr[temp2].push_back('+');

				//add the powers 
				if (i < temp_exp.length() && temp_exp[i] == '^') {

					paren = 0;

					//skip '~' if negative
					i += negative ? 2 : 1;

					do {

						if (is_paren(temp_exp[i])) {

							left_paren(temp_exp[i]) ? paren++ : paren--;

							if (!power_arr[temp2].length()) {
								i++;
								continue;
							}
						}

						power_arr[temp2].push_back(temp_exp[i]);
						i++;

					} while (paren || is_alpha(temp_exp[i]) || is_num(temp_exp[i]) || temp_exp[i] == '^');

					if (right_paren(power_arr[temp2][power_arr[temp2].length() - 1]))
						power_arr[temp2].pop_back();

				}
				else power_arr[temp2].push_back('1');
			}

			else i++;
		}

		buf.clear();

		//adding powers of common terms
		for (int j = 0; j < size; j++) {

			temp2 = 0;

			for (unsigned k = 0; k < power_arr[j].length(); k++)

				if (is_paren(power_arr[j][k]))
					left_paren(power_arr[j][k]) ? temp2++ : temp2--;

				else if (!temp2 && is_operator(power_arr[j][k]) && power_arr[j][k] != '^') {
					power_arr[j] = '(' + power_arr[j] + ')';
					break;
				}

			if (neg_arr[j] & 0x1)
				var_arr[j] = '~' + var_arr[j];

			if (power_arr[j] == "0")
				buf.push_back('1');

			else if (power_arr[j] == "1")
				buf += var_arr[j];

			else
				buf += var_arr[j] + '^' + power_arr[j];

			if (j != size - 1)
				buf.push_back('*');
		}

		temp2 = i;
		i = start + buf.length();
		exp = temp_exp = temp_exp.substr(0, start) + buf + temp_exp.substr(temp2, temp_exp.length() - temp2);

		delete [] var_arr;
		delete [] power_arr;
		delete [] neg_arr;
	}
}

void simplify_addition(string& exp) {

	string buf, temp, extra, *unique_arr, *term_arr, *add_arr;
	int  term_size, start, paren, unique_size, temp2;
	unsigned register i;
	bool constant;
	Lnode<string> *list, *p;
	Stack<Integer> start_point;

	list = NULL;
	start_point.push(Integer(0));

	//get starting points
	for (i = 0; i < exp.length(); i++) {

		if (left_paren(exp[i]) || exp[i] == ',') {

			i++;
			paren = 0;

			for (unsigned j = i; paren || (!right_paren(exp[j]) && exp[j] != ','); j++) {

				if (is_paren(exp[j]))
					left_paren(exp[j]) ? paren++ : paren--;

				else if (!paren && exp[j] == '+') {
					start_point.push(Integer(i));
					break;
				}
			}
		}
	}

	while (!start_point.isEmpty()) {

		start = start_point.pop().data;
		term_size = 0;
		extra.clear();

		for (i = start; i < exp.length() && !right_paren(exp[i]) && exp[i] != ','; i++) {

			if (left_paren(exp[i])) {

				paren = 0;

				do {

					if (is_paren(exp[i]))
						left_paren(exp[i]) ? paren++ : paren--;

					i++;

				} while (paren);

				i--;
			}

			//add the term into the list
			else if (is_alpha(exp[i]) || is_num(exp[i]) || exp[i] == '~') {

				temp.clear();
				paren = 0;

				do {

					temp.push_back(exp[i]);
					i++;

					if (is_paren(exp[i]))
						left_paren(exp[i]) ? paren++ : paren--;

				} while (i < exp.length() && exp[i] != ',' && paren != -1 && (paren || (exp[i] != '+')));
				i--;

				constant = true;

				for (unsigned j = 0; j < temp.length(); j++)
					if (is_alpha(temp[j])) {
						constant = false;
						break;
					}

				if (constant)
					extra.length() ? extra += '+' + temp : extra = temp;
				else
					add_dup(list, temp, term_size);
			}
		}

		//put the contents of the list into a static array
		term_arr = new string[term_size];

		for (int j = 0; j < term_size; j++) {
			p = list->next;
			term_arr[j] = list->data;
			delete list;
			list = p;
		}

		unique_size = 0;

		//get unique terms
		for (int j = 0; j < term_size; j++) {
			remove_constants(temp, term_arr[j]);
			add_nodup(list, temp, unique_size);
		}

		//add unique terms to static array
		unique_arr = new string[unique_size];
		add_arr = new string[unique_size];

		for (int j = 0; j < unique_size; j++) {
			p = list->next;
			unique_arr[j] = list->data;
			delete list;
			list = p;
		}

		//get the constant multiples of each unique term
		for (int j = 0; j < term_size; j++) {

			remove_constants(temp, term_arr[j]);
			temp2 = 0;

			while (temp != unique_arr[temp2])
				temp2++;

			extract_constants(temp, term_arr[j]);

			if (add_arr[temp2].length())
				add_arr[temp2] += '+' + temp;
			else
				add_arr[temp2] = temp;
		}

		// cout << "Terms:";
		// for (int j = 0; j < term_size; j++)
		// 	cout << term_arr[j] << " ";

		// cout << "\nUnique terms:";
		// for (int j = 0; j < unique_size; j++)
		// 	cout << unique_arr[j] << " ";

		// cout << "\nAdded terms:";
		// for (int j = 0; j < unique_size; j++)
		// 	cout << add_arr[j] << " ";
		// cout << endl;

		buf.clear();

		//combine the constant multiples to each term to simplify
		for (int j = 0; j < unique_size; j++) {

			temp2 = 0;

			for (unsigned k = 0; k < add_arr[j].length(); k++)

				if (is_paren(add_arr[j][k]))
					left_paren(add_arr[j][k]) ? temp2++ : temp2--;

				else if (!temp2 && is_operator(add_arr[j][k]) && add_arr[j][k] != '^') {
					add_arr[j] = '(' + add_arr[j] + ')';
					break;
				}

			if (add_arr[j] == "1")
				buf += unique_arr[j];

			else if (add_arr[j] != "1")
				buf += add_arr[j] + '*' + unique_arr[j];

			if (j != unique_size - 1)
				buf.push_back('+');
		}

		//putting everything together
		temp2 = i;
		i = start + buf.length();

		if (buf.length() && extra.length())
			buf += '+' + extra;

		else if (!buf.length() && extra.length())
			buf = extra;

		exp = exp.substr(0, start) + buf + exp.substr(temp2, exp.length() - temp2);

		delete [] term_arr;
		delete [] unique_arr;
		delete [] add_arr;
	}
}

void process_arithmetic(string& exp) {

	/* Strategy:

	for + and *, just extract constants (for +, constant terms and * constant factors)
	for ^ remember:

		2 ^ (a ^ (2 ^ 2) ) = 2 ^ (a ^ 4)
		2 ^ a ^ 2 ^ 2 = 2 ^ a ^ 4

	*/
}

void remove_constants(string& out, const string& exp) {

	out.clear();
	bool copy;
	unsigned j;

	for (unsigned i = 0; i < exp.length(); i++) {

		copy = false;

		for (j = i; j < exp.length() && exp[j] != '*'; j++) {

			if (is_alpha(exp[j])) {
				copy = true;
				break;
			}
		}

		while (i < exp.length() && exp[i] != '*') {

			if (copy)
				out.push_back(exp[i]);
			i++;
		}

		if (copy && exp[i] == '*')
			out.push_back('*');
	}

	if (is_operator(out[out.length() - 1]))
		out.pop_back();
}

void extract_constants(string& out, const string& exp) {

	out.clear();
	bool copy;
	unsigned j;

	for (unsigned i = 0; i < exp.length(); i++) {

		copy = true;

		for (j = i; j < exp.length() && exp[j] != '*'; j++) {

			if (is_alpha(exp[j])) {
				copy = false;
				break;
			}
		}

		while (i < exp.length() && exp[i] != '*') {

			if (copy)
				out.push_back(exp[i]);
			i++;
		}

		if (copy && exp[i] == '*')
			out.push_back('*');
	}

	if (out.length() == 0)
		out = '1';

	else if (is_operator(out[out.length() -1]))
		out.pop_back();
}

void add_nodup(Lnode<string>*& list, const string& str, int& count) {

	//adding new variable to the list in alphabetical order
	Lnode<string>* p = new Lnode<string>(str);
	Lnode<string>* q = list;
	Lnode<string>* r = NULL;

	count++;

	while (q != NULL && q->data.compare(str) < 0) {
		r = q;
		q = q->next;
	}

	//if the comparison returns 0 then don't add it to the list
	if (q != NULL && !q->data.compare(str)) {
		delete p;
		count--;
	}

	else if (q == NULL) {

		if (list == NULL)
			list = p;

		else r->next = p;
	}

	else if (r == NULL) {
		list = p;
		list->next = q;
	}

	else {
		r->next = p;
		p->next = q;
	}
}

void add_dup(Lnode<string>*& list, const string& str, int& count) {

	//adding new variable to the list in alphabetical order
	Lnode<string>* p = new Lnode<string>(str);
	Lnode<string>* q = list;
	Lnode<string>* r = NULL;

	count++;

	while (q != NULL && q->data.compare(str) < 0) {
		r = q;
		q = q->next;
	}

	if (q == NULL) {

		if (list == NULL)
			list = p;

		else r->next = p;
	}

	else if (r == NULL) {
		list = p;
		list->next = q;
	}

	else {
		r->next = p;
		p->next = q;
	}
}

/*void simplify_addition(string& exp) {

	string buf, temp, temp_exp, *var_arr, *term_arr;
	int var_size, term_size, start, paren, temp2, *var_count;
	unsigned register i;
	Lnode<string> *list, *p, *q, *r;
	Stack<Integer> start_point;

	list = NULL;
	start_point.push(Integer(0));
	temp_exp = exp;

	//get starting points
	for (i = 0; i < exp.length(); i++) {

		if (left_paren(exp[i]) || exp[i] == ',') {

			i++;
			paren = 0;

			for (unsigned j = i; paren || (!right_paren(exp[j]) && exp[j] != ','); j++) {

				if (is_paren(exp[j]))
					left_paren(exp[j]) ? paren++ : paren--;

				else if (!paren && exp[j] == '+') {
					start_point.push(Integer(i));
					break;
				}
			}
		}
	}

	while (!start_point.isEmpty()) {

		start = start_point.pop().data;
		term_size = 0;
		i = start;

		for (unsigned j = i; j < exp.length() && !right_paren(exp[j]) && exp[j] != ','; j++) {

			if (left_paren(exp[j])) {

				paren = 0;

				do {

					if (is_paren(exp[j]))
						left_paren(exp[j]) ? paren++ : paren--;

					j++;

				} while (paren);

				j--;
			}

			//add the term into the list
			else if (is_alpha(exp[j]) || is_num(exp[j])) {

				temp.clear();
				paren = 0;

				do {

					temp.push_back(exp[j]);
					j++;

					if (is_paren(exp[j]))
						left_paren(exp[j]) ? paren++ : paren--;

				} while (j < exp.length() && paren != -1 && (paren || (exp[j] != '+')));
				j--;

				term_size++;

				//adding new variable to the list in alphabetical order
				p = new Lnode<string>(temp);
				q = list;
				r = NULL;

				while (q != NULL && q->data.compare(temp) < 0) {
					r = q;
					q = q->next;
				}

				if (q == NULL) {

					if (list == NULL)
						list = p;

					else r->next = p;
				}

				else if (r == NULL) {
					list = p;
					list->next = q;
				}

				else {
					r->next = p;
					p->next = q;
				}
			}
		}

		//put the contents of the list into a static array
		//and set up var_arr
		term_arr = new string[term_size];
		var_size = 0;

		for (int j = 0; j < term_size; j++) {

			p = list->next;
			term_arr[j] = list->data;
			delete list;
			list = p;

		}

		//get a list of each individual term
		for (int j = 0; j < term_size; j++) {

			for (unsigned k = 0; k < term_arr[j].length(); k++) {

				if (is_alpha(term_arr[j][k]) || is_num(term_arr[j][k])) {

					temp.clear();
					paren = 0;

					do {

						temp.push_back(term_arr[j][k]);
						k++;

						if (is_paren(term_arr[j][k]))
							left_paren(term_arr[j][k]) ? paren++ : paren--;

					} while (k < term_arr[j].length() && paren != -1 && (paren || term_arr[j][k] != '*'));
					k--;

					var_size++;

					p = new Lnode<string>(temp);
					q = list;
					r = NULL;

					while (q != NULL && q->data.compare(temp) < 0) {
						r = q;
						q = q->next;
					}

					if (q != NULL && !q->data.compare(temp)) {
						delete p;
						var_size--;
					}

					else if (q == NULL) {

						if (list == NULL)
							list = p;

						else r->next = p;
					}

					else if (r == NULL) {
						list = p;
						list->next = q;
					}

					else {
						r->next = p;
						p->next = q;
					}
				}
			}
		}

		//store the unique terms and initialize their counts to 0
		var_arr = new string[var_size];
		var_count = new int[var_size];

		for (int j = 0; j < var_size; j++) {

			p = list->next;
			var_arr[j] = list->data;
			var_count[j] = 0;

			delete list;
			list = p;

		}

		for (int j = 0; j < term_size; j++) {

			for (unsigned k = 0; k < term_arr[j].length(); k++) {

				if (is_alpha(term_arr[j][k]) || is_num(term_arr[j][k])) {

					temp.clear();
					paren = 0;

					do {

						temp.push_back(term_arr[j][k]);
						k++;

						if (is_paren(term_arr[j][k]))
							left_paren(term_arr[j][k]) ? paren++ : paren--;

					} while (k < term_arr[j].length() && paren != -1 && (paren || term_arr[j][k] != '*'));
					k--;

					temp2 = 0;
					while (temp != var_arr[temp2])
						temp2++;

					var_count[temp2]++;
				}
			}
		}

		//consider a*b+a*c+d*e+d*f

		cout << "Expression terms: ";
		for (int j = 0; j < term_size; j++)
			cout << term_arr[j] << ' ';

		cout << endl;

		cout << "Unique terms: ";
		for (int j = 0; j < var_size; j++)
			cout << var_arr[j] << ": " << var_count[j] << "  ";

		cout << endl;

		delete [] term_arr;
	}

}*/
