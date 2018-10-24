#include "lexer.hpp"

CompilationException::CompilationException(const int row, const int column, const string message)
    : row(row), column(column), message(message)
{
}

const char *CompilationException::what() const noexcept
{
    return (message + " (" + to_string(row) + ", " + to_string(column) + ")").c_str();
}

TextReader::TextReader(const string &filename) : source(filename)
{
    _buffer = new string();
}

char TextReader::get_next_char()
{
    // TODO, \0 for eof, blanks included
}

void TextReader::retract()
{
    // move end_index back
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
    char next_char;
    reader = new TextReader(filename);
    while (next_char != END_OF_FILE)
    {
        next_char = reader->get_next_char();
        switch (get_state())
        {
        case LEX_DFA_languages:
            switch (next_char)
            {
            case LEX_DFA_languages:
                if (next_char == '0')
                {
                    set_state(LEX_DFA_octs1hexs1);
                }
                else if (isdigit(next_char))
                {
                    set_state(LEX_DFA_decs1floats1);
                }
                else if (isalpha(next_char))
                {
                    set_state(LEX_DFA_identifier1);
                }
                else if (isblank(next_char))
                {
                }
                else
                {
                    switch (next_char)
                    {
                    case '\'':
                        set_state(LEX_DFA_chars1);
                        break;
                    case '"':
                        set_state(LEX_DFA_string1);
                        break;
                    case '-':
                        set_state(LEX_DFA_UnaryOperators1);
                        break;
                    case '!':
                        set_state(LEX_DFA_UnaryOperators2);
                        break;
                    case '%':
                        set_state(LEX_DFA_UnaryOperators3);
                        break;
                    case '&':
                        set_state(LEX_DFA_UnaryOperators4);
                        break;
                    case '*':
                        set_state(LEX_DFA_UnaryOperators5);
                        break;
                    case '/':
                        set_state(LEX_DFA_UnaryOperators6comments1);
                        break;
                    case '^':
                        set_state(LEX_DFA_UnaryOperators7);
                        break;
                    case '|':
                        set_state(LEX_DFA_UnaryOperators8);
                        break;
                    case '+':
                        set_state(LEX_DFA_UnaryOperators9);
                        break;
                    case '<':
                        set_state(LEX_DFA_UnaryOperators10);
                        break;
                    case '=':
                        set_state(LEX_DFA_UnaryOperators11);
                        break;
                    case '>':
                        set_state(LEX_DFA_UnaryOperators12);
                        break;
                    case ':':
                    case '?':
                        receive_token(TernaryOperatorTokenType, false);
                        break;
                    case '~':
                        receive_token(UnaryOperatorTokenType, false);
                        break;
                    case '(':
                    case ')':
                    case '[':
                    case ']':
                    case '{':
                    case '}':
                    case ',':
                    case ';':
                        receive_token(DelimiterTokenType, false);
                        break;
                    default:
                        raise_error();
                        break;
                    }
                }
            }
            break;
        case LEX_DFA_identifier1:
            if (!isalnum(next_char))
            {
                receive_token(IdentifierTokenType, true);
            }
            break;
        case LEX_DFA_octs1hexs1:
            if (is_oct(next_char))
            {
                set_state(LEX_DFA_octs2);
            }
            else if (next_char == 'x')
            {
                set_state(LEX_DFA_hexs2);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_octs2:
            if (!is_oct(next_char))
            {
                receive_token(IntTokenType, true);
            }
            break;
        case LEX_DFA_hexs2:
            if (isxdigit(next_char))
            {
                set_state(LEX_DFA_hexs3);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_hexs3:
            if (!isxdigit(next_char))
            {
                receive_token(IntTokenType, true);
            }
            break;
        case LEX_DFA_decs1floats1:
            if (isdigit(next_char))
            {
            }
            else
            {
                switch (next_char)
                {
                case 'e':
                case 'E':
                    set_state(LEX_DFA_decs2);
                    break;
                case '.':
                    set_state(LEX_DFA_floats2);
                    break;
                default:
                    receive_token(IntTokenType, true);
                    break;
                }
            }
            break;
        case LEX_DFA_decs2:
            if (isdigit(next_char))
            {
                set_state(LEX_DFA_decs3);
            }
            else
            {
                switch (next_char)
                {
                case '+':
                case '-':
                    set_state(LEX_DFA_decs3);
                    break;
                default:
                    raise_error();
                    break;
                }
            }
            break;
        case LEX_DFA_decs3:
            if (!isdigit(next_char))
            {
                receive_token(IntTokenType, true);
            }
            break;
        case LEX_DFA_floats2:
            if (isdigit(next_char))
            {
                set_state(LEX_DFA_floats3);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_floats3:
            if (!isdigit(next_char))
            {
                switch (next_char)
                {
                case 'E':
                case 'e':
                    set_state(LEX_DFA_floats4);
                    break;
                default:
                    receive_token(FloatTokenType, true);
                }
            }
            break;
        case LEX_DFA_floats4:
            if (isdigit(next_char))
            {
                set_state(LEX_DFA_floats5);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_floats5:
            if (!isdigit(next_char))
            {
                receive_token(FloatTokenType, true);
            }
            break;
        case LEX_DFA_chars1:
            if (next_char == '\'')
            {
                raise_error();
            }
            else
            {
                set_state(LEX_DFA_chars2);
            }
            break;
        case LEX_DFA_chars2:
            if (next_char == '\'')
            {
                receive_token(CharTokenType, false);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_string1:
            if (next_char == '"')
            {
                receive_token(StringTokenType, false);
            }
            break;
        case LEX_DFA_UnaryOperators6comments1:
            switch (next_char)
            {
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            case '/':
                set_state(LEX_DFA_commentInLine2);
                break;
            case '*':
                set_state(LEX_DFA_commentCrossLine2);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_commentInLine2:
            if (next_char == LINE_DELIMITER)
            {
                receive_token(EmptyTokenType, false);
                // 表示行内注释结束
            }
            break;
        case LEX_DFA_commentCrossLine2:
            if (next_char == '*')
            {
                set_state(LEX_DFA_commentCrossLine3);
            }
            break;
        case LEX_DFA_commentCrossLine3:
            if (next_char == '/')
            {
                receive_token(EmptyTokenType, false);
                // 表示跨行注释结束
            }
            else
            {
                set_state(LEX_DFA_commentCrossLine2);
            }
            break;
        case LEX_DFA_UnaryOperators1:
            switch (next_char)
            {
            case '-':
                receive_token(UnaryOperatorTokenType, false);
                break;
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_UnaryOperators2:
        case LEX_DFA_UnaryOperators7:
            switch (next_char)
            {
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(UnaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_UnaryOperators3:
        case LEX_DFA_UnaryOperators5:
        case LEX_DFA_UnaryOperators11:
        case LEX_DFA_UnaryOperators13:
        case LEX_DFA_UnaryOperators14:
            switch (next_char)
            {
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_UnaryOperators4:
            switch (next_char)
            {
            case '&':
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(UnaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_UnaryOperators8:
            switch (next_char)
            {
            case '|':
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(UnaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_UnaryOperators9:
            switch (next_char)
            {
            case '+':
                receive_token(UnaryOperatorTokenType, false);
                break;
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_UnaryOperators10:
            switch (next_char)
            {
            case '<':
                set_state(LEX_DFA_UnaryOperators13);
                break;
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_UnaryOperators12:
            switch (next_char)
            {
            case '>':
                set_state(LEX_DFA_UnaryOperators14);
                break;
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        }
    }
}

void Lexer::receive_token(const TokenType &type, const bool do_retract)
{
    // 初始化状态机并送出符号
    string text = reader->get_current_string();
    Token token({type, nullptr});
    switch (type)
    {
    case EmptyTokenType:
        cout << "EmptyTokenType";
        break;
    case IdentifierTokenType:
        token.second.int_value = get_keyword_index(text);
        if (token.second.int_value == -1)
        {
            token.second.int_value = get_identifier_index(text);
            token.first = IdentifierTokenType;
        }
        break;
    case IntTokenType:
        token.second.int_value = atoi(text.c_str());
        break;
    case FloatTokenType:
        token.second.float_value = atof(text.c_str());
        break;
    case CharTokenType:
        token.second.char_value = text[1];
        break;
    case StringTokenType:
        token.second.string_value = new string(text);
        break;
    case UnaryOperatorTokenType:
    case BinaryOperatorTokenType:
    case TernaryOperatorTokenType:
        token.second.int_value = get_operator_index(text);
        break;
    case DelimiterTokenType:
        token.second.int_value = get_delimiter_index(text);
        break;
    }
    _token_queue.push(token);
    set_state(LEX_DFA_languages);
    if (do_retract)
    {
        reader->retract();
    }
    reader->reset_current_string();
}

void Lexer::raise_error()
{
    set_state(LEX_DFA_error);
}

int Lexer::get_keyword_index(const string text)
{
    int index = -1;
    try
    {
        index = KEYWORD_TABLE->at(text);
    }
    catch (const std::out_of_range &e)
    {
    }
    return index;
}

int Lexer::get_identifier_index(const string text)
{
    return (*identifier_table)[text];
}

int Lexer::get_operator_index(const string text)
{
    return OPERATOR_TABLE->at(text);
}

int Lexer::get_delimiter_index(const string text)
{
    return DELIMITER_TABLE->at(text);
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
    while (/* condition */)
    {
        /* code */
    }
}
