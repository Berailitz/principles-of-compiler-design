#include <cctype>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>

using namespace std;

const int MAX_IDENTIFIER_LENGTH = 1000;
const int MAX_TOKEN_QUEUE_SIZE = 10;
const char LINE_DELIMITER = '\n';
const char END_OF_FILE = '\0';
const string DEFAULT_ERROR_MESSAGE = "Error occored.";
const unordered_map<string, int> *KEYWORD_TABLE = new unordered_map<string, int>(
    {{"alignas", 0},
     {"alignof", 1},
     {"and", 2},
     {"and_eq", 3},
     {"asm", 4},
     {"auto", 5},
     {"bitand", 6},
     {"bitor", 7},
     {"bool", 8},
     {"break", 9},
     {"case", 10},
     {"catch", 11},
     {"char", 12},
     {"char16_t", 13},
     {"char32_t", 14},
     {"class", 15},
     {"compl", 16},
     {"concept", 17},
     {"const", 18},
     {"constexpr", 19},
     {"const_cast", 20},
     {"continue", 21},
     {"decltype", 22},
     {"default", 23},
     {"delete", 24},
     {"do", 25},
     {"double", 26},
     {"dynamic_cast", 27},
     {"else", 28},
     {"enum", 29},
     {"explicit", 30},
     {"export", 31},
     {"extern", 32},
     {"FALSE", 33},
     {"float", 34},
     {"for", 35},
     {"friend", 36},
     {"goto", 37},
     {"if", 38},
     {"inline", 39},
     {"int", 40},
     {"long", 41},
     {"mutable", 42},
     {"namespace", 43},
     {"new", 44},
     {"noexcept", 45},
     {"not", 46},
     {"not_eq", 47},
     {"nullptr", 48},
     {"operator", 49},
     {"or", 50},
     {"or_eq", 51},
     {"private", 52},
     {"protected", 53},
     {"public", 54},
     {"register", 55},
     {"reinterpret_cast", 56},
     {"requires", 57},
     {"return", 58},
     {"short", 59},
     {"signed", 60},
     {"sizeof", 61},
     {"static", 62},
     {"static_assert", 63},
     {"static_cast", 64},
     {"struct", 65},
     {"switch", 66},
     {"template", 67},
     {"this", 68},
     {"thread_local", 69},
     {"throw", 70},
     {"TRUE", 71},
     {"try", 72},
     {"typedef", 73},
     {"typeid", 74},
     {"typename", 75},
     {"union", 76},
     {"unsigned", 77},
     {"using", 78},
     {"virtual", 79},
     {"void", 80},
     {"volatile", 81},
     {"wchar_t", 82},
     {"while", 83},
     {"xor", 84},
     {"xor_eq", 85}});
const unordered_map<string, int> *OPERATOR_TABLE = new unordered_map<string, int>(
    {{"^", 0},
    {"&", 1},
    {"|", 2},
    {"~", 3},
    {"!", 4},
    {"<<", 5},
    {">>", 6},
    {"+", 7},
    {"-", 8},
    {"*", 9},
    {"/", 10},
    {"%", 11},
    {"=", 12},
    {"<", 13},
    {">", 14},
    {"+=", 15},
    {"-=", 16},
    {"*=", 17},
    {"/=", 18},
    {"%=", 19},
    {"^=", 20},
    {"&=", 21},
    {"|=", 22},
    {">>=", 23},
    {"<<=", 24},
    {"==", 25},
    {"!=", 26},
    {"<=", 27},
    {">=", 28},
    {"&&", 29},
    {"||", 30},
    {"++", 31},
    {"--", 32},
    {"->*", 33},
    {"->", 34},
    {"?", 35},
    {":", 36}});
const unordered_map<string, int> *DELIMITER_TABLE = new unordered_map<string, int>(
    {{"(", 0},
     {")", 1},
     {"[", 2},
     {"]", 3},
     {"{", 4},
     {"}", 5},
     {",", 6},
     {";", 7}});
