// Name: Ravi Kumar Choudhary
// Roll No: 13MA20033


// Requirement c++11 support
// compile: g++ -std=c++11 spn_block_cipher.cpp

/***************SAMPLE OUTPUT*************************/
// Enter the plain text: 0010011010110111
// Message: 0010011010110111
// Enter the key: 00111010100101001101011000111111
// Key: 00111010100101001101011000111111
// k[1] = 0011101010010100 k[2] = 1010100101001101 k[3] = 1001010011010110 k[4] = 0100110101100011 k[5] = 1101011000111111
// w[0] = 0010011010110111
// K[1] = 0011101010010100
// u[1] = 0001110000100011
// v[1] = 0100010111010001
// w[1] = 0010111000000111
// K[2] = 1010100101001101
// u[2] = 1000011101001010
// v[2] = 0011100000100110
// w[2] = 0100000110111000
// K[3] = 1001010011010110
// u[3] = 1101010101101110
// v[3] = 1001111110110000
// w[3] = 1110010001101110
// k[4] = 0100110101100011
// u[4] = 1010100100001101
// v[4] = 0110101011101001
// Encrypted Text: 1011110011010110


#include <iostream>
#include <bitset>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

void convert_string_to_binary(string my_string){
    for(size_t i = 0; i < my_string.size(); i++){
        cout << bitset<8>(my_string.c_str()[i]) << "\n";
    }
}

string binary_str_to_hex(string bs){
    bitset<4> bset(bs);
    stringstream res;
    res << hex << uppercase << bset.to_ulong();
    return res.str();
}

string hex_char_to_binary(char s){
    switch(s)
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
    }

}

string pi_s(string zbs){
    string hs = binary_str_to_hex(zbs);
    switch(hs[0]){
        case '0': return hex_char_to_binary('E');
        case '1': return hex_char_to_binary('4');
        case '2': return hex_char_to_binary('D');
        case '3': return hex_char_to_binary('1');
        case '4': return hex_char_to_binary('2');
        case '5': return hex_char_to_binary('F');
        case '6': return hex_char_to_binary('B');
        case '7': return hex_char_to_binary('8');
        case '8': return hex_char_to_binary('3');
        case '9': return hex_char_to_binary('A');
        case 'A': return hex_char_to_binary('6');
        case 'B': return hex_char_to_binary('C');
        case 'C': return hex_char_to_binary('5');
        case 'D': return hex_char_to_binary('9');
        case 'E': return hex_char_to_binary('0');
        case 'F': return hex_char_to_binary('7');
    }
}

int pi_p(int z){
    // int z = stoi(zbs,nullptr, 2);
    switch(z){
        case 1: return 1;
        case 2: return 5;
        case 3: return 9;
        case 4: return 13;
        case 5: return 2;
        case 6: return 6;
        case 7: return 10;
        case 8: return 14;
        case 9: return 3;
        case 10: return 7;
        case 11: return 11;
        case 12: return 15;
        case 13: return 4;
        case 14: return 8;
        case 15: return 12;
        case 16: return 16;
    }
}
// key is 32 bit string of 0's and 1's
string spn_cipher_encryption(string X, string key, int l, int m, int r){
    string Y;

    // first get all key
    vector<bitset<16>> ks;
    for(int i = 1; i <= 5; i++){
        string s = key.substr(4*i-4, 16);
        bitset<16> k(s);
        ks.push_back(k);
    }
    // print bitset
    for(int i = 0; i < ks.size(); i++){
        cout << "k[" << i+1 << "] = " << ks[i] << " ";
    }cout << endl;
    // get x as bitset and store in w0
    bitset<16> w0(X), u0, v0;
    for(int i=1; i<= r-1; i++){
        // cout << "w0: " << w0 << ", ks[" << i-1 << "]:" << ks[i-1] << endl;
        cout << "w[" << i-1 << "] = " << w0 << endl;
        cout << "K[" << i << "] = " << ks[i-1] << endl;
        u0 = w0 ^ ks[i-1];
        // cout << "xor: " << u0 << endl;
        // convert u0 to t
        string t = u0.to_string<char, string::traits_type, string::allocator_type>();
        cout << "u[" << i << "] = " << t << endl;
        string v0 = "";
        for(int j=1; j <= m; j++){
            // take 4bit out of 16bit and do pi_s
            string tp = t.substr(4*j-4, 4);
            v0 += pi_s(tp);
        }
        cout << "v[" << i << "] = " << v0 << endl;
        string wt = "";
        for(int k=0; k<v0.size(); k++){
            int tt = pi_p(k+1);
            wt += v0[tt-1];
        }
        //cout << "wt: " << wt << endl;
        // again assign bitset of wt to w0
        w0 = bitset<16>(wt);
    }
    // xor with wr-1 and kr
    // cout << "w0: " << w0 << ", ks[" << r << "]:" << ks[r-1] << endl;
    cout << "w[" << r-1 << "] = " << w0 << endl;
    cout << "k[" << r << "] = " << ks[r-1] << endl;

    bitset<16>ur = w0 ^ ks[r-1];
    cout << "u[" << r << "] = " << ur << endl;
    // convert ur to string
    string vr = "";
    string t = ur.to_string<char, string::traits_type, string::allocator_type>();
    for(int j=1; j<=m; j++){
        string tp = t.substr(4*j-4, 4);
        vr += pi_s(tp);
    }
    cout << "v[" << r << "] = " << vr << endl;
    bitset<16>y = bitset<16>(vr) ^ ks[r];
    Y = y.to_string<char, string::traits_type, string::allocator_type>();
    cout << "Encrypted Text: " << Y << endl;
    return Y;

}

int main(){
    string X; // x is plain text message;
    string key;
    cout << "Enter the plain text:\n";
    cin >> X;
    cout << "Message: " << X << endl;
    cout << "Enter the key:\n";
    cin >> key;
    cout << "Key: " << key << endl;
    int l, m , r;
    l = 4; m = 4; r = 4;
    spn_cipher_encryption(X, key, l, m, r);

    return 0;
}
