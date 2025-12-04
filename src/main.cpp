/**
 * Redes de Computadores: Internetworking, Roteamento e Transmissão
 * Integrantes:
 *  Cássio F. Braga
 *  Gabriel C. Walber 
 * 
 * Prof: Cristiano B. Both
 * 
 * Objetivo: Desenvolver e aplicar os conceitos de codificação de canal e modulação digital estudados na 
 *   disciplina de Redes de Computadores, analisando seu impacto na taxa de erro de bits (BER) 
 *   e na eficiência espectral de sistemas de comunicação.
 * 
 */

#include "main.h"

using namespace std;

#define VISIBILITY_MULTIPLIER 10
#define SENDER_RECEIVER_HEIGHT 500
#define SENDER_RECEIVER_WIDTH 630

#define SAMPLE_RATE 1000        // Samples por segundo
#define F_CARRIER 100           // Frequência da portadora

// Protótipos de função
vector<float> moduladorASK(const vector<float>& sinal_codificado);
vector<float> demoduladorASK(const vector<float>& sinal_recebido);
vector<float> moduladorBPSK(const vector<float>& sinal_codificado);
vector<float> demoduladorBPSK(const vector<float>& sinal_recebido);
vector<float> moduladorQPSK(const vector<float>& sinal_codificado);
vector<float> demoduladorQPSK(const vector<float>& sinal_recebido);

enum ModulationType {
  BPSK,
  QPSK,
  ASK,
  MAX_MODULATION_TYPES
};

