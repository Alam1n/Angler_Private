#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <deque>

#include "imgui.h"
#include <fstream>
#include "tinyfiledialogs.h" // Or any other file dialog lib you're using



#ifndef LOGS_WINDOW_H
#define LOGS_WINDOW_H

class LogsWindow
{
public:
    void AddLog(const std::string& message, const std::string& level = "INFO");
    void ClearLogs();
    void Show(bool* open, float width, float height, float x_pos, float y_pos);

private:
    std::deque<std::string> logs_;
    const size_t max_logs_ = 500; // Limit log size
    bool auto_scroll_ = true;

    std::string GetTimestamp();
    void SaveLogsToFile();
};

#endif // !LOGS_H
