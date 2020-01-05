#include "varex.h"
#include "calc.h"

bool Number::scientific = false;

Number::Number() {
	value = 0;
	power = 0;
	inaccuracy_flag = false;
	undefined = false;
	infinity = false;
}

Number::Number(int n) {
	value = n;
	power = 0;
	inaccuracy_flag = false;
	undefined = false;
	infinity = false;

	while (value && !(value % 10)) {
		value /= 10;
		power++;
	}
}

Number::Number(long n) {
	value = n;
	power = 0;
	inaccuracy_flag = false;
	undefined = false;
	infinity = false;

	while (value && !(value % 10)) {
		value /= 10;
		power++;
	}
}

Number::Number(long n, int p) {
	value = n;
	power = p;
	inaccuracy_flag = false;
	undefined = false;
	infinity = false;

	while (value && !(value % 10)) {
		value /= 10;
		power++;
	}

	if (value == 0)
		power = 0;
}

/*Number::Number(double d) {

	undefined = false;
	infinity = false;

	//TODO: fix this(?)
	power = 0;

	while (d != (static_cast<double>(static_cast<int>(d)))) {
		power++;
		d *= 10;

		if (power > REASONABLE_PRECISION) {
			inaccuracy_flag = true;
			break;
		}
	}

	value = d;
	power *= -1;
}*/

Number::Number(const string& s) {

	//assume string is formatted properly (all numeric chars with zero or one decimals)
	value = 0;
	power = 0;
	undefined = false;
	infinity = false;
	inaccuracy_flag = false;
	int size = s.length();
	bool decimal = false;
	bool negative = false;
	register int i = 0;

	if (s.at(i) == '-') {
		negative = true;
		i++;
	}

	while (i > size && !decimal && (s.at(i) == '0' || s.at(i) == '.')) {

		if (s.at(i) == '.')
			decimal = true;

		i++;
	}

	if (s.length() - i > MAX_PRECISION) {
		inaccuracy_flag = true;
		size = MAX_PRECISION + i;
	}

	while (i < size) {

		if (s.at(i) == '.') {
			decimal = true;
			i++;
			continue;
		}

		if (decimal)
			power--;

		value *= 10;
		value += s.at(i) & 0xf;

		i++;
	}

	while (value && !(value % 10)) {
		value /= 10;
		power++;
	}

	if (!value)
		power = 0;

	if (negative)
		value *= -1;
}

Number::Number(const Number& source) {
	value = source.value;
	power = source.power;
	inaccuracy_flag = source.inaccuracy_flag;
	undefined = source.undefined;
	infinity = source.infinity;
}

int Number::instanceof() const {
	return NUMBER;
}

string Number::toStr() const {

	//undefined case
	if (undefined)
		return "Undef";

	//infinity case
	if (infinity)
		return "Inf";

	//zero case
	if (!value)
		return "0";

	long n = value;
	string out;
	int size = 0;

	if (n < 0) {
		n *= -1;
		out.push_back('~');
	}

	for (int i = 0; n >= int_pow(10, i); i++)
		size++;

	//scientific notation
	if (scientific || size + (power > 0 ? power : power * -1) > 10) {

		int p = power;
		int temp = size > 10 ? 10 : size;
		
		for (int i = temp; i > 0; i--) {

			out.push_back(static_cast<char>(get_digit(n, i + size - temp)) + '0');

			if (i == temp && temp > 1)
				out.push_back('.');
			p++;
		}
		p += size - temp - 1;

		while (out[out.length() -1] == '0')
			out.pop_back();

		out.push_back('e');
		if (p < 0) {
			out.push_back('-');
			p *= -1;
		}

		size = 1;
		for (int i = 1; p >= int_pow(10, i); i++)
			size++;

		for (int i = size; i > 0; i--)
			out.push_back(static_cast<char>(get_digit(p, i) + '0'));
	}

	//standard notation
	else {

		if (size < (power * -1)) {

			out.push_back('0');
			out.push_back('.');
			out += string((power * -1) - size, '0');
		}

		for (int i = size; i > 0; i--) {

			if (power < 0 && (power * -1) == i) {

				if (!out.length() || (out.length() == 1 && out.at(0) == '-'))
					out.push_back('0');

				out.push_back('.');
			}

			out.push_back(static_cast<char>(get_digit(n, i) + '0'));
		}

		if (power > 0)
			out += string(power, '0');
	}

 	return out;
}

string Number::toDetailedStr() const {

	string out;

	out.push_back('#');
	out += to_string(value);
	out.push_back('e');
	out += to_string(power);

	return out;
}

