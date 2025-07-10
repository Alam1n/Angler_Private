#include "Session_Manger.h"

void SessionManager::saveSessionToFile(const std::string& path)
{
    SessionManager& sm = SessionManager::getInstance();
    json j;

    j["filePath"] = sm.getFilePath();
    j["output"] = sm.getOutput();
    j["features"] = sm.getFeatures();
    j["target"] = sm.getTarget();
    j["testSplit"] = sm.getTestSplit();
    j["randomSeed"] = sm.getRandomSeed();
    j["isFileLoaded"] = sm.getFileLoaded();
    j["isPreprocessed"] = sm.getPreprocessed();
    j["modelPresent"] = sm.getModelPresent();
    j["CSVWindowPresent"] = sm.getCSVWindowPresent();
    j["shaowMatrics"] = sm.getMatricsWindowPresent();
    j["isTrained"] = sm.getModelTrained();
    j["predictPresent"] = sm.getPredictionPresent();
    j["filename"] = sm.GetModelFileName();
    j["output_path"] = sm.getOutputPath();
    j["selected_model"] = sm.getSelectedModel();
    j["preprocessing_present"] = sm.getPreprocessingPresent();
    j["y_pred"] = sm.getYpred();
    j["y_test"] = sm.getYTest();
    j["ModelFilename"] = sm.GetModelFileName();
    j["metrics"]["accuracy"] = sm.getAccuracy();
    j["metrics"]["precision"] = sm.getPrecision();
    j["metrics"]["recall"] = sm.getRecall();
    j["metrics"]["f1"] = sm.getF1();

    j["logs"] = sm.getLogs();

    std::ofstream file(path);
    file << j.dump(4); // Pretty-print with 4-space indentation
}

void SessionManager::loadSessionFromFile(const std::string& path) {
    SessionManager& sm = SessionManager::getInstance();
    std::ifstream file(path);
    json j;
    file >> j;

    sm.setFilePath(j["filePath"]);
    sm.setOutput(j["output"]);
    sm.setFeatures(j["features"].get<std::vector<std::string>>());
    sm.setTarget(j["target"]);
    sm.setTestSplit(j["testSplit"]);
    sm.setRandomSeed(j["randomSeed"]);
    sm.setFileLoaded(j["isFileLoaded"]);
    sm.setPreprocessed(j["isPreprocessed"]);
    sm.setModelPresent(j["modelPresent"]);
    sm.setCSVWindowPresent(j["CSVWindowPresent"]);
    sm.setMatricsWindowPresent(j["shaowMatrics"]);
    sm.setModelTrained(j["isTrained"]);
    sm.setPredictionPresent(j["predictPresent"]);
    sm.setModelFilename(j["filename"]);
    sm.setOutputPath(j["output_path"]);
    sm.setSelectedModel(j["selected_model"]);
    sm.setPreprocessingPresent(j["preprocessing_present"]);
    sm.setYtest(j["y_test"]);
    sm.setYpred(j["y_pred"]);
    sm.setModelFilename(j["ModelFilename"]);

    sm.setAccuracy(j["metrics"]["accuracy"]);
    sm.setPrecision(j["metrics"]["precision"]);
    sm.setRecall(j["metrics"]["recall"]);
    sm.setF1(j["metrics"]["f1"]);

    // Optional: rebuild logs from file (if you want to)
    for (const auto& logEntry : j["logs"]) {
        sm.setLogs(logEntry, "", "", 500); // timestamp/level not parsed here, just for restoring
    }
}

