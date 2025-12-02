#ifndef PRINT_H
#define PRINT_H

void print(string s){
    cout << '\n' + s + '\n';
}

void print(vector<bool> bits){
    string s;
    for(bool b : bits){
        s += '0' + b;
    }
    print(s);
}

#endif