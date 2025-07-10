
#ifndef SESSION_MANAGER_h
#define SESSION_MANGER_h
#include <iostream>
#include <vector>
#include <deque>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include "python_binding.h"
using json = nlohmann::json;


class SessionManager {
public:
    // Get instance of the singleton
    static SessionManager& getInstance() {
        static SessionManager instance;
        return instance;
    }

    // Prevent copying and assignment
    SessionManager(const SessionManager&) = delete;
    void operator=(const SessionManager&) = delete;

    // Methods to set and get file path
    void setFilePath(std::string path) { filePath = path; }
    std::string getFilePath() const { 
        return filePath; }

   
 // Other dynamic data you want to store
    void setOutput(const std::string& out) { output = out; }
    std::string getOutput() const { return output; }

    // Setters for features and target
    void setFeatures(const std::vector<std::string>& features) {
        this->features = features;
    }

    void setTarget(const std::string& target) {
        this->target = target;
    }

    // Getters for features and target
    const std::vector<std::string>& getFeatures() const {
        return features;
    }
    
    const std::string& getTarget() const {
        return target;
    }

    // Setter and Getter for Test Split
    void setTestSplit(float value) { testSplit = value; }
    float getTestSplit() const { return testSplit; }

    // Setter and Getter for Random Seed
    void setRandomSeed(int value) { randomSeed = value; }
    int getRandomSeed() const { return randomSeed; }

    // Util Checkers
    void setFileLoaded(bool status) { isFileLoaded = status; }
    bool getFileLoaded() const { return isFileLoaded; }

    void setPreprocessed(bool status) { isPreprocessed = status; }
    bool getPreprocessed() const { return isPreprocessed; }

    void setDSupdate(bool status) { need_DS_Update = status; }
    bool getDSupdate() const { return need_DS_Update; }

    void setModelPresent(bool status) { modelPresent = status; }
    bool getModelPresent() const { return modelPresent; }

    void setCSVWindowPresent(bool status) { CSVWindowPresent = status; }
    bool getCSVWindowPresent() const { return CSVWindowPresent; }

    void setMatricsWindowPresent(bool status) { shaowMatrics= status; }
    bool getMatricsWindowPresent() const { return shaowMatrics; }
    
    void setLogsWindowPresent(bool status) { showLogs = status; }
    bool getLogsWindowPresent() const { return showLogs; }

    void setModelTrained(bool status) { isTrained = status; }
    bool getModelTrained() const { return isTrained; }

    void setPredictionPresent(bool status) { predictPresent = status;}
    bool getPredictionPresent() const { return predictPresent; }

    void setPreprocessingPresent(bool status) { preprocessingPresent = status; }
    bool getPreprocessingPresent() const { return preprocessingPresent; }

    void setModelMethod(std::string option) { modelOption = option; }
    std::string getModelMethod() const { return modelOption; }

    void setSelectedModel(std::string model) { SelectedModel = model; }
    std::string getSelectedModel()const { return SelectedModel; }

    void setModelFilename(std::string name) { filename = name; }
    std::string GetModelFileName() const { return filename; }

    std::deque<std::string> setLogs(const std::string& message, const std::string& timestamp, const std::string& level, const size_t max_logs_ = 500)
    {
        logs_.emplace_back("[" + timestamp + "] [" + level + "] " + message);
        if (logs_.size() > max_logs_) {
            logs_.pop_front(); // Keep log size manageable
        }
        return logs_;
    }
    std::deque<std::string> getLogs() const { return logs_; }



    void setOutputPath(std::string path) { output_path = path; }
    std::string getOutputPath() const { return output_path; }

    // metrics 
    void setAccuracy(float value) { accuracy = value; }
    float getAccuracy() const { return accuracy; }

    void setPrecision(float value) { precision = value; }
    float getPrecision() const { return precision; }

    void setRecall(float value) { recall = value; }
    float getRecall() const { return recall; }

    void setF1(float value) { f1 = value; }
    float getF1() const { return f1; }

    
    void setPredictedValues(const std::vector<float>& Y_pred, const std::vector<float>&  Y_test) {
        y_pred = Y_pred;
        y_test = Y_test;

    }
    const std::tuple<std::vector<float>, std::vector<float>>& getPredictedValues() const {
         return std::make_tuple(y_pred, y_test);
    }
    
    void setYpred(const std::vector<float>& Y_pred) {
        y_pred = Y_pred;
    }
    const std::vector<float>& getYpred() const {
        return y_pred;
    }
    void setYtest(const std::vector<float>& Y_test) {
        y_test = Y_test;
    }
    const std::vector<float>& getYTest() const {
        return y_test;
    }
   
    void saveSessionToFile(const std::string& path);
    void loadSessionFromFile(const std::string& path);
    



private:
    SessionManager() = default; // Private constructor

    std::string filePath; // Store file path
    std::string output;   // Store output or processed data
    std::string filename ;
    std::vector<std::string> features; // List to store selected feature columns
    std::vector<float> y_pred;
    std::vector <float> y_test;
    
    std::string target;  // List to store selected target column
    float trainTestSplit;

   

    int selectedCol = -1;                 // Index for the currently selected column in available list
    int selectedFeature = -1;             // Index for currently selected feature
    int selectedTarget = -1;              // Index for currently selected target

    float testSplit = 0.2f;
    int randomSeed = 42;

    bool need_DS_Update = false;

    bool isModelTrained = false;
    bool isFileLoaded = false;
    bool isPreprocessed = false;
    bool modelPresent = false;
    bool CSVWindowPresent = true;
    bool shaowMatrics = true;
    bool isTrained = false;
    bool predictPresent = false;
    bool preprocessingPresent = false;
    bool showLogs = true;
    
    std::string output_path;
    std::string SelectedModel;
    std::string modelOption = "Supervised";
    std::string param_filname;

    float accuracy;
    float precision;
    float recall;
    float f1;

    
    std::deque<std::string> logs_;
    
};

#endif // 

