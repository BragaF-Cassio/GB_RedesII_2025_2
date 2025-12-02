#ifndef CODIFICADOR_DE_FONTE_H
#define CODIFICADOR_DE_FONTE_H

#include <string>
#include <vector>

using namespace std;

vector<bool> codificadorDeFonte(const string& input){
    vector<bool> output;
    for(unsigned char c : input){
        for (int i = 7; i >= 0; --i){
            output.push_back((c >> i) & 1);
        }
    }
    return output;
}

#endif