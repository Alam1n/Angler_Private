/*#include <pybind11/embed.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace py = pybind11;

// Function to read a CSV file and store it in a 2D vector
std::vector<std::vector<float>> readCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<float>> data;
    std::string line, word;

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::vector<float> row;

        while (getline(ss, word, ',')) {
            row.push_back(std::stof(word));
        }
        data.push_back(row);
    }

    return data;
}
void sendDataToPython(const std::vector<std::vector<float>>& data) {
    py::scoped_interpreter guard{}; // Initialize Python interpreter

    try {
        py::module model = py::module::import("model"); // Import Python model script

        // Send data to Python and receive metrics
        py::object result = model.attr("receive_data")(data);

        // Cast the result (Python dictionary) to a py::dict
        py::dict metrics = result.cast<py::dict>();

        // Extract support vectors, coefficients, and intercept from the dictionary
        auto support_vectors = metrics["support_vectors"].cast<std::vector<std::vector<float>>>();
        auto coefficients = metrics["coef"].cast<std::vector<std::vector<float>>>();
        auto intercept = metrics["intercept"].cast<std::vector<float>>();

        // Output the results (just for demonstration)
        std::cout << "Support Vectors: " << std::endl;
        for (const auto& vec : support_vectors) {
            for (float v : vec) {
                std::cout << v << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "Coefficients: " << std::endl;
        for (const auto& coef_vec : coefficients) {
            for (float coef : coef_vec) {
                std::cout << coef << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "Intercept: ";
        for (float i : intercept) {
            std::cout << i << " ";
        }
        std::cout << std::endl;

    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
    }
}

int main() {
    std::vector<std::vector<float>> data = readCSV("recipes_muffins_cupcakes.csv");

    sendDataToPython(data);

    return 0;
}
*/