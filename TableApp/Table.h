#pragma once
#include <vector>
#include <string>
#include "Cell.h"
#include <fstream>

class Table
{
private:
	vector<vector<Cell>> table_cells;
	string file_name;

	bool is_operation_allowed();
	void clear();
	void calculate();
	int load_file();
	fstream input_stream;

	char eval_peek();
	char eval_get();
	bool is_digit();
	int eval_int_number();
	double eval_number();
	double eval_factor();
	double eval_term();
	double evaluate_expression();

public:
	int open_file(string fName);
	int close_file();
	int save_file();
	int save_as_file(string fName);
	int edit_cell(string row, string column, string value);
	void print_table();
	bool has_file();
	int set_cell(int row, int column, string value);
	string get_cell_result(size_t row, size_t column);
};

