#pragma once
#ifndef PREPROCESS_DATA_CELL_H
#define PREPROCESS_DATA_CELL_H

#include "Cell.h"
#include "imguihelper.h"

class PreprocessDataCell : public Cell {
public:
    PreprocessDataCell();

    void ShowCellUI() override;
    void Select_Target_and_Features();

    std::vector<std::string> ModelOption;
    
    ImguiHelper helper;
};

#endif // PREPROCESS_DATA_CELL_H
