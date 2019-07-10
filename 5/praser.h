#ifndef PRASER_H
#define PRASER_H

#include <unordered_set>
#include "token.h"

using PraserState = int; // 分析器的状态
using Candidate = vector<NodeType>;
using PraserRule = pair<NodeType, Candidate>; // 左侧非终结符，右侧候选式的长度
using PraserRuleList = vector<PraserRule>; // 产生式列表

// 分析表中操作，移进（含用于栈顶为非终结符的情况）、归约和接受（结束）
enum StackAction
{
    EmptyStackAction,
    ShiftStackAction,
    ReduceStackAction,
    AcceptStackAction
};
using StackActionArg = int; // ReduceStackAction对应的产生式的序号，或ShiftStackAction对应的下一个状态的序号
using PraserAction = pair<StackAction, StackActionArg>; // 分析表中的项
using PraserActionTable = unordered_map<pair<PraserState, NodeType>, PraserAction>; // 分析表
using PraserStack = vector<pair<int, NodeType>>; // 状态栈和符号栈

using NodeSet = unordered_set<NodeType>;
using DFAPart = pair<int, pair<int, NodeType>>; // 产生式的序号、点的序号和前缀
using DFAState = vector<DFAPart>;
using DFAStateSet = vector<DFAState>;

class Praser
{
  public:
    PraserActionTable &table = *new PraserActionTable;
    PraserRuleList &rules = *new PraserRuleList;
    DFAStateSet &set = *new DFAStateSet;
    Praser();
    Node *prase(const TokenList &tokens) const;
    void build_table_and_rules();
    void build_parser_rules(PraserRuleList &rules);
    void make_state(DFAState &state) const; // 使用非终结符make之
    bool grow_state(DFAState &state); // 要求本状态为新状态，检查各个产生式，make之，若不has，插入set，插入table，grow之
    int find_state(const DFAState new_state) const; // -1 for none
    int find_part(const DFAState state, const DFAPart part) const; // -1 for none
    NodeSet get_first(const Candidate nodes) const; // 求FIRST集
    void build_parser_table(PraserActionTable &table, PraserRuleList &rules);
  };

NodeType token_type_to_node_type(const TokenType token_type);

#endif
