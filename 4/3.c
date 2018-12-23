// 3.c
typedef int A[10][20];
typedef int B[20];
A a;
B *fun(){
    return (a);
}
int main(){
    fun();
}