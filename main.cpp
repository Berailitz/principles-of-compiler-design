#include  "lexer.cpp"

int main()
{
    queue<Token> token_queue;
    Lexer lexer(token_queue);
    LexerComsumer comsumer(token_queue);
    lexer.prase("test.cpp");
    comsumer.run();
    cin.get();
}
