// Instruction: make sure aes_sbox.txt file is in same directory of this file
// In this code key string and plain text string is hard coded
// if you want to use your key and string make sure to change the corresponding string value
// also make sure that both the string should be of 16 ASCII characters long
 
// to compile execute : g++ aes_cipher.cpp
// ro run             : ./a.out

#include <iostream>
#include <vector>
#include <bitset>
#include <cstdio>

using namespace std;

typedef vector< bitset<8> > vb;
// vector to store the sbox matrix
vector< vb > sbox(16, vb(16, 0));
vector< vb > tm(4, vb(4, 0));
vector< bitset<32> > rcon(10, 0);

int get_value_of_hex(char c){
    switch(c){
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'a':
            return 10;
        case 'b':
            return 11;
        case 'c':
            return 12;
        case 'd':
            return 13;
        case 'e':
            return 14;
        case 'f':
            return 15;
        default:
            return 0;
    }
}

void print_sbox(){
    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            cout << sbox[i][j].to_string() << " ";
        }cout << endl;
    }
}

void print_s(const vector< vb > &s){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            cout << s[i][j].to_string() << " ";
        }cout << endl;
    }
}

// given string of length 2 contain sbox value convert to 8bit
bitset<8> get_bitset_value(string ts){
    int c1 = get_value_of_hex(ts[0]), c2 = get_value_of_hex(ts[1]);
    // convert to bitset
    bitset<4> b1(c1);
    bitset<4> b2(c2);
    // concate bitset
    bitset<8> b;
    for(int i=7, j=3; i >= 4 && j >= 0; i--){
        b[i] = b1[j--];
    }
    // get next 4 bit from b2
    for(int i = 3; i >= 0; i--){
        b[i] = b2[i];
    }
    return b;
}

// method to get sbox matrix and save it to sbox vector
void get_sbox_from_file(){
    // open aes_sbox file in read only mode
    freopen("aes_sbox.txt", "r", stdin);
    // get the values and save it to sbox
    string ts;
    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            cin >> ts;
            sbox[i][j] = get_bitset_value(ts);
        }
    }
    // also get transformation matrix
    int x;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            cin >> x;
            tm[i][j] = bitset<8>(x);
        }
    }
    // get rcon value from the text file
    for(int i=0; i<10; i++){
        cin >> ts;
        string st = "";
        for(int j=0; j<8; j++){
            int y = get_value_of_hex(ts[j]);
            bitset<4> bst(y);
            st += bst.to_string();
        }
        bitset<32> wb(st);
        rcon[i] = wb;
    }
}

vector< vb > substitute_bytes(const vector< vb > &s){
    vector< vb > sd(4, vb(4, 0));
    // read the value of s & get corresponding mapping from sbox
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            bitset<8> t = s[i][j];
            // get left half 4bit and right half 4bit to get row and col
            bitset<4> t1, t2;
            for(int k=3; k>=0; k--){
                t1[k] = t[k+4];
            }
            for(int k=3; k>=0; k--){
                t2[k] = t[k];
            }
            // now convert biset to integer to get row and col to look in sbox
            int r = (int)t1.to_ulong(), c = (int)t2.to_ulong();
            bitset<8> tt = sbox[r][c];
            // now save this value to s dash
            sd[i][j] = tt;
        }
    }
    return sd;
}
// method to apply shift row operation of s dash
vector < vb > shift_row_transformation(const vector < vb > &sd){
    vector< vb > ssd(4, vb(4, 0));
    // keep first row as it is
    // for(int j=0; j<4; j++){
    //     ssd[0][j] = sd[0][j];
    // }
    // apply cyclic rotational shift
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            ssd[i][j] = sd[i][(j+i)%4];
        }
    }
    return ssd;
}

// method for galoid multiplication of two byte i.e bitset<8>
bitset<8> gmul(bitset<8> a, bitset<8> b){
    bitset<8> p;
    bitset<8> hi_bit_set;

    for(int i=0; i<8; i++){
        bitset<8> t = b & bitset<8>(1);
        if(t.to_ulong() != 0){
            p ^= a;
        }
        hi_bit_set = a & bitset<8>(0x80);
        a <<= 1;
        if(hi_bit_set.to_ulong() != 0){
            a ^= bitset<8>(0x1B);
        }
        b >>= 1;
    }
    return p;
}