const int LEX_DFA_error = -1;
const int LEX_DFA_languages = 1;
const int LEX_DFA_identifier1 = 2;
const int LEX_DFA_octs1hexs1 = 3;
const int LEX_DFA_octs2 = 4;
const int LEX_DFA_hexs2 = 5;
const int LEX_DFA_hexs3 = 6;
const int LEX_DFA_decs1floats1 = 7;
const int LEX_DFA_decs2 = 8;
const int LEX_DFA_decs3 = 9;
const int LEX_DFA_floats2 = 10;
const int LEX_DFA_floats3 = 11;
const int LEX_DFA_floats4 = 12;
const int LEX_DFA_floats5 = 13;
const int LEX_DFA_chars1 = 14;
const int LEX_DFA_chars2 = 15;
const int LEX_DFA_string1 = 16;
const int LEX_DFA_UnaryOperators6comments1 = 17;
const int LEX_DFA_commentInLine2 = 18;
const int LEX_DFA_commentCrossLine2 = 19;
const int LEX_DFA_commentCrossLine3 = 20;
const int LEX_DFA_UnaryOperators1 = 21;
const int LEX_DFA_UnaryOperators2 = 22;
const int LEX_DFA_UnaryOperators3 = 23;
const int LEX_DFA_UnaryOperators4 = 24;
const int LEX_DFA_UnaryOperators5 = 25;
const int LEX_DFA_UnaryOperators7 = 26;
const int LEX_DFA_UnaryOperators8 = 27;
const int LEX_DFA_UnaryOperators9 = 28;
const int LEX_DFA_UnaryOperators10 = 29;
const int LEX_DFA_UnaryOperators11 = 30;
const int LEX_DFA_UnaryOperators12 = 31;
const int LEX_DFA_UnaryOperators13 = 32;
const int LEX_DFA_UnaryOperators14 = 33;

class CompilationException : public exception
{
public:
    CompilationException(const int row, const int column, const string message);
    const int row = -1;
    const int column = -1;
    const string message = DEFAULT_ERROR_MESSAGE;
    const char* what() const noexcept;
};

class LexicalException : public exception
{

};

enum TokenType
{
    EmptyTokenType,
    IdentifierTokenType,
    KeywordTokenType,
    IntTokenType,
    FloatTokenType,
    CharTokenType,
    StringTokenType,
    UnaryOperatorTokenType,
    BinaryOperatorTokenType,
    TernaryOperatorTokenType,
    DelimiterTokenType
};
const string TOKEN_NAMES[] = {
    "EmptyToken",
    "IdentifierToken",
    "KeywordToken",
    "IntToken",
    "FloatToken",
    "CharToken",
    "StringToken",
    "UnaryOperatorToken",
    "BinaryOperatorToken",
    "TernaryOperatorToken",
    "DelimiterToken"};

union TokenValueUnion
{
    int int_value;
    double float_value;
    char char_value;
    string *string_value = nullptr; // union 内不可使用string
    TokenValueUnion(int int_value);
    TokenValueUnion(double float_value);
    TokenValueUnion(char char_value);
    TokenValueUnion(string *string_value);
};

using Token = pair<TokenType, TokenValueUnion>;

class TextReader
{
public:
    TextReader(const string &filename);
    ~TextReader();
    int start_index = 0;
    int end_index = 1;
    char get_next_char();
    void reset_current_string();
    string get_current_string() const;
    void retract();
private:
    string *_buffer = nullptr;
    ifstream source;
};

class Lexer
{
public:
    Lexer(queue<Token> &token_queue);
    ~Lexer();
    void prase(const string &filename);
private:
    int row = 0;
    int column = 0;
    int _state = LEX_DFA_languages;
    TextReader *reader = nullptr;
    unordered_map<string, int> *identifier_table;
    queue<Token> &_token_queue;
    void set_state(const int next_state);
    void retract();
    void raise_error();
    int get_state();
    int get_keyword_index(const string text);
    int get_identifier_index(const string text);
    int get_operator_index(const string text);
    int get_delimiter_index(const string text);
    void receive_token(const TokenType &type); // 将IdentifierTokenType视为关键字或标识符
    string dump_token(const Token &token) const;
    bool is_oct(const char next_char) const;
};

class LexerComsumer
{
public:
    LexerComsumer(queue<Token> &token_queue);
    void run();
private:
    const queue<Token> &_token_queue;
};
