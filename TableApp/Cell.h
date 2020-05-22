#pragma once
#include <string>
using namespace std;

class Cell
{
private:
	string value;
	string result;
	bool calculated = false;

public:
	void set_value(string );
	string get_value();
	void set_result(string );
	void set_result(double );
	string get_result();
	void clear_calculated();
	bool is_calculated();
};

