#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<map> 
#include <string> 
#include <sstream>
using namespace std;

#define Buffer_size 100												//输入缓冲区大小,可根据后面进行更改
#define keyWord_length 20


enum TokenType														//记号枚举类 
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

typedef struct _Buffer{																// 输入缓冲区的结构体 
	int check = 0;
	char standing;	
	int lines = 0;
	int columns = 0;								 
}Buffer;

int string2int(string str){															//字符串转换为整型 
    stringstream ss;
    ss << str;
    int result;
    ss >> result;
    return result;
}
const string TOKEN_TEXT_DELIMITER = " ";
class Token
{
  public:
    TokenType type = EmptyToken;
    int int_value = 0; 																// 同一类下各不同记号的序号
    string string_value;															// 标识符的ID（名字），int、float、boolean、char的值
    int line; 																		// 行号
    int column; 																	// 列号
    Token();
    Token(const string &string_value, const int line = -1, const int column = -1){	//传入string_value的构造函数 
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
	};
    Token(const string text,string flag){											
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
	};
    operator string() const{																		//把Token转换为string 
		string token_text = to_string(type) + TOKEN_TEXT_DELIMITER + string_value;
		return token_text + TOKEN_TEXT_DELIMITER + to_string(line) + TOKEN_TEXT_DELIMITER + to_string(column);
	};
};

class Error
{
  public:
    int line;
    int column;
    string msg;
    Error();
    Error(const string msg, const int line = -1, const int column = -1){
    	this->msg.assign(msg);
    	this->line = line;
    	this->column = column;
	};
    string what() const{
    	string error_text = to_string(line) + TOKEN_TEXT_DELIMITER + to_string(column) + TOKEN_TEXT_DELIMITER + msg;
		return error_text;
	}; // 返回由行号、列号和msg组成的人性化的错误说明字符串
};

void Initkey(void);
void Initbuffer(FILE* fp, Buffer* buffer,int &model);
char get_char(FILE* fp,Buffer* buffer,int &model,int &forward, int &x, int & y); 
void Lexer( FILE *fp); 


int lines = 1;
int length = 0;
vector<Token> tokenList;    
vector<Error> errorList;  
FILE *fp;   									// 测试文件指针 

int main(void){
	
    if((fp=fopen("test.txt","r"))==NULL)
    {
        printf("Test cannot open! \n");
        exit(0);
    }
    else
        printf("Tests start! \n");
    
	Lexer( fp);  
    for(int i=0;i<tokenList.size();i++){
    	//printf("%d\n",tokenList[i].type+3);
    	cout<<string(tokenList[i])<<endl;
    	/*string k;
		k.assign(string(tokenList[i]));
    	Token s(k,string("123"));
    	cout<<string(s)<<endl;
    	cout<<endl;*/
	}
	
	printf("错误列表如下：\n");
	for(int j=0;j<errorList.size();j++){
		cout<<errorList[j].what()<<endl;
	}
	if(errorList.size()==0){
		printf("没有错误！"); 
	}
    fclose(fp);
    return 0;
} 

