#include "token.h"


int string2int(string str){															//字符串转换为整型 
    stringstream ss;
    ss << str;
    int result;
    ss >> result;
    return result;
}

Token::Token()
{
}

Token::Token(const string &string_value, const int line, const int column){	//传入string_value的构造函数 
	this->line = line;
	this->column = column;
	this->string_value.assign(string_value);
	int k = 0;
	//cout<<string_value<<endl;
	if(string_value=="program"){												//判断不同类型的string_value 
		type = ProgramToken;
	}
	else if(string_value=="("){
		type = LeftRoundBracketToken;
	}
	else if(string_value==")"){
		type = RightRoundBracketToken;
	}
	else if(string_value==","){
		type = CommaToken;
	}
	else if(string_value==";"){
		type = SemicolonToken;
	}
	else if(string_value=="."){
		type = ProgramEndToken;
	}
	else if(string_value=="const"){
		type = ConstDecalrationToken;
	}
	else if(string_value=="=?"){
		type = ConstAssignToken;
		this->string_value.assign("=");
	}
	else if(string_value=="var"){
		type = VarDecalrationToken;
	}
	else if(string_value=="integer"){
		type = VarTypeToken;
		int_value = 1;
	}
	else if(string_value=="real"){
		type = VarTypeToken;
		int_value = 2;
	}
	else if(string_value=="boolean"){
		type = VarTypeToken;
		int_value = 3;
	}
	else if(string_value=="char"){
		type = VarTypeToken;
		int_value = 4;
	}
	else if(string_value==":"){
		type = ColonToken;
	}
	else if(string_value=="procedure"){
		type = ProcedureToken;
	}
	else if(string_value=="function"){
		type = FunctionToken;
	}
	else if(string_value=="begin"){
		type = BeginToken;
	}
	else if(string_value=="end"){
		type = EndToken;
	}
	else if(string_value.find("..")!= string::npos){
		type = ArrayRangeDelimiterToken;
		this->string_value.assign(this->string_value.substr(1,this->string_value.length()-1));
	}
	else if(string_value=="-"){
		type = MinusToken;
	}
	else if(string_value=="array"){
		type = ArrayToken;
	}
	else if(string_value=="["){
		type = LeftSquareBracketToken;
	}
	else if(string_value=="]"){
		type = RightSquareBracketToken;
	}
	else if(string_value=="of"){
		type = OfToken;
	}
	else if(string_value==":="){
		type = AssignToken;
	}
	else if(string_value=="if"){
		type = IfToken;
	}
	else if(string_value=="then"){
		type = ThenToken;
	}
	else if(string_value=="else"){
		type = ElseToken;
	}
	else if(string_value=="for"){
		type = ForToken;
	}
	else if(string_value=="to"){
		type = ToToken;
	}
	else if(string_value=="do"){
		type = DoToken;
	}
	else if(string_value=="read"){
		type = ReadToken;
	}
	else if(string_value=="write"){
		type = WriteToken;
	}
	else if(string_value==">"){
		type = RelopToken;
		int_value = 1;
	}
	else if(string_value=="="){
		type = RelopToken;
		int_value = 2;
	}
	else if(string_value=="<"){
		type = RelopToken;
		int_value = 3;
	}
	else if(string_value=="<="){
		type = RelopToken;
		int_value = 4;
	}
	else if(string_value==">="){
		type = RelopToken;
		int_value = 5;
	}
	else if(string_value=="<>"){
		type = RelopToken;
		int_value = 6;
	}
	else if(string_value=="+"){
		type = OpToken;
		int_value = 1;
	}
	else if(string_value=="*"){
		type = OpToken;
		int_value = 2;
	}
	else if(string_value=="/"){
		type = OpToken;
		int_value = 3;
	}
	else if(string_value=="div"){
		type = OpToken;
		int_value = 4;
	}
	else if(string_value=="mod"){
		type = OpToken;
		int_value = 5;
	}
	else if(string_value=="and"){
		type = OpToken;
		int_value = 6;
	}
	else if(string_value=="or"){
		type = OpToken;
		int_value = 7;
	}
	else if(string_value=="not"){
		type = NotOperatorToken;
	}
	else if(string_value=="false"||string_value =="true"){
		type = BooleanToken;
	}
	else if(string_value.find(".")!= string::npos){
		type = RealToken;
	}
	/*else if(string_value.find("?")!= string::npos){
		type = ArrayRangeDelimiterToken;
		this->string_value.assign(string_value.substr(1, string_value.length()-1));	
	}*/
	else if(string_value=="0"){
		type = IntToken;			
	}
	else if(!string2int(string_value)){
		type = IdentifierToken;
	}
	else{
		type = IntToken;
	}
	return; 
}

