#include <string>
#include <iostream>
#include <vector>
using namespace std;

const int t = 4;
const float v = 5.0;

#include "print.h"
#include "codificadorDeFonte.h"
#include "codificadorDeCanal.h"
#include "decodificadorDeCanal.h"
#include "decodificadorDeFonte.h"

vector<float> transmissor(string ASCII){
    vector<bool> binario = codificadorDeFonte(ASCII);
    print(binario);
    vector<float> bifase = codificadorDeCanal(binario);

    return bifase;
}

string receptor(vector<float> sinal){
    vector<bool> binario = decodificadorDeCanal(sinal);
    print(binario);
    string ASCII = decodificadorDeFonte(binario);

    return ASCII;
}

int main(){
    string mensagemOriginal;
    cout << "Digite a mensagem: ";
    getline(cin, mensagemOriginal);
    print(mensagemOriginal);

    vector<float> sinalTransmitido = transmissor(mensagemOriginal);

    vector<float> sinalRecebido = sinalTransmitido; // aplicar ruído

    string mensagemRecuperada = receptor(sinalRecebido);
    print(mensagemRecuperada);

    if(mensagemRecuperada != mensagemOriginal) cout << "Erro";
    return 0;
}
