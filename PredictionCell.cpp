#include "PredictionCell.h"

PredictionCell::PredictionCell() {
    filename = SessionManager::getInstance().GetModelFileName();
    features = SessionManager::getInstance().getFeatures();
    feature_length = features.size();
    NonNumerics = SessionManager::getInstance().getNonNumericsColumns();
}

void PredictionCell::ShowCellUI() {
    static float result;
    static bool showprediction = false;

    filename = SessionManager::getInstance().GetModelFileName();
    feature_length = features.size();
    Cell::ShowCellUI();

    static std::vector<float> numeric_features;
    static std::vector<std::string> string_features;

    if (numeric_features.size() != feature_length) {
        numeric_features = std::vector<float>(feature_length, 0.0f);
        string_features = std::vector<std::string>(feature_length, "");
    }

    ImGui::Text("Enter The Values of Your Features");
    tips.ShowToolTip(prediction_text, "prediction");

    for (int i = 0; i < feature_length; i++) {
        const std::string& featureName = features[i];
        std::string label = featureName + " Feature " + std::to_string(i);

        if (std::find(NonNumerics.begin(), NonNumerics.end(), featureName) != NonNumerics.end()) {
            // Input text for string features
            static char buffer[128] = "";
            ImGui::InputText(label.c_str(), buffer, IM_ARRAYSIZE(buffer));
            string_features[i] = std::string(buffer);
            numeric_features[i] = 0.0f; // Clear or dummy value
        }
        else {
            // Input float for numeric features
            ImGui::InputFloat(label.c_str(), &numeric_features[i]);
            string_features[i] = ""; // Clear or dummy value
        }
    }

    if (ImGui::Button("Make Prediction")) {
        result = predict_model(filename, numeric_features, string_features);
        showprediction = true;
    }

    if (showprediction) {
        ImGui::Text("Prediction: %.3f", result);
    }

    ImGui::Separator();
}