Token::Token(const string text,string flag){											
	string s;
	s.assign(text);
	int p;
	p = s.find(" ");
	int t = string2int(s.substr(0,p));
	this->type = TokenType(t);
	s.assign(s.substr(p+1,s.length()-p-1));
	p = s.find(" ");
	string_value.assign(s.substr(0,p));
	s.assign(s.substr(p+1,s.length()-p-1));
	p = s.find(" ");
	line = string2int(s.substr(0,p));
	s.assign(s.substr(p+1,s.length()-p-1));
	column =string2int(s); //stoi(s.substr(p+1,s.length()),0,10);
	return;
}

Token::Token(const Token * old_token): 
	type(old_token->type),
	int_value(old_token->int_value),
	string_value(old_token->string_value),
	line(old_token->line),
	column(old_token->column)
{
}

Token::operator string() const
{																		//把Token转换为string 
	string token_text = to_string(type) + TOKEN_TEXT_DELIMITER + string_value;
	return token_text + TOKEN_TEXT_DELIMITER + to_string(line) + TOKEN_TEXT_DELIMITER + to_string(column);
}


Error::Error()
{
}

Error::Error(const string msg, const int line, const int column){
	this->msg.assign(msg);
	this->line = line;
	this->column = column;
};
string Error::what() const
{
	string error_text = to_string(line) + TOKEN_TEXT_DELIMITER + to_string(column) + TOKEN_TEXT_DELIMITER + msg;
	return error_text;
}; // 返回由行号、列号和msg组成的人性化的错误说明字符串

ParameterType::ParameterType(const VariantType & old_variantType, const bool is_referfence) : VariantType(old_variantType), is_referfence(is_referfence)
{

}

VariantType::VariantType(const BasicVariantType type, const bool is_constant, const ArrayRange * old_scope) : type(type), is_constant(is_constant)
{
	if (old_scope != nullptr)
	{
		scope = new ArrayRange(*old_scope);
	}
}

VariantType::VariantType(const VariantType & old_variant_type) : type(old_variant_type.type), is_constant(old_variant_type.is_constant)
{
	if (old_variant_type.scope != nullptr)
	{
		scope = new ArrayRange(*old_variant_type.scope);
	}
}

Symbol::Symbol(VariantType * type, ParameterTypeList * parameters) : type(type), parameters(parameters)
{

}

Symbol::Symbol(const Symbol & old_symbol)
{
	if (old_symbol.type != nullptr)
	{
		type = new VariantType(*old_symbol.type);
	}
	if (old_symbol.parameters != nullptr)
	{
		parameters = new ParameterTypeList(*old_symbol.parameters);
	}
}

SymbolTable::SymbolTable(SymbolTable * parent, Symbol * source) : parent(parent), source(source), Symbols(*new unordered_map<string, Symbol>)
{
}

bool SymbolTable::insert(const string & key, const Symbol & value)
{
	if (Symbols.find(key) == Symbols.end())
	{
		Symbols[key] = value;
		return true;
	}
	else
	{
		Symbols[key] = value;
		return false;
	}
}

Symbol SymbolTable::find(const string & key) const
{
	auto it = Symbols.find(key);
	if (it == Symbols.end())
	{
		Symbol empty_symbol;
		Symbols[key] = empty_symbol;
		return empty_symbol;
	}
	else
	{
		return it->second;
	}
}

Node::Node(const NodeType type) : type(type)
{
}

Node::Node(const Token & old_token) : Token(old_token)
{
	type = token_type_to_node_type(old_token.type);
}

Node::Node(const Node & old_node) : type(old_node.type), children(*new NodeList(old_node.children))
{
	if (old_node.info != nullptr)
	{
		info = new NodeInfo(*old_node.info);
	}
}

Node & Node::operator=(const Node & old_node)
{
	type = old_node.type;
	//children = *new NodeList(old_node.children);
	if (old_node.info != nullptr)
	{
		info = new NodeInfo(*old_node.info);
	}
}

Node::~Node()
{
	delete info;
	delete &children;
}

NodeInfo::NodeInfo(const NodeInfo & old_node_info) : returnType(old_node_info.returnType)
{
	if (old_node_info.table != nullptr)
	{
		table = new SymbolTable(*old_node_info.table);
	}
	if (old_node_info.scope != nullptr)
	{
		scope = new ArrayRange(*old_node_info.scope);
	}
}
