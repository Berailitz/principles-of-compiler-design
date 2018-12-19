#include <deque>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace std;

enum ActionType
{
    ErrorActionType = -1,
    AcceptActionType,
    ShiftActionType,
    ReduceActionType,
    GotoActionType,
};

using DFAState = int;
using symbol = string;                                          // 字符，包含终结符与非终结符
const symbol END_MARK = "$";                                    // 表示终止符
const symbol EMPTY_SYMBOL = "-";                                // 符号栈占位符
using SymbolList = deque<symbol>;                               // 表示若干符号组成的字符串
using SymbolStack = vector<symbol>;                             // 表示符号栈
using StateStack = vector<DFAState>;                            // 表示状态栈
using ValStack = vector<int>;                                   // 表示属性栈
const int EMPTY_VAL = 0;                                        // 属性栈占位符
using AnalyseAction = pair<ActionType, int>;                    // 表示分析表中的一个动作
using AnalyseTableLine = unordered_map<symbol, AnalyseAction>;  // 表示分析表中的一行
using AnalyseTable = unordered_map<DFAState, AnalyseTableLine>; // 表示分析表
using Rule = pair<symbol, int>;                                 // 表示一个推到规则，即（左部）非终结符和（右部）产生式的长度
using RuleList = vector<Rule>;                                  // 表示所有产生式

const AnalyseAction defaultAction = {ErrorActionType, -1};

SymbolList *string_to_vector(const string &raw_string); // 将字符串转换为内部表示的符号列表

int tryInt(const symbol word); // 若word为一个非负整数，返回该非负整数，否则返回-1

class Calculator
{
  public:
    Calculator();
    ~Calculator();
    AnalyseAction SearchAnalyseTable(const DFAState state, const symbol &key);
    // 查分析表，若为空，则返回默认值defaultAction
    bool performAction(const AnalyseAction &action);
    // 若分析完成，返回true，否则返回false，若分析出错，产生异常
    bool calculate(const string stringText);
    void receive_text(istream &stream);

  private:
    AnalyseTable *table = new AnalyseTable();
    RuleList *ruleList = new RuleList();
    SymbolStack symbolStack;
    StateStack stateStack;
    ValStack valStack;
    SymbolList *words = nullptr;
    void buildAnalyseTable();
    void buildRuleList();
};

template <class T>
string container_to_string(T &container, string separator = " ", const int start_index = 0);
// 将一个内部表示的容器（符号列表等）转化为字符串以便输出
