#include <iostream>
#include <vector>
#include <bitset>
#include <cstdio>

using namespace std;

typedef vector< bitset<8> > vb;
// vector to store the sbox matrix
vector< vb > sbox(16, vb(16, 0));
vector< vb > tm(4, vb(4, 0));

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

int main(){
    string key, pt;
    cout << "Enter 16 ASCII characters Key, 1byte each\n";
    getline(cin, key);

    while(key.size() != 16){
        cout << "Please, Enter 16 ASCII characters Key only, 1byte each\n";
        getline(cin, key);
    }

    cout << "Enter 16 ASCII characters Plain Text, 1byte each\n";
    getline(cin, pt);

    while(pt.size() != 16){
        cout << "Please, Enter 16 ASCII characters Plain Text only, 1byte each\n";
        getline(cin, pt);
    }

    // convert each char to bitset and store in vts
    vector< bitset<8> > vts;
    for(int i=0; i<key.size(); i++){
        bitset<8> bt(key[i]);
        vts.push_back(bt);
    }

    // convert each char to bitset and store in vts
    vector< bitset<8> > vtsp;
    for(int i=0; i<pt.size(); i++){
        bitset<8> bt(pt[i]);
        vtsp.push_back(bt);
    }

    // to print the 128 bit message
    for(int i=0; i<vts.size(); i++){
        cout << vts[i].to_string() << " ";
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
    cout << "s\n";
    print_s(s);
    cout << "sp\n";
    print_s(sp);

    // get sbox value from file and print
    get_sbox_from_file();
    print_sbox();

    // first do matrix xor in s and sp
    s = matrix_xor(s, sp);
    cout << "after matrix xor s\n";
    print_s(s);
    // start point of algorithm
    // step 1: substitute bytes
    vector< vb > sd = substitute_bytes(s);
    cout << "substitute_bytes\n";
    print_s(sd);

    // step 2: shift row transformation on sd
    vector< vb > ssd = shift_row_transformation(sd);
    cout << "row shift\n";
    print_s(ssd);

    // print_s(tm);
    // step 3: mix column transformation
    vector< vb > mct = mix_column_tranformation(ssd);
    // print mct
    cout << "mct\n";
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            cout << mct[i][j].to_string() << " ";
        }cout << endl;
    }

    // bitset<8> u1(3);
    // bitset<8> u2(string("01101110"));
    // bitset<8> ttt = gmul(u1, u2);
    // cout << ttt.to_string() << endl;

    // char c = 'T';
    // bitset<8> b(c);
    // cout << b.to_string() << endl;

    return 0;
}
