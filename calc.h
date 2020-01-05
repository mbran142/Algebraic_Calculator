#ifndef _CALC_H
#define _CALC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include "stack.h"

namespace io
{
  using std::cout;
  using std::cin;
  using std::cerr;
  using std::fixed;
  using std::endl;
  using std::string;
  using std::ostream;
  using std::vector;
  using std::to_string;
};

using namespace io;

const string funclist[] = { "factorial", "sqrt", "log", "ln", "derivative", "integral" };
const int LIST_SIZE = sizeof(funclist) / sizeof(string);

//general
inline bool is_num(char);
inline bool is_alpha(char);
inline int precedence(char);
inline bool is_operator(char);
inline long int_pow(int, int);
inline int get_digit(int, int);
inline int get_digit(long, int);

class Integer {
public:
	Integer() : data(0) { };
	Integer(int i) : data(i) { };
	std::string to_str() const {

		int size = 1;
		int temp = data;

		for (int i = 1; data >= int_pow(10, i); i++)
			size++;

		std::string out = std::string(size, '0');

		for (int i = size - 1; i >= 0; i--) {
			out[i] = static_cast<char>((temp % 10) + '0');
			temp /= 10;
		}

		return out;
	}
	int data;
};

template <class T>
struct Lnode {
	T data;
	Lnode* next;
	Lnode(const T& new_data) : data(new_data), next(NULL) {};
	Lnode(const T& new_data, Lnode* next_node) : data(new_data), next(next_node) {};
};

//main.cpp
void get_msg(string&, char);
string generateRandomExpression();

//postfix.cpp
void parse_expression(string, string); //takes raw input, checks for validity and makes minor changes
void to_postfix(string, string); //converts expression to postfix
inline int precedence(char); //gets precedence value of input operator
inline int is_paren(char);
inline int left_paren(char);
inline int right_paren(char);

//eval.cpp
int eval_postfix(string input);

//varex.cpp
int in_funclist(const string&);
inline int is_paren(char c);
inline int left_paren(char c);
inline int right_paren(char c);
int in_funclist();

//general implementations
inline bool is_num(char c) {
	return !(c < '0' || c > '9');
}

inline bool is_alpha(char c) {
	return !((c < 'a' || c > 'z') && (c < 'A' || c > 'Z'));
}

inline bool is_operator(char c) {

	switch (c) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '^': return true;
		default : return false;
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
		default : return 5;
	}
}

inline int precedence(const string& s) {

	//functions have highest precedence
	if (s.length() > 1 || !is_operator(s[0]))
		return 4;

	switch (s[0]) {
		case '+': return 1;
		case '-': return 1;
		case '*': return 2;
		case '/': return 2;
		case '^': return 3;
		case '(': case ')':
		case '[': case ']':
		case '{': case '}': return 4;
		default : return 5;
	}
}

inline long int_pow(int n, int p) {

	long prod = 1;

	for (int i = 0; i < p; i++)
		prod *= n;

	return prod;
}

inline int get_digit(int num, int digit) {

	if (digit < 1) {
		cout << "get_digit() error. Program will now exit." << endl;
		exit(1);
	}

	num /= int_pow(10, digit - 1);
	return num % 10;
}

inline int get_digit(long num, int digit) {

	if (digit < 1) {
		cout << "get_digit() error. Program will now exit." << endl;
		exit(1);
	}

	num /= int_pow(10, digit - 1);
	return num % 10;
}

inline int is_paren(char c) {
	return left_paren(c) + right_paren(c);
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

#endif