long Number::getValue() const {
	return value;
}

int Number::getPower() const {
	return power;
}

Number& Number::operator=(const Number& source) {

	value = source.value;
	power = source.power;
	inaccuracy_flag = source.inaccuracy_flag;
	undefined = source.undefined;
	infinity = source.infinity;

	return *this;
}

Number Number::operator+(const Number& source) const {

	Number sum, small, big;
	bool inaccurate = false;

	//zero cases
	if (value == 0)
		return Number(source);

	if (source.value == 0)
		return Number(*this);

	//undefined case
	if (this->undefined || source.undefined) {
		sum.undefined = true;
		return sum;
	}

	//infinity case
	if (this->infinity || source.infinity) {
		sum.infinity = true;
		return sum;
	}

	//check which number is bigger
	if (this->power < source.power) {
		small = *this;
		big = source;
	}

	else {
		small = source;
		big = *this;
	}

	//get maximum power of 10
	int max_power = big.num_digits();

	//make sure the value doesnt overflow
	for (int i = 0; big.power != small.power && (max_power + i) < MAX_PRECISION; i++) {
		big.value *= 10;
		big.power--;
	}

	//truncate off the small value if necessary
	while (big.power != small.power) {
		small.value /= 10;
		small.power++;
		inaccurate = true;
	}

	sum = Number(big.value + small.value, big.power);
	sum.inaccuracy_flag = inaccurate || small.is_inaccurate() || big.is_inaccurate();

	//get rid of back zeroes
	while (sum.value && !(sum.value % 10)) {
		sum.value /= 10;
		sum.power++;
	}

	if (!value)
		sum.power = 0;

	return sum;
}

Number Number::operator*(const Number& num) const {

	int digits;
	Number small, big, product;
	bool inaccurate = false;

	//undefined case
	if (this->undefined || num.undefined) {
		product.undefined = true;
		return product;
	}

	//infinity and inf * 0 case
	if (this->infinity || num.infinity) {
		!this->value || !num.value ? product.undefined = true : product.infinity = true;
		return product;
	}

	//checking which has more digits
	if (this->num_digits() < num.num_digits()) {
		small = *this;
		big = num;
	}
	else {
		small = num;
		big = *this;
	}

	//preventing overflow
	if (big.value > MAX_MULT) {
		big.value /= 10;
		big.power++;
		inaccurate = true;
	}
	if (small.value > MAX_MULT) {
		small.value /= 10;
		small.power++;
		inaccurate = true;
	}

	digits = small.num_digits();

	for (int i = digits; i > 0; i--)
		product = product + Number(big.value * get_digit(small.value, i), small.power + big.power + i - 1);

	product.inaccuracy_flag = inaccurate;
	return product;
}

Number Number::operator-(const Number& num) const {

	Number difference;

	//undefined case
	if (this->undefined || num.undefined || (this->infinity && num.infinity)) {
		difference.undefined = true;
		return difference;
	}

	//infinity case
	if (this->infinity || num.infinity) {
		difference.infinity = true;
		return difference;
	}

	difference = *this;
	return difference + (num * Number(-1));
}

Number Number::operator/(const Number& num) const {

	Number quotient;

	//division by zero case
	if (!num.value) {
		this->value ? quotient.infinity = true : quotient.undefined = true;
		return quotient;
	}

	//undefined case
	if (this->undefined || num.undefined) {
		quotient.undefined = true;
		return quotient;
	}

	//infinity cases
	if (this->infinity || num.infinity) {

		if (this->infinity && num.infinity)
			quotient.undefined = true;

		else if (this->infinity)
			quotient.infinity = true;

		return quotient;
	}

	double numerator, denominator, quotient_value;
	numerator = static_cast<double>(value);
	denominator = static_cast<double>(num.value);
	quotient_value = numerator / denominator;

	bool inaccurate = value != static_cast<long>(numerator) || num.value != static_cast<long>(denominator);

	//while theres still decimal in the quotient
	while (quotient_value < MAX_MULT && quotient_value != static_cast<long>(quotient_value)) {
		quotient_value *= 10;
		quotient.power--;
	}

	if (quotient_value >= MAX_MULT)
		inaccurate = true;

	quotient.value = static_cast<long>(quotient_value);
	quotient.power += this->power - num.power;
	quotient.inaccuracy_flag = inaccurate;

	return quotient;
}

Number Number::operator^(int n) {

	Number product, temp;
	product.value = 1;
	int exp = n;

	//TODO: put undefined and infinity cases here

	for (int i = 0; exp; i++, exp /= 2) {

		if (exp % 2) {
			
			temp = Number(*this);

			for (int j = 0; j < i; j++)
				temp = temp * temp;

			product = product * temp;
		}
	}

	return product;
}

