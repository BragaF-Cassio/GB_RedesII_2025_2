#ifndef CODIFICADOR_DE_CANAL_H
#define CODIFICADOR_DE_CANAL_H

// Manchester / Bifase-L
vector<float> codificadorDeCanal(vector<bool> input){
    vector<float> output;
    for(bool b : input){
        if(b){
            output.insert(output.end(), t, -v);
            output.insert(output.end(), t, +v);
        }else{
            output.insert(output.end(), t, +v);
            output.insert(output.end(), t, -v);
        }
    }
    return output;
}

#endif