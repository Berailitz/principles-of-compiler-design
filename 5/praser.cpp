#include "praser.h"

void Praser::prase(const vector<Token&> tokens)
{

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
        Node &const_declaration = *(const_declaration.children[2]);
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

ArrayRange make_array_range(const Node &period_node)
{
    if (period_node.children.size() == 3)
    {
        return {make_range_from_period_children(period_node.children.begin())};
    }
    else
    {
        ArrayRange left_range = make_array_range(*period_node.children[0]);
        pair<int, int> right_range = make_range_from_period_children(period_node.children.begin() + 2);
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
        ArrayRange range = make_array_range(*type_node.children[2]);
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

bool prase_var_declaration(SymbolTable &table, Node &var_declaration)
{
    if (var_declaration.children.size() == 3)
    {
        prase_var_declaration_children(table, var_declaration.children.begin());
    }
    else
    {
        prase_var_declaration(table, *var_declaration.children[0]);
        Node &var_declaration = *(var_declaration.children[2]);
    }
}

bool prase_var_declarations(SymbolTable &program_table, Node &var_declarations)
{
    if (var_declarations.children.size() > 1)
    {
        prase_var_declaration(program_table, *(var_declarations.children[1]));
    }
}

SimpleParameterTypeList prase_value_parameter(Node &value_parameter)
{
    return {prase_id_list_from_idlist(*value_parameter.children[0]), prase_variant_type_from_type(*value_parameter.children[2])};
}

SimpleParameterTypeList prase_parameter(Node &parameter)
{
    Node &child_node = *parameter.children[0];
    if (child_node.children.size() == 2)
    {
        // 传引用
    }
    
}

bool prase_subprogram_declaration(SymbolTable &table, Node &subprogram_declaration)
{
    // 建立子符号表
    Node &subprogram_head = *subprogram_declaration.children[0];
    Node &subprogram_body = *subprogram_declaration.children[2];
    Symbol &source_symbol = table.find(subprogram_head.children[1]->string_value); // 上级符号表中的符号
    SymbolTable *sub_table = new SymbolTable(&table, &source_symbol); // 子符号表
    if (var_declaration.children.size() == 3)
    {
        prase_var_declaration_children(table, var_declaration.children.begin());
    }
    else
    {
        prase_var_declaration(table, *var_declaration.children[0]);
        Node &var_declaration = *(var_declaration.children[2]);
    }
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

// 若无错误，返回true，否则返回false
bool prase_function(SymbolTable &root_table, Node &root_node)
{
}
