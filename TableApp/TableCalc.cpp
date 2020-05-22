#include <iostream>
#include <string>
#include "Table.h"

using namespace std;

Table table;

void static print_help() 
{
    cout << "============== Table Calculator Help ===============\n\n";
    cout << "1. Help                        - print this menu\n";
    cout << "2. Open <filename>             - load a table from <filename>\n";
    cout << "3. Close                       - close current table, disable further operations\n";
    cout << "4. Save                        - save current table\n";
    cout << "5. Save As <filename>          - save current table into <filename>\n";
    cout << "6. Edit <row> <column> <value> - change a cell[<row>,<column>] to <value>\n";
    cout << "7. Print                       - print current table results\n";
    cout << "8. Exit                        - exit program\n\n";
}

vector<string> compare_command(string input, vector<string>commands) 
{
    vector<string> result(commands.size());
    for (int j = 0; j < commands.size(); j++)
    {
        input.erase(0, input.find_first_not_of("\t\n\v\f\r "));
        if (input.empty()) return vector<string>{};

        int k = input.find_first_of("\t\n\v\f\r ");
        if (k != 0) 
        {
            result[j] = input.substr(0, k);
            input.erase(0, k);
            if (commands[j].size() !=0 && result[j] != commands[j]) return vector<string>{};
        }
        else break;
    }
    if (!input.empty() && commands.size()!=1) result[commands.size()-1] += input;

    return result;
}

bool static input_parse(string input)
{
    bool result = false;
    vector<string> commands;
    if (compare_command(input, { "help" }).size() != 0) print_help();
    else if ((commands = compare_command(input, { "open", "" })).size() != 0) 
    {
        cout << "Opened \n" << commands[1] << endl;
        table.open_file(commands[1]);
    }
    else if (compare_command(input, { "close" }).size() != 0)
    {
        cout << "Closing file.\n";
        table.close_file();
    }
    else if ((commands = compare_command(input, { "save", "as", "" })).size() != 0)
    {
        cout << "Save as selected\n" << commands[2] << endl;
        table.save_as_file(commands[2]);
    }
    else if (compare_command(input, { "save" }).size() != 0) 
    {
        cout << "File saved.\n";
        table.save_file();
    }
    else if ((commands = compare_command(input, { "edit", "", "", "" })).size() != 0) 
    {
        cout << "Edited.\n" << endl;
        table.edit_cell(commands[1], commands[2], commands[3]);
    }
    else if (compare_command(input, { "print" }).size() != 0)
    {
        cout << "Print selected.\n";
        table.print_table();
    }
    else if (compare_command(input, { "exit" }).size() != 0)
    {
        cout << "Exiting the program...\n";
        result = true;
    }
    else
    {
        cout << "Wrong command!!!\n";
        print_help();
    }
    return result;
}

int main()
{
    string inputLine;

    cout << "Program for calculating excel-like tables\n\n";

    print_help();
    while (true) 
    {
        cout << "\n> ";
        getline(cin,inputLine);
        if (input_parse(inputLine)) break;
    }
    return 0;
}
