#include "lexer.cpp"

int main(int argc, char const *argv[])
{
    string filename;
    Lexer lexer;
    if (argc == 2)
    {
        lexer.prase(argv[1]);
    }
    else
    {
        cout << "Please enter target source filename: ";
        cin >> filename;
        cin.ignore();
        lexer.prase(filename);
    }
    cout << endl;
    lexer.print_stat();
    cout << "Press Enter to exit.";
    cin.get();
    return 0;
}
