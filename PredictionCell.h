#ifndef PREDICTION_CELL
#define PREDICTION_CELL

#include "Cell.h"
#include "python_binding.h"


class PredictionCell : public Cell {

public:
    PredictionCell();

    void ShowCellUI() override;

    void SetOutputPath(std::string filepath);

private:
    std::string filename;
    std::vector<float> features_values;
    size_t feature_length;
    std::vector<std::string> features;
    std::string prediction_text = "How It Works\nInput your values and click 'Run' to get the prediction result from your model.";
    std::vector<std::string> NonNumerics;
};
#endif // 