vector < vb > mix_column_tranformation(const vector < vb > &ssd){
    vector< vb > mct(4, vb(4, 0));
    // use transformation matrix tm and apply matrix field multiplication with ssd
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            for(int k=0; k<4; k++){
                mct[i][j] ^= gmul(tm[i][k], ssd[k][j]);
            }
        }
    }
    return mct;
}

vector < vb > matrix_xor(const vector < vb > &a, const vector< vb > &b){
    vector < vb > c(4, vb(4, 0));
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            c[i][j] = a[i][j] ^ b[i][j];
        }
    }
    return c;
}
// method for meging four eight bit into vector of four eight bit
bitset<32> get_word(bitset<8> a, bitset<8> b, bitset<8> c, bitset<8> d){
    // convert all 8 bit into string then concat all 4 string
    string s1 = a.to_string();
    string s2 = b.to_string();
    string s3 = c.to_string();
    string s4 = d.to_string();
    string s = s1 + s2 + s3 + s4;
    // convert concated string to bitset of 32 bit
    bitset<32> tw(s);

    return tw;
}
// function to do one left circular shift of 8bit on t
bitset<32> rot_word(bitset<32> t){
    t = (t << 8) | (t >> 24);
    return t;
}

// substitution function on 32bit word
bitset<32> sub_word(bitset<32> t){
    vector< bitset<8> > vt;
    string stt = "";
    for(int i=31; i>=0; i-=8){
        bitset<8> tp;
        for(int j=7; j>=0; j--){
            tp[j] = t[i+j-7];
        }
        vt.push_back(tp);
    }
    // now we have vector of 8bit of size 4 = 32bit
    for(int i=0; i<4; i++){
        bitset<8> tt = vt[i];

        // now get left half four bit and right half four bit
        bitset<4> t1, t2;
        for(int j=7; j>=4; j--){
            t1[j-4] = tt[j];
        }
        // get right half four bit
        for(int j=3; j>=0; j--){
            t2[j] = tt[j];
        }
        // get row and column
        int r = (int)t1.to_ulong(); int c = (int)t2.to_ulong();
        bitset<8> sv = sbox[r][c];
        // cout << "r: " << r << ", c: " << c << ", sv: " << sv.to_string() << endl;
        stt += sv.to_string();
    }
    bitset<32> wt(stt);
    return wt;
}

// key expansion method to generate round keys from key
vector< bitset<32> > key_expansion(const vector< bitset<8> > &key){
    // get w0, w1 , w2, w3
    vector<string> vbs(44, "");
    vector < bitset<32> > w(44, 0);
    for(int i=0; i<4; i++){
        w[i] = get_word(key[4*i], key[4*i+1], key[4*i+2], key[4*i+3]);
        vbs[i] = w[i].to_string();
    }
    // get all w from 0 to 43
    for(int i = 4; i < 44; i++){
        // vb is vector of 8bit
        bitset<32> temp = w[i-1];
        if(i%4==0){
            bitset<32> rw = rot_word(temp);
            bitset<32> sw = sub_word(rw);
            // cout << "rw: " << rw.to_string() << ", sw: " << sw.to_string() << "rcon: " << rcon[i/4-1].to_string() << endl;
            temp = sw ^ rcon[i/4-1];
        }
        w[i] = w[i-4] ^ temp;
        vbs[i] = w[i].to_string();
    }

    // for(int i=0; i<44; i++){
    //     cout << "w[" << i << "] = " << vbs[i] << endl;
    // }

    return w;
}

