#include "File_Reader_Cell.h"
#include "style.h"

void FileReaderCell::ShowCellUI()
{
    {
        Cell::ShowCellUI();
        ImGui::Separator();
        ImGui::PushFont(UIStyle::headingFont);
        // Later in your UI:
        ImGui::Text("File Manager");
        ImGui::PopFont();
        

        filePath = SessionManager::getInstance().getFilePath();

        
        ImGui::InputText("File Path", &filePath[0], filePath.capacity());
       
        tips.ShowToolTip(tooltipstext, "File_Loader");
        ImGui::Separator();
        if (SessionManager::getInstance().getDSupdate() == true) {

            input.clear();
            input = filePath;
            output.clear();
            error.clear();
            data = Read_and_Display(input);
            
            if (!info)
            {
                output = data[0];
            }
            else { output = data[1]; }
            SessionManager::getInstance().setDSupdate(false);

        }

        if (ImGui::Button("Search CSV File")) {
            const char* filterPatterns[] = { "*.csv" };
            const char* newfilePath = tinyfd_openFileDialog("Choose CSV File", "", 1, filterPatterns, "CSV Files", 0);

            if (newfilePath && strlen(newfilePath) > 0) {
                filePath = newfilePath;
               
                SessionManager::getInstance().setFilePath(filePath.c_str());

                // Directly load the file synchronously
                data = Read_and_Display(filePath);
                output = info ? data[1] : data[0];



                if (output.empty()) {
                    error = "Error loading file.";
                    log_window.AddLog("Error loading file.", "ERROR");
                }
                else
                {
                    SessionManager::getInstance().setFileLoaded(true);
                    SessionManager::getInstance().setDSupdate(true);
                    log_window.AddLog(filePath + " Was Selected and Loaded Successfully", "INFO");
                }
            }
            else {
                error = newfilePath ? "Invalid file path." : "No file selected.";
                log_window.AddLog(newfilePath ? " Invalid file path." : "No file selected.", "ERROR");
            }
        }

        ImGui::SameLine();
        if (!filePath.empty()) {
            if (ImGui::Button(info ? "Hide Info" : "Show Info")) {
                info = !info;
                SetFile(filePath);
            }
        }
        
        // Display loaded data or error
        if (!output.empty()) {
            ImGui::Text("Output: %s", output.c_str());
            ImGui::Separator();
        }
        else if (!error.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s", error.c_str());
        }

       


    }
}

void FileReaderCell::CSV_Display_button()
{ }

void FileReaderCell::SetFile(std::string filepath)
{
    input.clear();
    input = filepath;
    output.clear();
    error.clear();
    data = Read_and_Display(input);
    output = info ? data[1] : data[0];
    SessionManager::getInstance().setDSupdate(false);

}


