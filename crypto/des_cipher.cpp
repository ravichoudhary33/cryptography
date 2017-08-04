// note: bitset is indexing is from right side

// compile: g++ -std=c++11 des_cipher.cpp
// run:     ./a.out < des_map.txt

#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

typedef vector< bitset<4> > vb;
typedef vector< vb > vvb;

vector< vvb > s(8, vvb(4, vb(16, 0)));
// expansion mapping index
vector<int> ei(48, -1);
// initial permutation & final permuation mapping index
vector<int> ipi(64,-1), fpi(64, -1);
// permuation index mapping
// The P permutation shuffles the bits of a 32-bit half-block.
vector<int> pi(32, -1);
// 56bit key permutation
vector<int> pc_1i(56,-1), pc_2i(48, -1);
// to store number of rotation in specific round
vector<int> rot(16,-1);

// to store the vector of 16 48bit key
vector< bitset<48> > keys;


void preety_print_64bit(bitset<64> t){
    for(int i=63; i>=0; i--){
        cout << t[i];
        if(i%8==0){
            cout << " ";
        }
    }cout << endl;
}

void preety_print_32bit(bitset<32> t){
    for(int i=31; i>=0; i--){
        cout << t[i];
        if(i%4==0){
            cout << " ";
        }
    }cout << endl;
}

void preety_print_28bit(bitset<28> t){
    for(int i=27; i>=0; i--){
        cout << t[i];
        if(i%4==0){
            cout << " ";
        }
    }cout << endl;
}

void preety_print_48bit(bitset<48> t){
    for(int i=47; i>=0; i--){
        cout << t[i];
        if(i%6==0){
            cout << " ";
        }
    }cout << endl;
}

void preety_print_56bit(bitset<56> k){
    for(int i=55; i>=0; i--){
        cout << k[i];
        if(i%7==0){
            cout << " ";
        }
    }cout << endl;
}


// expansion function takes 32 bit as input and return 48 bit
bitset<48> expantion_function(bitset<32> a){
    bitset<48> e;
    for(int i=47; i>=0; i--){
        e[i] = a[ei[i]];
    }
    return e;
}

void get_mapping_from_txt_file(){
    int x;
    // first get sbox mapping
    for(int i=0; i<8; i++){
        for(int j=0; j<4; j++){
            for(int k=0; k<16; k++){
                cin >> x;
                s[i][j][k] = bitset<4>(x);
            }
        }
    }
    // then get mapping for expansion function
    for(int i=47; i>=0; i--){
        cin >> x;
        ei[i] = 32 - x;
    }
    // then get mapping for initial permutation of 64 bit
    for(int i=63; i>=0; i--){
        cin >> x;
        ipi[i] = 64-x;
    }
    // then get mapping for final permutation of 64 bit
    for(int i=63; i>=0; i--){
        cin >> x;
        fpi[i] = 64 - x;
    }
    // get permutation index mapping
    for(int i=31; i>=0; i--){
        cin >> x;
        pi[i] = 32 - x;
    }
    // get permutation index for pc_1 i mapping
    for(int i=55; i>=0; i--){
        cin >> x;
        pc_1i[i] = 64 - x;
    }
    // get permutation index for pc_2 i mapping
    for(int i=47; i>=0; i--){
        cin >> x;
        pc_2i[i] = 56 - x;
    }

    // get number of rotation value in rot vector
    for(int i=0; i<16; i++){
        cin >> x;
        rot[i] = x;
    }

}

// print permutation index mapping
void print_pi_map(){
    for(int i=31; i>=0; i--){
        cout << pi[i] << " ";
    }cout << endl;
}

// for printing the mapping index for expansion function
void print_ei_map(){
    for(int i=47; i>=0; i--){
        cout << ei[i] << " ";
    }cout << endl;
}
// for printing the mapping index for initial permutation
void print_ipi_map(){
    for(int i=63; i>=0; i--){
        cout << ipi[i] << " ";
    }cout << endl;
}

// for printing the mapping index for final permutation ip inverse
void print_fpi_map(){
    for(int i=63; i>=0; i--){
        cout << fpi[i] << " ";
    }cout << endl;
}

void print_sbox(){
    for(int i=0; i<4; i++){
        for(int j=0; j<16; j++){
            cout << s[1][i][j].to_ulong() << " ";
        }cout << endl;
    }
}
// this method take input as plane text as 64bit and return permutation of 64bit input
bitset<64> initial_permutation_function(bitset<64> pt){
    bitset<64> t;
    for(int i=63; i>=0; i--){
        t[i] = pt[ipi[i]];
    }
    return t;
}

// this method take input as plane text as 64bit and return inverse permutation of 64bit input
bitset<64> final_permutation_function(bitset<64> pt){
    bitset<64> t;
    for(int i=63; i>=0; i--){
        t[i] = pt[fpi[i]];
    }
    return t;
}

// function to transform 64bit key to 56bit using pc_1i
bitset<56> apply_pc_1(bitset<64> key){
    bitset<56> tk;
    for(int i=55; i>=0; i--){
        tk[i] = key[pc_1i[i]];
    }
    return tk;
}

// function to transform 56bit key to 48bit using pc_2i
bitset<48> apply_pc_2(bitset<56> key){
    bitset<48> tk;
    for(int i=47; i>=0; i--){
        tk[i] = key[pc_2i[i]];
    }
    return tk;
}

// function to permorm rotational left shift m times
bitset<28> rotational_left_shift(bitset<28> b, int m){
    b = (b << m) | (b >> (28-m));
    return b;
}

