输入
====

1. 终结符、非终结符均为不包含空格的字符串
2. 各终结符、非终结间用空格分隔
3. 产生式中用`->`表示推导
2. 产生式间使用换行符分隔
2. 候选式间使用`|`分隔
3. 依次输入非终结符、终结符、文法产生式和开始符号

输出
====

1. 预测分析表

数据结构
====

<!-- 3. 字符类型: `enum WordType`
   1. `TerminalType`
   1. `NonterminalType` -->
1. 终结符: `Terminal`
   1. `using Terminal = string`
2. 候选式: `Candidate`
   1. `using Candidate = list<Terminal>`
2. 非终结符: `Nonterminal`
   1. `Terminal name`
   1. `vector<Candidate> *candidates = nullptr`: 候选式表
   1. `unordered_set<Terminal> *firsts = nullptr`: `FIRST`集
   1. `unordered_set<Terminal> *follows = nullptr`: `FOLLOW`集
   2. `unordered_map<Terminal, int> *table = nullptr`: 分析表
2. 生成式: `Rule`
   1. `string nonterminal`
   1. `list<Candidate> *candidates = nullptr`
   2. `void print()`
4. 语法分析器: `Analyser`
   1. `unordered_set<Terminal> *terminals = nullptr`: 终结符表
   1. `unordered_map<Terminal, Nonterminal> *nonterminals = nullptr`: 非终结符表
   2. `string code_text`
   2. `stack<Terminal> symbol_stack`
   1. `void get_input()`: 提示输入文法，主函数
   2. `Rule *build_rule(string grammar_text)`: 从产生式文本建立语法规则对象
   <!-- 5. `void add_terminal(Terminal word)`
   5. `void add_nonterminal(Terminal word)` -->
   2. `void add_rule(Rule &new_rule)`: 插入产生式对应的语法规则
   3. `void build_table()`: 建立分析表
   3. `void print_table()`: 输出预测分析表
   4. `void analyse(string code_text)`: 分析文法并输出分析过程

说明
====

1. 使用`@`表示集合中的空字符
1. 使用`$`表示集合中的结束符
2. 使用空候选式表示空产生式，如`A -> | a`
3. 假设输入的一定是`LL(1)`文法，即不含左递归和公共左因子
