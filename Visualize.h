#ifndef Visualize_H
#define Visualize_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "imgui.h"

void Display_CSV(float width, float hight, float x_pos, float y_pos);
void Display_MetricsGraph(float width, float hight, float x_pos, float y_pos);

class Visualize
{
public:

    Visualize();
    void RenderCSV(const std::vector<std::vector<std::string>>& data);
    void PreprocessingUI();
    std::vector<std::vector<std::string>> LoadCSV(const std::string& fileName, size_t maxRows);
    void RenderSavedPlot(const std::string& imagePath, float width, float hight);
    void RenderMetricsGraph(const std::string& title, float accuracy, float precision, float recall, float f1, float width, float hight);
    std::tuple<std::vector<float>, std::vector<float>> PredictedValues;
    std::stringstream ss;
};




#endif