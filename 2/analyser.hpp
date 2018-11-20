#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

using symbol = string;
using Terminal = symbol;
using Candidate = vector<symbol>;
using SymbolList = vector<symbol>;
using CandidateList = vector<Candidate>;
using TerminalSet = set<Terminal>;
using AnalyseTable = unordered_map<Terminal, int>;
using SymbolStack = vector<Terminal>;

const string PRODUCTION_MARK = "->";
const symbol EMPTY_MARK = "@";
const symbol END_MARK = "$";
const symbol SYNCH_MARK = "synch";
const int SYNCH_MARK_INDEX = -1;
const string CANDIDATE_DELIMITER = "|";
const string COLUMN_DELIMITER = "\t";

class Nonterminal
{
public:
    Nonterminal();
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

using NonterminalTable = unordered_map<symbol, Nonterminal>;

class Analyser
{
public:
    Analyser();
    TerminalSet *terminals = nullptr;
    NonterminalTable *nonterminals = nullptr;
    symbol start_symbol;
    void create_grammar(istream &stream);
    Rule *build_rule(string grammar_text);
    void add_rule(Rule &new_rule);
    bool is_nonterminal(symbol word) const;
    TerminalSet get_firsts(symbol word) const;
    TerminalSet get_firsts(Candidate &candidate) const;
    void calculate_firsts();
    void calculate_follows();
    void build_table();
    void print_table() const;
    void receive_text(istream &stream);
    void analyse(string code_text);
};

SymbolList *string_to_vector(const string &raw_string);
bool merge_set(TerminalSet &destination, const TerminalSet &source);
// return true if destination updates

template<class T>
string container_to_string(T &container, string separator = " ", const int start_index = 0);