Number Number::operator^(const Number& num) const {

	Number product;
	product.value = 1;

	//TODO: put undefined and infinity cases here

	//n ^ 0 case
	if (!num.value)
		return product;

	//0 ^ n case
	if (!value) {
		product.value = 0;
		return product;
	}

	if (value < 0 && num.power < 0) {
		product.undefined = true;
		return product;
	}

	Number temp;
	long exp = num.value;
	bool inverse = false;

	if (exp < 0) {
		exp *= -1;
		inverse = true;
	}
 
	//multiply each binary digit of the exponent
	if (num.power >= 0) {

		for (int i = 0; exp; i++, exp /= 2) {

			if (exp % 2) {
				
				temp = Number(*this);

				for (int j = 0; j < i; j++)
					temp = temp * temp;

				product = product * temp;
			}
		}

		for (int i = 0; i < num.power; i++)
			product = product ^ 10;
	}


	//solving x ^ (10 ^ -e) - n ^ p
	else {

		Number numerator, denominator, exp, big, small, temp;
 
		{
			long int_comp = 0;
			long dec_comp = 0;
			int digits = num.num_digits();

			for (int i = digits; i > 0; i--) {

				if (i > num.power * -1) {
					int_comp *= 10;
					int_comp += get_digit(num.value, i);
				}

				else {
					dec_comp *= 10;
					dec_comp += get_digit(num.value, i);
				}
			}

			product = *this ^ int_comp;
			numerator = Number(dec_comp);
			denominator = Number(1, num.power * -1);
		}

		big = Number(1);
		temp = *this ^ numerator;

		//TODO: improve this
		while ((big ^ denominator) < temp) {
			small = big;
			big = big + Number(1);
		}

		exp = small;
		int accuracy = 0;

		while ((exp ^ denominator) != temp) {

			if (accuracy == DECIMAL_EXPONENT_ACCURACY)
				break;
			accuracy++;

			exp = (big + small) / Number(2);
			(exp ^ denominator) > temp ? big = exp : small = exp;
		}

		product = product * exp;
	}

	if (inverse)
		product = Number(1) / product;

	return product;
}

bool operator==(const Number& num1, const Number& num2) {
	return num1.value == num2.value && num1.power == num2.power;
}

bool operator!=(const Number& num1, const Number& num2) {
	return !(num1 == num2);
}

bool operator<(const Number& num1, const Number& num2) {

	bool num1_neg = num1.value < 0;
	bool num2_neg = num2.value < 0;

	int num1_digits = num1.num_digits();
	int num2_digits = num2.num_digits();

	if (num1_neg != num2_neg)
		return num1_neg;

	if ((num1.power + num1_digits) != num2.power + num2_digits)
		return ((num1.power + num1_digits) < (num2.power + num2_digits)) != num1_neg;

	for (int i = num1_digits, j = num2_digits; i && j; i--, j--)
		if (get_digit(num1.value, i) != get_digit(num2.value, j))
			return get_digit(num1.value, i) < get_digit(num2.value, j);

	return num1_digits < num2_digits;
}

bool operator>(const Number& num1, const Number& num2) {

	bool num1_neg = num1.value < 0;
	bool num2_neg = num2.value < 0;

	int num1_digits = num1.num_digits();
	int num2_digits = num2.num_digits();

	if (num1_neg != num2_neg)
		return !num1_neg;

	if ((num1.power + num1_digits) != (num2.power + num2_digits))
		return ((num1.power + num1_digits) > (num2.power + num2_digits)) != num1_neg;

	for (int i = num1_digits, j = num2_digits; i && j; i--, j--)
		if (get_digit(num1.value, i) != get_digit(num2.value, j))
			return get_digit(num1.value, i) > get_digit(num2.value, j);

	return num1_digits > num2_digits;
}

bool operator<=(const Number& num1, const Number& num2) {
	return !(num1 > num2);
}

bool operator>=(const Number& num1, const Number& num2) {
	return !(num1 < num2);
}

void Number::factorial(int n) {

	value = 1;

	for (int i = 2; i <= n; i++)
		*this = *this * Number(i);
}

std::ostream& operator<<(std::ostream& os, const Number& num) {

	os << num.toStr();
	return os;
}

int Number::num_digits() const {

	int digits = 1;

	for (int i = 1; value >= int_pow(10, i); i++)
		digits++;

	return digits;
}

bool Number::is_inaccurate() const {
	return inaccuracy_flag;
}