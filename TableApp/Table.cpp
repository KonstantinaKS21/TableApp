#include <stddef.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "Table.h"

/* Number of rows/columns to add on vector resize, avoid constant memory reallocation and copy*/
#define ADD_CAPACITY 1

string::iterator expression_to_parse;
string eval_string;
bool eval_error = false;


int Table::open_file(string fName)
{
	int err = 0;
	
	input_stream.open(fName, fstream::in);
	if (input_stream.is_open())
	{
		load_file();
		input_stream.close();
		file_name = fName;
		return 0;
	}

	// Maybe file does not exist? create it
	input_stream.open(fName, fstream::out);
	if (input_stream.is_open()) 
	{
		cout << "File " << fName << " created\n";
		err = 0;
		file_name = fName;
	}
	else
	{
		#pragma warning(suppress:4996)
		cout << strerror(errno) << endl;
		err = errno;
		file_name.clear();
	}
	
	return err;
}

/*
	Clear the table and disable further operation
*/
int Table::close_file()
{
	if (!is_operation_allowed()) return -1;
	clear();
	file_name.clear();
	cout << "Table data has been cleared from the memory.\n";
	return 0;
}

string escape_string(string s)
{
	string ret;
	for (string::iterator it = s.begin(); it != s.end(); it++) 
	{
		if (*it == ',') ret += '\\';
		ret += *it;
	}
	return ret;
}

/*
	Save table in current file
*/
int Table::save_file()
{
	if (!is_operation_allowed()) return -1;
	input_stream.open(file_name, fstream::out|fstream::trunc);
	for (int row = 1; row < table_cells.size(); row++)
	{
		if (table_cells[row].empty())
		{
			input_stream << "\n";
			continue;
		}
		size_t rows = table_cells[row].size();
		for (int column = 1; column < table_cells[row].size() - 1; column++)
		{
			input_stream << escape_string(table_cells[row][column].get_value()) << ",";
		}

		input_stream << escape_string(table_cells[row][table_cells[row].size()-1].get_value());

		if (row != table_cells.size()-1) input_stream << endl;
	}

	//input_stream.close();
	return 0;
}

/*
	Save table in different file
*/
int Table::save_as_file(string fName)
{
	if (!is_operation_allowed()) return -1;
	file_name = fName;
	return save_file();
}

int Table::set_cell(int row, int column, string value) 
{
	if (row < 0 || row >= INT_MAX - ADD_CAPACITY) 
	{
		cout << "Row index is outside the range\n";
		return -2;
	}
	if (column < 0 || column >= INT_MAX - ADD_CAPACITY)
	{
		cout << "Column index is outside the range\n";
		return -2;
	}
	if (table_cells.size() <= row) table_cells.resize((size_t)row + ADD_CAPACITY);
	if (table_cells[row].size() <= column) table_cells[row].resize((size_t)column + ADD_CAPACITY);
	table_cells[row][column].set_value(value);
	return 0;
}


/*
	Chnage cell value
*/
int Table::edit_cell(string str_row, string str_column, string value)
{
	if (!is_operation_allowed()) return -1;
	int int_row;
	try 
	{
		int_row = stoi(str_row, nullptr, 0);
	}
	catch (exception e)
	{
		cout << "<row> " << str_row << " is not a valid number\n";
		return -1;
	}
	int int_column;
	try
	{
		int_column = stoi(str_column, nullptr, 0);
	}
	catch (exception e)
	{
		cout << "<col> " << str_column << " is not a valid number\n";
		return -1;
	}
	set_cell(int_row, int_column, value);
	calculate();
	return 0;
}

void print_spacer(vector<size_t> cw) 
{
	for (size_t i = 1; i < cw.size(); i++)
	{
		cout << "+";
		for (size_t j = 0; j < cw[i]; j++) cout << "-";
	}
	cout << "+\n";
}

/*
	Display table
*/
void Table::print_table()
{
	//if (!is_operation_allowed()) return -1;
	//if (table_cells.empty()) return 0;
	vector<size_t> cw;
	for (int r = 0; r < table_cells.size(); r++)
	{
		if (table_cells[r].size() > cw.size())
		{
			cw.resize(table_cells[r].size(), 0);
		}
	}
	//for (int i = 0; i < tableCells[0].size(); i++) cw[i] = 0;
	for (int r = 1; r < table_cells.size(); r++)
	{
		for (int c = 1; c < table_cells[r].size(); c++)
		{
			if (cw[c] < table_cells[r][c].get_result().size())
			{
				cw[c] = table_cells[r][c].get_result().size();
			}
		}
	}

	print_spacer(cw);
	for (int r = 1; r < table_cells.size(); r++)
	{
		std::cout << "|";
		for (int c = 1; c < cw.size(); c++) 
		{
			if (table_cells[r].size()<=c)	
				cout << setw(cw[c]+1) << setfill(' ') << "|";
			else
				cout << setw(cw[c]) << table_cells[r][c].get_result() << "|";
		}
		cout << "\n";
		print_spacer(cw);
	}

	//return 0;
}

