#include "lexer.hpp"

CompilationException::CompilationException(const int row, const int column, const string message)
    : row(row), column(column), message(message)
{

}

const char* CompilationException::what() const noexcept
{
    return (message + " (" + to_string(row) + ", " + to_string(column) + ")").c_str();
}

TextReader::TextReader(const string &filename) : source(filename)
{
    _buffer = new string();
    _start = _buffer->begin();
    _end = _buffer->begin();
}

char TextReader::get_next_char(const int length)
{
    // TODO
}

void TextReader::retract(const int length)
{
    // TODO
}

Lexer::Lexer(queue<Token> &token_queue)
    : _token_queue(token_queue),
      identifier_table(new unordered_map<string, int>)
{
    row = 0;
    column = 0;
}

void Lexer::prase(const string &filename)
{
    // TODO
}

int Lexer::get_keyword_index(const string text)
{
    int index = -1;
    try
    {
        index = KEYWORD_TABLE->at(text);
    }
    catch(const std::out_of_range& e)
    {

    }
    return index;
}

int Lexer::get_identifier_index(const string text)
{
    return (*identifier_table)[text];
}

string Lexer::dump_token(const Token &token) const
{
    string text = TOKEN_NAMES[token.first];
    switch (token.first)
    {
        case EmptyTokenType:
            break;
        case IdentifierTokenType:
        case KeywordTokenType:
        case IntTokenType:
        case UnaryOperatorTokenType:
        case BinaryOperatorTokenType:
        case TernaryOperatorTokenType:
        case DelimiterTokenType:
            text += to_string(token.second.int_value);
            break;
        case FloatTokenType:
            text += to_string(token.second.float_value);
            break;
        case CharTokenType:
            text += to_string(token.second.char_value);
            break;
        case StringTokenType:
            text += *(token.second.string_value);
            break;
    }
    return text;
}

LexerComsumer::LexerComsumer(queue<Token> &token_queue)
    : _token_queue(token_queue)
{

}

void LexerComsumer::run()
{
    while(/* condition */){
        /* code */
    }
}
