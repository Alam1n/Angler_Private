#include "PrepocessDataCell.h"
#include "python_binding.h"
#include "style.h"

// Constructor
PreprocessDataCell::PreprocessDataCell() : Cell() {
    helper.Assign_values();
}

void PreprocessDataCell::ShowCellUI() {

    Cell::ShowCellUI();
    ImGui::PushFont(UIStyle::headingFont);
    ImGui::Text("Start Preprocessing Data ");
    ImGui::PopFont();
    
    if (!helper.path.empty()) {

        ImGui::Text("Using File: %s", helper.path.c_str());

        // Button to fetch columns
               
        helper.showPreprocessingOptions = true; // Show the preprocessing list
        
        tips.ShowToolTip(helper.preprocessing_technique_text, "preprocessing_technique");
        if (helper.showPreprocessingOptions) {
            ImGui::PushFont(UIStyle::subheadingFont);
            ImGui::Text("Choose a preprocessing option:");
            ImGui::PopFont();

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
        ImGui::PushFont(UIStyle::subheadingFont);
		ImGui::Text("Selected Preprocessing: %s", helper.selectedPreprocessingOption.c_str());
        ImGui::PopFont();


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
        if (helper.selectedPreprocessingOption == "Text Cleaning")
        {
            helper.Text_Cleaning_UI();
        }
        if (helper.selectedPreprocessingOption == "Handle Missing Values")
        {
            helper.HandleMissingValue();
        }


        Select_Target_and_Features();  // Call your function for selecting features and target
        helper.RefreshColumns();

        ImGui::Separator();
    }
}

   
void PreprocessDataCell::Select_Target_and_Features()
{
    
    if (!helper.columns.empty()) {
        // Define your options (can be in a helper or local)
        static std::vector<std::string> modelOptions = { "Supervised", "Unsupervised" };
        static std::string selectedModelOption = modelOptions[0]; // Default selection

        // ImGui Combo for selecting model type
        if (ImGui::BeginCombo("Model Option", selectedModelOption.c_str())) {
            for (const auto& option : modelOptions) {
                bool is_selected = (selectedModelOption == option);
                if (ImGui::Selectable(option.c_str(), is_selected)) {
                    selectedModelOption = option;
                    log_window.AddLog("Model option selected: " + option, "INFO"); // Optional log
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus(); // Keep selected on top
            }
            ImGui::EndCombo();
        }

        // Available Columns
        // --- Available Columns with Search ---
        ImGui::PushFont(UIStyle::subheadingFont);
        ImGui::Text("Available Columns:");
        ImGui::PopFont();
        tips.ShowToolTip(helper.Feature_And_Target_text, "Features_And_Target");
        ImGui::PushID("Features_And_Target");

        // Add a search input field
        static char searchBuffer[128] = "";
        ImGui::InputTextWithHint("##SearchColumns", "Search columns...", searchBuffer, IM_ARRAYSIZE(searchBuffer));

        // Filter columns using search
        std::vector<std::string> filteredColumns;
        for (const auto& col : helper.columnsForFeatures) {
            std::string colLower = col;
            std::string searchLower = searchBuffer;
            std::transform(colLower.begin(), colLower.end(), colLower.begin(), ::tolower);
            std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

            if (colLower.find(searchLower) != std::string::npos) {
                filteredColumns.push_back(col);
            }
        }

        // Convert to const char* for ImGui
        std::vector<const char*> filteredCStrs;
        for (const auto& str : filteredColumns) {
            filteredCStrs.push_back(str.c_str());
        }

        // Track selected column name (optional)
        static std::string selectedColName;
        static int filteredIndex = -1;

        // Render filtered ListBox
        ImGui::ListBox("##FilteredColumns", &filteredIndex,
            [](void* data, int idx, const char** out_text) {
                auto& vec = *static_cast<std::vector<const char*>*>(data);
                *out_text = vec[idx];
                return true;
            },
            &filteredCStrs, filteredCStrs.size(), 6);

        // Update selectedCol from filtered list
        if (filteredIndex >= 0 && filteredIndex < filteredColumns.size()) {
            auto it = std::find(helper.columnsForFeatures.begin(), helper.columnsForFeatures.end(), filteredColumns[filteredIndex]);
            if (it != helper.columnsForFeatures.end()) {
                helper.selectedCol = std::distance(helper.columnsForFeatures.begin(), it);
            }
        }

        // Features Section
        ImGui::Text("Features:");
        ImGui::SameLine();
        if (ImGui::Button("Add Feature")) {
            if (helper.selectedCol != -1) {
                helper.featureList.push_back(helper.columnsForFeatures[helper.selectedCol]);
                log_window.AddLog(helper.columnsForFeatures[helper.selectedCol] + ": Was Added to the List of Features");
                helper.columnsForFeatures.erase(helper.columnsForFeatures.begin() + helper.selectedCol);
                SessionManager::getInstance().setFeatures(helper.featureList);
                helper.selectedCol = -1;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Features")) {
            // Add features back to columns
            for (const auto& feature : helper.featureList) {
                if (std::find(helper.columnsForFeatures.begin(), helper.columnsForFeatures.end(), feature) == helper.columnsForFeatures.end()) {
                    helper.columnsForFeatures.push_back(feature);
                }
            }
            helper.featureList.clear();  // Clear all features 
            log_window.AddLog("Features was Cleared");
        }

        // Display selected features
        for (int i = 0; i < helper.featureList.size(); ++i) {
            ImGui::PushID(i);  // Unique ID for ImGui elements

            ImGui::Text("%s", helper.featureList[i].c_str());
            ImGui::SameLine();

            // Style the 'X' button a bit smaller
            if (ImGui::SmallButton("x")) {
                // Return it back to available columns
                if (std::find(helper.columnsForFeatures.begin(), helper.columnsForFeatures.end(), helper.featureList[i]) == helper.columnsForFeatures.end()) {
                    helper.columnsForFeatures.push_back(helper.featureList[i]);
                }

                log_window.AddLog(helper.featureList[i] + ": Removed from Features");
                helper.featureList.erase(helper.featureList.begin() + i);
                SessionManager::getInstance().setFeatures(helper.featureList);
                --i; // Adjust index after erase
            }

            ImGui::PopID();
        }

       
        if (selectedModelOption == "Supervised") {
            // Target Section
            ImGui::Text("Target:");
            ImGui::SameLine();
            if (ImGui::Button("Add Target")) {
                if (helper.selectedCol != -1 && helper.targetList.empty()) { // Only allow one target
                    helper.targetList.push_back(helper.columnsForFeatures[helper.selectedCol]);
                    log_window.AddLog(helper.columnsForFeatures[helper.selectedCol] + ": Was Made Target");
                    helper.columnsForFeatures.erase(helper.columnsForFeatures.begin() + helper.selectedCol);
                    SessionManager::getInstance().setTarget(helper.targetList[0]);
                    helper.selectedCol = -1;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear Target")) {
                // Add target back to columns
                if (!helper.targetList.empty()) {
                    const auto& target = helper.targetList[0]; // Only one target
                    if (std::find(helper.columnsForFeatures.begin(), helper.columnsForFeatures.end(), target) == helper.columnsForFeatures.end()) {
                        helper.columnsForFeatures.push_back(target);
                    }
                    helper.targetList.clear();  // Clear target selection
                    log_window.AddLog("Target Cleared");
                }
            }
            // Display selected target
            if (!helper.targetList.empty()) {
                ImGui::Text("Selected Target: %s", helper.targetList[0].c_str());
            }

            if (helper.featureList.size() >= 1 && !helper.targetList.empty() && SessionManager::getInstance().getModelPresent() == false) {
                ImGui::Text("Features And Taget are ready");
                SessionManager::getInstance().setModelMethod("Supervised");
                SessionManager::getInstance().setPreprocessed(true);
            }
            else
            {
                SessionManager::getInstance().setPreprocessed(false);
            }
        }
        else {
            if (helper.featureList.size() >= 1 && SessionManager::getInstance().getModelPresent() == false) {
                ImGui::Text("Features are ready");
                SessionManager::getInstance().setModelMethod("Unsupervised");
                SessionManager::getInstance().setPreprocessed(true);
            }
            else
            {
                SessionManager::getInstance().setPreprocessed(false);
            }
        }

        ImGui::PopID(); // End Features section
    }   
}