#include "lexer.cpp"

int main()
{
    Lexer lexer;
    lexer.prase("test.cpp");
    cout << endl;
    lexer.print_stat();
    cout << "Press Enter to exit.";
    cin.get();
    return 0;
}
