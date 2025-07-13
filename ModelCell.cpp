#include "ModelCell.h"
#include "style.h"

ModelCell::ModelCell() : Cell()
{
     modelTrained = false;  // Flag to indicate if the model is trained
     testSplit = 0.2f; // Default test split percentage
     randomSeed = 42;    // Default random seed
    // Display the accurancy result
    show_accuracy = false;
     show_precision = false;
     show_recall = false;
     show_f1 = false;
     show_mse = false;
     show_r2 = false;

    feature = SessionManager::getInstance().getFeatures();
    data = SessionManager::getInstance().getFilePath();
    target = SessionManager::getInstance().getTarget();
      // Allocate a buffer for the model name, adjust size as needed
    // Assuming this is part of a class
    classificationModels = { "SVM", "Decision Tree", "Random Forest", "Logistic Regression", "GaussianNB", "KNeighborsClassifier"};
    regressionModels = { "Linear Regression", "SVR", "Ridge", "KNeighborsRegressor"};
    clusterModels = { "Kmeans" };

    if (SessionManager::getInstance().getModelMethod() == "Supervised")
    {
        ModelOptions.insert(ModelOptions.end(), classificationModels.begin(), classificationModels.end());
        ModelOptions.insert(ModelOptions.end(), regressionModels.begin(), regressionModels.end());
    }
    else if(SessionManager::getInstance().getModelMethod() == "Unsupervised")
    {
        ModelOptions.insert(ModelOptions.end(), clusterModels.begin(), clusterModels.end());
    }
    if (selectedModelOption == "" )
    {
        selectedModelOption = SessionManager::getInstance().getSelectedModel();
        modelTrained = SessionManager::getInstance().getModelTrained();
        std::vector<float> Metrics_list = { SessionManager::getInstance().getAccuracy(), SessionManager::getInstance().getPrecision(), SessionManager::getInstance().getRecall(), SessionManager::getInstance().getF1()};
        metrics_float.insert(metrics_float.end(), Metrics_list.begin(), Metrics_list.end());
    }
    else{ selectedModelOption = "None"; }
    
    showModelOptions = false; // Flag to show/hide options

    log_window.AddLog("Model Cell Loaded Successful");  
    InitializePythonBindings();
}

void ModelCell::ShowCellUI()
{
        Cell::ShowCellUI();
        ImGui::PushFont(UIStyle::headingFont);
        ImGui::Text("Model Training");
        ImGui::PopFont();
        
        // Test Split Input Field
        ImGui::Text("Enter Test Split (e.g., 0.2 for 20%%):");
        ImGui::InputFloat("Test Split", &testSplit);
        ImGui::SameLine();
        if (ImGui::Button("Set Test Split")) {
            SessionManager::getInstance().setTestSplit(testSplit);
        }
        tips.ShowToolTip(test_split_text, "test_split");

        // Random Seed Input Field
        ImGui::Text("Enter Random Seed:");
        ImGui::InputInt("Random Seed", &randomSeed);
        ImGui::SameLine();
        if (ImGui::Button("Set Random Seed")) {
            SessionManager::getInstance().setRandomSeed(randomSeed);
        }
        tips.ShowToolTip(random_seed_text, "random_seed");
     
        ImGui::Text("Select Model");
        // Begin Combo for selecting preprocessing option
        if (ImGui::BeginCombo("Model Options", selectedModelOption.c_str())){
            
            // Loop through preprocessing options
            for (const auto& option : ModelOptions) {
                bool is_selected = (selectedModelOption == option);
                
                // Display each option as a selectable item
                if (ImGui::Selectable(option.c_str(), is_selected)) {
                    selectedModelOption = option; // Store selected option
                    showModelOptions = false;    // Hide options after selection

                    ss.str("");        // clear the buffer
                    ss.clear();        // clear any error flags
                    // Trigger specific preprocessing actions
                    ss << option << " Selected!";
                    // Call the function or trigger functionality for "Generate Columns"
                    std::cout << ss.str() << std::endl;
                    log_window.AddLog(ss.str(), "INFO");
            }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();  // This ensures proper focus behavior.
                }

        }
            ImGui::EndCombo();  // End the combo box

    }
        tips.ShowToolTip(model_text, "model");
        if (std::find(classificationModels.begin(), classificationModels.end(), selectedModelOption) != classificationModels.end())
        {
            Classification_Models();
        }
        else if (std::find(regressionModels.begin(), regressionModels.end(), selectedModelOption) != regressionModels.end())
        {
            Regression_Models();
        }
        else if (std::find(clusterModels.begin(), clusterModels.end(), selectedModelOption) != clusterModels.end())
        {
            Unsupervised_Models();
        }

        SessionManager::getInstance().setSelectedModel(selectedModelOption);
     
        ImGui::Separator();
}