string Table::get_cell_result(size_t row, size_t col) 
{
	if (table_cells.empty() || row >= table_cells.size()) return "";
	if (col >= table_cells[row].size()) return "";
	if (table_cells[row][col].is_calculated()) return table_cells[row][col].get_result();
	// calculate the cell
	double result = 0;
	string str = table_cells[row][col].get_value();
	if (str.empty()) {
		table_cells[row][col].set_result("");
		return "";
	};
	// skip all leading whitespaces
	string not_string = str;
	not_string.erase(0, not_string.find_first_not_of("\t\n\v\f\r "));
	if (not_string.empty()) 
	{
		table_cells[row][col].set_result(str);
		return str;
	}
	if (not_string[0] == '=')
	{
		not_string.erase(0,1);
		eval_string = "";
		for (string::iterator it = not_string.begin(); it != not_string.end(); it++)
			if (*it != ' ') eval_string += *it;
		expression_to_parse = eval_string.begin();
		result = evaluate_expression();
	}
	else 
	{
		// something else? try to convert to number
		try 
		{
			result = stof(not_string);
		}
		catch (exception& e) 
		{
			table_cells[row][col].set_result(str);
			return str;
		}
	}
	if (eval_error) str = "ERROR";
	else str = to_string(result);
	eval_error = false;
	table_cells[row][col].set_result(str);
	return str;
}

/*
	Private
*/

/*
	Did OPEN command execute successfully?
*/
bool Table::has_file()
{
	return !file_name.empty();
}

/*
	If OPEN didn't succeed, display a proper message
*/
bool Table::is_operation_allowed()
{
	if (!has_file()) 
	{
		cout << "Operation not allowed! No opened file.\n";
		return false;
	}
	return true;
}

/*
	Clear all cells of the table
*/
void Table::clear()
{
	table_cells.clear();
}

/*
	Calculate current table
*/
void Table::calculate()
{
	for (int row = 0; row < table_cells.size(); row++)
		for (int column = 0; column < table_cells[row].size(); column++)
			table_cells[row][column].clear_calculated();
	for (int row = 0; row < table_cells.size(); row++)
		for (int column = 0; column < table_cells[row].size(); column++)
			get_cell_result(row, column);
	//return 0;
}

int Table::load_file()
{
	string line;
	int row = 0;

	while (getline(input_stream, line))
	{
		row++;
		if (line.empty()) continue;
		int column = 1;
		string token;
		bool esc = false;
		for (string::iterator it = line.begin(); it != line.end(); it++)
		{
			if (*it == ',' && !esc) 
			{
				if (!token.empty()) set_cell(row, column, token);
				column++;
				token.clear();
				continue;
			}
			if (*it == '\\' && !esc)
			{
				esc = true;
				continue;
			}
			token += *it;
			esc = false;
		}
		if (!token.empty()) set_cell(row, column, token);
	}
	// Calculate the entire table
	calculate();
	return 0;
}


char Table::eval_peek() 
{
	if (expression_to_parse == eval_string.end()) return '\0';
	return *expression_to_parse;
}

char Table::eval_get()
{
	return *expression_to_parse++;
}

bool Table::is_digit()
{
	return eval_peek() >= '0' && eval_peek() <= '9';
}

int Table::eval_int_number() 
{
	int result = eval_get() - '0';
	while (is_digit()) 
	{
		result = 10 * result + eval_get() - '0';
	}
	return result;
}

double Table::eval_number()
{
	double result = (double)eval_int_number();
	if (eval_peek() == '.')
	{
		eval_get(); // '.'
		if (is_digit())
		{
			double result2 = (double)eval_int_number();
			while (result2 >= 1.0) result2 /= 10.0;
			result += result2;
		}
		else
		{
			// Detected '.' but no digits after it
			eval_error = true;
			result = 0;
		}
	}
	return result;
}

double Table::eval_factor()
{
	if (is_digit())
		return eval_number();
	else if (eval_peek() == 'R') 
	{
		eval_get(); // 'R' cell reference
		size_t row, column;
		if (is_digit())
			row = eval_int_number();
		else
		{
			eval_error = true;
			return 0;
		}
		if (eval_peek() != 'C')
		{
			eval_error = true;
			return 0;
		}
		eval_get();
		if (!is_digit()) 
		{
			eval_error = true;
			return 0;
		}
		column = eval_int_number();
		try
		{
			return stof(get_cell_result(row, column));
		}
		catch (std::exception& e)
		{
			return 0;
		}
	}
	else if (eval_peek() == '-')
	{
		eval_get();
		return -eval_factor();
	}
	eval_error = true;
	return 0; // error
}

double Table::eval_term()
{
	double result = eval_factor();
	while (eval_peek() == '*' || eval_peek() == '/')
		if (eval_get() == '*')
			result *= eval_factor();
		else 
		{
			double res = eval_factor();
			if (eval_error || res == 0) 
			{
				result = 0;
				eval_error = true;
			}
			else result /= res;

		}
	return result;
}

double Table::evaluate_expression() 
{
	eval_error = false;
	double result = eval_term();
	while (eval_peek() == '+' || eval_peek() == '-')
		if (eval_get() == '+')
			result += eval_term();
		else
			result -= eval_term();
	return result;

}