#include  "lexer.cpp"

int main()
{
    queue<Token> token_queue;
    Lexer lexer(token_queue);
    LexerComsumer comsumer(token_queue);
    lexer.prase("test.cpp");
    cout << "Lexer finished." << endl;
    cout << endl;
    cout << "Tokens:" << endl;
    comsumer.run();
    cout << "Press Enter to exit.";
    cin.get();
    return 0;
}
