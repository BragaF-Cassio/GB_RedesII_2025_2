#ifndef DECODIFICADOR_DE_FONTE_H
#define DECODIFICADOR_DE_FONTE_H

string decodificadorDeFonte(const vector<bool>& input){
    string output;
    for(int bit = 0; bit < input.size();){
        char c = 0;
        for(int i = 7; i >= 0; --i, ++bit){
            c |= (input[bit] << i);
        }
        output += c;
    }
    return output;
}

#endif