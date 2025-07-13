#include "imgui.h"
#include "imgui-master/backends/imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "python_binding.h"
#include "Cell.h"
#include "Visualize.h"
#include <filesystem>
#include "style.h"


namespace fs = std::filesystem;


LogsWindow logsWindow;

bool showLogWindow = true; // A flag to control visibility
std::string savePath;
std::string exportPath;
std::string loadPath;
std::string NoteBookPath;

// Error callback for GLFW
static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}
std::vector<std::string> window_list = { "Main", "CSV", "Metrics", "Log"};
std::string selectedWindowOption; // Ensure the global variable is initialized

void PingPython() {
    static auto lastPing = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPing).count();

    if (elapsed >= 1) {
        try {
            py::gil_scoped_acquire acquire;
            py::eval("1");  // Light ping to wake Python
            // Optional: remove this if you only want console logs
            // log_window.AddLog("Pinged Python interpreter.", "DEBUG");
            //std::cout << "Pinged Python interpreter.\n";
        }
        catch (const std::exception& e) {
            logsWindow.AddLog(std::string("Ping failed: ") + e.what(), "ERROR");
        }

        lastPing = now;
    }
}


std::string OpenSaveDialog()
{
    const char* filter_patterns[] = { "*.json" };  // Optional file type filter
    const char* path = tinyfd_saveFileDialog(
        "Save Project",               // Dialog title
        ".json",             // Default file name
        1,                          // Number of file types in filter
        filter_patterns,            // File type filter
        "json Files"   // Filter description
    );
    
    return path ? std::string(path) : "";  // Return empty string if no file chosen
}
std::string LoadSaveDialog()
{
    try {
        const char* filter_Patterns[] = { "*.json" };
        const char* loadPath = tinyfd_openFileDialog("Choose Project json File", ".json", 1, filter_Patterns, "json Files", 0);
        std::string filepath;
        
        if (loadPath && strlen(loadPath) > 0) {
            filepath = loadPath;
            return filepath;
        }
        else
        {
            return "";
        }
    }
    catch (const std::exception& e)
    {
        logsWindow.AddLog(e.what(), "ERROR");
        return "";
    }
}
std::string OpenNoteBookDialog()
{
    const char* filter_patterns[] = { "*.ipynb" };  // Optional file type filter
    const char* path = tinyfd_saveFileDialog(
        "Save Project",               // Dialog title
        ".ipynb",             // Default file name
        1,                          // Number of file types in filter
        filter_patterns,            // File type filter
        "ipynb Files"   // Filter description
    );

    return path ? std::string(path) : "";  // Return empty string if no file chosen
}


void RenderMenuBar() {
    // Position and size for the sidebar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            
            if (ImGui::MenuItem("Save")) {
                if (exportPath.empty())
                {
                    exportPath = OpenSaveDialog();
                    if (!exportPath.empty()) {
                        
                        size_t found = exportPath.find(".json");
                        if (found != std::string::npos)
                        {
                            std::cout << "Substring found at position: " << found << std::endl;
                            SessionManager::getInstance().saveSessionToFile(exportPath);
                        }
                        else
                        {
                            std::cout << "Substring not found." << std::endl;
                            exportPath = exportPath.append(".json");
                            SessionManager::getInstance().saveSessionToFile(exportPath);
                        }
                    }
                    else {
                        logsWindow.AddLog("No File was Slected", "INFO");
                    }
                }
                else {
                    SessionManager::getInstance().saveSessionToFile(exportPath);
                }
                // Call your save function here
                logsWindow.AddLog("Save triggered", "INFO");

            }
            if (ImGui::MenuItem("Save As")) {
                exportPath = OpenSaveDialog();
                if (!exportPath.empty()) {

                    size_t found = exportPath.find(".json");
                    if (found != std::string::npos)
                    {
                        std::cout << "Substring found at position: " << found << std::endl;
                        SessionManager::getInstance().saveSessionToFile(exportPath);
                    }
                    else
                    {
                        std::cout << "Substring not found." << std::endl;
                        exportPath = exportPath.append(".json");
                        SessionManager::getInstance().saveSessionToFile(exportPath);
                    }
                }
                else
                {
                    logsWindow.AddLog("No File was Slected", "INFO");
                }

                // Call your save-as dialog/function
            }
            if (ImGui::MenuItem("Load")) {
                logsWindow.AddLog("Load triggered", "INFO");
                // Call your load function here
                loadPath = LoadSaveDialog();
                if (!loadPath.empty())
                {
                    SessionManager::getInstance().loadSessionFromFile(loadPath.c_str());
                    RebuildCellsFromSessionState();
                }
                else
                {
                    logsWindow.AddLog("No File was Slected", "INFO");
                }
                
               
            }
            if (ImGui::MenuItem("Export As Code"))
            {
                logsWindow.AddLog("NotebookExports triggered", "INFO");
                NoteBookPath = OpenNoteBookDialog();
                if (!NoteBookPath.empty())
                {

                    size_t found = exportPath.find(".ipynb");
                    if (found != std::string::npos)
                    {
                        std::cout << "Substring found at position: " << found << std::endl;
                        export_notebook(NoteBookPath);
                    }
                    else
                    {
                        std::cout << "Substring not found." << std::endl;
                        NoteBookPath = NoteBookPath.append(".ipynb");
                        export_notebook(NoteBookPath);
                    }
                    
                }
                else
                {
                    logsWindow.AddLog("No File was Slected", "INFO");
                }

            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    
    }
}

