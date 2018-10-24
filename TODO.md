程序架构
======

- 主程序、主线程
- 词法分析线程（生产者）
- 消费者、输出

数据结构
====

符号说明
---
1. `[a-bc-de...]`: 序列中`a`至`b`、`c`至`d`或`e`中的任意字符
1. `[^a-bc-de...]`: 不在序列中`a`至`b`、`c`至`d`或`e`中的任意字符
2. `languages`: 所有合法字符串
3. `ints`: 一个无符号整数
4. `octs`: 8进制无符号整数
5. `oct = [0-7]`: 0 至 7 的一位阿拉伯数字
6. `decs`: 10进制无符号整数
<!-- 6. `decs_no_e`: 10进制无符号、没有使用科学记数法表示的整数 -->
7. `dec = [0-9]`: 0 至 9 的一位阿拉伯数字
8. `dec1 = [1-9]`: 1 至 9 的一位阿拉伯数字
9. `hexs`: 16进制无符号整数
10. `hex = [1-9a-eA-E]`: 0 至 9 的一位阿拉伯数字，或`A`至`E`的一位大小写字母
11. `floats`: 10进制无符号浮点数
12. `float_no_e`: 10进制无符号、没有使用科学记数法表示的浮点数
13. `chars`: 字符型
14. `char`: 一位任意字符
15. `charNoSq = [^']`: 一位非单引号的字符
16. `charInString = [^"\n]`: 一位非双引号、也非换行符的字符
17. `charNoBl = [^\n]`: 一位非换行符的字符，可出现在单行注释中
18. `charNoStar = [^*]`: 一位非星号的字符，用于跨行注释的识别
19. `charNoBs = [^/]`: 一位非反斜杠的字符，用于跨行注释的识别
20. `letter`: 一位大小写字母字符
21. `strings`: 字符串型
22. `keyword`: 关键字
23. `identifier`: 标识符
24. 注：为便于识别，必要时，我们使用空格分隔正则表达式中的各个符号

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

1. `EmptyTokenType`, 空记号
1. `IdentifierTokenType`, 标识符, ``
1. `KeywordTokenType`, 关键字
1. `IntTokenType`, 整型
1. `FloatTokenType`, 浮点型
1. `CharTokenType`, 字符型
1. `StringTokenType`, 字符串
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

- `pair<TokenType, TokenValueUnion>`

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

识别逻辑
===

来源
---

1. 在编译器`Clang++`上进行的测试（版本号：`3.9.1`）
1. 互联网（`cppreference.com`等）

关键字和标识符
---

1. 考虑到关键字和标识符均服从正则表达式$letter (letter|ddec|\_)^*$，我们先将其视为同一类型的符号，识别出再进行分类
2. 其文法为:
$$ identifier \rightarrow letter \ identifier1 $$
$$ identifier1 \rightarrow \varepsilon | letter \ identifier1 | dec \ identifier1 $$

整数
---

1. 我们分别识别`8`进制、`10`进制和`16`进制的整数
2. `8`进制整数：以`0`开始，由`0`至`7`的整数组成，且长度不小于`2`，即
   $$ octs = 0 \ {oct}^+ $$
   其文法为
   $$ octs \rightarrow 0 \ octs1 $$
   $$ octs1 \rightarrow oct \ octs2 $$
   $$ octs2 \rightarrow \varepsilon | oct \ octs2 $$
3. `10`进制整数：若不以科学记数法表示，以$dec1$开始，由$dec$组成；若以科学记数法表示，则在不以科学记数法表示的整数的基础上附加大小写英语字母$e$，正负号，和任意数字，即加上$(E|e)(+|-)?dec+$。综上所述，其正则式为
   $$ decs = dec1 \ dec+((E|e)(+|-)?dec+)? $$
   其文法为
   $$ decs \rightarrow dec1 \ decs1  $$
   $$ decs1 \rightarrow \varepsilon | dec \ decs1 | E \ decs2 | e \ decs2 $$
   $$ decs2 \rightarrow + \ decs3 | - \ decs3 | dec \ decs3  $$
   $$ decs3 \rightarrow \varepsilon | dec \ decs3  $$
4. `16`进制整数：以`0x`开始，由`hex`组成，且长度不小于`3`，即
   $$ hexs = 0 \ x  \ {hex}^+ $$
   其文法为
   $$ hexs \rightarrow 0 \ hexs1 $$
   $$ hexs1 \rightarrow x \ hexs2 $$
   $$ hexs2 \rightarrow hex \ hexs3 $$
   $$ hexs3 \rightarrow \varepsilon | hex \ hexs3 $$