void Initbuffer(FILE* fp, Buffer* buffer,int &model){
	//初始输入缓冲区
	int start,stop,flag=0;
	if(model==0){
		start = 0;
		stop = Buffer_size;
		model = 1;
	}
	else if(model==1){
		start = 0;
		stop = Buffer_size/2;
		model = 2;
	}
	else if(model==2){
		start = Buffer_size/2;
		stop = Buffer_size;
		model = 1;
	}
	else{
	}
	char ch = getc(fp);
	length++;
	for(int i=start;i<stop;i++){
		if(ch==EOF){
			//printf("结束%d ",i); 
			buffer[i].standing  = '@';
			buffer[i].lines  = lines;
			buffer[i].columns  = length;	
			flag = 1;
			break; 
		}
		else if(ch==32){
			if(buffer[i-1].standing==32){
				while(ch == 32||ch == '\n'||ch == 9){
					ch = getc(fp);
					length++;
					if(ch == '\n'){
						lines++;
						length = 0;
					}
				}
				i--;
			}
			else{
				buffer[i].standing = ch;
				buffer[i].lines  = lines;
				buffer[i].columns  = length;
				//printf("空格%d ",i);
				ch = getc(fp);
				length++;
				while(ch == 32||ch == '\n'||ch == 9){
					ch = getc(fp);
					length++;
					if(ch == '\n'){
						lines++;
						length = 0;
					}
				}
			}
		}
		else if(ch=='{'){											//"{"的判断 
			long int temp = ftell(fp);
			ch = getc(fp);
			length++;
			if(ch == '\n'){
				lines++;
				length = 0;
			}
			int temp_lines = lines;
			int temp_length = length;
			while(ch!=EOF){
				if(ch=='}'){
					ch = getc(fp);
					length++;
					//printf("注释/*%d ",i);
					i--;
					break;
				}
				else if(ch == '\n'){
					ch = getc(fp);
					lines++;
					length = 0;
				}
				else {
					ch = getc(fp);
					length++;
				}
			}
			if(ch==EOF){											//无"}"匹配 			//error 
				Error temp_error("[Expected }.]",lines,length);
				errorList.push_back(temp_error);
				fseek(fp,temp,0);
				lines = temp_lines;
				length = temp_length;
				ch =  getc(fp);
				length++;
				i--;
			}
		}
		else if(ch=='\n'||ch == 9){
			if(ch == '\n'){
				lines++;
				length = 0;
			}
			if(buffer[i-1].standing!=32){
				buffer[i].standing =32;
				buffer[i].lines  = lines;
				buffer[i].columns  = length;	
			}
			while(ch==32||ch=='\n'||ch==9){
				ch = getc(fp);
				length++;
				if(ch == '\n'){
					lines++;
					length = 0;
				}
			}		
			i = i - 1;
		}
		else{
			buffer[i].standing = ch;
			buffer[i].lines  = lines;
			buffer[i].columns  = length;		
			//printf("正常%d %c",i,ch);
			ch = getc(fp);
			length++;
		}
	}
	if(flag==0)	
		fseek(fp,-1L,1);
	//printf("@@@@%d\n",start);
	//printf("begin\n");
	//printf("\n");
	/*for(int i=start;i<stop;i++){
		printf("%d%c ",i,buffer[i].standing);
	}*/
	//printf("\n\n");
	return;
}  

char get_char(FILE* fp,Buffer* buffer,int &model,int &forward, int &x, int &y){
	int stop;
	if(model==1){
		stop = Buffer_size;
	}
	if(model==2){
		stop = Buffer_size/2;
	}
	if(forward<(stop - 1)){
		forward++;
		x = buffer[forward-1].lines;
		y = buffer[forward-1].columns;
		return buffer[forward-1].standing;
	}		
	else {
		Initbuffer(fp,buffer,model);
		if(model==2){
			forward = 0;
		}
		else forward++;
		x = buffer[stop-1].lines;
		y = buffer[stop-1].columns;
		return buffer[stop-1].standing;
	}
}

void exchange(string v, int c_line, int c_length){
	Token s(v,c_line,c_length);
	tokenList.push_back(s);
}

void exchange_error(string v, int c_line, int c_length){
	Error s(v,c_line,c_length);
	errorList.push_back(s);
}

