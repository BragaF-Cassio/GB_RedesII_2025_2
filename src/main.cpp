// Dear ImGui: standalone example application for SDL3 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "main.h"

using namespace std;

#define VISIBILITY_MULTIPLIER 10
#define SENDER_RECEIVER_HEIGHT 500
#define SENDER_RECEIVER_WIDTH 600

// Main code
int main(int, char**)
{
    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
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

    // Create window with graphics context
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    float noise_level = 0.2f;
    string ASCII = "abc";
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
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

        vector<float> bifase;
        vector<float> samples_codificador_canal;

        vector<float> canal_ruido;

        vector<float> sinal_recebido_com_ruido;

        vector<bool> binario_recebido;
        vector<float> samples_decodificador_canal;

        // Janela de gráficos do transmissor
        {
            ImGui::SetNextWindowSize(ImVec2(SENDER_RECEIVER_WIDTH, SENDER_RECEIVER_HEIGHT), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
            ImGui::Begin("Janela do Transmissor", nullptr, ImGuiWindowFlags_NoCollapse);
            
            ImGui::InputText("Mensagem", &ASCII);

            // Codificador de Fonte
            binario_transmite = codificadorDeFonte(ASCII);
            for (bool bit : binario_transmite) {
                for(int i = 0; i < VISIBILITY_MULTIPLIER; ++i)  // Repete cada bit VISIBILITY_MULTIPLIER vezes para melhor visualização
                    samples_codificador_fonte.push_back((bit ? 1.0f : 0.0f)*MAX_VOLTAGE_LEVEL);
            }
            ImGui::PlotLines(
                "Amostras Codificador Fonte",                     // label
                samples_codificador_fonte.data(),                // values pointer
                static_cast<int>(samples_codificador_fonte.size()), // count
                0,  
                nullptr,   
                -MAX_VOLTAGE_LEVEL,   
                MAX_VOLTAGE_LEVEL,  
                ImVec2(0, 125)                 // graph_size: width=0 (auto), height=125
            );
            ImGui::Text("Bits transmitidos: %d", static_cast<int>(binario_transmite.size()));
            ImGui::Text("Bits: %s", [&]() {
                string bits_str;
                for (bool bit : binario_transmite) {
                    bits_str += bit ? '1' : '0';
                }
                return bits_str.c_str();
            }());

            // Codificador de Canal
            bifase = codificadorDeCanal(binario_transmite);
            samples_codificador_canal = bifase;
            ImGui::PlotLines(
                "Amostras Codificador Canal",                     // label
                samples_codificador_canal.data(),                // values pointer
                static_cast<int>(samples_codificador_canal.size()), // count
                0,                             // values_offset
                nullptr,                       // overlay_text
                -MAX_VOLTAGE_LEVEL,                       // scale_min (auto)
                MAX_VOLTAGE_LEVEL,                       // scale_max (auto)
                ImVec2(0, 125)                 // graph_size: width=0 (auto), height=200
            );
            ImGui::Text("Bits codificados: %d", static_cast<int>(bifase.size()/T_CONSTANT));
            ImGui::Text("Bits: %s", [&]() {
                string bits_str;
                for (size_t i = 0; i < bifase.size(); i+=T_CONSTANT) {
                    // Cada bit é representado por 2*VISIBILITY_MULTIPLIER amostras em codificação biphase
                    bool bit = bifase[i] > 0.0f; // Verifica o primeiro nível para determinar o bit
                    bits_str += bit ? '1' : '0';
                }
                return bits_str.c_str();
            }());
            ImGui::End();
        }

        // Janela de gráficos do ruído
        {
            ImGui::SetNextWindowSize(ImVec2(SENDER_RECEIVER_WIDTH, SENDER_RECEIVER_HEIGHT/2), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(10, SENDER_RECEIVER_HEIGHT + 10), ImGuiCond_Once);
            ImGui::Begin("Meio em que o sinal trafega", nullptr, ImGuiWindowFlags_NoCollapse);

             ImGui::SliderFloat("Nível de Ruído", &noise_level, 0.0f, 1.0f*(MAX_VOLTAGE_LEVEL*2), "%.2f");

            // Ruído branco
            // (Simplesmente adiciona valores aleatórios entre -NOISE_LEVEL e +NOISE_LEVEL)
            for (int i = 0; i < bifase.size(); ++i) {
                canal_ruido.push_back(((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * noise_level);
            }
            ImGui::PlotLines(
                "Ruído",                     // label
                canal_ruido.data(),                        // values pointer
                static_cast<int>(canal_ruido.size()),      // count
                0,                             // values_offset
                nullptr,                       // overlay_text
                -MAX_VOLTAGE_LEVEL,                       // scale_min (auto)
                MAX_VOLTAGE_LEVEL,                       // scale_max (auto)
                ImVec2(0, 75)                 // graph_size: width=0 (auto), height=200
            );

            for (int i = 0; i < bifase.size(); ++i) {
                sinal_recebido_com_ruido.push_back(bifase[i] + canal_ruido[i]);
            }
            ImGui::PlotLines(
                "Sinal + Ruído",                     // label
                sinal_recebido_com_ruido.data(),                // values pointer
                static_cast<int>(sinal_recebido_com_ruido.size()), // count
                0,                             // values_offset
                nullptr,                       // overlay_text
                -MAX_VOLTAGE_LEVEL*2,                   
                MAX_VOLTAGE_LEVEL*2,                
                ImVec2(0, 100)                 // graph_size: width=0 (auto), height=200
            );

            ImGui::End();
        }

        // Janela de gráficos do receptor
        {
            ImGui::SetNextWindowSize(ImVec2(SENDER_RECEIVER_WIDTH, SENDER_RECEIVER_HEIGHT), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(SENDER_RECEIVER_WIDTH + 10, 10), ImGuiCond_Once);
            ImGui::Begin("Janela do Receptor", nullptr, ImGuiWindowFlags_NoCollapse);

            // Decodificador de Canal
            string bits_str;
            binario_recebido = decodificadorDeCanal(sinal_recebido_com_ruido);
            for (bool bit : binario_recebido) {
                bits_str += bit ? '1' : '0';
                for(int i = 0; i < VISIBILITY_MULTIPLIER; ++i)  // Repete cada bit 10 vezes para melhor visualização
                    samples_decodificador_canal.push_back((bit ? 1.0f : 0.0f)*MAX_VOLTAGE_LEVEL);
            }
            ImGui::PlotLines(
                "Sinal Decodificado do Canal",                     // label
                samples_decodificador_canal.data(),                // values pointer
                static_cast<int>(samples_decodificador_canal.size()), // count
                0,                             // values_offset
                nullptr,                       // overlay_text
                -MAX_VOLTAGE_LEVEL,                       // scale_min (auto)
                MAX_VOLTAGE_LEVEL,                       // scale_max (auto)
                ImVec2(0, 125)                 // graph_size: width=0 (auto), height=200
            );
            ImGui::Text("Bits recebidos: %d", static_cast<int>(binario_recebido.size()));
            ImGui::Text("Bits: %s", bits_str.c_str());

            // Decodificador de Fonte
            string ASCII_resultante = decodificadorDeFonte(binario_recebido);

            // Mostra o texto decodificado
            ImGui::Text("Texto decodificado = %s", ASCII_resultante.c_str());

            ImGui::End();
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        /*{
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }*/

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
