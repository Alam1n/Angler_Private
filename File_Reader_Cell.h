#pragma once
#ifndef FILE_READER_CELL_H
#define FILE_READER_CELL_H

#include "Cell.h"
#include "python_binding.h"
#include <vector>






class FileReaderCell : public Cell {
public:

    // Constructor
    FileReaderCell() : Cell() {}

    // Atomic flag for loading status
    std::atomic<bool> isLoading{ false };
    std::string loadedData;
    std::vector<std::string> data;
    std::string filePath;
    bool info = false;
    void ShowCellUI() override;
    void CSV_Display_button();
    void SetFile(std::string filepath);
    
    std::string tooltipstext = "How It Works\nClick on 'Search CSV' to locate the CSV file you want to work with.\nClick on 'Display CSV' to get a better view of your data.\nClick on 'Start Preprocessing' when you're ready to proceed.";

};

#endif // FILE_READER_CELL_H


