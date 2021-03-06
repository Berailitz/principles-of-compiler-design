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
    read_file(0);
}

TextReader::~TextReader()
{
    delete[] _buffer;
}

void TextReader::read_file(const int start_position)
{
    fill_n(_buffer + start_position, LEXER_MID_BUFFER_SIZE, END_OF_FILE);
    source.read(_buffer + start_position, LEXER_MID_BUFFER_SIZE);
}

char TextReader::get_next_char()
{
    // blanks included, check is_in_retraction
    char current_char, next_char;
    if (!is_in_retraction)
    {
        if (end_index == 0)
        {
            current_char = _buffer[LEXER_BUFFER_SIZE - 1];
        }
        else
        {
            current_char = _buffer[end_index - 1];
        }
        if (current_char == LINE_DELIMITER)
        {
            row++;
            column = 0;
            cout << endl << "Line " << row << ":" << endl;
        }
        else
        {
            column++;
            word_counter++;
        }
        end_index++;
        if (end_index == LEXER_MID_BUFFER_SIZE)
        {
            read_file(LEXER_MID_BUFFER_SIZE);
        }
        else if (end_index == LEXER_BUFFER_SIZE)
        {
            read_file(0);
            end_index = 0;
        }
    }
    if (end_index == 0)
    {
        next_char = _buffer[LEXER_BUFFER_SIZE - 1];
    }
    else
    {
        next_char = _buffer[end_index - 1];
    }
    is_in_retraction = false;
    return next_char;
}

