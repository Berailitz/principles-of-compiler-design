#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

using symbol = string;
using Terminal = symbol;
using Candidate = vector<symbol>;
using CandidateList = vector<Candidate>;
using TerminalSet = set<Terminal>;
using AnalyseTable = unordered_map<Terminal, int>;
using SymbolStack = stack<Terminal>;

const string PRODUCTION_MARK = "->";
const symbol EMPTY_MARK = "@";
const symbol END_MARK = "$";
const string CANDIDATE_DELIMITER = "|";

class Nonterminal
{
public:
    Nonterminal();
    symbol name;
    CandidateList *candidates = nullptr;
    TerminalSet *firsts = nullptr;
    TerminalSet *follows = nullptr;
    AnalyseTable *table = nullptr;
};

class Rule
{
public:
    Rule();
    symbol nonterminal;
    CandidateList *candidates = nullptr;
};

using NonterminalTable = unordered_map<symbol, Nonterminal>;

class Analyser
{
public:
    Analyser();
    TerminalSet *terminals = nullptr;
    NonterminalTable *nonterminals = nullptr;
    symbol start_symbol;
    void get_input();
    Rule *build_rule(string grammar_text);
    void add_rule(Rule &new_rule);
    bool is_nonterminal(symbol word) const;
    TerminalSet get_firsts(symbol word) const;
    TerminalSet get_firsts(Candidate &candidate) const;
    void calculate_firsts();
    void calculate_follows();
    void build_table();
    void print_table() const;
    void analyse(string code_text);
};

bool merge_set(TerminalSet &destination, const TerminalSet &source);
// return true if destination updates
