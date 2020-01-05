void Expression::updateInfix(const string&, int size) {

	//TODO: make this store in a class instance vectory<Holder*> variable (infix)
	Stack<string> s;
	Stack<string> t;
	string buf, temp_s;
	unsigned temp;
	char low_op;
	int paren;

	if (infix != NULL)
		this->clearPostfix();
	else infix = new vector<Holder*>(size);

	for (typename vector<Holder*>::iterator itr = postfix->begin(); itr != postfix->end(); ++itr) {

		buf = (*itr)->toStr();

		//operand
		if ((*itr)->is_operand())
			s.push(buf);

		//function
		else if (!isOperator(buf[0])) {

			temp = 0;
			unsigned j = 0;
			temp_s.clear();

			while (buf[j] != '_') {
				temp_s.push_back(buf[j]);
				j++;
			}
			
			do {
				j++;
				temp *= 10;
				temp += buf[j] & 0xf;
			} while (j < buf.length() - 1);

			for (j = 0; j < temp; j++)
				t.push(s.pop());

			temp_s.push_back('(');

			while (!t.isEmpty())
				temp_s += t.pop() + ',';

			temp_s[temp_s.length() - 1] = ')';
			s.push(temp_s);
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
				else if (!paren && isOperator(temp_s[temp]) && precedence(temp_s[temp]) < precedence(low_op))
					low_op = temp_s[temp];

				temp++;
			}

			if (precedence(low_op) < precedence(buf[0]) || ((buf[0] == '/' || buf[0] == '-' || buf[0] == '^') && precedence(low_op) <= precedence(buf[0])))
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
				else if (!paren && isOperator(s.peek()[temp]) && precedence(s.peek()[temp]) < precedence(low_op))
					low_op = s.peek()[temp];

				temp++;
			}

			if (precedence(low_op) < precedence(buf[0]))
				s.peek() = '(' + s.peek() + ')';

			buf += temp_s;
			temp_s = s.pop();
			temp_s += buf;

			s.push(temp_s);
		}
	}

	buf = s.pop();
		
}