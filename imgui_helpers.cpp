#include "imguihelper.h"
#include "Session_Manger.h"
#include "tooltips.h"
#include "imgui.h"
#include "python_binding.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>

ToolTips tips;
void ImguiHelper::Assign_values(){
    std::memset(newColumnName, 0, sizeof(newColumnName));
    selectedOperation = 0;
    selectedCol = -1;
    selectedPreprocessingOption = "None";
    showPreprocessingOptions = false;
    path = SessionManager::getInstance().getFilePath();
    preprocessingOptions = { "Generate and Remove Columns",  "Normalize Columns", "Label Encoding","OneHot Encoding" , "Handle Missing Values", "Text Cleaning"};
    column_groups = ReturnColumns(path);
    columns = std::get<0>(column_groups); // Populate the column list from Python
    
    selectedColumns.resize(columns.size(), false);
    columnsForFeatures = columns;      // Columns for feature selection
    columnsForNormalization = std::get<1>(column_groups); // Columns for normalization
    columnsForEncoding = std::get<2>(column_groups);
    columnNamesCStr.reserve(columns.size());
    for (const auto& col : columns)
        columnNamesCStr.push_back(col.c_str());

    SessionManager::getInstance().SetNonNumericsColumns(columnsForEncoding);

}

void ImguiHelper::Select_Target_and_Features()
{

    if (!columns.empty()) {
        // Available Columns
        ImGui::Text("Available Columns:");
        tips.ShowToolTip(Feature_And_Target_text, "Features_And_Target");
        ImGui::PushID("Features_And_Target");
        ImGui::ListBox("##Columns", &selectedCol,
            [](void* data, int idx, const char** out_text) {
                auto& cols = *static_cast<std::vector<std::string>*>(data);
                *out_text = cols[idx].c_str();
                return true;
            },
            &columnsForFeatures, columnsForFeatures.size(), 5);


        // Features Section
        ImGui::Text("Features:");
        ImGui::SameLine();
        if (ImGui::Button("Add Feature")) {
            if (selectedCol != -1) {
                featureList.push_back(columnsForFeatures[selectedCol]);
                log_window.AddLog(columnsForFeatures[selectedCol] + ": Was Added to the List of Features");
                columnsForFeatures.erase(columnsForFeatures.begin() + selectedCol);
                SessionManager::getInstance().setFeatures(featureList);
                selectedCol = -1;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Features")) {
            // Add features back to columns
            for (const auto& feature : featureList) {
                if (std::find(columnsForFeatures.begin(), columnsForFeatures.end(), feature) == columnsForFeatures.end()) {
                    columnsForFeatures.push_back(feature);
                }
            }
            featureList.clear();  // Clear all features 
            log_window.AddLog("Features was Cleared");
        }

        // Display selected features
        for (const auto& feature : featureList) {
            ImGui::Text("%s", feature.c_str());
        }

        // Target Section
        ImGui::Text("Target:");
        ImGui::SameLine();
        if (ImGui::Button("Add Target")) {
            if (selectedCol != -1 && targetList.empty()) { // Only allow one target
                targetList.push_back(columnsForFeatures[selectedCol]);
                log_window.AddLog(columnsForFeatures[selectedCol] + ": Was Made Target");
                columnsForFeatures.erase(columnsForFeatures.begin() + selectedCol);
                SessionManager::getInstance().setTarget(targetList[0]);
                selectedCol = -1;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Target")) {
            // Add target back to columns
            if (!targetList.empty()) {
                const auto& target = targetList[0]; // Only one target
                if (std::find(columnsForFeatures.begin(), columnsForFeatures.end(), target) == columnsForFeatures.end()) {
                    columnsForFeatures.push_back(target);
                }
                targetList.clear();  // Clear target selection
                log_window.AddLog("Target Cleared");
            }
        }



        // Display selected target
        if (!targetList.empty()) {
            ImGui::Text("Selected Target: %s", targetList[0].c_str());
        }

        if (featureList.size() >= 2 && !targetList.empty() && SessionManager::getInstance().getModelPresent() == false) {
            ImGui::Text("Features And Taget are ready");
            SessionManager::getInstance().setPreprocessed(true);

        }
        else
        {
            SessionManager::getInstance().setPreprocessed(false);
        }
        ImGui::PopID(); // End Features section
    }
}



void ImguiHelper::Label_Encoding(std::string encoding_type)
{
    if (!path.empty()) {
        static int selectedColumnIndex = 0;
        tips.ShowToolTip(Label_Encoding_text, "Label_Encoding_Columns");
        // Check if columns exist
        if (!columnsForEncoding.empty()) {
            // Prepare column strings and C-string pointers
            std::vector<std::string> columnStrings(columnsForEncoding.begin(), columnsForEncoding.end());
            std::vector<const char*> columnCStrs;
            for (const auto& str : columnStrings) {
                columnCStrs.push_back(str.c_str());
            }

            // Dropdown for selecting a column
            if (ImGui::Combo("Select Column", &selectedColumnIndex, columnCStrs.data(), static_cast<int>(columnCStrs.size()))) {
                std::cout << "Selected column index: " << selectedColumnIndex << "\n";
            }

            // Generate new column name
            if (encoding_type == "label") { std::string newColumnName = columnsForEncoding[selectedColumnIndex] + "_label_encoded"; }
            else if (encoding_type == "onehot") { std::string newColumnName = columnsForEncoding[selectedColumnIndex] + "_onehot_encoded"; }
            // Button to apply label encoding
            if (ImGui::Button("Apply encoding_type Encoding")) {
                std::string columnName = columnsForEncoding[selectedColumnIndex];
                columnsForEncoding = LabelEncodeColumn(path, columnName, newColumnName, encoding_type);
                log_window.AddLog(columnName + ": Was " + encoding_type + "Encoded");
                RefreshColumns();
                // Reset selectedColumnIndex to avoid invalid selections
                if (selectedColumnIndex >= static_cast<int>(columnsForEncoding.size())) {
                    selectedColumnIndex = 0;
                }
            }
        }
        else {
            ImGui::Text("No columns available for selection.");
            log_window.AddLog("No columns available for selection.", "ERROR");
        }
    }
}



void ImguiHelper::Text_Cleaning_UI()
{
    static int selectedTextColumnIndex = 0;
    if (!columnsForEncoding.empty()) {
        std::vector<const char*> columnCStrs;
        for (const auto& col : columnsForEncoding)
            columnCStrs.push_back(col.c_str());

        ImGui::Combo("Select Text Column", &selectedTextColumnIndex, columnCStrs.data(), columnCStrs.size());

        static bool lowercase = false;
        static bool punctuation = false;
        static bool stopwords = false;
        static bool lemmatize = false;
        static bool remove_extra_spaces = false;

        ImGui::Checkbox("Remove Extra Spaces", &remove_extra_spaces);
        ImGui::Checkbox("Lowercase", &lowercase);
        ImGui::Checkbox("Remove Punctuation", &punctuation);
        ImGui::Checkbox("Remove Stopwords", &stopwords);
        ImGui::Checkbox("Lemmatize", &lemmatize);

        if (ImGui::Button("Apply Cleaning")) {
            std::string columnName = columnsForEncoding[selectedTextColumnIndex];
            if (remove_extra_spaces) selected_cleaning_steps.push_back("remove_extra_spaces");
            if (lowercase) selected_cleaning_steps.push_back("lowercase");
            if (punctuation) selected_cleaning_steps.push_back("remove_punctuation");
            if (stopwords) selected_cleaning_steps.push_back("remove_stopwords");
            if (lemmatize) selected_cleaning_steps.push_back("lemmatize");
            ApplyTextCleaning(path, columnName, selected_cleaning_steps);
            log_window.AddLog("Text cleaning applied to: " + columnName);
            RefreshColumns();
        }
    }
}



void ImguiHelper::Generate_Columns() {
    if (path.empty()) return;

    if (!columns.empty()) {
        tips.ShowToolTip(Generate_Columns_text, "Generate_Columns");

        // Complex Expression Builder
        if (ImGui::CollapsingHeader("Create Complex Operation")) {
            for (int i = 0; i < inputs.size(); ++i) {
                ImGui::PushID(i);

                ImGui::SetNextItemWidth(100);
                ImGui::Combo("Pre Op", &inputs[i].selected_pre_op, pre_ops, IM_ARRAYSIZE(pre_ops));

                ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::Combo("Column", &inputs[i].selected_column, columnNamesCStr.data(), columnNamesCStr.size());

                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::Combo("Post Op", &inputs[i].selected_post_op, post_ops, IM_ARRAYSIZE(post_ops));

                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::InputFloat("Number", &inputs[i].selected_number);  // numericConstant should be a float member of your helper
          
                ImGui::SameLine();
                if (ImGui::Button("Remove")) {
                    inputs.erase(inputs.begin() + i);
                    ImGui::PopID();
                    break;
                }

                ImGui::PopID();
            }

            if (ImGui::Button("Add Column Operation")) {
                inputs.emplace_back(); // Adds a new operation row
            }

            ImGui::PushID("new_col_name");
            ImGui::InputText("New Column Name", newColumnName, IM_ARRAYSIZE(newColumnName));
            ImGui::PopID();
           
             
                if (ImGui::Button("Create New Column")) {
                    std::string expression;
                    for (auto& input : inputs) {
                        expression += pre_ops[input.selected_pre_op];
                        expression += "df[\"" + columns[input.selected_column] + "\"]";
                        expression += std::string(post_ops[input.selected_post_op]) + " ";
                        expression += std::to_string(input.selected_number);

                    }
                    std::cout << "Generated expression: " << expression << " | Size: " << expression.size() + 1 << std::endl;

                    if (!expression.empty()) {
                        CreateNewColumn_exp(path, expression, newColumnName);
                        log_window.AddLog("Generated Expression: " + expression);
                        RefreshColumns();
                    }
                
            }
        }

        //  Delete Columns Section (Unchanged)
        if (ImGui::CollapsingHeader("Delete Columns")) {
            for (size_t i = 0; i < columns.size(); ++i) {
                bool isSelected = selectedColumns[i];
                if (ImGui::Checkbox(("Remove " + columns[i]).c_str(), &isSelected)) {
                    selectedColumns[i] = isSelected;
                    log_window.AddLog(columns[i] + ": Was Selected for Deletion");
                }
            }

            if (ImGui::Button("Delete Selected Columns")) {
                std::vector<std::string> columnsToRemove;
                for (size_t i = 0; i < columns.size(); ++i) {
                    if (selectedColumns[i]) {
                        columnsToRemove.push_back(columns[i]);
                        log_window.AddLog(columns[i] + " Was Deleted");
                    }
                }

                auto updatedColumns = RemoveColumns(path, columnsToRemove);
                if (!updatedColumns.empty()) {
                    columns = updatedColumns;
                    selectedColumns.assign(columns.size(), false);
                    RefreshColumns();
                    std::cout << "Columns after delete: " << columns.size() << std::endl;
                }
                else {
                    log_window.AddLog("Failed to remove columns.", "ERROR");
                }
            }
        }
    }
}





void ImguiHelper::Select_Normalization() {
    if (!columnsForNormalization.empty()) {
        ImGui::PushID("Normalization");
        // Step 1: Display Columns for Normalization with checkboxes
        tips.ShowToolTip(Normalization_text, "Normalization_Columns");
        if (ImGui::CollapsingHeader("Select Columns for Normalization")) {
            for (size_t i = 0; i < columnsForNormalization.size(); ++i) {
                if (i < selectedColumns.size()) {
                    bool isSelected = selectedColumns[i];
                    if (ImGui::Checkbox(columnsForNormalization[i].c_str(), &isSelected)) {
                        selectedColumns[i] = isSelected;
                        log_window.AddLog(columnsForNormalization[i] + ": Was selected for Normalization");
                    }
                }
                else {
                    ImGui::Text("Mismatch between columns and selectedColumns");
                    log_window.AddLog("Mismatch between columns and selectedColumns");
                }
            }
        }


        // Step 3: Select the normalization method
        const char* methods[] = { "Min-Max Scaling", "Z-score Normalization", "Robust Scaling" };
        static int selectedMethod = 0; // Currently selected method
        if (ImGui::BeginCombo("Normalization Method", methods[selectedMethod])) {
            for (int n = 0; n < IM_ARRAYSIZE(methods); n++) {
                bool is_selected = (selectedMethod == n);
                if (ImGui::Selectable(methods[n], is_selected)) {
                    selectedMethod = n;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Step 4: Button to apply normalization
        // Apply Normalization
        ImGui::Text("Actions:");
        // Button to normalize columns
        if (ImGui::Button("Normalize Columns")) {
            // Check if any column is selected
            bool anyColumnSelected = std::any_of(selectedColumns.begin(), selectedColumns.end(), [](bool selected) { return selected; });

            if (anyColumnSelected) {
                // Collect selected column names
                std::vector<std::string> selectedColumnNames;
                std::vector<std::string> newColumnNames;
                std::vector<std::string> selectedMethods;

                for (size_t i = 0; i < selectedColumns.size(); ++i) {
                    if (selectedColumns[i]) {
                        selectedColumnNames.push_back(columnsForNormalization[i]);

                        // Create new column name by appending the method
                        std::string newColumnName = columnsForNormalization[i] + "_" + methods[selectedMethod];
                        newColumnNames.push_back(newColumnName);
                        selectedMethods.push_back(methods[selectedMethod]);
                    }
                }

                // Ensure you have a method selected for each column
                if (!selectedColumnNames.empty()) {
                    // Call backend function to normalize columns with new names
                    auto updatedColumns = CreateNormalizedColumn(path, selectedColumnNames, selectedMethods, newColumnNames);

                    // Update UI based on success or failure
                    if (!updatedColumns.empty()) {
                        ImGui::Text("Columns normalized successfully!");
                        columnsForNormalization = updatedColumns; // Reload columns after normalization
                        log_window.AddLog("Selected Column Was Nurmalized");
                        selectedColumns.resize(columnsForNormalization.size(), false); // Reset selection tracking
                        RefreshColumns();

                    }
                    else {
                        ImGui::Text("Error normalizing columns.");
                        log_window.AddLog("Error normalizing columns.", "ERROR");
                    }
                }
                else {
                    ImGui::Text("Mismatch between selected columns and methods.");
                    log_window.AddLog("Mismatch between selected columns and methods.", "ERROR");
                }
            }
            else {
                ImGui::Text("Please select at least one column before normalizing.");
                log_window.AddLog("Please select at least one column before normalizing.", "ERROR");
            }
        }

        // Step 5: Show applied normalizations (optional, if needed)
        ImGui::Separator();
        ImGui::Text("Applied Normalizations:");
        for (const auto& [column, method] : columnNormalizationMap) {
            ImGui::Text("%s: %s", column.c_str(), method.c_str());
        }
        ImGui::PopID();
    }
}


void ImguiHelper::HandleMissingValue()
{
    if (!columnsForNormalization.empty()) {
        ImGui::PushID("HnadleMisssingValues");

        if (ImGui::CollapsingHeader("Select Columns for Handling Missing Values")) {
            for (size_t i = 0; i < columnsForNormalization.size(); ++i) {
                if (i < selectedColumns.size()) {
                    bool isSelected = selectedColumns[i];
                    if (ImGui::Checkbox(columnsForNormalization[i].c_str(), &isSelected)) {
                        selectedColumns[i] = isSelected;
                        log_window.AddLog(columnsForNormalization[i] + ": selected for Missing Value Handling");
                    }
                }
                else {
                    ImGui::Text("Mismatch between columns and selectedColumns");
                    log_window.AddLog("Mismatch between columns and selectedColumns");
                }
            }
        }

        // Select method: Drop or Mean Fill
        const char* methods[] = { "Drop Rows", "Fill with Mean" };
        static int selectedMethod = 0; // Default to "Drop Rows"
        if (ImGui::BeginCombo("Missing Value Method", methods[selectedMethod])) {
            for (int n = 0; n < IM_ARRAYSIZE(methods); ++n) {
                bool is_selected = (selectedMethod == n);
                if (ImGui::Selectable(methods[n], is_selected)) {
                    selectedMethod = n;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Button to apply
        if (ImGui::Button("Handle Missing Values")) {
            std::vector<std::string> selectedColumnNames;
            for (size_t i = 0; i < selectedColumns.size(); ++i) {
                if (selectedColumns[i]) {
                    selectedColumnNames.push_back(columnsForNormalization[i]);
                }
            }

            if (!selectedColumnNames.empty()) {
                std::string method = (selectedMethod == 0) ? "drop" : "mean";
                std::vector<std::string> updatedCols = HandleMissingValues(path, selectedColumnNames, method);

                if (!updatedCols.empty()) {
                    columnsForNormalization = updatedCols;
                    selectedColumns.resize(columnsForNormalization.size(), false);
                    log_window.AddLog("Missing values handled using method: " + method, "SUCCESS");
                    RefreshColumns();
                }
                else {
                    log_window.AddLog("Error handling missing values", "ERROR");
                }
            }
            else {
                log_window.AddLog("No columns selected for handling missing values", "ERROR");
            }
        }
        ImGui::PopID();
    }
}

void ImguiHelper::RefreshColumns() {
    column_groups = ReturnColumns(path);
    columns = std::get<0>(column_groups); // Fetch updated columns from Python
    selectedColumns.resize(columns.size(), false); // Reset selection tracking

    // Start with full column list
    columnsForFeatures = columns;

    // Remove already selected features
    for (const auto& feature : featureList) {
        columnsForFeatures.erase(
            std::remove(columnsForFeatures.begin(), columnsForFeatures.end(), feature),
            columnsForFeatures.end()
        );
    }

    // Remove target if one exists
    if (!targetList.empty()) {
        columnsForFeatures.erase(
            std::remove(columnsForFeatures.begin(), columnsForFeatures.end(), targetList[0]),
            columnsForFeatures.end()
        );
    }

    // Optional: update normalization columns too
    columnsForNormalization = std::get<1>(column_groups);
    columnsForEncoding = std::get<2>(column_groups);

    // Rebuild C-style strings
    columnNamesCStr.clear();
    columnNamesCStr.reserve(columns.size());
    for (const auto& col : columns)
        columnNamesCStr.push_back(col.c_str());

    i = 0;
    SessionManager::getInstance().setDSupdate(true);
}
