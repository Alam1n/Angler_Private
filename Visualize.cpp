#include "Visualize.h"
#include <GLFW/glfw3.h>
#include "Cell.h"
#include "imguihelper.h"
#include "python_binding.h"
#include "ThreadManager.h"
#include <future>
#include "LogsWindow.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

LogsWindow logs;
ImguiHelper helper;

std::vector<std::vector<std::string>> Visualize::LoadCSV(const std::string& fileName, size_t maxRows) {
    std::ifstream file(fileName);
    std::vector<std::vector<std::string>> data;
    std::string line;
    size_t rowCount = 0;
    maxRows = 1000 ;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return data;
    }

    while (std::getline(file, line) && rowCount < maxRows) {  // Load only `maxRows` rows
        std::stringstream ss(line);
        std::vector<std::string> row;
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);
        rowCount++;
    }

    file.close();
    return data;
}


void Visualize::RenderSavedPlot(const std::string& imagePath, float width, float height) {
    static std::unordered_map<std::string, GLuint> textureCache;

    if (textureCache.find(imagePath) == textureCache.end()) {
        int image_width = 0, image_height = 0, channels = 0;
        unsigned char* image_data = stbi_load(imagePath.c_str(), &image_width, &image_height, &channels, 4);
        if (!image_data) {
            ImGui::Text("Failed to load: %s", imagePath.c_str());
            return;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

        stbi_image_free(image_data);
        textureCache[imagePath] = textureID;
    }

    GLuint textureID = textureCache[imagePath];
    ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2(width, height));
}


void Visualize::RenderMetricsGraph(const std::string& title, float accuracy, float precision, float recall, float f1, float width, float hight) {
    
}

Visualize::Visualize()
{
    helper.Assign_values();
    helper.RefreshColumns();
}

void Visualize::RenderCSV(const std::vector<std::vector<std::string>>& data) {
    if (data.empty()) {
        ImGui::Text("No data to display.");
        return;
    }

    size_t columnCount = data[0].size();
    size_t rowCount = data.size();

    // Horizontal and vertical scrolling enabled
    if (ImGui::BeginTable("CSV Table", columnCount,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedFit)) {

        ImGui::TableSetupScrollFreeze(0, 1); // Freeze first row and column
        for (size_t col = 0; col < columnCount; ++col) {
            ImGui::TableSetupColumn(std::to_string(col).c_str(), ImGuiTableColumnFlags_None, 150.0f);  // Adjust width to 150.0f
        }
        ImGui::TableHeadersRow();

        // Efficient scrolling
        ImGuiListClipper clipper;
        clipper.Begin((int)rowCount);
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                ImGui::TableNextRow();
                for (size_t col = 0; col < columnCount; ++col) {
                    ImGui::TableSetColumnIndex((int)col);
                    ImGui::TextUnformatted(data[row][col].c_str());
                }
            }
        }
        clipper.End();

        ImGui::EndTable();
    }
}

