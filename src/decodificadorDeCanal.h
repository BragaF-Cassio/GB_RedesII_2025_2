#ifndef DECODIFICADOR_DE_CANAL_H
#define DECODIFICADOR_DE_CANAL_H

// Manchester / Bifase-L
vector<bool> decodificadorDeCanal(vector<float> input){
    vector<bool> output;
    for(int i = t/2; i < input.size(); i += 2*t){
        output.push_back(input[i] - input[i + t] < 0);
    }
    return output;
}

#endif