void TextReader::reset_current_string()
{
    if (end_index == 0)
    {
        if (is_in_retraction)
        {
            start_index = LEXER_BUFFER_SIZE - 1;
        }
        else
        {
            start_index = 0;
        }
    }
    else
    {
        if (is_in_retraction)
        {
            start_index = end_index - 1;
        }
        else
        {
            start_index = end_index;
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
        else
        {
            current_string.append(_buffer + start_index, end_index - start_index);
        }
    }
    else
    {
        current_string.append(_buffer + start_index, LEXER_BUFFER_SIZE - start_index);
        if (is_in_retraction)
        {
            if (end_index > 0)
            {
                current_string.append(_buffer, end_index - 1);
            }
        }
        else
        {
            current_string.append(_buffer, end_index);
        }
    }
    return current_string;
}

int TextReader::get_column() const
{
    return column;
}

int TextReader::get_row() const
{
    return row;
}

int TextReader::get_word_counter() const
{
    return word_counter;
}

Lexer::Lexer()
    : identifier_table(new unordered_map<string, int>),
      token_counter(new int[TOKEN_TYPE_COUNTER])
{
    fill_n(token_counter, TOKEN_TYPE_COUNTER, 0);
}

Lexer::~Lexer()
{
    delete reader;
    delete identifier_table;
    delete[] token_counter;
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
    reader = new TextReader(filename);
    cout << "Line 1" << endl;
    next_char = reader->get_next_char();
    while (next_char != END_OF_FILE)
    {
        switch (get_state())
        {
        case LEX_DFA_languages:
            if (next_char == '0')
            {
                set_state(LEX_DFA_octs1hexs1dec0);
            }
            else if (isdigit(next_char))
            {
                set_state(LEX_DFA_decs1floats1);
            }
            else if (is_c_letter(next_char))
            {
                set_state(LEX_DFA_identifier1);
            }
            else
            {
                switch (next_char)
                {
                case '\'':
                    set_state(LEX_DFA_chars1);
                    break;
                case '"':
                    set_state(LEX_DFA_strings1);
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
                case '.':
                    receive_token(BinaryOperatorTokenType, false);
                    break;
                case '~':
                    receive_token(UnaryOperatorTokenType, false);
                    break;
                case '#':
                    set_state(LEX_DFA_commentOfMacros1);
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
            if (!(is_c_letter(next_char) || isdigit(next_char)))
            {
                receive_token(IdentifierTokenType, true);
            }
            break;
        case LEX_DFA_octs1hexs1dec0:
            if (is_oct(next_char))
            {
                set_state(LEX_DFA_octs2);
            }
            else if (next_char == 'x')
            {
                set_state(LEX_DFA_hexs2);
            } else if (next_char == '.')
            {
                set_state(LEX_DFA_floats2);
            }
            else
            {
                receive_token(IntTokenType, true);
            }
            break;
        case LEX_DFA_octs2:
            if (next_char == '.')
            {
                set_state(LEX_DFA_floats2);
            }
            else if (!is_oct(next_char))
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
                set_state(LEX_DFA_decs4);
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
            if (isdigit(next_char))
            {
                set_state(LEX_DFA_decs4);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_decs4:
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
                set_state(LEX_DFA_floats6);
            }
            else
            {
                switch (next_char)
                {
                case '+':
                case '-':
                    set_state(LEX_DFA_floats5);
                    break;
                default:
                    raise_error();
                    break;
                }
            }
            break;
        case LEX_DFA_floats5:
            if (isdigit(next_char))
            {
                set_state(LEX_DFA_floats6);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_floats6:
            if (!isdigit(next_char))
            {
                receive_token(FloatTokenType, true);
            }
            break;
        case LEX_DFA_chars1:
            if (next_char == '\\')
            {
                set_state(LEX_DFA_chars2);
            }
            else if (next_char != '\'')
            {
                set_state(LEX_DFA_chars3);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_chars2:
            set_state(LEX_DFA_chars3);
            break;
        case LEX_DFA_chars3:
            if (next_char == '\'')
            {
                receive_token(CharTokenType, false);
            }
            else
            {
                raise_error();
            }
            break;
        case LEX_DFA_strings1:
            if (next_char == '"')
            {
                receive_token(StringTokenType, false);
            }
            else if (next_char == '\\')
            {
                set_state(LEX_DFA_strings2);
            }
            break;
        case LEX_DFA_strings2:
            set_state(LEX_DFA_strings1);
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
        case LEX_DFA_commentOfMacros1:
            if (next_char == LINE_DELIMITER)
            {
                receive_token(EmptyTokenType, true);
                // ???????????????????????????????????????
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
                // ????????????????????????
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
            break;
        }
        next_char = reader->get_next_char();
    }
    cout << "Lexer finished." << endl;
}

void Lexer::receive_token(const TokenType &type, const bool do_retract)
{
    // ?????????????????????????????????
    string text;
    Token token(type, 1);
    if (do_retract)
    {
        reader->is_in_retraction = true;
    }
    text = reader->get_current_string();
    token.second.string_value = text;
    switch (type)
    {
    case EmptyTokenType:
        token.second.string_value = text;
    case StringTokenType:
        token.second.string_value = text.substr(1, text.length() - 2);
        break;
    case KeywordTokenType:
    case IdentifierTokenType:
        token.second.string_value = text;
        token.first = KeywordTokenType;
        token.second.int_value = get_keyword_index(text);
        if (token.second.int_value == -1)
        {
            token.second.int_value = get_identifier_index(text);
            token.first = IdentifierTokenType;
        }
        break;
    case IntTokenType:
        token.second.string_value = text;
        if (is_in_error)
        {
            token.second.int_value = 0;
        }
        else
        {
            token.second.int_value = atoi(text.c_str());
        }
        break;
    case FloatTokenType:
        token.second.string_value = text;
        if (is_in_error)
        {
            token.second.float_value = 0;
        }
        else
        {
            token.second.float_value = atof(text.c_str());
        }
        break;
    case CharTokenType:
        token.second.string_value = text.substr(1, text.length() - 2);
        token.second.char_value = text[1];
        break;
    case UnaryOperatorTokenType:
    case BinaryOperatorTokenType:
    case TernaryOperatorTokenType:
        token.second.string_value = text;
        token.second.int_value = get_operator_index(text);
        break;
    case DelimiterTokenType:
        token.second.string_value = text;
        token.second.int_value = get_delimiter_index(text);
        break;
    }
    cout << dump_token(token);
    token_counter[type]++;
    set_state(LEX_DFA_languages);
    reader->reset_current_string();
}

void Lexer::raise_error()
{
    cout << "<ERROR at (" << to_string(reader->get_row()) << ", " << to_string(reader->get_column()) << "): ";
    is_in_error = true;
    error_counter++;
    switch (get_state())
    {
    case LEX_DFA_hexs2:
        cout << "HEX word excepted, maybe `0`?";
        cout << " Jump to the next line.>" << endl;
        receive_token(IntTokenType, false);
        break;
    case LEX_DFA_decs2:
    case LEX_DFA_floats4:
        cout << "a digit or `+` or `-` excepted, maybe `0`?";
        cout << " Jump to the next line.>" << endl;
        receive_token(IntTokenType, false);
        break;
    case LEX_DFA_decs3:
        cout << "a digit excepted, maybe `0`?";
        cout << " Jump to the next line.>" << endl;
        receive_token(IntTokenType, false);
        break;
    case LEX_DFA_floats2:
    case LEX_DFA_floats5:
        cout << "a digit excepted, maybe `0`?";
        cout << " Jump to the next line.>" << endl;
        receive_token(FloatTokenType, false);
        break;
    case LEX_DFA_chars1:
        cout << "a character excepted, maybe `a`?";
        cout << " Jump to the next line.>" << endl;
        receive_token(CharTokenType, false);
        break;
    case LEX_DFA_chars3:
        cout << "missing `'` ?";
        cout << " Jump to the next line.>" << endl;
        receive_token(CharTokenType, false);
        break;
    }
    while (next_char != ';')
    {
        next_char = reader->get_next_char();
    }
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
    int index = -1;
    try
    {
        index = identifier_table->at(text);
    }
    catch (const std::out_of_range &e)
    {
        index = identifier_table->size();
        identifier_table->insert({text, index});
    }
    return index;
}

int Lexer::get_operator_index(const string text)
{
    return OPERATOR_TABLE->at(text);
}

int Lexer::get_delimiter_index(const string text)
{
    return DELIMITER_TABLE->at(text);
}

bool Lexer::is_oct(const char target_char) const
{
    return target_char >= '0' && target_char <= '7';
}

bool Lexer::is_c_letter(const char target_char) const
{
    return target_char == '_' || isalpha(target_char);
}

void Lexer::print_stat() const
{
    cout << "Token counters:" << endl;
    for (int i = 0; i < TOKEN_TYPE_COUNTER; i++)
    {
        cout << TOKEN_NAMES[i] << ": " << token_counter[i] << endl;
    }
    cout << reader->get_row() << " lines." << endl;
    cout << reader->get_word_counter() << " characters." << endl;
    cout << error_counter << " errors." << endl;
}

string dump_token(const Token &token)
{
    string text = "<" + TOKEN_NAMES[token.first];
    switch (token.first)
    {
    case EmptyTokenType:
    case IdentifierTokenType:
    case KeywordTokenType:
        text += (", " + to_string(token.second.int_value));
        break;
    case IntTokenType:
    case UnaryOperatorTokenType:
    case BinaryOperatorTokenType:
    case TernaryOperatorTokenType:
    case DelimiterTokenType:
        text += (", " + to_string(token.second.int_value));
        break;
    case FloatTokenType:
        text += (", " + to_string(token.second.float_value));
        break;
    case CharTokenType:
        text += (", " + string(1, token.second.char_value));
        break;
    case StringTokenType:
        break;
    }
    text += (", `" + token.second.string_value + "`");
    return text + ">" + LINE_DELIMITER;
}