void Lexer( FILE *fp){
	Buffer buffer[Buffer_size];
	int model = 0;
	int forward = 0;
	int x=1;
	int y=1; 
	Initbuffer( fp, buffer, model);
 	/*for(int i=0;i<Buffer_size;i++){
		printf("%d %d %d %c\n", i, buffer[i].lines,buffer[i].columns, buffer[i].standing);
	}*/
	
	//int forward = 0;												//指向缓冲区下一个读取的位置 
	/*char ch = get_char( fp, buffer,model,forward);
	while(ch!='@'){
		printf("%c",ch);
		ch = get_char(fp,buffer,model,forward);
	}*/
	int state = 0;													//当前状态指示，0->起始状态
	int _C_line = 1;
	int _C_length = 1;
	int is_const = 0;
	int is_array = 0;
	char C = get_char( fp, buffer,model,forward, x, y);				//存放当前读入的字符
	char _C = C;
	string v = "";
	while(C!='@'){
		//printf("%c %d\n",C,state);
		switch(state){
			case 0:
				switch(C){
				 	case 'a': state = 1;break;						//设置标识符状态 
				 	case 'b': state = 1;break;
				 	case 'c': state = 1;break;
				 	case 'd': state = 1;break;
				 	case 'e': state = 1;break;
				 	case 'f': state = 1;break;
				 	case 'g': state = 1;break;
				 	case 'h': state = 1;break;
				 	case 'i': state = 1;break;
				 	case 'j': state = 1;break;
				 	case 'k': state = 1;break;
				 	case 'l': state = 1;break;
				 	case 'm': state = 1;break;
				 	case 'n': state = 1;break;
				 	case 'o': state = 1;break;
				 	case 'p': state = 1;break;
				 	case 'q': state = 1;break;
				 	case 'r': state = 1;break;
				 	case 's': state = 1;break;
				 	case 't': state = 1;break;
				 	case 'u': state = 1;break;
				 	case 'v': state = 1;break;
				 	case 'w': state = 1;break;				 	
				 	case 'x': state = 1;break;
				 	case 'y': state = 1;break;
				 	case 'z': state = 1;break;
				 	case 'A': state = 1;break;
				 	case 'B': state = 1;break;
				 	case 'C': state = 1;break;
				 	case 'D': state = 1;break;
				 	case 'E': state = 1;break;
				 	case 'F': state = 1;break;
				 	case 'G': state = 1;break;
				 	case 'H': state = 1;break;
				 	case 'I': state = 1;break;
				 	case 'J': state = 1;break;
				 	case 'K': state = 1;break;
				 	case 'L': state = 1;break;
				 	case 'M': state = 1;break;
				 	case 'N': state = 1;break;
				 	case 'O': state = 1;break;
				 	case 'P': state = 1;break;
				 	case 'Q': state = 1;break;
				 	case 'R': state = 1;break;
				 	case 'S': state = 1;break;
				 	case 'T': state = 1;break;
				 	case 'U': state = 1;break;
				 	case 'V': state = 1;break;
				 	case 'W': state = 1;break;				 	
				 	case 'X': state = 1;break;
				 	case 'Y': state = 1;break;
				 	case 'Z': state = 1;break;
				 	
				 	case '1': state = 2;break;								//设置常数符状态 
				 	case '2': state = 2;break;
				 	case '3': state = 2;break;
				 	case '4': state = 2;break;
				 	case '5': state = 2;break;
				 	case '6': state = 2;break;
				 	case '7': state = 2;break;
				 	case '8': state = 2;break;
				 	case '9': state = 2;break;
				 	case '0': state = 2;break;
				 	
				 	case '(': state = 3;break;
					case ')': state = 3;break;
					case ',': state = 3;break;
					case ';': state = 3;break;
					case '.': state = 3;break;
					case '-': state = 3;break;
					case '+': state = 3;break;
					case '*': state = 3;break;
					case '/': state = 3;break;
					case '[': state = 3;break;
					case ']': state = 3;break;
					case '=': state = 3;break; 
					case ':': state = 4;break;
					case '<': state = 4;break;
					case '>': state = 4;break;
				 	case ' ': state = 0;
							  _C_line = x;
							  _C_length = y;
							  C = get_char( fp, buffer,model,forward, x,y);
							  v.assign("");
							  break;
				 	default: state = 0;
				 			 exchange_error("[No such character.]",x,y);
					 		 break;  
				}
				break;
			case 1:
				v = v + C;
				_C_line = x;
				_C_length = y;
				C = get_char( fp, buffer,model,forward,x,y);
				switch(C){
				 	case 'a': state = 1;break;						//设置标识符状态 
				 	case 'b': state = 1;break;
				 	case 'c': state = 1;break;
				 	case 'd': state = 1;break;
				 	case 'e': state = 1;break;
				 	case 'f': state = 1;break;
				 	case 'g': state = 1;break;
				 	case 'h': state = 1;break;
				 	case 'i': state = 1;break;
				 	case 'j': state = 1;break;
				 	case 'k': state = 1;break;
				 	case 'l': state = 1;break;
				 	case 'm': state = 1;break;
				 	case 'n': state = 1;break;
				 	case 'o': state = 1;break;
				 	case 'p': state = 1;break;
				 	case 'q': state = 1;break;
				 	case 'r': state = 1;break;
				 	case 's': state = 1;break;
				 	case 't': state = 1;break;
				 	case 'u': state = 1;break;
				 	case 'v': state = 1;break;
				 	case 'w': state = 1;break;				 	
				 	case 'x': state = 1;break;
				 	case 'y': state = 1;break;
				 	case 'z': state = 1;break;
				 	case 'A': state = 1;break;
				 	case 'B': state = 1;break;
				 	case 'C': state = 1;break;
				 	case 'D': state = 1;break;
				 	case 'E': state = 1;break;
				 	case 'F': state = 1;break;
				 	case 'G': state = 1;break;
				 	case 'H': state = 1;break;
				 	case 'I': state = 1;break;
				 	case 'J': state = 1;break;
				 	case 'K': state = 1;break;
				 	case 'L': state = 1;break;
				 	case 'M': state = 1;break;
				 	case 'N': state = 1;break;
				 	case 'O': state = 1;break;
				 	case 'P': state = 1;break;
				 	case 'Q': state = 1;break;
				 	case 'R': state = 1;break;
				 	case 'S': state = 1;break;
				 	case 'T': state = 1;break;
				 	case 'U': state = 1;break;
				 	case 'V': state = 1;break;
				 	case 'W': state = 1;break;				 	
				 	case 'X': state = 1;break;
				 	case 'Y': state = 1;break;
				 	case 'Z': state = 1;break;				 	
				 	case '1': state = 1;break;								//设置常数符状态 
				 	case '2': state = 1;break;
				 	case '3': state = 1;break;
				 	case '4': state = 1;break;
				 	case '5': state = 1;break;
				 	case '6': state = 1;break;
				 	case '7': state = 1;break;
				 	case '8': state = 1;break;
				 	case '9': state = 1;break;
				 	case '0': state = 1;break;
				 	
				 	case '(': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ')': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ',': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ';': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '.': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '-': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '+': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '*': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '/': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '[': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ']': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '=': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ':': state = 4;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '<': state = 4;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '>': state = 4;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
				 	case ' ': state = 0;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  _C_line = x;
							  _C_length = y;
							  break;
				 	default: state = 0;printf("%c",C);exchange_error("[Identifier error.]",x,y);break;  										//error
				}
				break;
			case 2:																											//数字 
				v = v + C;
				_C_line = x;
				_C_length = y;
				C = get_char( fp, buffer,model,forward , x,y);
				switch(C){			 	
				 	case '1': state = 2;break;								//设置常数符状态 
				 	case '2': state = 2;break;
				 	case '3': state = 2;break;
				 	case '4': state = 2;break;
				 	case '5': state = 2;break;
				 	case '6': state = 2;break;
				 	case '7': state = 2;break;
				 	case '8': state = 2;break;
				 	case '9': state = 2;break;
				 	case '0': state = 2;break;
				 	case '.': 
							  _C_line = x;
							  _C_length = y;
							  C = get_char( fp, buffer,model,forward,x,y);
				 			  if(C>='0'&&C<='9'||C=='.'){
				 			  	  v = v+'.';
				 			  	  state = 5;
							   }
							   else{
							   	  state = 0;														//error 
					 		  	  exchange(v,_C_line,_C_length);
					 		  	  exchange_error("float error.",_C_line,_C_length);
								  v.assign("");
								  _C_line = x;
								  _C_length = y;
								  C = get_char( fp, buffer,model,forward,x,y);    
							   } 
							   break;
				 	
				 	case '(': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ')': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ',': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ';': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '-': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '+': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '*': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '/': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;							  
					case '[': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ']': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '=': state = 3;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case ':': state = 4;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '<': state = 4;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
					case '>': state = 4;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
				 	case ' ': state = 0;
					 		  exchange(v,_C_line,_C_length);
							  v.assign("");
							  break;
				 	default: state = 0;exchange_error("[Int error.]",x,y);break;  
				}
				break;
			case 3:																										//检测特殊符号 
				v = v + C;
				is_const = tokenList.size();
				if(tokenList[is_const-2].string_value == "const"&&C=='='){
					v.assign("=?");
				}
				_C = C;
				exchange(v,x,y);
			 	state = 0;
				_C_line = x;
				_C_length = y;
				C = get_char( fp, buffer,model,forward, x, y);
				v.assign("");
				break;
			case 4:																										//检测:= <> <= >= 
				v = v + C;
				_C_line = x;
				_C_length = y;
				_C = C;
				C = get_char( fp, buffer,model,forward, x, y);
				if(C>='a'&&C<='z'||C>='A'&&C>='Z'){
					state = 1;
					exchange(v,_C_line,_C_length);
					v.assign("");
				}
				else{
					switch(C){					 	
					 	case '1': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;								//设置常数符状态 
					 	case '2': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '3': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '4': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '5': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '6': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '7': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '8': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '9': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	case '0': state = 2;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign(""); 
						 		  break;
					 	
					 	case '(': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ')': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ',': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ';': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '+': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '*': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '/': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '-': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '[': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ']': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '=': state = 0;
								  v = v + C;
								  _C_line = x;
							   	  _C_length = y;
								  C = get_char( fp, buffer,model,forward, x, y);
					 		 	  exchange(v,x,y);
								  v.assign("");
								  break;
						case '>': 
								  if(_C=='<'){
									  state = 0;
									  v = v + C;
									  _C_line = x;
								   	  _C_length = y;
									  C = get_char( fp, buffer,model,forward, x, y);
						 		 	  exchange(v,x,y);
									  v.assign("");
								  }
								  else{
								  	state = 0;
									_C_line = x;
								    _C_length = y;
								    C = get_char( fp, buffer,model,forward, x, y);
						 		 	exchange(v,x,y);
									v.assign("");								  	
								  	//
								  }
								  break;
					 	case ' ': state = 0;
					 		  	  exchange(v,_C_line,_C_length);
								  _C_line = x;
							   	  _C_length = y;
								  C = get_char( fp, buffer,model,forward, x, y);
								  v.assign("");
								  break;
					 	default: state = 0;exchange_error("[Operator error.]",x,y);break;  										//error
					}									
				}
				break;
			case 5:																												//检测小数点后的部分 
				v = v + C;
				_C_line = x;
				_C_length = y;
				C = get_char( fp, buffer,model,forward, x, y);
				if(C>='0'&&C<='9'){
					state = 5;
				}
				else if(C=='@'||C>='a'&&C<='z'||C>='A'&&C>='Z'){
					state = 0;
					exchange(v,_C_line,_C_length);
				    v.assign("");
				}
				else{
					switch(C){					 	
					 	case '(': state = 3;
					 		 	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ')': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ',': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ';': state = 3;
					 		 	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '-': state = 3;
					 		 	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '+': state = 3;
					 		 	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '*': state = 3;
					 		 	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '/': state = 3;
					 		 	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '[': state = 3;
					 		  	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ']': state = 3;
					 		 	  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '=': state = 3;
					 			  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case ':': state = 4;
					 			  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '<': state = 4;
					 			  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
						case '>': state = 4;
					 			  exchange(v,_C_line,_C_length);
								  v.assign("");
								  break;
					 	case ' ': state = 0;
					 			  exchange(v,_C_line,_C_length);
								  _C_line = x;
							   	  _C_length = y;
								  C = get_char( fp, buffer,model,forward, x, y);
								  v.assign("");
								  break;
					 	default: state = 0;exchange_error("[Float error.]",x,y);break;  										//error
					}									
				}
				break;				
			default:
				state = 0;
				v.assign("");
				break;
		}
	}
	
}
