#include "lexer.hpp"

CompilationException::CompilationException(const int row, const int column, const string message)
    : row(row), column(column), message(message)
{

}

const char* CompilationException::what() const noexcept
{
    return (message + " (" + to_string(row) + ", " + to_string(column) + ")").c_str();
}

Token::Token(TokenType type, TokenValueUnion value) :
    _type(type), _value(value)
{

}

Token::Token(const Token &old_token)
{
    initialize(&old_token);
}

Token::Token(const Token *old_token)
{
    initialize(old_token);
}

void Token::set_value(TokenValueUnion value)
{
    _value = value;
}

TokenValueUnion Token::get_value() const
{
    return _value;
}

TokenType Token::get_type() const
{
    return _type;
}

string Token::dump() const
{
    string text = TOKEN_NAMES[_type];
    switch (_type)
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
            text += to_string(_value.int_value);
            break;
        case FloatTokenType:
            text += to_string(_value.float_value);
            break;
        case CharTokenType:
            text += to_string(_value.char_value);
            break;
        case StringTokenType:
            text += *(_value.string_value);
            break;
    }
    return text;
}

Token& Token::operator=(const Token &other)
{
    if (this != &other)
    {
        initialize(&other);
    }
    return *this;
}

void Token::initialize(const Token *old_token)
{
    if (old_token == nullptr)
    {
        _type = EmptyTokenType;
        _value.string_value = nullptr;
    }
    else {
        _type = old_token->get_type();
        _value = old_token->get_value();
    }
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
    TextReader reader(filename);
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
