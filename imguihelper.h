
#include <iostream>
#include "LogsWindow.h"
#include <unordered_map>

#ifndef Im_Gui_Helper
#define Im_Gui_Helper
class ImguiHelper {
public:

    void Assign_values();
    
    void Select_Target_and_Features();
    void Label_Encoding(std::string encoding_type);
    void Text_Cleaning_UI();
    void Generate_Columns();
    void Select_Normalization();
    void HandleMissingValue();
    void RefreshColumns();

    std::vector<const char*> columnNamesCStr;
    std::string path;
    std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::string>> column_groups;
    std::vector<std::string> columns;

    
    
    std::vector<std::string> columnsForFeatures;      // Columns for feature selection
    std::vector<std::string> columnsForNormalization; // Columns for normalization
    std::vector<std::string> columnsForEncoding;
    std::vector<std::string> featureList; // List to store selected feature columns
    std::vector<std::string> targetList;  // List to store selected target column
    std::vector<char> selectedColumns;
    std::vector<std::string> selected_cleaning_steps;// In your ImGuiHelpers.cpp


    
    char newColumnName[128];
    const char* operations[3] = { "sum", "product", "mean" };
    int selectedOperation;
    int selectedCol;
    int i;


    std::vector<std::string> preprocessingOptions;
    std::string selectedPreprocessingOption;
    bool showPreprocessingOptions;

    


    std::string operation;
    std::string columnName;

    std::vector<std::string> columnStrings;
    std::vector<const char*> columnCStrs;
    static int selectedColumnIndex;

    std::string Feature_And_Target_text = "How It Works\nSelect feature and target columns by clicking on their names, then click 'Add Feature' or 'Add Target' to assign them.\nIf you make a mistake, click 'Clear Features' or 'Clear Target' to reset.\nI'll improve this system soon, so please bear with me for now.";
    std::string preprocessing_technique_text = "How It Works\nSelect a preprocessing technique to modify your data.\nMore details will be provided based on the selected technique.";
    std::string Normalization_text = "How It Works\nNormalization is a preprocessing technique that scales your data values to a common range—typically between 0 and 1.";
    std::string Generate_Columns_text = "How It Works\n'Generate Columns' creates new columns by applying mathematical operations to your data.";
    std::string Label_Encoding_text = "How It Works\nLabel encoding converts categorical data (like strings) into numerical values for machine learning models.";

    // Structures
    struct ColumnInput {
        int selected_column = 0;
        int selected_pre_op = 0;
        int selected_post_op = 0;
        float selected_number = 0;
    };
    std::vector<ColumnInput> inputs;

    // Sample data
    const char* pre_ops[10] = { "", "sqr(", "log(", "abs(", "sqrt(", ")", "+", "-", "*", "/" };
    const char* post_ops[9] = { "", "+", "-", "*", "/", "+)", "-)", "*)", ")"};


    // Member variables
    std::unordered_map<std::string, std::string> columnNormalizationMap; // Tracks normalization per column
    int selectedMethod = 0; // Selected normalization method
    std::vector<std::string> normalizationMethods = { "Min-Max Scaling", "Z-score Normalization", "Robust Scaling" };
private:
    LogsWindow log_window;

};
#endif // 