void ModelCell::DisplayNewMetrics(const std::vector<float>& metrics)
{
    if (std::find(regressionModels.begin(), regressionModels.end(), selectedModelOption) != regressionModels.end())
    {
        ImGui::Checkbox("Mean Square Error", &show_mse);
        ImGui::Checkbox("R2", &show_r2);

        if (show_mse && metrics.size() > 0) {
            ImGui::Text("Mean Squared Error : %.4f", metrics[0]);
        }
        if (show_r2 && metrics.size() > 1) {
            ImGui::Text("R² Score: %.4f", metrics[1]);
        }
    }
    else if(std::find(classificationModels.begin(), classificationModels.end(), selectedModelOption) != classificationModels.end()) {
        ImGui::Checkbox("Accuracy", &show_accuracy);
        ImGui::Checkbox("Precision", &show_precision);
        ImGui::Checkbox("Recall", &show_recall);
        ImGui::Checkbox("F1 Score", &show_f1);

        if (metrics.size() >= 4) {
            SetMetrics(metrics[0], metrics[1], metrics[2], metrics[3]);
        }

        if (show_accuracy && metrics.size() > 0) {
            ImGui::Text("Accuracy: %.4f", metrics[0]);
        }
        if (show_precision && metrics.size() > 1) {
            ImGui::Text("Precision: %.4f", metrics[1]);
        }
        if (show_recall && metrics.size() > 2) {
            ImGui::Text("Recall: %.4f", metrics[2]);
        }
        if (show_f1 && metrics.size() > 3) {
            ImGui::Text("F1 Score: %.4f", metrics[3]);
        }
    }
    
}

std::string ModelCell::OpenSaveDialog()
{
    const char* filter_patterns[] = { "*.joblib" };  // Optional file type filter
    const char* path = tinyfd_saveFileDialog(
        "Save Model",               // Dialog title
        "model.joblib",             // Default file name
        1,                          // Number of file types in filter
        filter_patterns,            // File type filter
        "Joblib Files (*.joblib)"   // Filter description
    );

    return path ? std::string(path) : "";  // Return empty string if no file chosen
}


void ModelCell::SetMetrics(float accuracy, float precision, float recall, float f1)
{
    SessionManager::getInstance().setAccuracy(accuracy);
    SessionManager::getInstance().setPrecision(precision);
    SessionManager::getInstance().setRecall(recall);
    SessionManager::getInstance().setF1(f1);

}

