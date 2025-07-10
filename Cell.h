
#ifndef CELL_H
#define CELL_H

#include <string>
#include <iostream>
#include "imgui.h"
#include <cstring>
#include <thread>
#include <atomic>
#include <future>
#include <vector>
#include "tinyfiledialogs.h"
#include "Session_Manger.h"
#include "LogsWindow.h"
#include "ThreadManager.h"
#include <algorithm>
#include <filesystem>
#include "tooltips.h"

namespace fs = std::filesystem;



void InitializeCells();
// Render the cells UI
void RenderCellsUI(float width, float hight, float x_pos, float y_pos);
void RebuildCellsFromSessionState();
void ClearSpecificCells();

class Cell {
public:
    std::string input;    // Input data (could represent a function or command)
    std::string output;   // Output data (result of execution)
    std::string error;    // Error message (if any)
    
    LogsWindow log_window;
    ToolTips tips;

    bool isFilePath = false;
    
    char inputBuffer[1024];

    Cell() : input(""), output(""), error(""), isFilePath(false){
        memset(inputBuffer, 0, sizeof(inputBuffer));
         }

   

    virtual void ShowCellUI()
    {}
    
};



#endif // CELL_H
