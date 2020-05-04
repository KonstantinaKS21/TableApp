#include <iostream>
#include <string>

using namespace std;

void static PrintHelp()
{
    cout << "============== Table Calculator Help ===============\n\n";
    cout << "1. Help                        - print this help\n";
    cout << "2. Open <filename>             - load a table from <filename>\n";
    cout << "3. Close                       - close current file\n";
    cout << "4. Save                        - save current file\n";
    cout << "5. Save As <filename>          - save current file in <filename>\n";
    cout << "6. Edit <row> <column> <value> - edit data in selected cell\n";
    cout << "7. Print                       - print help menu\n";
    cout << "8. Exit                        - exit the program\n\n";
}

bool static InputParse(string in) 
{
    
    bool ret = false;

    if (in.compare("help") == 0) PrintHelp();
    else if (in.compare("open") == 0) cout << "Open selected\n";
    else if (in.compare("close") == 0) cout << "Close selected\n";
    else if (in.compare("save") == 0) cout << "Save selected\n";
    else if (in.compare("save as") == 0) cout << "Save As selected\n";
    else if (in.compare("edit") == 0) cout << "Edit selected\n";
    else if (in.compare("print") == 0) cout << "Print selected\n";
    else if (in.compare("exit") == 0)
    {
        cout << "Exiting the program\n";
        ret = true;
    }
    else
    {
        cout << "Wrong command\n";
        PrintHelp();
    }
    return ret;
}


int main()
{
    string inputLine;

    cout << "Program for calculating excel like tables\n\n";

    PrintHelp();
    while (true) 
    {
        cout << "\n> ";
        getline(cin,inputLine);
        if (InputParse(inputLine)) break;
    }

    return 0;
}
