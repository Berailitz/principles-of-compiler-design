#include "lexer.hpp"

TokenValueUnion::TokenValueUnion()
{
}

TokenValueUnion::TokenValueUnion(int int_value) : int_value(int_value)
{
}

TokenValueUnion::TokenValueUnion(double float_value) : float_value(float_value)
{
}

TokenValueUnion::TokenValueUnion(char char_value) : char_value(char_value)
{
}

TokenValueUnion::TokenValueUnion(string &string_value) : string_value(string_value)
{
}

TextReader::TextReader(const string &filename) : source(filename)
{
    source.read(_buffer, LEXER_BUFFER_SIZE);
}

TextReader::~TextReader()
{
    delete[] _buffer;
}

char TextReader::get_next_char()
{
    // blanks included, check is_in_retraction
    if (!is_in_retraction)
    {
        if (end_index == LEXER_MID_BUFFER_SIZE)
        {
            source.read(_buffer + LEXER_MID_BUFFER_SIZE, LEXER_MID_BUFFER_SIZE);
            end_index++;
        }
        else if (end_index == LEXER_BUFFER_SIZE)
        {
            source.read(_buffer, LEXER_MID_BUFFER_SIZE);
            end_index = 0;
        }
        else
        {
            end_index++;
        }
    }
    is_in_retraction = false;
    if (end_index == 0)
    {
        return _buffer[LEXER_BUFFER_SIZE - 1];
    }
    else
    {
        return _buffer[end_index - 1];
    }
}

void TextReader::reset_current_string()
{
    if (end_index == 0)
    {
        if (is_in_retraction)
        {
            start_index = 0;
        }
        else {
            start_index = LEXER_BUFFER_SIZE - 1;
        }
    }
    else
    {
        if (is_in_retraction)
        {
            start_index = end_index;
        }
        else {
            start_index = end_index - 1;
        }
    }
}

string TextReader::get_current_string() const
{
    string current_string = "";
    if (end_index > start_index)
    {
        if (is_in_retraction)
        {
            current_string.append(_buffer + start_index, end_index - start_index - 1);
        }
        else {
            current_string.append(_buffer + start_index, end_index - start_index);
        }
    }
    else
    {
        current_string.append(_buffer + start_index, LEXER_BUFFER_SIZE - start_index);
        if (is_in_retraction)
        {
            current_string.append(_buffer, end_index);
        }
        else {
            current_string.append(_buffer, end_index + 1);
        }
    }
    return current_string;
}

Lexer::Lexer(queue<Token> &token_queue)
    : _token_queue(token_queue),
      identifier_table(new unordered_map<string, int>)
{
    row = 0;
    column = 0;
}

Lexer::~Lexer()
{
    delete reader;
    delete identifier_table;
}

void Lexer::set_state(const LEX_DFA_STATE next_state)
{
    _state = next_state;
}

LEX_DFA_STATE Lexer::get_state()
{
    return _state;
}

void Lexer::prase(const string &filename)
{
    char next_char;
    reader = new TextReader(filename);
    next_char = reader->get_next_char();
    while (next_char != END_OF_FILE)
    {
        switch (get_state())
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
                    set_state(LEX_DFA_Operators1);
                    break;
                case '!':
                    set_state(LEX_DFA_Operators2);
                    break;
                case '%':
                    set_state(LEX_DFA_Operators3);
                    break;
                case '&':
                    set_state(LEX_DFA_Operators4);
                    break;
                case '*':
                    set_state(LEX_DFA_Operators5);
                    break;
                case '/':
                    set_state(LEX_DFA_Operators6comments1);
                    break;
                case '^':
                    set_state(LEX_DFA_Operators7);
                    break;
                case '|':
                    set_state(LEX_DFA_Operators8);
                    break;
                case '+':
                    set_state(LEX_DFA_Operators9);
                    break;
                case '<':
                    set_state(LEX_DFA_Operators10);
                    break;
                case '=':
                    set_state(LEX_DFA_Operators11);
                    break;
                case '>':
                    set_state(LEX_DFA_Operators12);
                    break;
                case ':':
                    set_state(LEX_DFA_Operators15);
                    break;
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
                reader->reset_current_string();
                    break;
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
        case LEX_DFA_Operators6comments1:
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
        case LEX_DFA_Operators1:
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
        case LEX_DFA_Operators2:
        case LEX_DFA_Operators7:
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
        case LEX_DFA_Operators3:
        case LEX_DFA_Operators5:
        case LEX_DFA_Operators11:
        case LEX_DFA_Operators13:
        case LEX_DFA_Operators14:
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
        case LEX_DFA_Operators4:
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
        case LEX_DFA_Operators8:
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
        case LEX_DFA_Operators9:
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
        case LEX_DFA_Operators10:
            switch (next_char)
            {
            case '<':
                set_state(LEX_DFA_Operators13);
                break;
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_Operators12:
            switch (next_char)
            {
            case '>':
                set_state(LEX_DFA_Operators14);
                break;
            case '=':
                receive_token(BinaryOperatorTokenType, false);
                break;
            default:
                receive_token(BinaryOperatorTokenType, true);
                break;
            }
            break;
        case LEX_DFA_Operators15:
            if (next_char == ':')
            {
                receive_token(BinaryOperatorTokenType, false);
            }
            else
            {
                receive_token(BinaryOperatorTokenType, true);
            }
            break;
        case LEX_DFA_error:
            cout << "ERROR";
            break;
        }
        next_char = reader->get_next_char();
    }
}

void Lexer::receive_token(const TokenType &type, const bool do_retract)
{
    // 初始化状态机并送出符号
    string text;
    Token token(type, 1);
    if (do_retract)
    {
        reader->is_in_retraction = true;
    }
    text = reader->get_current_string();
    switch (type)
    {
    case EmptyTokenType:
        cout << "EmptyTokenType";
        break;
    case KeywordTokenType:
    case IdentifierTokenType:
        token.first = KeywordTokenType;
        token.second.string_value = text;
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
        token.second.string_value = text;
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
    reader->reset_current_string();
    cout << dump_token(token);
}

void Lexer::raise_error()
{
    set_state(LEX_DFA_error);
}

int Lexer::get_keyword_index(const string text)
{
    int index = -1;
    cout << "`" << text << "`" << endl;
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
    cout << "`" << text << "`" << endl;
    return (*identifier_table)[text];
}

int Lexer::get_operator_index(const string text)
{
    cout << "`" << text << "`" << endl;
    return OPERATOR_TABLE->at(text);
}

int Lexer::get_delimiter_index(const string text)
{
    cout << "`" << text << "`" << endl;
    return DELIMITER_TABLE->at(text);
}

bool Lexer::is_oct(const char next_char) const
{
    return next_char >= '0' && next_char <= '7';
}

string dump_token(const Token &token)
{
    string text = "<" + TOKEN_NAMES[token.first] + ", ";
    switch (token.first)
    {
    case EmptyTokenType:
        break;
    case IdentifierTokenType:
    case KeywordTokenType:
        text += (to_string(token.second.int_value) + ", " + token.second.string_value);
        break;
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
        text += token.second.string_value;
        break;
    }
    return text + ">" + LINE_DELIMITER;
}

LexerComsumer::LexerComsumer(queue<Token> &token_queue)
    : _token_queue(token_queue)
{
}

void LexerComsumer::run()
{
    while (!_token_queue.empty())
    {
        cout << dump_token(_token_queue.front());
        _token_queue.pop();
    }
}
