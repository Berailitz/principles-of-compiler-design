#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <unordered_set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>

#pragma comment(lib, "user32")

using namespace std;

using symbol = string; // 字符，包含终结符与非终结符
using Terminal = symbol; // 终结符
using Candidate = vector<symbol>; // 候选式
using SymbolList = vector<symbol>; // 表示若干符号组成的字符串
using SymbolSet = unordered_set<symbol>; // 符号集，用于消除左递归
using PrefixMap = unordered_multimap<symbol, int>; // 前缀表，用于消除公共左因子
using CandidateList = vector<Candidate>; // 候选式列表，即产生式的左部
using TerminalSet = unordered_set<Terminal>; // 终结符集，用于表示FIRST集和FOLLOW集等
using AnalyseTable = unordered_map<Terminal, int>; // 分析表中的行
using SymbolStack = vector<Terminal>; // 符号栈，用户进行LL(1)分析

const string PRODUCTION_MARK = "->"; // 产生式中的“定义为”符号
const symbol EMPTY_MARK = "@"; // 表示空产生式
const symbol END_MARK = "$";
const symbol SYNCH_MARK = "synch"; // 表示栈底或字符串尾
const int SYNCH_MARK_INDEX = -1; // 同步符号对应的候选式序号
const string CANDIDATE_DELIMITER = "|"; // 分隔符，用于分隔各个候选式

// 非终结符类
class Nonterminal
{
  public:
    Nonterminal(string name);
    Nonterminal(const Nonterminal *old_nonterminal);
    Nonterminal(const Nonterminal &old_nonterminal);
    ~Nonterminal();
    symbol name;
    CandidateList *candidates = nullptr;
    TerminalSet *firsts = nullptr;
    TerminalSet *follows = nullptr;
    AnalyseTable *table = nullptr;
    void initialize(const Nonterminal *old_nonterminal);
};

// 文法规则类，代表一个产生式
class Rule
{
  public:
    Rule();
    Rule(const Rule *rule);
    Rule(const Rule &rule);
    ~Rule();
    symbol nonterminal;
    CandidateList *candidates = nullptr;
};

using NonterminalTable = unordered_map<symbol, Nonterminal>; // 非终结符表

class Analyser
{
  public:
    Analyser();
    TerminalSet *terminals = nullptr;
    NonterminalTable *nonterminals = nullptr;
    symbol start_symbol;
    void raise_error(string message); // 错误回调函数
    void clear_grammar(); // 初始化分析器，清楚预测分析表
    void create_grammar(istream &stream); // 从输入流构建文法规则和分析表
    void receive_grammar(istream &stream); // 从输入流构建文法规则
    void print_grammar() const; // 输出文法规则
    Rule *build_rule(string grammar_text); // 从输入字符串建立文法规则对象
    void add_rule(Rule &new_rule); // 从文法规则对象建立对应的文法
    bool is_nonterminal(symbol word) const; // 检查一个符号是否为终结符
    TerminalSet get_firsts(symbol word) const; // 计算某个符号的FIRST集
    TerminalSet get_firsts(Candidate &candidate) const; // 计算某个字符串的FIRST集
    void calculate_firsts(); // 计算所有符号的FIRST集
    void add_candidates(CandidateList &candidates, Candidate prefix, Candidate postfix);
    // 考虑空产生式，将由prefix前缀、postfix后缀组成的候选式假如candidates候选式列表中
    void eliminate_empty(); // 消除空产生式
    bool eliminate_direct_recursion(Nonterminal &nonterminal);
    // 消除一个终结符对应的产生式中的直接左递归
    void eliminate_recursion(); // 消除直接和间接左递归
    void eliminate_common_prefix(); // 提取左公因子
    void calculate_follows(); // 计算所有FOLLOW集
    bool build_table(bool no_error = true); // 构建预测分析表
    void print_table() const; // 输出预测分析表
    void print_firsts() const; // 输出FIRST集
    void print_follows() const; // 输出FOLLOW集
    void receive_text(istream &stream); // 从输入流分析句子
    void analyse(string code_text); // 分析一个句子
};

SymbolList *string_to_vector(const string &raw_string); // 将字符串转换为内部表示的符号列表
bool merge_set(TerminalSet &destination, const TerminalSet &source);
// 取并集并保存至destination集合，若destination集合增大了，返回真

template <class T>
string container_to_string(T &container, string separator = " ", const int start_index = 0);
// 将一个内部表示的容器（符号列表等）转化为字符串以便输出

template <class T>
string reversed_container_to_string(T &container, string separator = " ", const int start_index = 0);
// 将一个内部表示的容器（符号栈等）转化为反向的字符串以便输出
