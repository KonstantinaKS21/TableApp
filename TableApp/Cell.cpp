#include "Cell.h"


void Cell::set_value(string input_value)
{
	value = input_value;
	calculated = false;
}

string Cell::get_value()
{
	return value;
}

void Cell::set_result(string str_result) 
{
	calculated = true;
	result = str_result;
}

void Cell::set_result(double double_result) 
{
	calculated = true;
	result = to_string(double_result);
}

string Cell::get_result()
{
	return result;
}

void Cell::clear_calculated()
{
	calculated = false;
	result.clear();
}

bool Cell::is_calculated()
{
	return calculated;
}
