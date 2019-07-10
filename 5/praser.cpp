#include "praser.h"

void Praser::build_table_and_rules()
{
    
}

Node *Praser::prase(const TokenList &tokens) const
{
    TokenList::const_iterator it = tokens.begin();
    PraserState state = 0;
    PraserStack &stack = *new PraserStack;
    NodeList &nodes = *new NodeList;
    PraserAction action = table.at({state, token_type_to_node_type(it->type)});
    while (action.first != AcceptStackAction)
    {
        switch (action.first)
        {
        case EmptyStackAction:
            break;
        case ShiftStackAction:
            stack.push_back({action.second, token_type_to_node_type(it->type)});
            nodes.push_back(new Node(*it));
            it++;
            break;
        case ReduceStackAction:
            PraserRule rule = rules[action.second];
            Node *parent = new Node(rule.first);
            parent->children = move(nodes);
            for (int i = 0; i < rule.second; i++)
            {
                stack.pop_back();
            }
            stack.push_back({table.at({state, rule.first}).second, rule.first});
            nodes.push_back(parent);
            break;
        case AcceptStackAction:
            break;
        }
        action = table.at({state, token_type_to_node_type(it->type)});
    }
    return nodes[0];
}

NodeType token_type_to_node_type(const TokenType token_type)
{
    return static_cast<NodeType>(token_type);
}

BasicVariantType node_to_basic_variant_type(const Node &node)
{
    switch (node.type)
    {
    case IntToken:
        return IntVariant;
    case RealToken:
        return RealVariant;
    case BooleanToken:
        return BooleanVariant;
    case CharToken:
        return CharVariant;
    case VarTypeNode:
        switch (node.int_value)
        {
        case IntVarType:
            return IntVariant;
        case RealVarType:
            return IntVariant;
        case BooleanVarType:
            return BooleanVariant;
        case CharVarType:
            return CharVariant;
        default:
            return EmptyVariant;
        }
    default:
        return EmptyVariant;
    }
}

bool prase_const_declaration_children(SymbolTable &table, const NodeList::iterator &it)
{
    Node &id_node = **it;
    Node &value_node = **(it + 2);
    table.insert(id_node.string_value, *new Symbol(new VariantType(node_to_basic_variant_type(value_node), true)));
}

bool prase_const_declaration(SymbolTable &table, const Node &const_declaration)
{
    if (const_declaration.children.size() == 3)
    {
        prase_const_declaration_children(table, const_declaration.children.begin());
    }
    else
    {
        prase_const_declaration(table, *const_declaration.children[0]);
        prase_const_declaration_children(table, *(const_declaration.children[2]));
    }
}

bool prase_const_declarations(SymbolTable &program_table, const Node &const_declarations)
{
    if (const_declarations.children.size() > 1)
    {
        prase_const_declaration(program_table, *(const_declarations.children[1]));
    }
}

pair<int, int> make_range_from_period_children(const NodeList::iterator &it)
{
    return {stoi(it->string_value), stoi((it + 2)->string_value)};
}

ArrayRange prase_period(const Node &period)
{
    if (period.children.size() == 3)
    {
        return {make_range_from_period_children(period.children.begin())};
    }
    else
    {
        ArrayRange left_range = prase_period(*period.children[0]);
        pair<int, int> right_range = make_range_from_period_children(period.children.begin() + 2);
        left_range.push_back(right_range);
        return left_range;
    }
}

VariantType prase_variant_type_from_type(const Node &type_node)
{
    if (type_node.children.size() == 1)
    {
        // basic_type->sth.
        return node_to_basic_variant_type(*type_node.children[0]->children[0]);
    }
    else
    {
        // array
        ArrayRange range = prase_period(*type_node.children[2]);
        return VariantType(node_to_basic_variant_type(*type_node.children[5]), false, new ArrayRange(range));
    }
}

IDList prase_id_list_from_idlist(const Node &idlist)
{
    if (idlist.children.size() == 1)
    {
        return {idlist.children[0]->string_value};
    }
    else
    {
        IDList left_list = prase_id_list_from_idlist(*idlist.children[0]);
        left_list.push_back(idlist.children[2]->string_value);
        return left_list;
    }
}

bool prase_var_declaration_children(SymbolTable &table, const NodeList::iterator &it)
{
    IDList id_list = prase_id_list_from_idlist(**it);
    VariantType variant_type = prase_variant_type_from_type(**(it + 2));
    for (const string &name : id_list)
    {
        table.insert(name, *new Symbol(new VariantType(variant_type)));
    }
}

bool prase_var_declaration(SymbolTable &table, const Node &var_declaration)
{
    if (var_declaration.children.size() == 3)
    {
        prase_var_declaration_children(table, var_declaration.children.begin());
    }
    else
    {
        prase_var_declaration(table, *var_declaration.children[0]);
        prase_var_declaration_children(table, *(var_declaration.children[2]));
    }
}