void ModelCell::Regression_Models()
{
    static std::future < std::tuple < std::vector<float>, std::vector<float>, std::vector<float>, std::string >> futureResult;
    static bool isProcessing = false;
    static bool logProcessingStarted = false;
   
    if (!isProcessing && ImGui::Button("Run Model")) {
        ss.str("");        // clear the buffer
        ss.clear();        // clear any error flags
        ss << "Starting" << selectedModelOption << "Model Training...";

        log_window.AddLog(ss.str(), "INFO");

        feature = SessionManager::getInstance().getFeatures();
        target = SessionManager::getInstance().getTarget();
        // Start the async task
        futureResult = ThreadManager::getInstance().submitTask(Run_Regression, selectedModelOption, data, target, feature, randomSeed, testSplit);
        isProcessing = true;
        logProcessingStarted = false;  // Reset the flag
    }
  
    if (isProcessing) {
        ImGui::Text("Processing... Please wait.");

        // Only log once when processing starts
        if (!logProcessingStarted) {
            ss << selectedModelOption << "Model Training Ongoing...";
            log_window.AddLog(ss.str(), "INFO");
            logProcessingStarted = true;
        }

        // Check if the task is complete without blocking
        auto status = futureResult.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            // Get the result
            py::gil_scoped_acquire gil;  // Acquire GIL before touching Python stuff

            result_r = futureResult.get();
            y_pred_r = std::get<0>(result_r);
            y_true_r = std::get<1>(result_r);
            metrics_float = std::get<2>(result_r);
            std::cout << metrics_float[0] << std::endl ;
            temp_model_path = std::get<3>(result_r);
            SessionManager::getInstance().setModelFilename(temp_model_path);
           

            modelTrained = !trainedModel.is_none();  // Check if training was successful

            if (modelTrained) {
                ss << selectedModelOption << "Model Training Completed Successfully!";
                log_window.AddLog(ss.str(), "SUCCESS");
                std::cout << "Model trained successfully!" << std::endl;
            }
            else {
                log_window.AddLog("Model training failed.", "ERROR");
                std::cerr << "Model training failed." << std::endl;
            }

            isProcessing = false;  // Mark processing as done
        }
    }

    // Handling model export and metrics display
    if (modelTrained) {

        // Assume `std::string temp_model_path;` is set after Python runs the model
        if (ImGui::Button("Export Model")) {
            std::string exportPath = OpenSaveDialog();  // Target path chosen by user
            if (!exportPath.empty()) {
                bool success = MoveFileTo(exportPath, temp_model_path);  // We'll define this next
                if (success) {
                    ImGui::Text("Model exported successfully to %s", exportPath.c_str());
                    log_window.AddLog("Model exported successfully!", "SUCCESS");
                    log_window.AddLog("Classification Graph Ready.");
                    SessionManager::getInstance().setModelFilename(exportPath);
                    
                }
                else {
                    ImGui::Text("Failed to export model.");
                    log_window.AddLog("Model export failed!", "ERROR");
                }
            }
        }

        // Display metrics if available
        if (!metrics.is_none()) {
            DisplayNewMetrics(metrics_float);
            
            
                //SessionManager::getInstance().setPredictedValues(y_pred_r, y_true_r);
                SessionManager::getInstance().setYpred(y_pred_r);
                SessionManager::getInstance().setYtest(y_true_r);
                
            

        }
        else {
            log_window.AddLog("Failed to Retrieve Metrics.", "ERROR");
            std::cerr << "Failed to retrieve metrics." << std::endl;
        }
        SessionManager::getInstance().setModelTrained(true);
    }
}


void ModelCell::Classification_Models()
{
    static std::future<std::tuple<std::vector<int>, std::vector<int>, std::vector<float>, std::string>> futureResult;
    static bool isProcessing = false;
    static bool logProcessingStarted = false;
   
    if (!isProcessing && ImGui::Button("Run Model")) {
        ss.str("");
        ss.clear();
        ss << "Starting " << selectedModelOption << " Model Training...";
        log_window.AddLog(ss.str(), "INFO");
        feature = SessionManager::getInstance().getFeatures();
        target = SessionManager::getInstance().getTarget();
        futureResult = ThreadManager::getInstance().submitTask(Run_Classification,
            selectedModelOption, data, target, feature, randomSeed, testSplit);

        isProcessing = true;
        logProcessingStarted = false;
    }
    
    if (isProcessing) {
        ImGui::Text("Processing... Please wait.");

        if (!logProcessingStarted) {
            ss.str("");
            ss.clear();
            ss << selectedModelOption << " Model Training Ongoing...";
            log_window.AddLog(ss.str(), "INFO");
            logProcessingStarted = true;
        }

        auto status = futureResult.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            // Safely acquire GIL before touching Python again (for safety if needed later)
            py::gil_scoped_acquire gil;

            result_c = futureResult.get();
            y_pred = std::get<0>(result_c);
            y_true = std::get<1>(result_c);
            metrics_float = std::get<2>(result_c);
            temp_model_path = std::get<3>(result_c);
            SessionManager::getInstance().setModelFilename(temp_model_path);


            modelTrained = !y_pred.empty();  // If prediction results exist, assume success

            if (modelTrained) {
                ss.str(""); ss.clear();
                ss << selectedModelOption << " Model Training Completed Successfully!";
                log_window.AddLog(ss.str(), "SUCCESS");
                std::cout << "Model trained successfully!" << std::endl;
            }
            else {
                log_window.AddLog("Model training failed.", "ERROR");
                std::cerr << "Model training failed." << std::endl;
            }

            isProcessing = false;
        }
    }

    //Export and Display
    if (modelTrained) {
        SessionManager::getInstance().setModelTrained(true);
        // Assume `std::string temp_model_path;` is set after Python runs the model
        if (ImGui::Button("Export Model")) {
            std::string exportPath = OpenSaveDialog();  // Target path chosen by user
            if (!exportPath.empty()) {
                bool success = MoveFileTo(exportPath, temp_model_path);  // We'll define this next
                if (success) {
                    ImGui::Text("Model exported successfully to %s", exportPath.c_str());
                    log_window.AddLog("Model exported successfully!", "SUCCESS");
                    log_window.AddLog("Classification Graph Ready.");
                    SessionManager::getInstance().setModelFilename(exportPath);
                    
                }
                else {
                    ImGui::Text("Failed to export model.");
                    log_window.AddLog("Model export failed!", "ERROR");
                }
            }
        }

        if (!metrics_float.empty()) {
            DisplayNewMetrics(metrics_float);  // You'll update this function to take vector<float>
            
                std::vector<float> Y_pred(y_pred.begin(), y_pred.end());
                std::vector<float> Y_test(y_true.begin(), y_true.end());
                SessionManager::getInstance().setYpred(Y_pred);
                SessionManager::getInstance().setYtest(Y_test);

            
            
        }
        else {
            log_window.AddLog("Failed to Retrieve Metrics.", "ERROR");
            std::cerr << "Failed to retrieve metrics." << std::endl;
        }
    }
}

