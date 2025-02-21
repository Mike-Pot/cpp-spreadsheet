#pragma once

#include "cell.h"
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
public:
    using ROW = std::vector<Cell*>;
    using SHEET = std::vector<ROW>;

    Sheet() = default;
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;


private:
    SHEET data_ = SHEET(Position::MAX_ROWS);
    std::map<int, std::set<int>> nonempty_cols_;
    std::map<int, std::set<int>> nonempty_rows_; 

    CellInterface* GetCellIn(Position pos) const;
    template<typename F>
    void Print(std::ostream& output, F func) const
    {
        if (nonempty_cols_.empty())
        {
            return;
        }
        size_t max_row_ = nonempty_cols_.rbegin()->first;
        size_t max_col_ = nonempty_rows_.rbegin()->first;        
        for (size_t i = 0; i <= max_row_; i++)
        {
            for (size_t j = 0; j <= max_col_; j++)
            {
                if (j < data_[i].size() && data_[i][j])
                {
                    func(i,j);
                }
                if (i != max_row_ || j != max_col_)
                {
                    output << '\t';
                }
            }
            output << '\n';
        }
    }    
};
  
