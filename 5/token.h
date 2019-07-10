#ifndef TOKEN_H
#define TOKEN_H

#include <algorithm>
#include <cctype>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum TokenType
{
    EmptyToken, // 空类型
    ProgramToken, // 程序记号
    LeftRoundBracketToken, // 左圆括号记号
    RightRoundBracketToken, // 右圆括号记号
    CommaToken, // 逗号记号
    IdentifierToken, // 标识符
    SemicolonToken, // 分号记号
    ProgramEndToken, // 程序结束记号
    ConstDecalrationToken, // 常量声明记号
    ConstAssignToken, // 常量赋值记号
    VarDecalrationToken, // 变量声明记号
    VarTypeToken,//变量类型记号
    ColonToken, // 冒号记号
    ProcedureToken, // 过程记号
    FunctionToken, // 函数记号
    BeginToken, // 块开始记号
    EndToken, // 块结束记号
    MinusToken, // 负号运算符
    ArrayToken, // 数组记号
    LeftSquareBracketToken, // 左方括号记号
    RightSquareBracketToken, // 右方括号记号
    ArrayRangeDelimiterToken, // 数组范围限制分隔
    OfToken, // OF记号
    IntToken, // 整数
    RealToken, // 实数
    BooleanToken, // 布尔值
    CharToken, // 字符值
    AssignToken, // 赋值运算符
    IfToken, // IF记号
    ThenToken, // Then记号
    ElseToken, // Else记号
    ForToken, // For记号
    ToToken, // To记号
    DoToken, // Do记号
    ReadToken, // Read记号
    WriteToken, // Write记号
    RelopToken, // 关系运算符
    OpToken,//算术逻辑运算符
    NotOperatorToken // 取反运算符
};

enum RelopIndex
{
    GraterThan,
    EqualTo,
    LessThan,
    NotGraterThan,
    NotLessThan,
    NotEqualTo,
};
enum OpIndex
{
    AddOp,
    OrOp,
    TimeOp,
    DivOp,
    DivWordOp,
    ModOp,
    AndOp
};
enum VarType
{
    IntVarType,
    RealVarType,
    BooleanVarType,
    CharVarType
};
const string TOKEN_TEXT_DELIMITER = " ";

class Token
{
  public:
    TokenType type = EmptyToken;
    int int_value = 0; // 同一类下各不同记号的序号
    string string_value = ""; // 标识符的ID（名字），int、float、boolean、char的值
    int line = -1; // 行号、
    int column = -1; // 列号
    Token();
    Token(const string &string_value, const int line = -1, const int column = -1);
    Token(const string text, const string flag);
    operator string() const;
};
using TokenList = vector<Token>;

// 描述基本的变量类型和数组元素类型
enum BasicVariantType
{
    EmptyVariant = 0,
    IntVariant = 4,
    RealVariant = 8,
    CharVariant = 2,
    BooleanVariant = 1
};

using ArrayRange = vector<pair<int, int>>; // 表示数组各维维度上下界

// 描述所有变量的类型
class VariantType
{
public:
    BasicVariantType type = EmptyVariant;
    ArrayRange *scope = nullptr; // 若不为空指针，则为数组，存储数组维度上下界
    bool is_constant = false;
    VariantType(const BasicVariantType type, const bool is_constant = false, const ArrayRange *scope = nullptr);
    VariantType(const VariantType &old_variantType);
};

// 描述所有函数、参数的类型
class ParameterType : public VariantType
{
public:
    bool is_referfence = false; // 记录是否为传引用
    ParameterType(const VariantType &old_variantType, const bool is_referfence = false);
};

using IDList = vector<string>;
using ParameterTypeList = vector<pair<string, ParameterType>>; // 参数列表，ID和类型
using SimpleParameterTypeList = pair<IDList, VariantType>; // 同类型参数列表，对应parameter节点

// 表示一个符号
class Symbol
{
public:
    VariantType *type = nullptr; // 若不为空指针，则为函数，存储函数的返回值；若为变量，则是变量的type
    ParameterTypeList *parameters = nullptr; // 若不为空指针，则为函数或过程，存储其形参列表；通过检查vector的长度来判断是否是函数标识符
    Symbol(const VariantType *type = nullptr, const ParameterTypeList *parameters = nullptr);
    Symbol(const Symbol &old_symbol);
};

