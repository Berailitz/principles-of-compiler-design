#include "lexer.cpp"

int main(int argc, char const *argv[])
{
    Lexer lexer;
    if (argc == 2)
    {
        lexer.prase(argv[1]);
    }
    else
    {
        lexer.prase("test.cpp");
    }
    cout << endl;
    lexer.print_stat();
    cout << "Press Enter to exit.";
    cin.get();
    return 0;
}
