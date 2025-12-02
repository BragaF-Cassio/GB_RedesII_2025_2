#ifndef CODIFICADOR_DE_CANAL_H
#define CODIFICADOR_DE_CANAL_H

#include <string>
#include <vector>
#include "shared.h"

using namespace std;

// Manchester / Bifase-L
vector<float> codificadorDeCanal(vector<bool> input){
    vector<float> output;
    for(bool b : input){
        if(b){
            output.insert(output.end(), T_CONSTANT, -MAX_VOLTAGE_LEVEL);
            output.insert(output.end(), T_CONSTANT, +MAX_VOLTAGE_LEVEL);
        }else{
            output.insert(output.end(), T_CONSTANT, +MAX_VOLTAGE_LEVEL);
            output.insert(output.end(), T_CONSTANT, -MAX_VOLTAGE_LEVEL);
        }
    }
    return output;
}

#endif