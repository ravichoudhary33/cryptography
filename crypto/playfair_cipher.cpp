// Name: Ravi Kumar Choudhary
// Roll No: 13MA20033

#include <iostream>
#include <string>
#include <map>

using namespace std;

void print_mat(char key_mat[][7]){
    for(int i=0; i<5; i++){
        for(int j=0; j<5; j++){
            cout << key_mat[i][j] << " ";
        }cout << "\n";
    }
}

string remove_dup_char(string &kw, char key_mat[][7]){
    string s = "";
    for(int i=0; i<kw.size();){
        s += kw[i];
        while(kw[i] == kw[i+1]){
            i+=1;
        }
        i++;
    }
    return s;
}


void build_matrix(string kw, char key_mat[][7]){
    string u_kw = "";
    //string u_kw = remove_dup_char(kw, key_mat);
    //cout << u_kw << "\n";

    // initialize boolean for lookup
    bool seen_kw[26] = {false};
    bool seen[26] = {false};
    bool taken[26] = {false};

    // set the boolean values for seen char
    for(int i=0; i<kw.size(); i++){
        seen_kw[kw[i]-'a'] = true;
    }

    cout << "seen_kw\n";
    for(int i=0; i<26; i++){
        cout << seen_kw[i] << " ";
    }cout << "\n";

    // by using above arrays removing duplicates
    for(int i=0; i<kw.size(); i++){
        if(seen_kw[kw[i]-'a']){
            u_kw += kw[i];
            seen_kw[kw[i]-'a'] = false;
        }
    }
    cout << "kw = " << u_kw << "\n";

    for(int i=0; i<u_kw.size(); i++){
        seen[u_kw[i]-'a'] = true;
    }

    for(int i=0; i<26; i++){
        cout << seen[i] << " ";
    }cout << "\n";

    char c = 'a';

    for(int i=0,k=0; i<5; i++){
        for(int j=0; j<5; j++){
            key_mat[i][j] = u_kw[k++];
            if(k > u_kw.size()){
                while(seen[c-'a'] || c == 'j') {
                    c++;
                }
                key_mat[i][j] = c++;
            }
        }
    }
}

map< char, pair<int,int> > get_position_of_char_in_matrix(char key_mat[][7]){
    map< char,pair<int,int> > pos;
    for(int i=0; i<5; i++){
        for(int j=0; j<5; j++){
            pos[key_mat[i][j]] = make_pair(i,j);
        }
    }
    return pos;
}

string encode(char c1, char c2, map< char, pair<int,int> > &pos, char key_mat[][7]){
    pair<int,int> p1,p2;
    string es = "";
    p1 = pos[c1];
    p2 = pos[c2];
    if(p1.first == p2.first){
        //same row
        es += key_mat[p1.first][(p1.second + 1)%5];
        es += key_mat[p2.first][(p2.second + 1)%5];
    }else if(p1.second == p2.second){

        es += key_mat[(p1.first + 1)%5][p1.second];
        es += key_mat[(p2.first + 1)%5][p2.second];
    }else {
        // rectangle case
        es += key_mat[p1.first][p2.second];
        es += key_mat[p2.first][p1.second];
    }
    cout << "encode: " << es << "\n";
    return es;
}

string decode(char c1, char c2, map< char, pair<int,int> > &pos, char key_mat[][7]){
    pair<int,int> p1,p2;
    string es = "";
    p1 = pos[c1];
    p2 = pos[c2];
    if(p1.first == p2.first){
        //same row
        es += key_mat[p1.first][(p1.second - 1 + 5)%5];
        es += key_mat[p2.first][(p2.second - 1 + 5)%5];
    }else if(p1.second == p2.second){

        es += key_mat[(p1.first - 1 + 5)%5][p1.second];
        es += key_mat[(p2.first - 1 + 5)%5][p2.second];
    }else {
        // rectangle case
        es += key_mat[p1.first][p2.second];
        es += key_mat[p2.first][p1.second];
    }
    cout << "decode: " << es << "\n";
    return es;
}

string encrypt(string message, map< char, pair<int,int> > &pos, char key_mat[][7]){
    string pm = "";
    string encrypted_message = "";
    for(int i=0,j=0; i<message.size(); ){
        if(message[i] != message[i+1]){
            string es = encode(message[i], message[i+1], pos, key_mat);
            encrypted_message += es;

            pm += message[i];
            pm += message[i+1];
            i+=2;
        }else{
            string es = encode(message[i],'x', pos, key_mat);
            encrypted_message += es;

            pm += message[i];
            pm += 'x';
            i++;
        }
        // pm += " ";
        // encrypted_message += " ";
    }
    cout << "paired message: " << pm << "\n";
    return encrypted_message;
}

string decrypt(string secret_message, map< char, pair<int,int> > &pos, char key_mat[][7]){
    string decrypted_message = "";
    for(int i=0; i<secret_message.size(); i+=2){
        string ds = decode(secret_message[i], secret_message[i+1], pos, key_mat);
        decrypted_message += ds;
        cout << decrypted_message << "\n";
    }
    cout << "end of the decryption\n";
}

// assumption no j in string
int main(){
    char key_mat[7][7];
    string key;
    cout << "Enter the key\n";
    cin >> key;
    //cout << remove_dup_char("keywoordddssssssss");
    build_matrix(key, key_mat);
    print_mat(key_mat);
    map< char, pair<int,int> > pos = get_position_of_char_in_matrix(key_mat);
    map< char, pair<int,int> >::iterator itr = pos.begin();
    // while(itr != pos.end()){
    //     cout << itr->first << ", " << itr->second.first << " " << itr->second.second << "\n";
    //     itr++;
    // }
    string message;
    cout << "Enter your secret message\n";
    cin >> message;
    string encrypted_message = encrypt(message, pos, key_mat);
    cout << encrypted_message << "\n";
    // string decrypted_message = decrypt(encrypted_message, pos, key_mat);
    // cout << decrypted_message << "\n";
    // cout << decrypt(encrypted_message, pos, key_mat) << "\n";

    return 0;
}
