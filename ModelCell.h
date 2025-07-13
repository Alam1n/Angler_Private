
#ifndef MODEL_CELL
#define MODEL_CELL

#include "Cell.h"
#include "python_binding.h"

class ModelCell : public Cell {
public:
 
    // Constructor
    ModelCell();

    void ShowCellUI() override;

   
    void DisplayNewMetrics(const std::vector<float>& metrics);
   

    
    
    std::string OpenSaveDialog();

    void SetMetrics(float accuracy, float precision, float recall, float f1);

    void Regression_Models();
    void Classification_Models();

    void Unsupervised_Models();

    bool MoveFileTo(const std::string& destination, const std::string& source);





private:

    bool modelTrained;  // Flag to indicate if the model is trained
    float testSplit; // Default test split percentage
    int randomSeed;    // Default random seed
    int k_clusters = 2;
    // Display the accurancy result
    bool show_accuracy;
    bool show_precision;
    bool show_recall;
    bool show_f1;
    bool show_mse;
    bool show_r2;
    bool show_score;
    
    std::vector<std::string> feature;
    std::string data ;
    std::string target;
    char modelName[256] = "";  // Allocate a buffer for the model name, adjust size as needed
    // Assuming this is part of a class
    py::object trainedModel;
    py::tuple metrics;
    std::tuple<std::vector<int>, std::vector<int>, std::vector<float>, std::string> result_c;
    std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::string> result_r;
    std::tuple<std::vector<int>, float, std::string> result_u;
    std::vector<std::string> ModelOptions ;
    std::string selectedModelOption;
    bool showModelOptions; // Flag to show/hide options
    

    std::vector<std::string> classificationModels;
    std::vector<std::string> regressionModels;
    std::vector<std::string> clusterModels;
    std::stringstream ss;

    std::vector<int> y_pred;
    std::vector<int> y_true;
    std::vector<float> y_pred_r;
    std::vector<float> y_true_r;

    std::vector<int> cluster_labels;
    float silhouette_score;
    std::vector<float> metrics_float;
    std::string temp_model_path;

    std::string test_split_text = "What It Is\nThe test split determines how much of the data will be used for testing. The rest will be used for training.";
    std::string random_seed_text = "What It Is\nA random seed is a value used to initialize a pseudorandom number generator. It ensures reproducible results.";
    std::string model_text = "How It Works\nSelect the model you want, then click 'Run Model' to train and evaluate it.";

    
};

#endif // 
