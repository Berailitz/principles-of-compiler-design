#ifndef PRASER_H
#define PRASER_H

#include "token.h"

using PraserState = int; // 分析器的状态
using PraserReducePattern = pair<NodeType, int>; // 左侧非终结符，右侧候选式的长度
using PraserReducePatternList = vector<PraserReducePattern>; // 产生式列表

// 分析表中操作，仅转换状态（用于栈顶为非终结符的情况）、移进、归约和接受（结束）
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

class Praser
{
  public:
    PraserState state;
    Praser();
    void prase(const vector<Token&> tokens);
};

#endif
