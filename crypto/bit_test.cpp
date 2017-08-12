#include <iostream>
#include <bitset>

using namespace std;

int main(){
    bitset<8> a(1), b(2);
    a <<= 1;
    bitset<8> c = a & b;
    if(c.to_ulong() >  0){
        cout << "hello" << endl;
    }else{
        cout << "world" << endl;
    }
    return 0;
}
