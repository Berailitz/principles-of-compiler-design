// 2.c
typedef int A[10][20];
A a;
A *fun(){
    return (&a);
}
int main(){
    fun();
}