// Main code
int main(int, char**)
{
    // Configuração do SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Cria janela com contexto gráfico
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("Analisador de Graficos - Redes II", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Configura estilo de cores
    ImGui::StyleColorsDark();

    // Configura escala
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Configura backend e plataforma do ImGui
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    float noise_level = 0.0f;
    enum ModulationType current_modulation_type = BPSK; // 0: BPSK, 1: QPSK
    unsigned long total_bits = 0;
    unsigned long error_bits = 0;
    double current_time = 0.0;
    double last_time = 0.0;
    double ber = 0.0;
    string ASCII = "abc";

    // Main loop
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        vector<bool> binario_transmite;
        vector<float> samples_codificador_fonte;
        vector<float> bifase_codificador;
        vector<float> samples_codificador_canal;
        vector<float> samples_modulador_psk;
        vector<float> sinal_transmitido;

        vector<float> canal_ruido;

        vector<float> sinal_recebido_com_ruido;
        vector<float> bifase_demodulador_psk;
        vector<float> samples_demodulador_psk;
        vector<bool> binario_recebido;
        vector<float> samples_decodificador_canal;

        // Janela de gráficos do transmissor
        {
            ImGui::SetNextWindowSize(ImVec2(SENDER_RECEIVER_WIDTH, SENDER_RECEIVER_HEIGHT), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
            ImGui::Begin("Janela do Transmissor", nullptr, ImGuiWindowFlags_NoCollapse);

            // Fonte = Mensagem de input do usuário
            ImGui::InputText("Mensagem", &ASCII);

            // Codificador de Fonte = Transforma os símbolos em bits conforme tabela ASCII
            binario_transmite = codificadorDeFonte(ASCII);
            for (bool bit : binario_transmite) {
                for(int i = 0; i < VISIBILITY_MULTIPLIER; ++i)  // Repete cada bit VISIBILITY_MULTIPLIER vezes para melhor visualização
                    samples_codificador_fonte.push_back((bit ? 1.0f : 0.0f)*MAX_VOLTAGE_LEVEL);
            }
            ImGui::PlotLines(
                "Amostras Codificador Fonte", samples_codificador_fonte.data(),
                static_cast<int>(samples_codificador_fonte.size()),
                0, nullptr,   
                -MAX_VOLTAGE_LEVEL, MAX_VOLTAGE_LEVEL,  
                ImVec2(0, 75)
            );
            ImGui::Text("Bits transmitidos: %d", static_cast<int>(binario_transmite.size()));
            ImGui::Text("Bits: %s", [&]() {
                string bits_str;
                for (bool bit : binario_transmite) {
                    bits_str += bit ? '1' : '0';
                }
                return bits_str.c_str();
            }());

            // Codificador de Canal = Codifica os bits da fonte no modo Manchester
            bifase_codificador = codificadorDeCanal(binario_transmite);
            for (float data : bifase_codificador) {
                for(int i = 0; i < VISIBILITY_MULTIPLIER; ++i)  // Repete cada bit VISIBILITY_MULTIPLIER vezes para melhor visualização
                    samples_codificador_canal.push_back(data);
            }
            ImGui::PlotLines(
                "Codificação Manchester", samples_codificador_canal.data(),
                static_cast<int>(samples_codificador_canal.size()),
                0, nullptr,           
                -MAX_VOLTAGE_LEVEL, MAX_VOLTAGE_LEVEL,   
                ImVec2(0, 75)                 // graph_size: width=0 (auto), height=200
            );
            ImGui::Text("Bits codificados: %d", static_cast<int>(bifase_codificador.size()/T_CONSTANT));
            ImGui::Text("Bits: %s", [&]() {
                string bits_str;
                for (size_t i = 0; i < bifase_codificador.size(); i+=T_CONSTANT) {
                    // Cada bit é representado por 2*VISIBILITY_MULTIPLIER amostras em codificação biphase
                    bool bit = bifase_codificador[i] > 0.0f; // Verifica o primeiro nível para determinar o bit
                    bits_str += bit ? '1' : '0';
                }
                return bits_str.c_str();
            }());

            // Seleciona o tipo de modulação
            ImGui::Combo("Tipo de Modulação", (int *)(&current_modulation_type), "BPSK\0QPSK\0ASK\0", MAX_MODULATION_TYPES);

            // Utiliza a modulação selecionada
            if(current_modulation_type == BPSK)
                samples_modulador_psk = moduladorBPSK(bifase_codificador);
            else if(current_modulation_type == QPSK)
                samples_modulador_psk = moduladorQPSK(bifase_codificador);
            if(current_modulation_type == ASK)
                samples_modulador_psk = moduladorASK(bifase_codificador);

            ImGui::PlotLines(
                "Sinal Modulado", samples_modulador_psk.data(),
                static_cast<int>(samples_modulador_psk.size()),
                0, nullptr,           
                -MAX_VOLTAGE_LEVEL, MAX_VOLTAGE_LEVEL,   
                ImVec2(0, 75)
            );

            // Sinal enviado para o meio de comunicação
            sinal_transmitido = samples_modulador_psk;

            ImGui::End();
        }

        // Janela do meio de comunicação. Controla o ruído e mostra o sinal resultante do que foi transmitido + ruído
        {
            ImGui::SetNextWindowSize(ImVec2(SENDER_RECEIVER_WIDTH, SENDER_RECEIVER_HEIGHT/2), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(10, SENDER_RECEIVER_HEIGHT + 10), ImGuiCond_Once);
            ImGui::Begin("Meio em que o sinal trafega", nullptr, ImGuiWindowFlags_NoCollapse);

            // Define o nível de ruído
            ImGui::SliderFloat("Amplitude do Ruído [em Volts]", &noise_level, 0.0f, MAX_VOLTAGE_LEVEL, "%.5f");

            // Ruído branco
            // (Simplesmente adiciona valores aleatórios entre -NOISE_LEVEL e +NOISE_LEVEL)
            for (int i = 0; i < sinal_transmitido.size(); ++i) {
                canal_ruido.push_back(((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * noise_level);
            }

            if(fpclassify(noise_level) != FP_ZERO)
                ImGui::Text("SNR [dB]: %.2f", 20*log10(MAX_VOLTAGE_LEVEL/noise_level));
            else
                ImGui::Text("Sem ruído ativo!");

            ImGui::PlotLines(
                "Ruído", canal_ruido.data(),
                static_cast<int>(canal_ruido.size()),
                0, nullptr,
                -MAX_VOLTAGE_LEVEL, MAX_VOLTAGE_LEVEL, 
                ImVec2(0, 75)
            );

            // Soma o sinal e o ruído para obter a componente sinal + ruído, que é enviada para o receptor
            for (int i = 0; i < sinal_transmitido.size(); ++i) {
                sinal_recebido_com_ruido.push_back(sinal_transmitido[i] + canal_ruido[i]);
            }
            ImGui::PlotLines(
                "Sinal + Ruído", sinal_recebido_com_ruido.data(),
                static_cast<int>(sinal_recebido_com_ruido.size()), 
                0, nullptr,
                -MAX_VOLTAGE_LEVEL*2, MAX_VOLTAGE_LEVEL*2,                
                ImVec2(0, 75)
            );

            ImGui::End();
        }

        // Janela do receptor
        {
            ImGui::SetNextWindowSize(ImVec2(SENDER_RECEIVER_WIDTH, SENDER_RECEIVER_HEIGHT), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(SENDER_RECEIVER_WIDTH + 10, 10), ImGuiCond_Once);
            ImGui::Begin("Janela do Receptor", nullptr, ImGuiWindowFlags_NoCollapse);

            // Demodulador do sinal após receber do meio
            // Demodula conforme o que foi selecionado na janela do transmissor (deve ser o mesmo em ambos os lados)
            if(current_modulation_type == BPSK)
                bifase_demodulador_psk = demoduladorBPSK(sinal_recebido_com_ruido);
            else if(current_modulation_type == QPSK)
                bifase_demodulador_psk = demoduladorQPSK(sinal_recebido_com_ruido);
            else if(current_modulation_type == ASK)
                bifase_demodulador_psk = demoduladorASK(sinal_recebido_com_ruido);

            for (float data : bifase_demodulador_psk) {
                for(int i = 0; i < VISIBILITY_MULTIPLIER; ++i)  // Repete cada bit VISIBILITY_MULTIPLIER vezes para melhor visualização
                    samples_demodulador_psk.push_back(data);
            }
            ImGui::PlotLines(
                "Sinal Demodulado", samples_demodulador_psk.data(),
                static_cast<int>(samples_demodulador_psk.size()),
                0, nullptr,
                -MAX_VOLTAGE_LEVEL, MAX_VOLTAGE_LEVEL,
                ImVec2(0, 75)
            );

            // Decodificador de Canal. Trasnforma os bits 
            string bits_str;
            binario_recebido = decodificadorDeCanal(bifase_demodulador_psk);
            for (bool bit : binario_recebido) {
                bits_str += bit ? '1' : '0';
                for(int i = 0; i < VISIBILITY_MULTIPLIER; ++i)  // Repete cada bit 10 vezes para melhor visualização
                    samples_decodificador_canal.push_back((bit ? 1.0f : 0.0f)*MAX_VOLTAGE_LEVEL);
            }
            ImGui::PlotLines(
                "Sinal Decodificado do Canal", samples_decodificador_canal.data(),
                static_cast<int>(samples_decodificador_canal.size()),
                0, nullptr,
                -MAX_VOLTAGE_LEVEL, MAX_VOLTAGE_LEVEL,
                ImVec2(0, 75)
            );
            ImGui::Text("Bits recebidos: %d", static_cast<int>(binario_recebido.size()));
            ImGui::Text("Bits: %s", bits_str.c_str());

            // Decodificador de Fonte
            string ASCII_resultante = decodificadorDeFonte(binario_recebido);
            // Mostra o texto decodificado
            ImGui::Text("Texto decodificado = %s", ASCII_resultante.c_str());

            // Calcula os bits com erro
            for(int i = 0; i < binario_recebido.size() && i < binario_transmite.size(); ++i) {
                total_bits++;
                if(binario_recebido[i] != binario_transmite[i])
                    error_bits++;
            }

            // Atualiza a informação da taxa de erro de bits (Bit Error Rate)
            current_time += ImGui::GetIO().DeltaTime;
            if(current_time - last_time >= 1.0) {
                last_time = current_time;
                ber = (total_bits > 0) ? static_cast<double>(error_bits) / static_cast<double>(total_bits) : 0.0;
                ber *= 100; // Converte para porcentagem
                total_bits = 0;
                error_bits = 0;
            }
            ImGui::Text("Taxa de Erro de Bit (BER): %.3f%%", ber);
            
            ImGui::End();
        }

        // Renderiza
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Limpa e destrói itens antes de finalizar
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

const float PI = 3.14159265f;

vector<float> moduladorBPSK(const vector<float>& sinal_codificado) {
    const float carrier_signal = SAMPLE_RATE/F_CARRIER;
    vector<float> sinal_modulado;
    for (size_t i = 0; i < sinal_codificado.size(); ++i) {
        for(int j = 0; j < carrier_signal; j++)
        {
            float t = (((i*carrier_signal)+j) / static_cast<float>(SAMPLE_RATE));
            float portadora = sinf(2.0f * PI * F_CARRIER * t);
            sinal_modulado.push_back(portadora * sinal_codificado[i]);
        }
    }
    return sinal_modulado;
}

vector<float> demoduladorBPSK(const vector<float>& sinal_recebido) {
    const float carrier_signal = SAMPLE_RATE/F_CARRIER;
    vector<float> sinal_demodulado;
    for (size_t i = 0; i < sinal_recebido.size(); i+=carrier_signal) {
        for(int j = 0; j < carrier_signal; j++)
        {
            float t = (((i*carrier_signal)+j) / static_cast<float>(SAMPLE_RATE));
            float portadora = sinf(2.0f * PI * F_CARRIER * t);
            float produto = sinal_recebido[i+j] * portadora;

            if(produto > -0.01f && produto < 0.01f) continue;

            if(produto > 0.0f)
                produto = MAX_VOLTAGE_LEVEL;
            else
                produto = -MAX_VOLTAGE_LEVEL;

            sinal_demodulado.push_back(produto);
            break;
        }
    }
    return sinal_demodulado;
}

vector<float> moduladorQPSK(const vector<float>& sinal_codificado) {
    vector<float> sinal_modulado;
    for (size_t i = 0; i < sinal_codificado.size(); i += 2) {
        float bit_I = (i < sinal_codificado.size() && sinal_codificado[i] > 0.0f) ? 1.0f : -1.0f;
        float bit_Q = (i + 1 < sinal_codificado.size() && sinal_codificado[i + 1] > 0.0f) ? 1.0f : -1.0f;
        for (size_t j = 0; j < 2; ++j) {
            size_t sample_idx = i + j;
            float portadora_I = cosf(2.0f * PI * F_CARRIER * (sample_idx / static_cast<float>(SAMPLE_RATE)));
            float portadora_Q = sinf(2.0f * PI * F_CARRIER * (sample_idx / static_cast<float>(SAMPLE_RATE)));
            float simbolo = (bit_I * portadora_I + bit_Q * portadora_Q) * MAX_VOLTAGE_LEVEL;
            sinal_modulado.push_back(simbolo);
        }
    }
    return sinal_modulado;
}

vector<float> demoduladorQPSK(const vector<float>& sinal_recebido) {
    vector<float> sinal_demodulado;
    for (size_t i = 0; i < sinal_recebido.size(); i += 2) {
        float soma_I = 0.0f;
        float soma_Q = 0.0f;
        for (size_t j = 0; j < 2; ++j) {
            size_t sample_idx = i + j;
            if (sample_idx >= sinal_recebido.size()) break;
            float portadora_I = cosf(2.0f * PI * F_CARRIER * (sample_idx / static_cast<float>(SAMPLE_RATE)));
            float portadora_Q = sinf(2.0f * PI * F_CARRIER * (sample_idx / static_cast<float>(SAMPLE_RATE)));
            soma_I += sinal_recebido[sample_idx] * portadora_I;
            soma_Q += sinal_recebido[sample_idx] * portadora_Q;
        }
        sinal_demodulado.push_back(soma_I);
        sinal_demodulado.push_back(soma_Q);
    }
    return sinal_demodulado;
}

vector<float> moduladorASK(const vector<float>& sinal_codificado) {
    const float carrier_signal = SAMPLE_RATE/F_CARRIER;
    vector<float> sinal_modulado;
    for (size_t i = 0; i < sinal_codificado.size(); ++i) {
        for(int j = 0; j < carrier_signal; j++)
        {
            float t = (((i*carrier_signal)+j) / static_cast<float>(SAMPLE_RATE));
            float portadora = sinf(2.0f * PI * F_CARRIER * t);
            if(sinal_codificado[i] > 0.0f)
                sinal_modulado.push_back(portadora * sinal_codificado[i]);
            else
                sinal_modulado.push_back(0);
        }
    }
    return sinal_modulado;
}

vector<float> demoduladorASK(const vector<float>& sinal_recebido) {
    const float carrier_signal = SAMPLE_RATE/F_CARRIER;
    vector<float> sinal_demodulado;
    int j;
    for (size_t i = 0; i < sinal_recebido.size(); i+=carrier_signal) {
        for(j = 0; j < carrier_signal; j++)
        {
            float t = (((i*carrier_signal)+j) / static_cast<float>(SAMPLE_RATE));
            float portadora = sinf(2.0f * PI * F_CARRIER * t);
            float produto = sinal_recebido[i+j];

            if(produto > -(MAX_VOLTAGE_LEVEL/10) && produto < (MAX_VOLTAGE_LEVEL/10))
                continue;
            else
                produto = MAX_VOLTAGE_LEVEL;

            sinal_demodulado.push_back(produto);
            break;
        }
        if(j >= carrier_signal) sinal_demodulado.push_back(-MAX_VOLTAGE_LEVEL);
    }
    return sinal_demodulado;
}