void Visualize::PreprocessingUI() {
    ImGui::Begin("Preprocessing Window", nullptr, ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse);
    // Position and size for the sidebar
    ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.7f, 50)); // Start at 70% width
    ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.3f, ImGui::GetIO().DisplaySize.y)); // Top half    

    
    helper.showPreprocessingOptions = true; // Show the preprocessing list
   
    if (helper.showPreprocessingOptions) {
        ImGui::Text("Choose a preprocessing option:");

        // Begin Combo for selecting preprocessing option
        if (ImGui::BeginCombo("Preprocessing Options", helper.selectedPreprocessingOption.c_str())) {
            // Loop through preprocessing options
            for (const auto& option : helper.preprocessingOptions) {
                bool is_selected = (helper.selectedPreprocessingOption == option);

                // Display each option as a selectable item
                if (ImGui::Selectable(option.c_str(), is_selected)) {
                    helper.selectedPreprocessingOption = option; // Store selected option
                    helper.showPreprocessingOptions = false;    // Hide options after selection


                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();  // End the combo box
        }
    }

    // Display the currently selected preprocessing option
    ImGui::Text("Selected Preprocessing: %s", helper.selectedPreprocessingOption.c_str());


    if (helper.selectedPreprocessingOption == "Generate and Remove Columns")
    {
        helper.Generate_Columns();
    }
    if (helper.selectedPreprocessingOption == "Normalize Columns")
    {
        helper.Select_Normalization();
    }
    if (helper.selectedPreprocessingOption == "Label Encoding")
    {
        helper.Label_Encoding("label");
    }
    if (helper.selectedPreprocessingOption == "OneHot Encoding")
    {
        helper.Label_Encoding("onehot");
    }
    if (helper.selectedPreprocessingOption == "Standardization")
    {
        helper.Standadization();
    }


    

    ImGui::Columns(1); // Reset to 1 column
    ImGui::End();
}



void Display_CSV(float width, float height, float x_pos, float y_pos) {
    ImGui::Begin("CSV Window", nullptr, ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse);
    
    ImGui::SetWindowSize(ImVec2(width*0.7f, height), ImGuiCond_Always);
    ImGui::SetWindowPos(ImVec2(x_pos, y_pos));

    static Visualize visualize;
    static std::vector<std::vector<std::string>> csvData;
       
    // Load only when needed
    if (csvData.empty() || SessionManager::getInstance().getDSupdate()) {
        csvData = visualize.LoadCSV(SessionManager::getInstance().getFilePath(), 1000); // Load chunks
    }
    helper.RefreshColumns();

    visualize.PreprocessingUI();
    visualize.RenderCSV(csvData);

    ImGui::End();
}

void Display_MetricsGraph(float width, float height, float x_pos, float y_pos)
{
    static int selectedX = 0;
    static int selectedY = 0;
    static int selectedPlotType = 0;
    static std::vector<std::string> imagePaths;
    static const int maxImages = 6;
    static std::string pendingPath;  // Temp hold
    static std::future <std::string> futureImagePath;
    static bool isProcessing = false;
    
    int imagePerRow = 3;
    float padding = 10.0f;
    float cellWidth = (width - (imagePerRow + 1) * padding) / imagePerRow;
    std::vector<float> y_pred;
    std::vector<float> y_test;
    static Visualize visualize;
    static bool hasLoggedMainPlot = false;
    static bool hasLoggedPredictedPlot = false;
    static bool hasLoggedStart = false;
    static bool hasLoggedDone = false;

    static int maximizedIndex = -1;


    const char* plotTypes[] = { "scatter", "line", "bar", "hist", "confusion_matrix"};

    ImGui::SetNextWindowPos(ImVec2(x_pos, y_pos));
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::Begin("Visualization Dashboard", nullptr, ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse);

    // --- Controls Panel ---
    ImGui::Text("Plot Generator");
    ImGui::Separator();

    ImGui::Combo("X Axis", &selectedX, helper.columnNamesCStr.data(), helper.columnNamesCStr.size());
    ImGui::Combo("Y Axis", &selectedY, helper.columnNamesCStr.data(), helper.columnNamesCStr.size());
    ImGui::Combo("Plot Type", &selectedPlotType, plotTypes, IM_ARRAYSIZE(plotTypes));

    ImGui::SameLine();
    // 1. Trigger the async task
    if (ImGui::Button("Generate Plot") && imagePaths.size() < maxImages && !isProcessing) {
        isProcessing = true;
        hasLoggedStart = false; // Reset for this run
        hasLoggedDone = false;
        futureImagePath = ThreadManager::getInstance().submitTask(
            [path = helper.path, x = helper.columns[selectedX], y = helper.columns[selectedY], type = plotTypes[selectedPlotType]] {
                return GeneratePlotImage(path, x, y, type);
            }
        );
        if (!hasLoggedStart) {
            visualize.ss.str("");
            visualize.ss << "Started generating plot: X=" << helper.columns[selectedX]
                << " Y=" << helper.columns[selectedY]
                << " Type=" << plotTypes[selectedPlotType];
            logs.AddLog(visualize.ss.str(), "INFO");
            hasLoggedStart = true;
        }
    }

    // 2. In every frame, check for completion
    if (isProcessing) {
        ImGui::Text("Generating plot...");
        auto status = futureImagePath.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            py::gil_scoped_acquire gil;
            pendingPath = futureImagePath.get();
            isProcessing = false;

            if (!pendingPath.empty()) {
                imagePaths.push_back(pendingPath);
                if (status == std::future_status::ready) {
                   
                    isProcessing = false;
                    hasLoggedMainPlot = false;
                }
            }
            if (!hasLoggedDone) {
                visualize.ss.str("");
                visualize.ss << "Finished generating plot: " << pendingPath;
                logs.AddLog(visualize.ss.str(), "SUCCESS");
                hasLoggedDone = true;
            }
        }
    }
    
    ImGui::SameLine();
   
    if (SessionManager::getInstance().getModelTrained())
    {
        //visualize.PredictedValues = SessionManager::getInstance().getPredictedValues();
        y_pred = SessionManager::getInstance().getYpred();
        y_test = SessionManager::getInstance().getYTest();


        // 1. Trigger the async task
        if (ImGui::Button("Generate Predicted Plot") && imagePaths.size() < maxImages && !isProcessing) {
           
            isProcessing = true;
            
            futureImagePath = ThreadManager::getInstance().submitTask(
                [x = y_test, y = y_pred, type = plotTypes[selectedPlotType]] {
                    return GeneratePredictionPlotImage(x, y, type);
                }
            );
            if (!hasLoggedPredictedPlot) {
                visualize.ss.str("");
                visualize.ss << "Generating predicted plot...";
                logs.AddLog(visualize.ss.str(), "INFO");
                hasLoggedPredictedPlot = true;
            }
        }

        // 2. In every frame, check for completion
        if (isProcessing) {
            ImGui::Text("Generating plot...");
            auto status = futureImagePath.wait_for(std::chrono::milliseconds(0));
            if (status == std::future_status::ready) {
                py::gil_scoped_acquire gil;
                pendingPath = futureImagePath.get();
                isProcessing = false;

                if (!pendingPath.empty()) {
                    imagePaths.push_back(pendingPath);
                    if (status == std::future_status::ready) {
                        isProcessing = false;
                        hasLoggedPredictedPlot = false;
                    }
                }
            }
        }
        
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear All")) {
        imagePaths.clear();
    }

    ImGui::Separator();

    // --- Image Display Grid ---
   
    for (int i = 0; i < imagePaths.size(); ++i) {
        if (i % imagePerRow != 0) ImGui::SameLine();

        ImGui::PushID(i);
        ImGui::BeginChild("ImageSlot", ImVec2(cellWidth, 270), true);

        // Top-right "X" button
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 40, 5));
        if (ImGui::SmallButton("x")) {
            imagePaths.erase(imagePaths.begin() + i);
            --i;
            ImGui::EndChild();
            ImGui::PopID();
            continue;
        }

        // Top-right maximize "⬜" button
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 65, 5));
        if (ImGui::SmallButton("<>")) {
            maximizedIndex = i;
        }

        // Reset position and show thumbnail
        ImGui::SetCursorPos(ImVec2(10, 30));
        visualize.RenderSavedPlot(imagePaths[i], cellWidth - 20, 230);

        ImGui::EndChild();
        ImGui::PopID();
    }
    if (maximizedIndex >= 0 && maximizedIndex < imagePaths.size()) {
        ImGui::OpenPopup("MaximizedPlot");
    }

    if (ImGui::BeginPopupModal("MaximizedPlot", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Full View Plot");

        ImGui::Separator();

        // Display large version of image
        float maxWidth = ImGui::GetIO().DisplaySize.x * 0.8f;
        float maxHeight = ImGui::GetIO().DisplaySize.y * 0.8f;

        visualize.RenderSavedPlot(imagePaths[maximizedIndex], maxWidth, maxHeight);

        if (ImGui::Button("Close")) {
            maximizedIndex = -1;
            ImGui::CloseCurrentPopup();
        }
        helper.RefreshColumns();
        ImGui::EndPopup();
        
    }
    
    ImGui::End(); // Visualization Dashboard
}

