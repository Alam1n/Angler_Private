#include "PredictionCell.h"


PredictionCell::PredictionCell()
{

    filename = SessionManager::getInstance().GetModelFileName();
    std::cout << filename << std::endl;
    features = SessionManager::getInstance().getFeatures();
    feature_length = SessionManager::getInstance().getFeatures().size();
   
   
    
}

void  PredictionCell::ShowCellUI()
{
    static float result;
    static bool showprediction = false;

    Cell::ShowCellUI();
    // Assuming feature_length is known beforehand
   
    static std::vector<float> features_values;
    if (features_values.size() != feature_length) {
        features_values = std::vector<float>(feature_length, 0.0f);
    }
    // Test Split Input Field
    ImGui::Text("Enter The Values of Your Features");
    tips.ShowToolTip(prediction_text, "prediction");
    for(int i =0; i < feature_length; i++)
    { 
        std::string label = features[i] + " Feature " + std::to_string(i) ;

        ImGui::InputFloat(label.c_str(), &features_values[i]);
    }
    
    if (ImGui::Button("Make Prediction")) {
        
        result = predict_model(filename, features_values);
        showprediction = true;
        
    }
    if (showprediction) {
        ImGui::Text("Prediction: %.3f", result);
    }
    ImGui::Separator();
    
}

