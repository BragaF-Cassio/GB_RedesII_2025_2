#ifndef DECODIFICADOR_DE_CANAL_H
#define DECODIFICADOR_DE_CANAL_H

#include <string>
#include <vector>

using namespace std;

// Manchester / Bifase-L
vector<bool> decodificadorDeCanal(vector<float> input){
    vector<bool> output;
    for(int i = T_CONSTANT/2; i < input.size(); i += 2*T_CONSTANT){
        output.push_back(input[i] - input[i + T_CONSTANT] < 0);
    }
    return output;
}

#endif