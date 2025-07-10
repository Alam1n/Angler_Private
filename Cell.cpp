#include "Cell.h"
#include "imgui.h"
#include "File_Reader_Cell.h"
#include "PrepocessDataCell.h"
#include "ModelCell.h"
#include "PredictionCell.h"

// A single Cell to be rendered
std::vector<std::unique_ptr<Cell>> cells;
//Initialize some cells (you might want to do this in a constructoe or initialization function)
void InitializeCells() {

    cells.emplace_back(std::make_unique<FileReaderCell>());
    
    // Add more Cells as needed

    
    
}

// Render the UI for the single cell
void RenderCellsUI(float width, float hight , float x_pos, float y_pos) {
    ImGui::Begin("Cells Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);

    // Set the window size to match the main window size.
    ImVec2 mainWindowSize = ImGui::GetIO().DisplaySize; // Get the size of the main window
    ImVec2 cellWindowSize = ImVec2(width, hight); // Adjust multiplier as needed

    ImGui::SetWindowPos(ImVec2(x_pos, y_pos));

    // Render the single cell
    for (auto& cell : cells)
    {
        cell->ShowCellUI();
    }

    ImGui::SetWindowSize(cellWindowSize, ImGuiCond_Always);
    if (SessionManager::getInstance().getFileLoaded())
    {
        ClearSpecificCells();    
            cells.emplace_back(std::make_unique<PreprocessDataCell>());  // Add the new cell dynamically
            SessionManager::getInstance().setFileLoaded(false);
            SessionManager::getInstance().setPreprocessingPresent(true);
        
    }

    if (SessionManager::getInstance().getPreprocessed())
    {
        if (ImGui::Button("Add Model Cell")) {
            cells.emplace_back(std::make_unique<ModelCell>());  // Add the new cell dynamically
            SessionManager::getInstance().setPreprocessed(false);
            SessionManager::getInstance().setModelPresent(true);
        }
    }

    if (SessionManager::getInstance().getModelTrained() and !SessionManager::getInstance().getPredictionPresent())
    {
        cells.emplace_back(std::make_unique<PredictionCell>());
        SessionManager::getInstance().setModelTrained(false);
        SessionManager::getInstance().setPredictionPresent(true);
    }

    ImGui::End();

}
void RebuildCellsFromSessionState() {
    cells.clear();

    auto& sm = SessionManager::getInstance();

    if (!sm.getFilePath().empty()) {
        auto fileCell = std::make_unique<FileReaderCell>();
        fileCell->SetFile(sm.getFilePath()); // if you want to pass the path
        cells.push_back(std::move(fileCell));
    }

    if (sm.getPreprocessingPresent()) {
        auto preCell = std::make_unique<PreprocessDataCell>();
        cells.push_back(std::move(preCell));
    }

    if (sm.getModelPresent()) {
        auto modelCell = std::make_unique<ModelCell>();
        cells.push_back(std::move(modelCell));
    }

    if (sm.getPredictionPresent()) {
        auto predCell = std::make_unique<PredictionCell>();
        cells.push_back(std::move(predCell));
    }
}




void ClearSpecificCells() {
    cells.erase(
        std::remove_if(cells.begin(), cells.end(),
            [](const std::unique_ptr<Cell>& cell) {
                // Check if the cell is of the type to be removed
                return dynamic_cast<PreprocessDataCell*>(cell.get()) ||
                    dynamic_cast<ModelCell*>(cell.get()) ;
            }),
        cells.end());
}