int main(){
    string key, pt;

    // change the key and plane text value of string according to your need
    // but make sure that of the them should be of 16 ASCII characters long
    key = "Thats my Kung Fu";
    pt = "Two One Nine Two";

    // cout << "Enter 16 ASCII characters Key, 1byte each\n";
    // getline(cin, key);
    //
    // while(key.size() != 16){
    //     cout << "Please, Enter 16 ASCII characters Key only, 1byte each\n";
    //     getline(cin, key);
    // }
    //
    // cout << "Enter 16 ASCII characters Plain Text, 1byte each\n";
    // getline(cin, pt);
    //
    // while(pt.size() != 16){
    //     cout << "Please, Enter 16 ASCII characters Plain Text only, 1byte each\n";
    //     getline(cin, pt);
    // }

    cout << "\n[Key] = " << key << endl;
    cout << "[Message] = " << pt << endl;

    // convert each char of key to bitset and store in vts is vector of size 16 of bitset<8>
    vector< bitset<8> > vts;
    for(int i=0; i<key.size(); i++){
        bitset<8> bt(key[i]);
        vts.push_back(bt);
    }

    // convert each char of plane text to bitset and store in vtsp
    vector< bitset<8> > vtsp;
    for(int i=0; i<pt.size(); i++){
        bitset<8> bt(pt[i]);
        vtsp.push_back(bt);
    }

    // to print the 128 bit key
    cout << "\n[128bit encoded key]\n";
    for(int i=0; i<vts.size(); i++){
        cout << vts[i].to_string() << " ";
    }cout << endl;

    // to print the 128 bit message
    cout << "\n[128bit encoded message]\n";
    for(int i=0; i<vtsp.size(); i++){
        cout << vtsp[i].to_string() << " ";
    }cout << endl;

    // now save vts into a 4 by 4 matrix column wise
    vector< vb > s(4, vb(4, 0));
    int k = 0;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            s[j][i] = vts[k++];
        }
    }

    // now save vtsp into a 4 by 4 matrix column wise
    vector< vb > sp(4, vb(4, 0));
    int _k = 0;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            sp[j][i] = vtsp[_k++];
        }
    }

    // display s
    cout << "\n[encoded key matrix of 4 by 4 each element is of 8bit]\n";
    print_s(s);
    cout << "\n[encoded plain text matrix of 4 by 4 each element is of 8bit]\n";
    print_s(sp);

    // get sbox value from file and print
    get_sbox_from_file();
    cout << "\n[sbox matrix of quick look up]\n";
    print_sbox();

    // first do matrix xor in s and sp
    s = matrix_xor(s, sp);
    cout << "\n[after applying matrix xor on encoded key matrix and plain text matrix]\n";
    print_s(s);
    // start point of algorithm
    // step 1: substitute bytes
    vector< vb > sd = substitute_bytes(s);
    cout << "\n[after applying substitute_bytes on new state matrix this is S']\n";
    print_s(sd);

    // step 2: shift row transformation on sd
    vector< vb > ssd = shift_row_transformation(sd);
    cout << "\n[after applying row shift on S' matrix]\n";
    print_s(ssd);

    // print_s(tm);
    // step 3: mix column transformation
    vector< vb > mct = mix_column_tranformation(ssd);
    // print mct
    cout << "\n[after applying mix column transformation on new state matrix]\n";
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            cout << mct[i][j].to_string() << " ";
        }cout << endl;
    }

    // get word vector from the key
    vector< bitset<32> > vw =  key_expansion(vts);
    //cout << "vw size: " << vw.size() << endl;
    // first round key
    string sfrk = "";
    for(int i=4; i<=7; i++){
        sfrk += vw[i].to_string();
    }
    bitset<128> frk(sfrk);
    // cout << "frk: " << frk.to_string() << endl;
    // first convert 128 bit into vector of 8bit of size 16
    vector< bitset<8> > vfrk;
    for(int i=127; i>=0; i-=8){
        bitset<8> tp;
        for(int j=7; j>=0; j--){
            tp[j] = frk[i+j-7];
        }
        // cout << tp.to_string() << " ";
        vfrk.push_back(tp);
    }cout << endl;

    // pritn vfrk
    cout << "\n[first round key vector of 8bit]" << endl;
    for(int i=0; i<16; i++){
        cout << vfrk[i].to_string() << " ";
    }cout << endl;

    // convert frk into matrix form
    vector< vb > frkm(4, vb(4, 0));
    int h = 0;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            frkm[j][i] = vfrk[h++];
        }
    }
    cout << "\n[first round key in matrix form]" << endl;
    print_s(frkm);

    // again take matrix xor
    mct = matrix_xor(mct, frkm);
    cout << "\n[xor yield of mix column transformation matrx with first round key matrix]\n";
    print_s(mct);

    cout << "\n[AES after one round]" << endl;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            cout << mct[j][i] << " ";
        }
    }cout << "\n\n";
    // bitset<8> u1(3);
    // bitset<8> u2(string("01101110"));
    // bitset<8> ttt = gmul(u1, u2);
    // cout << ttt.to_string() << endl;

    // char c = 'T';
    // bitset<8> b(c);
    // cout << b.to_string() << endl;

    return 0;
}