void RenderTopBar() {
    // Begin top bar
    ImGui::Begin("Top Bar", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse
    );

    // Position and size for the top bar
    ImGui::SetWindowPos(ImVec2(0, 18));
    ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 42)); // Full width, 40px height

    // Style for flat horizontal buttons (optional)
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

    // Horizontal buttons
    
    if (ImGui::Button("Main Window")) {
        selectedWindowOption = "Main";
    }
    ImGui::SameLine();
 
  
    if (SessionManager::getInstance().getCSVWindowPresent() and !SessionManager::getInstance().getFilePath().empty()) {
        if (ImGui::Button("CSV Window")) {
            selectedWindowOption = "CSV";
        }
        ImGui::SameLine();
    }
    
    if (SessionManager::getInstance().getMatricsWindowPresent() and !SessionManager::getInstance().getFilePath().empty()) {
        if (ImGui::Button("Metrics Window")) {
            selectedWindowOption = "Metrics";
        }
        ImGui::SameLine();
    }

    if (SessionManager::getInstance().getLogsWindowPresent()) {
        
        if (ImGui::Button("Logs")) {
            selectedWindowOption = "Logs";
        }
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void RenderSidebar() {
    ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse);

    // Position and size for the sidebar
    ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.7f, 0)); // Start at 70% width
    ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.3f, ImGui::GetIO().DisplaySize.y * 0.5f)); // Top half

    // Sidebar buttons
    if (ImGui::Button("main")) {
        selectedWindowOption = "Main"; // Update the global variable
    }
   
    if (SessionManager::getInstance().getCSVWindowPresent())
    {
        ImGui::SameLine();
        if (ImGui::Button("CSV")) {
            selectedWindowOption = "CSV"; // Update the global variable
        }
    }
    
    if (SessionManager::getInstance().getMatricsWindowPresent())
    {
        if (ImGui::Button("Metrics")) {
            selectedWindowOption = "Metrics"; // Update the global variable
        }
    }
    ImGui::End();
}

void RenderLogs(float width, float height, float x_pos, float y_pos) {
    // Logs at the bottom-right
    ImGui::Begin("Logs", nullptr, ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse);

    ImGui::SetWindowPos(ImVec2(x_pos, y_pos)); // Start at (0, 0)
    ImGui::SetWindowSize(ImVec2(width, height)); // Take 70% width
    logsWindow.Show(&showLogWindow, width, height, x_pos, y_pos);

    ImGui::End();
}


void RenderMainDisplay() {
    ImGui::Begin("Main Display", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoCollapse);
   
    float width = ImGui::GetIO().DisplaySize.x;
    float height = ImGui::GetIO().DisplaySize.y -50;
    float x_pos = 0;
    float y_pos = 55;
    ImGui::SetWindowPos(ImVec2(x_pos, y_pos)); // Start at (0, 0)
    ImGui::SetWindowSize(ImVec2(width, height)); // Take 70% width
    UIStyle::ApplyDarkTheme();

    // Render content based on the selected option
    if (selectedWindowOption == "Main") {
        RenderCellsUI(width, height, x_pos, y_pos); // Dynamic dimensions
    }
    else if (selectedWindowOption == "Logs") {
        RenderLogs(width, height, x_pos, y_pos);
    }
    else if (selectedWindowOption == "CSV") {
        Display_CSV(width,height, x_pos, y_pos); // Dynamic dimensions
    }
    else if (selectedWindowOption == "Metrics") {
        Display_MetricsGraph(width, height, x_pos, y_pos); // Dynamic dimensions
    }
    ImGui::End();
    
}

int main(int, char**) {
    // Set up error callback and initialize GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;

    // OpenGL version setup
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For macOS compatibility
#endif

    // Create the window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "NoCodeMl Window", NULL, NULL);
    if (window == NULL)
        return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
  
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    UIStyle::ApplyFont(io);


    // Initialize ImGui GLFW and OpenGL3 bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    
    selectedWindowOption = "Main";
    

    start_python();

    InitializeCells();

    // Main event loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();  // Poll for window events

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        RenderMenuBar();

        PingPython(); // <-- This is your heartbeat


        // Render the cells UI
        RenderMainDisplay();
        RenderTopBar();
        
       

        // Render ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
