#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map> 
#include <string> 
#include <sstream>
#include "token.h"
using namespace std;

#define Buffer_size 100												//输入缓冲区大小,可根据后面进行更改
#define keyWord_length 20

int lines = 1;
int length = 0;
TokenList &tokenList = *new TokenList;    
ErrorList &errorList = *new ErrorList;  
FILE *fp;   									// 测试文件指针 

typedef struct _Buffer{																// 输入缓冲区的结构体 
	int check = 0;
	char standing;	
	int lines = 0;
	int columns = 0;								 
}Buffer;


void Initkey(void);
void Initbuffer(FILE* fp, Buffer* buffer,int &model);
char get_char(FILE* fp,Buffer* buffer,int &model,int &forward, int &x, int & y); 
void Lexer( FILE *fp); 


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


pair<TokenList, ErrorList> lexer(const string filename)
{
    if((fp=fopen(filename.c_str(),"r"))==NULL)
    {
        printf("Test cannot open! \n");
        exit(0);
    }
    else
        printf("Tests start! \n");
	Lexer( fp);  
	return {tokenList, errorList};
}


int main(void){
	lexer("test.txt");
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
