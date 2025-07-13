
#ifndef PYTHON_BINDINGS_H
#define PYTHON_BINDINGS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>
#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include "ThreadManager.h"
#include <future>
#include <atomic>
#include <sstream>

namespace py = pybind11;

//Veraibles that go through the thread and need to be catched
inline py::object cached_classification_function;
inline py::object cached_regression_function;
inline py::object cached_prediction_function;
inline py::object cached_unsupervised_function;
inline bool is_python_initialized = false;

// Primme python Side
void start_python();

// Uitls

std::vector<std::string> Read_and_Display(const std::string& filename);

void export_notebook(const std::string& path);


std::tuple <std::vector<std::string>, std::vector<std::string>, std::vector<std::string>> ReturnColumns(const std::string& filename);
std::vector<std::string> CreateNewColumn_exp(const std::string& filename, const std::string& expression, const std::string& newColumnName);
std::string params_filename();
std::string GeneratePredictionPlotImage(std::vector<float> y_pred, std::vector<float> y_test, const std::string& plot_type);

std::string GeneratePlotImage(const std::string& filepath, const std::string& x_column, const std::string& y_column, const std::string& plot_type);

std::vector<std::string> CreateNewColumn(
    const std::string& filename,
    const std::vector<std::string>& column,
    const std::string& operation,
    const std::string& newColumnName);

std::vector<std::string> CreateStandardizedColumn(const std::string& filename, const std::vector<std::string>& columns, const std::vector<std::string>& newColumnNames);

std::vector<std::string> RemoveColumns(const std::string& filename, const std::vector<std::string>& columnsToRemove);
  
struct AsyncResult {
    std::future<std::tuple<py::object, py::tuple>> future;
    std::atomic<bool> is_running{ true };
};


// Export function
void Export_Model(py::object model, std::string filePath);

std::vector<std::string> CreateNormalizedColumn(const std::string& filename, const std::vector<std::string>& columns, const std::vector<std::string>& methods,
    const std::vector<std::string>& newColumnNames);

std::vector<std::string> LabelEncodeColumn(const std::string& filename, const std::string& columnName, const std::string& newColumnName, const std::string& encoding_type);

// ModeFuncitons
std::tuple<std::vector<int>, std::vector<int>, std::vector<float>, std::string>Run_Classification(std::string model_type, std::string data, std::string target, std::vector<std::string> features, int randomSeed, float testSplit);
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::string> Run_Regression(std::string model_type, std::string data, std::string target, std::vector<std::string> features, int randomSeed, float testSplit);

std::vector<std::string> HandleMissingValues(const std::string& filename, const std::vector<std::string>& columns, const std::string& method);

float predict_model(std::string filename, std::vector<float> features);

std::tuple<std::vector<int>, float, std::string> Run_Unsupervised(std::string model_type, std::string data, std::vector<std::string> features, int randomSeed, int n_clusters);

void InitializePythonBindings();


#endif // PYTHON_BINDINGS_H
