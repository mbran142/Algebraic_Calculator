#include "varex.h"
#include "definition.h"

using namespace io;

DefinitionList defList;

/*
MAIN TODO LIST
	- figure out a good way to organize the DefinitionList definitions (use holders <-> expressions?)
	- implement the DefinitionList class (mostly validateInput and addDefinition)
	- figure out a way to nicely query the DefinitionList
*/

int main() {

	
	// string divToMult[] = { "a/b", "a/(b+c)", "a/b+c", "a/b^c", "a/(b+c)^(d+e)", "a/b^c^d^e", "a/b^c^(d^e)",
	// 					   "a/f(a)", "a/f(f(1,1),1)", "a/f(a/f(a))", "a/f(b/f(a/b^c^d^e,a/f(y/z)))^f(1,2,3)^x" };
	// const int DIVTOMULT = sizeof divToMult / sizeof divToMult[0];
	// string subToAdd[] = { "a-b", "a-b+c", "a-(b+c)", "a-b*c", "a-b^c", "a-f(a)", "a-(b-(c-(d-e)))",
	// 					"a-f(b-f(c-f(d-e)))", "a-f(b-c)-f(d-e)" };
	// const int SUBTOADD = sizeof subToAdd / sizeof subToAdd[0];
	// int index = 0;

	string s;

	do {

		cout << "Enter expression: ";
		get_msg(s, '\0');
		// cout << subToAdd[index] << endl;
		// s = subToAdd[index];

		if (s != "q") {

			try {

				if (s.find(":=") != string::npos)
					defList.tryDefining(s);

				else {
					Expression e(s);
					cout << "   Postfix expression: " << e.getPostfixString() << endl;
					cout << "     Infix epxression: " << e << endl;
					e.simplify();
					cout << "Simplified expression: " << e << endl;
				}

			} catch (const char* msg) {
				cerr << msg << endl;
			}

			cout << endl;
		}

	} while (s != "q");
	// } while (++index < SUBTOADD);
 	
 	
 	/*
	bool done, again;
	srand(time(NULL));
	string s;
	Expression e1, e2;

	for (int i = 0; i < 4; ++i) {
		s += static_cast<char>('a' + i);
		s += " := ";
		s += static_cast<char>('1' + i);
		defList.tryDefining(s);
		s.clear();
	}

	do {

		//get a valid random expression
		again = true;

		while (again) {

			again = false;
			try {

				s = generateRandomExpression();
				e2 = e1 = Expression(s);
				e2.simplify();

				if (e1.evaluate() != e2.evaluate())
					cout << "  Original expression: " << s << endl
						 << "    Parsed expression: " << e1 << " = " << e1.evaluate() << endl
						 << "Simplified expression: " << e2 << " = " << e2.evaluate() << endl << endl;

				else again = true;

			} catch (const char* msg) {
				again = true;
			}
		}

		cout << "Another? [y/n]: ";
		get_msg(s, '\0');
		done = (s[0] | ' ') != 'y';

	} while (!done);
	*/

	return 0;
}

string generateRandomExpression() {

	string output;
	bool num = false;
	int randNum;

	for (int i = 0; i < 25; i++) {

		if ((rand() % (i + 1)) > 15)
			return output;

		if (num) {

			if (!(rand() % 5))
				output.push_back('~');

			randNum = rand() % 12;
			output.push_back(randNum < 9 ? '1' + randNum : 'a' + randNum - 9);
		}
		else {
			randNum = rand() % 9;
			switch (randNum) {
				case 0: output.push_back('+'); break;
				case 1: output.push_back('-'); break;
				case 2: output.push_back('*'); break;
				case 3: output.push_back('/'); break;
				case 4: output.push_back('^'); break;
				case 5: case 6: output.push_back('('); break; 
				case 7: case 8: output.push_back(')'); break; 
				case 9: num = !num;
			}
		}

		num = !num;
	}

	return output;
}

void get_msg(string& input, char delim) {

	char temp;
	input.clear();

	input.push_back(getc(stdin));
	temp = getc(stdin);

	while ((temp != '\n') | (delim && input.at(input.size()) != delim)) {
		input.push_back(temp);
		temp = getc(stdin);
	}
}