// 存储作用域内的符号信息
class SymbolTable
{
public:
    SymbolTable *parent = nullptr; // 指向上一级符号表，主符号表中为空
    Symbol *source = nullptr; // 指向本级符号表对应的符号，如函数符号，以便查询返回值类型等，主符号表中为空
    unordered_map<string, Symbol> &Symbols; // 符号表的内容
    SymbolTable(const SymbolTable *parent = nullptr, const Symbol *source = nullptr);
    bool insert(const string &key, const Symbol &value); // 若存在，返回假，并报错
    Symbol &find(const string &key) const;
};

class NodeInfo
{
public:
    VariantType returnType;
    SymbolTable *table = nullptr;
    ArrayRange *scope = nullptr;//数组范围 ，顺便判断是不是数组
};

enum NodeType
{
    // 以下为终结符
    EmptyNode, // 空类型
    ProgramNode, // 程序节点
    LeftRoundBracketNode, // 左圆括号节点
    RightRoundBracketNode, // 右圆括号节点
    CommaNode, // 逗号节点
    IdentifierNode, // 标识符
    SemicolonNode, // 分号节点
    ProgramEndNode, // 程序结束节点
    ConstDecalrationNode, // 常量声明节点
    ConstAssignNode, // 常量赋值节点
    VarDecalrationNode, // 变量声明节点
    VarTypeNode,//变量类型节点
    ColonNode, // 冒号节点
    ProcedureNode, // 过程节点
    FunctionNode, // 函数节点
    BeginNode, // 块开始节点
    EndNode, // 块结束节点
    MinusNode, // 负号运算符
    ArrayNode, // 数组节点
    LeftSquareBracketNode, // 左方括号节点
    RightSquareBracketNode, // 右方括号节点
    ArrayRangeDelimiterNode, // 数组范围限制分隔
    OfNode, // OF节点
    IntNode, // 整数
    RealNode, // 实数
    BooleanNode, // 布尔值
    CharNode, // 字符值
    AssignNode, // 赋值运算符
    IfNode, // IF节点
    ThenNode, // Then节点
    ElseNode, // Else节点
    ForNode, // For节点
    ToNode, // To节点
    DoNode, // Do节点
    ReadNode, // Read节点
    WriteNode, // Write节点
    RelopNode, // 关系运算符节点
    OpNode,//算术逻辑运算符节点
    NotOperatorNode, // 取反运算符节点
    // 以下为非终结符
    ProgramStructNode,
    ProgramHeadNode,
    ProgramIDNode,
    IDListNode,
    ProgramBodyNode,
    ConstDeclarationsNode,
    VarDeclarationsNode,
    SubprogramDeclarationsNode,
    CompoundStatementNode,
    ConstDeclarationNode,
    ConstValueNode,
    VarDeclarationNode,
    TypeNode,
    BasicTypeNode,
    PeriodNode,
    SubprogramNode,
    SubprogramHeadNode,
    SubprogramBodyNode,
    FormalParameterNode,
    ParameterListNode,
    ParameterNode,
    VarParameterNode,
    ValueParameterNode,
    StatementListNode,
    StatementNode,
    VariableNode,
    ExpressionNode,
    ProcedureCallNode,
    ElsePartNode,
    VariableListNode,
    ExpressionListNode,
    IDVarpartNode,
    SimpleExpressionNode,
    TermNode,
    FactorNode,
};

class Node;
using NodeList = vector<Node*>;

class Node : public Token
{
public:
    NodeType type = EmptyNode;
    NodeList &children = *new NodeList; // 记录其下属的子节点
    NodeInfo *info = nullptr;
    Node(const NodeType type = EmptyNode);
    Node(const Token &old_token);
    Node(const Node &old_node);
    Node &operator=(const Node &old_node);
    ~Node();
};

class Error
{
  public:
    int line;
    int column;
    string msg;
    Error();
    Error(const string msg, const int line = -1, const int column = -1);
    string what() const; // 返回由行号、列号和msg组成的人性化的错误说明字符串
};
using ErrorList = vector<Error>;

#endif
