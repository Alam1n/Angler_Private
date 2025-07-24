#include "python_binding.h"
#include "LogsWindow.h"

// Define the global scoped interpreter guard
py::scoped_interpreter guard{};


LogsWindow log_window;
void start_python() {
    try {

        py::gil_scoped_acquire acquire; // Always safe!
        py::module module = py::module::import("model"); // Change to your actual module name
        module.attr("start_py")(); // Don't forget the parentheses to call the function!
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
}


std::vector<std::string> Read_and_Display(const std::string& filename)
{
    try {
        py::module module = py::module::import("model");

        // Call the Python function
        py::tuple result = module.attr("read_display")(filename).cast<py::tuple>();

        std::vector<std::string> output;
        output.push_back(result[0].cast<std::string>());
        output.push_back(result[1].cast<std::string>());

        return output;
    }
    catch (const pybind11::error_already_set& e)
    {
        std::cerr << "Python error: " << e.what() << std::endl;
        return { "Error in Python execution", "" };
    }
}
void export_notebook(const std::string& path)
{
    try {
        py::gil_scoped_acquire acquire;
        py::module module = py::module::import("model");
        module.attr("generate_notebook_filename")(path);  // <-- pass the user save path
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
}

std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::string>> ReturnColumns(const std::string& filename)
{
    try {
        // Import the Python module
        py::module module = py::module::import("model");

        // Call the Python function
        py::tuple result = module.attr("return_columns")(filename);

        // Extract and cast the tuple elements
        std::vector<std::string> columns = result[0].cast<std::vector<std::string>>();
        std::vector<std::string> numericCols = result[1].cast<std::vector<std::string>>();
        std::vector<std::string> nonNumericCols = result[2].cast<std::vector<std::string>>();

        // Return the C++ tuple
        return std::make_tuple(columns, numericCols, nonNumericCols);
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        return std::make_tuple(std::vector<std::string>{}, std::vector<std::string>{}, std::vector<std::string>{});
    }
}

std::vector<std::string> CreateNewColumn_exp(const std::string& filename, const std::string& expression, const std::string& newColumnName)
{
    try {
        // Import the Python module
        py::module module = py::module::import("model");

        // Call the new Python function that accepts expression directly
        py::object result = module.attr("apply_custom_expression")(filename, expression, newColumnName);

        // Check if result is an error message (string) or updated list of columns
        if (py::isinstance<py::str>(result)) {
            std::string errorMsg = result.cast<std::string>();
            std::cerr << "Python error: " << errorMsg << std::endl;
            log_window.AddLog("Python error: " + errorMsg, "ERROR");
            return {};
        }

        // Convert result to a vector of strings (updated list of columns)
        return result.cast<std::vector<std::string>>();
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}

std::string GeneratePredictionPlotImage(
     std::vector<float> y_pred,
    std::vector<float> y_test,
    const std::string& plot_type)
{
    try {
        py::gil_scoped_acquire acquire;
        py::module module = py::module::import("model"); // Replace with your actual module name if different

        // Call the Python function
        py::object result = module.attr("generate_prediction_plot")(y_pred, y_test, plot_type);

        // Handle string result (path or error)
        std::string resultStr = result.cast<std::string>();

        if (resultStr.rfind("temp_images/", 0) == 0) {
            return resultStr; // It's a valid image path
        }
        else {
            log_window.AddLog("Python Error: " + resultStr, "ERROR");
            return ""; // Return empty if error
        }
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}
std::string params_filename()
{
    try {

        py::module module = py::module::import("model"); // Replace with your actual module name if different

        // Call the Python function
        py::object result = module.attr("Set_preprocessing_params")();
        std::string resultStr = result.cast<std::string>();
        if (!resultStr.empty() && resultStr.find("ERROR") == std::string::npos) {
            return resultStr; // It's a valid path
        }
        else {
            log_window.AddLog("Python Error: " + resultStr, "ERROR");
            return ""; // Return empty if error
        }


    }
    catch (const py::error_already_set& e)
    {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}
std::string GeneratePlotImage(const std::string& filepath,
    const std::string& x_column,
    const std::string& y_column,
    const std::string& plot_type)
{
    try {
        py::gil_scoped_acquire acquire;
        py::module module = py::module::import("model"); // Replace with your actual module name if different

        // Call the Python function
        py::object result = module.attr("generate_plot")(filepath, x_column, y_column, plot_type);

        // Handle string result (path or error)
        std::string resultStr = result.cast<std::string>();

        if (resultStr.rfind("temp_images/", 0) == 0) {
            return resultStr; // It's a valid image path
        }
        else {
            log_window.AddLog("Python Error: " + resultStr, "ERROR");
            return ""; // Return empty if error
        }
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}


std::vector<std::string> CreateNewColumn(const std::string& filename, const std::vector<std::string>& column, const std::string& operation, const std::string& newColumnName)
{
    try {
        // Import the Python module
        py::module module = py::module::import("model");

        // Convert C++ vector to Python list
        py::list pyColumns;
        for (const auto& col : column) {
            pyColumns.append(col);
        }

        // Call Python function
        py::object result = module.attr("create_new_column")(filename, pyColumns, operation, newColumnName);

        // Check if result is an error message (string) or updated list of columns
        if (py::isinstance<py::str>(result)) {
            std::cerr << "Python error: " << result.cast<std::string>() << std::endl;
            log_window.AddLog("Python error: " + result.cast<std::string>(), "ERROR");
            return {};
        }

        // Convert result to a vector of strings (updated list of columns)
        return result.cast<std::vector<std::string>>();
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}
std::vector<std::string> CreateStandardizedColumn(
    const std::string& filename,
    const std::vector<std::string>& columns,
    const std::vector<std::string>& newColumnNames)
{
    try {
        py::module module = py::module::import("model");

        py::list pyColumns, pyNewColumnNames;
        for (const auto& col : columns) pyColumns.append(col);
        for (const auto& newName : newColumnNames) pyNewColumnNames.append(newName);

        py::object result = module.attr("apply_standardization")(filename, pyColumns, pyNewColumnNames);

        if (py::isinstance<py::str>(result)) {
            log_window.AddLog(result.cast<std::string>(), "ERROR");
            return {};
        }

        return result.cast<std::vector<std::string>>();
    }
    catch (const py::error_already_set& e) {
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}

std::vector<std::string> RemoveColumns(const std::string& filename, const std::vector<std::string>& columnsToRemove) {
    try {
        // Import the Python module
        py::module module = py::module::import("model");

        // Convert C++ vector to Python list
        py::list pyColumnsToRemove;
        for (const auto& col : columnsToRemove) {
            pyColumnsToRemove.append(col);
        }

        // Call the Python function
        py::object result = module.attr("remove_columns")(filename, pyColumnsToRemove);

        // Check if the result is an error message (string)
        if (py::isinstance<py::str>(result)) {
            std::cerr << "Python error: " << result.cast<std::string>() << std::endl;
            return {};
        }

        // Convert the result to a vector of strings (remaining columns)
        return result.cast<std::vector<std::string>>();
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}

std::tuple<py::object, py::tuple> Run_SVM_Model(std::string data, std::string target, std::vector<std::string> features, int randomSeed, float testSplit)
{
    try {

        py::gil_scoped_acquire acquire;
        // Import Python module and function
        py::object train_model = py::module::import("model");
        py::object train_and_evaluate = train_model.attr("train_and_evaluate");

        // Call the Python function and get the trained model and metrics
        py::tuple result = train_and_evaluate(data, features, target, randomSeed, testSplit);

        // Extract the trained model and metrics
        py::object trained_model = result[0];
        py::tuple metrics = result[1];

        // Output some metrics as example
        std::cout << "Model accuracy: " << metrics[0].cast<float>() << std::endl;

        return std::make_tuple(trained_model, metrics);  // Return as a tuple of py::objects
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return std::make_tuple(py::none(), py::none());  // Return None if an error occurs
    }
}

void Export_Model(py::object model, std::string filePath)
{
    try {
        // Import the export function from the Python module
        py::object export_model = py::module::import("model").attr("export_model");
        export_model(model, filePath);  // Call the export function with the model and path
    }
    catch (const std::exception& e) {
        std::cerr << "Error exporting model: " << e.what() << std::endl;
    }
}


std::vector<std::string> CreateNormalizedColumn(const std::string& filename,
    const std::vector<std::string>& columns,
    const std::vector<std::string>& methods,
    const std::vector<std::string>& newColumnNames)
{
    try {
        // Import the Python module
        py::module module = py::module::import("model");

        // Convert C++ vectors to Python lists
        py::list pyColumns, pyMethods, pyNewColumnNames;
        for (const auto& col : columns) {
            pyColumns.append(col);
        }
        for (const auto& method : methods) {
            pyMethods.append(method);
        }
        for (const auto& newName : newColumnNames) {
            pyNewColumnNames.append(newName);
        }

        // Call the Python function to normalize columns
        py::object result = module.attr("apply_normalization")(filename, pyColumns, pyMethods, pyNewColumnNames);

        // Check if result is an error message (string) or updated list of columns
        if (py::isinstance<py::str>(result)) {
            std::cerr << "Python error: " << result.cast<std::string>() << std::endl;
            log_window.AddLog("Python error: " + result.cast<std::string>(), "ERROR");
            return {};
        }

        // Convert result to a vector of strings (updated list of columns)
        return result.cast<std::vector<std::string>>();
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}



std::vector<std::string> LabelEncodeColumn(const std::string& filename, const std::string& columnName, const std::string& newColumnName, const std::string& encoding_type) {
    try {
        // Import the Python module
        py::module module = py::module::import("model");

        // Call the Python function
        py::object result = module.attr("encode_column")(filename, columnName, newColumnName, encoding_type);

        // Parse the result
        if (py::isinstance<py::dict>(result)) {
            py::dict resultDict = result.cast<py::dict>();
            py::list updatedColumns = resultDict["columns"];
            py::dict encodingMap = resultDict["encoding_map"];

            // Log the encoding map for debugging or display
            std::cout << "Label Encoding Map:" << std::endl;
            for (auto item : encodingMap) {
                std::cout << item.first.cast<std::string>() << " -> " << item.second.cast<int>() << std::endl;
            }

            // Convert the updated columns to a C++ vector and return
            return updatedColumns.cast<std::vector<std::string>>();
        }
        else if (py::isinstance<py::str>(result)) {
            std::cerr << "Python error: " << result.cast<std::string>() << std::endl;
            log_window.AddLog("Python error: " + result.cast<std::string>(), "ERROR");
        }

        return {};
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python exception: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}

void ApplyTextCleaning(const std::string& filename, const std::string& columnName, const std::vector<std::string>& steps) {
    try {
        py::module module = py::module::import("model");
        py::list pySteps;
        for (const auto& step : steps)
            pySteps.append(step);

        module.attr("apply_text_cleaning")(filename, columnName, pySteps);
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python exception: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
    }
}




std::tuple<std::vector<int>, std::vector<int>, std::vector<float>, std::string> Run_Classification(std::string model_type,std::string data, std::string target, std::vector<std::string> features, int randomSeed, float testSplit) {
    try {
        // Acquire the GIL before calling any Python code
        py::gil_scoped_acquire acquire;

        std::stringstream ss;
        ss << "Calling Python to train and evaluate classification " << model_type << " model...";
        log_window.AddLog(ss.str(), "INFO");

        if (!is_python_initialized) {
            log_window.AddLog("Python function not initialized!", "ERROR");
            return std::make_tuple(std::vector<int>{}, std::vector<int>{}, std::vector<float>{}, std::string{});
        }

        // Import and call the Python function
        py::tuple result = cached_classification_function(model_type, data, features, target, randomSeed, testSplit);

        // Extract each item safely
        std::vector<int> y_pred = result[0].cast<std::vector<int>>();
        std::vector<int> y_test = result[1].cast<std::vector<int>>();
        std::vector<float> metrics = result[2].cast<std::vector<float>>();
        std::string filename = result[3].cast<std::string>();

        std::cout << "Model accuracy: " << metrics[0] << std::endl;

        ss.str(""); ss.clear();
        ss << "Python training completed for " << model_type << " successfully.";
        log_window.AddLog(ss.str(), "SUCCESS");

        return std::make_tuple(y_pred, y_test, metrics, filename);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return std::make_tuple(std::vector<int>{}, std::vector<int>{}, std::vector<float>{}, std::string{});
    }
}

std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::string> Run_Regression(std::string model_type, std::string data, std::string target, std::vector<std::string> features, int randomSeed, float testSplit)
{
    try{
        // Acquire the GIL before calling any Python code
        py::gil_scoped_acquire acquire;

        std::stringstream ss;
        ss << "Calling Python to train and evaluate classification " << model_type << " model...";
        log_window.AddLog(ss.str(), "INFO");

        if (!is_python_initialized) {
            log_window.AddLog("Python function not initialized!", "ERROR");
            return std::make_tuple(std::vector<float>{}, std::vector<float>{}, std::vector<float>{}, std::string{});
        }

        // Import and call the Python function
        py::tuple result = cached_regression_function(model_type, data, features, target, randomSeed, testSplit);

        // Extract each item safely
        std::vector<float> y_pred = result[0].cast<std::vector<float>>();
        std::vector<float> y_test = result[1].cast<std::vector<float>>();
        std::vector<float> metrics = result[2].cast<std::vector<float>>();
        std::string filename = result[3].cast<std::string>();

        std::cout << "Model MSE: " << metrics[0] << std::endl;
        std::cout << "First Value of Y_pred: " << y_pred[0] << std::endl;
        std::cout << "First Value of Y_pred: " << y_test[0] << std::endl;

        ss.str(""); ss.clear();
        ss << "Python training completed for " << model_type << " successfully.";
        log_window.AddLog(ss.str(), "SUCCESS");

        return std::make_tuple(y_pred, y_test, metrics, filename);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return std::make_tuple(std::vector<float>{}, std::vector<float>{}, std::vector<float>{}, std::string{});
    }
}

std::vector<std::string> HandleMissingValues(
    const std::string& filename,
    const std::vector<std::string>& columns,
    const std::string& method
) {
    try {
        py::module module = py::module::import("model");

        // Convert C++ vector to Python list
        py::list pyColumns;
        for (const auto& col : columns) {
            pyColumns.append(col);
        }

        // Call the Python function
        py::object result = module.attr("handle_missing_values")(filename, pyColumns, method);

        // If Python returned updated columns, cast and return
        if (py::isinstance<py::list>(result)) {
            return result.cast<std::vector<std::string>>();
        }
        else if (py::isinstance<py::str>(result)) {
            std::cerr << "Python error: " << result.cast<std::string>() << std::endl;
            log_window.AddLog("Python error: " + result.cast<std::string>(), "ERROR");
        }

        return {};
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return {};
    }
}


float predict_model(std::string filename, std::vector<float> numeric_features, std::vector<std::string> string_features)
{
    try {
        py::gil_scoped_acquire acquire;

        std::stringstream ss;
        ss << "Making Prediction";
        log_window.AddLog(ss.str(), "INFO");

        if (!is_python_initialized) {
            log_window.AddLog("Python function not initialized!", "ERROR");
            return 0.0f;
        }
        // Import and call the Python function
        py::float_ result = cached_prediction_function(filename, numeric_features, string_features);

        ss.str(""); ss.clear();
        ss << "Prediction completed  successfully.";
        log_window.AddLog(ss.str(), "SUCCESS");

        ss.str(""); ss.clear();
        ss << "Prediction result: " << result;
        log_window.AddLog(ss.str(), "INFO");


        return result;

    }
    catch (const std::exception& e) {
        
        std::cerr << "Exception caught: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return 0.0f;
    }
    
}

std::tuple<std::vector<int>, float, std::string> Run_Unsupervised(std::string model_type,
    std::string data,
    std::vector<std::string> features,
    int randomSeed,
    int n_clusters) {
    try {
        py::gil_scoped_acquire acquire;

        std::stringstream ss;
        ss << "Calling Python to train unsupervised " << model_type << " model...";
        log_window.AddLog(ss.str(), "INFO");

        if (!is_python_initialized) {
            log_window.AddLog("Python environment not initialized!", "ERROR");
            return std::make_tuple(std::vector<int>{}, 0.0f, std::string{});
        }

        // Call the cached Python function
        py::tuple result = cached_unsupervised_function(model_type, data, features, n_clusters, randomSeed);

        // Parse the tuple from Python: (labels, score, filename)
        std::vector<int> labels = result[0].cast<std::vector<int>>();
        float silhouette_score = result[1].cast<float>();
        std::string filename = result[2].cast<std::string>();

        ss.str(""); ss.clear();
        ss << "Clustering completed for " << model_type << " with silhouette score: " << silhouette_score;
        log_window.AddLog(ss.str(), "SUCCESS");

        return std::make_tuple(labels, silhouette_score, filename);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        log_window.AddLog(e.what(), "ERROR");
        return std::make_tuple(std::vector<int>{}, 0.0f, std::string{});
    }
}


// New init function — call once during app startup (main thread)
void InitializePythonBindings() {
    py::gil_scoped_acquire acquire;

    if (!is_python_initialized) {
        py::module model_module = py::module::import("model");
        cached_classification_function = model_module.attr("ThreadClassification_Models");
        cached_regression_function = model_module.attr("Thread_Regression_Models");
        cached_prediction_function = model_module.attr("predict_model");
        cached_unsupervised_function = model_module.attr("ThreadUnsupervised_Models");

        is_python_initialized = true;
        std::cout << "[PythonBinder] Cached Python functions initialized." << std::endl;
    }
}