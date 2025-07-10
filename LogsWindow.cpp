#include "LogsWindow.h"
#include "Session_Manger.h"


void LogsWindow::AddLog(const std::string& message, const std::string& level) {
    auto timestamp = GetTimestamp();
    SessionManager::getInstance().setLogs(message, timestamp, level);
}
void LogsWindow::ClearLogs() {
    logs_.clear();
}

void LogsWindow::Show(bool* open, float width, float height, float x_pos, float y_pos) {
    if (!*open) return;

    ImGui::Begin("Log Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::SetWindowPos(ImVec2(x_pos, y_pos)); // Start below the sidebar
    ImGui::SetWindowSize(ImVec2(width, height)); // Bottom half

    // Optional: Add filtering controls
    if (ImGui::Button("Clear Logs")) ClearLogs();
    ImGui::SameLine();
    if (ImGui::Button("Save Logs")) SaveLogsToFile();
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &auto_scroll_);

    
    // Log display
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    logs_ = SessionManager::getInstance().getLogs();
    for (const auto& log : logs_) {
        ImGui::TextUnformatted(log.c_str());
        
    }
    
    if (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);


    ImGui::EndChild();
    ImGui::End();
}

std::string LogsWindow::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    struct tm timeinfo;  // Define the time structure
    localtime_s(&timeinfo, &in_time_t);  // Safe version of localtime

    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");  // Use std::put_time with the safe structure
    return ss.str();
}

void LogsWindow::SaveLogsToFile() {
    const char* filters[] = { "*.txt" };
    const char* savePath = tinyfd_saveFileDialog("Save Log File", "logs.txt", 1, filters, "Text files");

    if (savePath) {
        std::ofstream file(savePath);
        if (file.is_open()) {
            for (const auto& log : logs_) {
                file << log << "\n";
            }
            file.close();
        }
        else {
            // Optional: show a dialog or log that saving failed
            tinyfd_messageBox("Error", "Failed to open file for writing.", "ok", "error", 1);
        }
    }
}