5. 综上所述，整数的文法为
   $$ ints \rightarrow 0 \ octs1hexs1 | dec1 \ decs1 $$
   $$ octs1hexs1 \rightarrow oct \ octs2 | x \ hexs2 $$
   $$ octs2 \rightarrow \varepsilon | oct \ octs2 $$
   $$ hexs2 \rightarrow hex \ hexs3 $$
   $$ hexs3 \rightarrow \varepsilon | hex \ hexs3 $$
   $$ decs1 \rightarrow \varepsilon | dec \ decs1 | E \ decs2 | e \ decs2 $$
   $$ decs2 \rightarrow + \ decs3 | - \ decs3 | dec \ decs3  $$
   $$ decs3 \rightarrow \varepsilon | dec \ decs3  $$
6. 依据上述文法识别到整型后，我们通过`atoi`函数将其转换为整数

浮点数
---

1. 对于非科学记数法表示的浮点数，其小数点前、后的部分可分别视为一个`10`进制整数；对于科学记数法表示的浮点数，其指数部分可以视为一个`10`进制有符号或无符号整数。故其正则式为
   $$ floats = decs.{dec}^+(e|E)(+|-)dec $$
   其文法为
   $$ floats \rightarrow dec1 \ floats1 $$
   $$ floats1 \rightarrow dec \ floats1 | . \ floats2 $$
   $$ floats2 \rightarrow dec \ floats3 $$
   $$ floats3 \rightarrow \varepsilon | dec \ floats3 | e \ floats4 | E \ floats4 $$
   $$ floats4 \rightarrow dec \ floats5 $$
   $$ floats5 \rightarrow \varepsilon | dec \ floats5 $$
6. 依据上述文法识别到浮点型后，我们通过`atof`函数将其转换为浮点数

（单个）字符
---

1. 由单引号、非单引号字符、单引号组成，其正则式为
   $$ ` \ charNoSq \ ' $$
   其文法为
   $$ chars \rightarrow ' \ chars1 $$
   $$ chars1 \rightarrow charNoSq \ chars2 $$
   $$ chars2 \rightarrow ' $$

字符串
---

1. 由双引号、非双引号也非换行符的字符、双引号组成，其正则式为
   $$ '' \ {charInString}^* \ '' $$
   其文法为
   $$ strings \rightarrow '' \ string1 $$
   $$ string1 \rightarrow '' | charInString \ string1 $$

运算符
---

1. 我们借用语法分析中的首符号集的概念，对各个运算符进行分析，得到如下文法
   $$ UnaryOperators \rightarrow : | ? | \sim | - \ UnaryOperators1 | ! \ UnaryOperators2 | \% \ UnaryOperators3 | \& \ UnaryOperators4 | * \ UnaryOperators5 | / | UnaryOperators6 | \wedge \ UnaryOperators7 | \ | \ UnaryOperators8 | + \ UnaryOperators9 | < \ UnaryOperators10 | = \ UnaryOperators11 | > \ UnaryOperators12 $$
   $$ UnaryOperators1 \rightarrow \varepsilon | - | = $$
   $$ UnaryOperators2 \rightarrow \varepsilon | = $$
   $$ UnaryOperators3 \rightarrow \varepsilon | = $$
   $$ UnaryOperators4 \rightarrow \varepsilon | \& | = $$
   $$ UnaryOperators5 \rightarrow \varepsilon | = $$
   $$ UnaryOperators6 \rightarrow \varepsilon | = $$
   $$ UnaryOperators7 \rightarrow \varepsilon | = $$
   $$ UnaryOperators8 \rightarrow \varepsilon | \ | | \ = $$
   $$ UnaryOperators9 \rightarrow \varepsilon | + | = $$
   $$ UnaryOperators10 \rightarrow \varepsilon | = | < UnaryOperators13 $$
   $$ UnaryOperators11 \rightarrow \varepsilon | = $$
   $$ UnaryOperators12 \rightarrow \varepsilon | = | > UnaryOperators14 $$
   $$ UnaryOperators13 \rightarrow \varepsilon | = $$
   $$ UnaryOperators14 \rightarrow \varepsilon | = $$

界符
---

1. 分析可知每个界符（终结符）均仅在一个非终结符的首符号集中，故凡遇到终结符界符，即可认定遇到了界符符号，其文法为
   $$ delimiters \rightarrow (  \ | \  )  \ | \  [  \ | \  ]  \ | \  \{  \ | \  \}  \ | \  ,  \ | \  ; $$

注释
---

1. 单行注释: 由`//`开始，遇到换行符，其正则式为
   $$ / \ / \ {char}^* \ \backslash n $$
   其文法为
   $$ commentInLine \rightarrow / \ commentInLine1 $$
   $$ commentInLine1 \rightarrow / \ commentInLine2 $$
   $$ commentInLine2 \rightarrow \backslash n | charNoBl \ commentInLine2 $$
2. 跨行注释: 由`/*`开始，遇到`*/`结束，其正则式为
   $$ /*({charNoStar}^*|*{charNoBs})^**/ $$
   其文法为
   $$ commentCrossLine \rightarrow / \ commentCrossLine1 $$
   $$ commentCrossLine1 \rightarrow * \ commentCrossLine2 $$
   $$ commentCrossLine2 \rightarrow * \ commentCrossLine3 | charNoStar \ commentCrossLine2 $$
   $$ commentCrossLine3 \rightarrow / | charNoBs \ commentCrossLine2 $$
