#ifndef _VAREX_H
#define _VAREX_H

#include "calc.h"

const int MAX_PRECISION = 18;
const int REASONABLE_PRECISION = 12;
const long MAX_MULT = 100000000000000000L;
const long MAX_DIV = 1000000000000000000L;
const int DECIMAL_EXPONENT_ACCURACY = 100;
const int MAX_INT = 0x7fffffff;

const int OPERATOR = 1;
const int NUMBER = 2;
const int VARIABLE = 3;

using std::string;
using std::vector;
using std::ostream;

class Holder {
public:
	virtual int instanceof() const = 0;
	virtual bool isOperator() const = 0;
	virtual bool isFunction() const = 0;
	virtual bool isOperand() const = 0;
	virtual bool isParen() const = 0;
	virtual bool rightParen() const = 0;
	virtual bool leftParen() const = 0;
	virtual bool isComma() const  = 0;
	virtual string toStr() const = 0;
	virtual string toDetailedStr() const = 0;
	virtual Holder* clone() const = 0;
	virtual ~Holder() { };
protected:
	string name;
};

class Operator : public Holder {
public:
	Operator(char);
	Operator(const string&, int);
	Operator(const Operator&);
	virtual int instanceof() const;
	virtual bool isOperator() const { return !(this->isParen() || this->isComma()); }
	virtual bool isFunction() const { return (name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z'); }
	virtual bool isOperand() const { return false; }
	virtual bool isParen() const { return name == "(" || name == ")"; }
	virtual bool rightParen() const { return name == ")"; }
	virtual bool leftParen() const { return name == "("; }
	virtual bool isComma() const { return name == ","; }
	virtual string toStr() const;
	virtual string toDetailedStr() const;
	virtual Holder* clone() const { return new Operator(*this); }
	int getArgs() const { return args; }
protected:
	int args;
};

class Variable : public Holder {
public:
	Variable(char);
	Variable(const string&);
	virtual int instanceof() const;
	virtual bool isOperator() const { return false; }
	virtual bool isFunction() const { return false; }
	virtual bool isOperand() const { return true; }
	virtual bool isParen() const { return false; }
	virtual bool rightParen() const { return false; }
	virtual bool leftParen() const { return false; }
	virtual bool isComma() const { return false; }
	virtual string toStr() const;
	virtual string toDetailedStr() const;
	virtual Holder* clone() const { return new Variable(*this); }
	Variable& operator=(const Variable&);
};

class Number : public Holder {
public:
	Number();
	Number(int);
	Number(long);
	Number(long, int);
	//Number(double);
	Number(const std::string&);
	Number(const Number&);
	virtual int instanceof() const;
	virtual bool isOperator() const { return false; }
	virtual bool isFunction() const { return false; }
	virtual bool isOperand() const { return true; }
	virtual bool isParen() const { return false; }
	virtual bool rightParen() const { return false; }
	virtual bool leftParen() const { return false; }
	virtual bool isComma() const { return false; }
	virtual std::string toStr() const;
	virtual std::string toDetailedStr() const;
	virtual Holder* clone() const { return new Number(*this); }
	long getValue() const;
	int getPower() const;
	Number& operator=(const Number&);
	Number operator+(const Number&) const;
	Number operator*(const Number&) const;
	Number operator-(const Number&) const;
	Number operator/(const Number&) const;
	Number operator^(const Number&) const;
	Number operator^(int);
	friend bool operator==(const Number&, const Number&);
	friend bool operator!=(const Number&, const Number&);
	friend bool operator<(const Number&, const Number&);
	friend bool operator>(const Number&, const Number&);
	friend bool operator<=(const Number&, const Number&);
	friend bool operator>=(const Number&, const Number&);
	friend std::ostream& operator<<(std::ostream&, const Number&);
	void factorial(int);
	int num_digits() const;
	bool is_inaccurate() const;
	static void scientific_display(bool b) { scientific = b; };
protected:
	long value;
	int power;
	bool inaccuracy_flag;
	bool undefined;
	bool infinity;
	static bool scientific;
};

class Expression {
public:
	Expression();
	Expression(const string&);
	Expression(const Expression&);
	~Expression();
	static int processExpression(string&, const string&);
	string evaluate();
	string toStr();
	string getPostfixString();
	Expression& operator=(const Expression&);
	friend ostream& operator<<(ostream&, const Expression&);
	void simplify();
private:
	vector<Holder*>* postfix;
	vector<Holder*>* infix;
	void createPostfix(const string&, int = 0);
	void updateInfix(int = 0);
	void freePostfix();
	void freeInfix();

	//simplifying functions
	void divToMult();
	void subToAdd();
	bool simplifyExponents();
	bool simplifyAddition();
	bool processArithmetic();
	void revertNegatives();
};

string vprint(vector<Holder*>*);

#endif

//Old expression stuff
/*class Expression {
public:
	Expression(const string&);
	~Expression();
	void updatePostfix(const string&, int);
	void updateInfix(const string&, int);
	void simplify();
	friend ostream& operator<<(ostream&, const Expression&);
private:
	vector<Holder*> *postfix;
	vector<Holder*> *infix;
	void div_to_mult();
	void sub_to_add();
	void simplify_exponents();
	void simplify_addition();
	void process_arithmetic();
	void revert_negatives();
	static int process_expression(string&, const string&);
	void clearPostfix();
	void clearInfix();
};*/