// method to get all 16 keys of bitset 48
void get_keys_from_key(bitset<64> key){
    // first apply pc_1
    bitset<56> tk = apply_pc_1(key);
    cout << "\nK = ";
    preety_print_64bit(key);
    cout << "K+ = ";
    preety_print_56bit(tk);
    // to store left 28bit of key and right 28bit key
    bitset<28> c,d;
    // now get c
    for(int j=55; j>=28; j--){
        c[j-28] = tk[j];
    }
    // and get d
    for(int j=27; j>=0; j--){
        d[j] = tk[j];
    }
    // for loop to get all the keys
    for(int i=0; i<16; i++){

        // now apply rotational left shit on c and d using the rot mapping
        c = rotational_left_shift(c, rot[i]);
        d = rotational_left_shift(d, rot[i]);

        // now get 56bit key from c and d
        bitset<56> rk;
        // get left 28bit first
        for(int j=55; j>=28; j--){
            rk[j] = c[j-28];
        }
        // get right 28bit
        for(int j=27; j>=0; j--){
            rk[j] = d[j];
        }
        // now apply pc_2 and get 48bit key and store it to vector
        bitset<48> rrk = apply_pc_2(rk);
        // save the key
        // preety_print_48bit(rrk);

        keys.push_back(rrk);

    }
}


void print_keys(){
    cout << "\n";
    for(int i=0; i<16; i++){
        cout << "K[" << i+1 << "] = ";
        preety_print_48bit(keys[i]);
    }
}

bitset<32> permutation_function(bitset<32> m){
    bitset<32> pm;
    for(int i=31; i>=0; i--){
        pm[i] = m[pi[i]];
    }
    return pm;
}


bitset<32> _f(bitset<32> a, bitset<48> k){
    // step 1 first convert a from 32 bit to 48 to apply xor with k
    bitset<48> e = expantion_function(a);
    cout << "E = ";
    preety_print_48bit(e);

    // step 2 take xor with k
    bitset<48> b = e ^ k;
    vector< bitset<6> > vbs;
    bitset<6> t;
    for(int i=47, j=5; i>=0;){
        while(j>=0){
            t[j--] = b[i--];
        }
        vbs.push_back(t);
        j = 5;
    }

    // step 4 apply sbox funtion on all 8block of 6bit number
    bitset<32> m;
    for(int i=0, j=31; i<8; i++){
        // get row and col to look in sbox mapping from 6bit number
        bitset<2> rb;
        rb[1] = vbs[i][5]; rb[0] = vbs[i][0];
        bitset<4> cb;
        cb[3] = vbs[i][4]; cb[2] = vbs[i][3]; cb[1] = vbs[i][2]; cb[0] = vbs[i][1];
        // get the row and colum
        int r = (int)rb.to_ulong(), c = (int)cb.to_ulong();
        // get the 4bit value using sbox
        bitset<4> t = s[i][r][c];
        for(int k=3; k>=0; k--){
            // save the bit in 32bit m
            m[j--] = t[k];
        }
    }
    // step 5 apply permutation function on 32 bit m
    bitset<32> pm = permutation_function(m);

    return pm;
}

// one round feister cipher function taking 64bit text with 48bit key
bitset<64> feister_cipher(bitset<64> lr, bitset<48> k, int i){
    bitset<32> l, r;
    // get left half 32bit
    for(int i=63; i>=32; i--){
        l[i-32] = lr[i];
    }
    // get right half 32bit
    for(int i=31; i>=0; i--){
        r[i] = lr[i];
    }

    // now apply _f function on 48bit key and 32bit r
    bitset<32> t = _f(r, k);
    // now xor t with l
    t ^= l;

    cout << "\nL[" << i+1 << "] = ";
    preety_print_32bit(r);
    cout << "R[" << i+1 << "] = ";
    preety_print_32bit(t);

    // now form 64 bit number with r as left 32bit and t as right 32bit
    bitset<64> nlr;
    for(int i=63; i>=32; i--){
        nlr[i] = r[i-32];
    }
    // fill the right half 32bit with t
    for(int i=31; i>=0; i--){
        nlr[i] = t[i];
    }
    // return new 64bit lr
    return nlr;
}
// function to get pre output
bitset<64> get_pre_output(bitset<64> _ip){
    bitset<32> l, r;
    // get left half 32bit
    for(int i=63; i>=32; i--){
        l[i-32] = _ip[i];
    }
    // get right half 32bit
    for(int i=31; i>=0; i--){
        r[i] = _ip[i];
    }
    // now put r in left half of _ip and l in right half
    for(int i=63; i>=32; i--){
        _ip[i] = r[i-32];
    }
    for(int i=31; i>=0; i--){
        _ip[i] = l[i];
    }
    return _ip;
}

void des_cipher(bitset<64> pt, bitset<64> key){
    // step 1 first get all keys from key
    get_keys_from_key(key);
    print_keys();
    // step 2 perform initial permutation function
    cout << "\nMSG = ";
    preety_print_64bit(pt);
    bitset<64> _ip = initial_permutation_function(pt);
    cout << "IP = ";
    preety_print_64bit(_ip);

    // step 3 perform feister_cipher operation for 16 times
    for(int i=0; i<16; i++){
        _ip = feister_cipher(_ip, keys[i],i);
    }
    // now to find pre output swap left and right halfs
    _ip = get_pre_output(_ip);
    // step 4 perform inverse permutation_function
    _ip = final_permutation_function(_ip);
    // _ip is the encrypted text
    cout << "ENCRYPTED MSG = ";
    preety_print_64bit(_ip);
    cout << "\n";
}


int main(){

    get_mapping_from_txt_file();

    bitset<64>msg("0000000100100011010001010110011110001001101010111100110111101111");
    bitset<64>key("0001001100110100010101110111100110011011101111001101111111110001");
    des_cipher(msg, key);

    return 0;
}
