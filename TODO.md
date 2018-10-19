程序架构
======

- 主程序、主线程
- 词法分析线程（生产者）
- 消费者、输出

数据结构
====

常量
----

1. `const int MAX_IDENTIFIER_LENGTH = 1000`
1. `const int MAX_TOKEN_QUEUE_SIZE = 10`
1. `const char LINE_DELIMITER = '\n'`
1. `const string DEFAULT_ERROR_MESSAGE = "Error occored."`
1. `const unordered_map<string, int> *KEYWORD_TABLE`, 关键字表，堆区

错误类: `CompilationException`
-----

- `int row = -1`
- `int column = -1`
- `string message`
- `const char* what() const throw()`

错误类: `LexicalException`
----

- 表示词法错误

记号类型: `enum TokenType`
----

1. `IdentifierTokenType`
1. `KeywordTokenType`
1. `IntTokenType`
1. `FloatTokenType`
1. `CharTokenType`
1. `StringTokenType`
1. `UnaryOperatorTokenType`, 一元运算符
1. `BinaryOperatorTokenType`, 二元运算符
1. `TernaryOperatorTokenType`, 三元运算符
1. `DelimiterTokenType`, 界符

常值联合: `union TokenValueUnion`
----

1. `int int_value`, 纯整数、符号表的序号、关键字的序号、运算符的序号
1. `float float_value`, 纯小数
1. `char char_value`, 字符
1. `string &string_value`, 字符

记号类: `Token`
---

- `Token()`
- `Token(TokenType type, TokenValueUnion value)`
- `Token(const Token &oldToken)`
- `Token(const Token *oldToken)`
- `TokenType _type`
- `TokenValueUnion _value`, 即 union-with-class
- `void initialize(const Token *oldToken = nullptr)`
- `TokenValueUnion get_value() const`
- `TokenType get_type() const`
- `string to_string() const`

记号队列: `queue<Token> token_queue`
----

- 记号类组成的队列
- 位于堆区

词法分析器类: `Lexer`
----

- `Lexer(queue<Token> &token_queue)`
- `string *_buffer`, 缓冲区，堆区
- `int row = 0`
- `int column = 0`
- `unordered_map<string, int> *identifier_table`, 符号表，堆区
- `string::iterator _start`
- `string::iterator _end`
- `queue<Token> *_token_queue`
- `istream source`
- `void initialize()`
- `void prase()`
- `bool open(const string filename)`
- `char get_next_char(const int length = 1)`, 获取下一个非空字符
- `void retract(const int length = 1)`, 回退`length`个字符
- `int get_keyword_index(const string text)`, 不是则返回`-1`
- `int get_identifier_index(const string text)`

词法分析消费者类: `LexerComsumer`
----

- `LexerComsumer(queue<Token> &token_queue)`
- `const queue<Token> &_token_queue`
- `void run()`

运行流程
====

主线程
-----

1. 启动其他线程
1. `join`并等待

词法分析
------

1. 读取字符至缓冲区
1. 若记号队列未满，识别记号并放入记号队列，必要时回退
1. 如遇错误，就地初始化至状态`0`并输出

消费者
-----

1. 依次从记号队列中取出记号
1. 输出

备注
====

1. 假设字符均合法
1. 将所有`(`, `)`, `[`, `]`, `{`, `}`, `,`视为界符，即
    1. 将`[index]`中的括号视为界符，数字视为常数、标识符或表达式
    1. 将`(type)`中的括号视为界符，类型视为关键字或标识符
    1. 将`{value1, value2}`中的`,`视为界符
1. 将`static_cast`, `dynamic_cast`, `const_cast`, `reinterpret_cast`, `new`, `delete`, `typeid`, `noexcept`, `alignof`视为关键字
1. 将`sizeof`, `sizeof...`, `=`, `-=`, `+=`, `*=`, `/=`, `%=`, `^=`, `&=`, `|=`, `<<`, `>>`, `>>=`, `<<=`, `?`, `:`视为运算符
1. 暂不考虑`,`作为运算符时，先后计算其左值和右值，并返回右值的特性
1. `IntTokenType`, `FloatTokenType`均为带符号数，故将`(+|-)(digit|float)`中的`+`, `-`视为常数的一部分，因此将`+`, `-`视为二元运算符