3. 综上所述，注释的文法为
   $$ comments \rightarrow / \ comments1 $$
   $$ comments1 \rightarrow / \ commentInLine2 |* \ commentCrossLine2  $$
   $$ commentInLine2 \rightarrow \backslash n | charNoBl \ commentInLine2 $$
   $$ commentCrossLine2 \rightarrow * \ commentCrossLine3 | charNoStar \ commentCrossLine2 $$
   $$ commentCrossLine3 \rightarrow / | charNoBs \ commentCrossLine2 $$

总结
---

1. 我们合并上述各词法可得
   $$ languages \rightarrow 0 \ octs1hexs1 | dec1 \ decs1floats1 | ' \ chars1 | '' \ string1 | - \ UnaryOperators1 | ! \ UnaryOperators2 | \% \ UnaryOperators3 | \& \ UnaryOperators4 | * \ UnaryOperators5 | / \ UnaryOperators6comments1 | \wedge \ UnaryOperators7 | \ | \ UnaryOperators8 | + \ UnaryOperators9 | < \ UnaryOperators10 | = \ UnaryOperators11 | > \ UnaryOperators12 | letter \ identifier1 | : | ? | \sim | (  \ | \  )  \ | \  [  \ | \  ]  \ | \  \{  \ | \  \}  \ | \  ,  \ | \  ; $$
   $$ identifier1 \rightarrow \varepsilon | letter \ identifier1 | dec \ identifier1 $$
   $$ octs1hexs1 \rightarrow oct \ octs2 | x \ hexs2 $$
   $$ octs2 \rightarrow \varepsilon | oct \ octs2 $$
   $$ hexs2 \rightarrow hex \ hexs3 $$
   $$ hexs3 \rightarrow \varepsilon | hex \ hexs3 $$
   $$ decs1floats1 \rightarrow \varepsilon | dec \ decs1floats1 | E \ decs2 | e \ decs2 | . \ floats2 $$
   $$ decs2 \rightarrow + \ decs3 | - \ decs3 | dec \ decs3  $$
   $$ decs3 \rightarrow \varepsilon | dec \ decs3  $$
   $$ floats2 \rightarrow dec \ floats3 $$
   $$ floats3 \rightarrow \varepsilon | dec \ floats3 | e \ floats4 | E \ floats4 $$
   $$ floats4 \rightarrow dec \ floats5 $$
   $$ floats5 \rightarrow \varepsilon | dec \ floats5 $$
   $$ chars1 \rightarrow charNoSq \ chars2 $$
   $$ chars2 \rightarrow '' $$
   $$ string1 \rightarrow '' | charInString \ string1 $$
   $$ UnaryOperators6comments1 \rightarrow \varepsilon | = | / \ commentInLine2 |* \ commentCrossLine2  $$
   $$ commentInLine2 \rightarrow \backslash n | charNoBl \ commentInLine2 $$
   $$ commentCrossLine2 \rightarrow * \ commentCrossLine3 | charNoStar \ commentCrossLine2 $$
   $$ commentCrossLine3 \rightarrow / | charNoBs \ commentCrossLine2 $$
   $$ UnaryOperators1 \rightarrow \varepsilon | - | = $$
   $$ UnaryOperators2 \rightarrow \varepsilon | = $$
   $$ UnaryOperators3 \rightarrow \varepsilon | = $$
   $$ UnaryOperators4 \rightarrow \varepsilon | \& | = $$
   $$ UnaryOperators5 \rightarrow \varepsilon | = $$
   $$ UnaryOperators7 \rightarrow \varepsilon | = $$
   $$ UnaryOperators8 \rightarrow \varepsilon | \ | | \ = $$
   $$ UnaryOperators9 \rightarrow \varepsilon | + | = $$
   $$ UnaryOperators10 \rightarrow \varepsilon | = | < UnaryOperators13 $$
   $$ UnaryOperators11 \rightarrow \varepsilon | = $$
   $$ UnaryOperators12 \rightarrow \varepsilon | = | > UnaryOperators14 $$
   $$ UnaryOperators13 \rightarrow \varepsilon | = $$
   $$ UnaryOperators14 \rightarrow \varepsilon | = $$

运行流程
====

主线程
-----

1. 启动其他线程
2. `join`子线程并等待

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
2. 暂不考虑运算符`sizeof`, `sizeof...`
3. 将`=`, `-=`, `+=`, `*=`, `/=`, `%=`, `^=`, `&=`, `|=`, `<<`, `>>`, `>>=`, `<<=`, `?`, `:`视为运算符
4. 暂不考虑`,`作为运算符时，先后计算其左值和右值，并返回右值的特性
5. `IntTokenType`, `FloatTokenType`均为无符号数，故将`(+|-)(digit|float)`中的`+`, `-`视为运算符
