#include "calc.h"

int eval_postfix(string input) {

	return 1;

	/*Stack<Number> n;
	int num_1, num_2;

	for (unsigned i = 0; i < input.length(); i++) {

		if (input.at(i) == ' ')
			continue;

		if (is_num(input.at(i))) {

			num_1 = 0;

			while (is_num(input.at(i))) {

				num_1 *= 10;
				num_1 += input.at(i) & 0xf;
				i++;

				if (i == input.length())
					break;
			}

			n.push(Number(num_1));
		}

		else {
			num_2 = n.pop().number;
			num_1 = n.pop().number;

			switch (input.at(i)) {
				case '+': n.push(Number(num_1 + num_2)); break;
				case '-': n.push(Number(num_1 - num_2)); break;
				case '*': n.push(Number(num_1 * num_2)); break;
				case '/': n.push(Number(num_1 / num_2)); break;
				case '^': n.push(Number(int_pow(num_1, num_2))); break;
				default : n.push(Number(0)); //error
			}
		}
	}

	return n.pop().number;*/
}