bool prase_var_declarations(SymbolTable &program_table, const Node &var_declarations)
{
    if (var_declarations.children.size() > 1)
    {
        prase_var_declaration(program_table, *(var_declarations.children[1]));
    }
}

SimpleParameterTypeList prase_value_parameter(const Node &value_parameter)
{
    return {prase_id_list_from_idlist(*value_parameter.children[0]), prase_variant_type_from_type(*value_parameter.children[2])};
}

ParameterTypeList prase_parameter(const Node &parameter)
{
    Node &child_node = *parameter.children[0];
    bool is_reference;
    SimpleParameterTypeList parameters({}, EmptyVariant);
    ParameterTypeList same_type_list;
    if (child_node.children.size() == 2)
    {
        // 传引用
        parameters = prase_value_parameter(*parameter.children[1]);
        is_reference = true;
    }
    else
    {
        parameters = prase_value_parameter(child_node);
        is_reference = false;
    }
    for (const string &name : parameters.first)
    {
        same_type_list.push_back({name, ParameterType(parameters.second)});
    }
    return same_type_list;
}

ParameterTypeList prase_parameter_list(const Node &parameter_list)
{
    if (parameter_list.children.size() == 1)
    {
        // 仅一类参数
        return prase_parameter(*parameter_list.children[0]);
    }
    else
    {
        ParameterTypeList left_list = prase_parameter_list(*parameter_list.children[0]);
        ParameterTypeList right_list = prase_parameter(*parameter_list.children[2]);
        left_list.insert(left_list.end(), right_list.begin(), right_list.end());
        return left_list;
    }
}

ParameterTypeList prase_formal_parameter(const Node &formal_parameter)
{
    if (formal_parameter.children.size == 3)
    {
        return prase_parameter_list(*formal_parameter.children[1]);
    }
    else
    {
        return {};
    }
}

bool prase_statement(SymbolTable &table, Node &statement)
{
    if (statement.children.size() > 0 && statement.children[0]->type == ForNode)
    {
        SymbolTable *sub_table = new SymbolTable(&table); // 子符号表
        statement.info->table = sub_table;
        sub_table->insert(statement.children[1]->string_value, *new Symbol(new VariantType(IntVariant)));
        prase_statement(*sub_table, *statement.children[7]);
    }
}

bool prase_statement_list(SymbolTable &table, Node &statement_list)
{
    if (statement_list.children.size() == 3)
    {
        prase_statement_list(table, *statement_list.children[0]);
        prase_statement(table, *statement_list.children[2]);
    }
    prase_statement(table, *statement_list.children[0]);
}

bool prase_compound_statement(SymbolTable &table, Node &subprogram_body)
{
    prase_statement_list(table, *subprogram_body.children[1]);
}

bool prase_subprogram_body(SymbolTable &table, Node &subprogram_body)
{
    prase_const_declarations(table, *subprogram_body.children[0]);
    prase_var_declarations(table, *subprogram_body.children[1]);
    prase_compound_statement(table, *subprogram_body.children[2]);
}

bool prase_subprogram_declaration(SymbolTable &table, Node &subprogram_declaration)
{
    // 建立子符号表
    Node &subprogram_head = *subprogram_declaration.children[0];
    Node &subprogram_body = *subprogram_declaration.children[2];
    Symbol &source_symbol = table.find(subprogram_head.children[1]->string_value); // 上级符号表中的符号
    subprogram_declaration.info->table = new SymbolTable(&table, &source_symbol); // 子符号表
    source_symbol.parameters = new ParameterTypeList(prase_formal_parameter(*subprogram_head.children[2]));
    if (subprogram_head.children.size() == 4)
    {
        // 为函数，有返回值
        source_symbol.type = new VariantType(prase_variant_type_from_type(*subprogram_head.children[4]));
    }
    prase_subprogram_body(*subprogram_declaration.info->table, subprogram_body);
}

bool prase_subprogram_declarations(SymbolTable &program_table, Node &subprogram_declarations)
{
    if (subprogram_declarations.children.size() > 1)
    {
        prase_subprogram_declarations(program_table, *(subprogram_declarations.children[0]));
        prase_subprogram_declaration(program_table, *(subprogram_declarations.children[1]));
    }
}

bool prase_program(SymbolTable &program_table, Node &program_node)
{
    Node &program_body = *(program_node.children[2]);
    Node &subprogram_declarations = *(program_body.children[2]);
    Node &compound_statement = *(program_body.children[3]);
    prase_const_declarations(program_table, *(program_body.children[0]));
    prase_var_declarations(program_table, *(program_body.children[1]));
}