void ModelCell::Unsupervised_Models()
{
    static std::future<std::tuple<std::vector<int>, float, std::string>> futureResult;
    static bool isProcessing = false;
    static bool logProcessingStarted = false;

    ImGui::SliderInt("Number of Clusters (k)", &k_clusters, 2, 20);
    if (!isProcessing && ImGui::Button("Run Unsupervised Model")) {
        ss.str(""); ss.clear();
        ss << "Starting " << selectedModelOption << " Clustering...";
        log_window.AddLog(ss.str(), "INFO");

        feature = SessionManager::getInstance().getFeatures();
        futureResult = ThreadManager::getInstance().submitTask(Run_Unsupervised,
            selectedModelOption, data, feature, randomSeed, k_clusters); // assume k_clusters is int

        isProcessing = true;
        logProcessingStarted = false;
    }

    if (isProcessing) {
        ImGui::Text("Clustering in progress...");

        if (!logProcessingStarted) {
            ss.str(""); ss.clear();
            ss << selectedModelOption << " clustering ongoing...";
            log_window.AddLog(ss.str(), "INFO");
            logProcessingStarted = true;
        }

        auto status = futureResult.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            py::gil_scoped_acquire gil;

            result_u = futureResult.get();
            cluster_labels = std::get<0>(result_u);
            silhouette_score = std::get<1>(result_u);
            temp_model_path = std::get<2>(result_u);
            SessionManager::getInstance().setModelFilename(temp_model_path);

            modelTrained = !cluster_labels.empty();

            if (modelTrained) {
                ss.str(""); ss.clear();
                ss << selectedModelOption << " clustering completed. Silhouette Score: " << silhouette_score;
                log_window.AddLog(ss.str(), "SUCCESS");
            }
            else {
                log_window.AddLog("Clustering failed.", "ERROR");
            }

            isProcessing = false;
        }
    }

    if (modelTrained) {
        SessionManager::getInstance().setModelTrained(true);

        if (ImGui::Button("Export Clustering Model")) {
            std::string exportPath = OpenSaveDialog();
            if (!exportPath.empty()) {
                bool success = MoveFileTo(exportPath, temp_model_path);
                if (success) {
                    log_window.AddLog("Clustering model exported!", "SUCCESS");
                    SessionManager::getInstance().setModelFilename(exportPath);
                }
                else {
                    log_window.AddLog("Failed to export clustering model.", "ERROR");
                }
            }
        }

        ImGui::Text("Silhouette Score: %.4f", silhouette_score);
        if (ImGui::Button("Visualize Clusters")) {
            SessionManager::getInstance().setMatricsWindowPresent(true);
            log_window.AddLog("Clustering visualization activated.");
        }
    }
}




bool ModelCell::MoveFileTo(const std::string& destination, const std::string& source) {
    try {
        fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
        fs::remove(source);  // Optional: remove temp after copying
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "File move error: " << e.what() << '\n';
        return false;
    }
}
