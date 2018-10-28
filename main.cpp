#include "lexer.cpp"

int main(int argc, char const *argv[])
{
    string filename;
    Lexer lexer;
    if (argc == 2)
    {
         filename = argv[1];
    }
    else
    {
        cout << "Please enter target source filename: ";
        cin >> filename;
        cin.ignore();
    }
    cout << "Prasing `" << filename << "`." << endl;;
    lexer.prase(filename);
    cout << endl;
    lexer.print_stat();
    cout << endl;
    cout << "Press Enter to exit.";
    cin.get();
    return 0;
}
