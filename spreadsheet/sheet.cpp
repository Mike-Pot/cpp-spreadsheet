#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <optional>

using namespace std::literals;

Sheet::~Sheet()
{
    for (int i = 0; i < Position::MAX_ROWS; i++)
    {
        for (size_t j = 0; j < data_[i].size(); j++)
        {
            delete data_[i][j];
        }
    }
}

void Sheet::SetCell(Position pos, std::string text)
{  
    if (!pos.IsValid())
    {
        throw InvalidPositionException("");
    } 

    size_t col = pos.col;
    size_t row = pos.row;
    ROW& row_v = data_[pos.row];

    if (row_v.size() <= col)
    {
        row_v.resize(col + 1);
    }

    Cell* cell = row_v[col];
    if (!cell)
    {     
        cell = new Cell(*this);       
        row_v[col] = cell;
        nonempty_cols_[row].insert(col);
        nonempty_rows_[col].insert(row); 
    }
    cell->Set(text);   
}

const CellInterface* Sheet::GetCell(Position pos) const
{
    return GetCellIn(pos); 
}

CellInterface* Sheet::GetCell(Position pos)
{
    return GetCellIn(pos);   
}

void Sheet::ClearCell(Position pos)
{
    if (!pos.IsValid())
    {
        throw InvalidPositionException("");
    }
    
    size_t col = pos.col;
    size_t row = pos.row;
    ROW& row_v = data_[row];

    if (col < row_v.size())
    {
        delete row_v[col];
        row_v[col] = nullptr;

        nonempty_cols_[row].erase(col);
        if (nonempty_cols_[row].empty())
        {
            nonempty_cols_.erase(row);
        }

        nonempty_rows_[col].erase(row);
        if (nonempty_rows_[col].empty())
        {
            nonempty_rows_.erase(col);
        }
    }    
}

Size Sheet::GetPrintableSize() const
{
    return  nonempty_cols_.empty() ? Size{0, 0}
                  : Size{nonempty_cols_.rbegin()->first+1, nonempty_rows_.rbegin()->first+1}; 
}

void Sheet::PrintValues(std::ostream& output) const
{
    auto print_func = [&](size_t i, size_t j) {
        std::visit([&](auto arg) {output << arg; }, data_[i][j]->GetValue());
    };
    Print(output, print_func);    
}
void Sheet::PrintTexts(std::ostream& output) const
{
    Print(output, [&](size_t i, size_t j) { output << data_[i][j]->GetText(); });    
}

CellInterface* Sheet::GetCellIn(Position pos) const
{
    if (!pos.IsValid())
    {
        throw InvalidPositionException("");
    }
    return (size_t)pos.col < data_[pos.row].size() ?
        data_[pos.row][pos.col] : nullptr;
    /*
    if ((size_t)pos.col < data_[pos.row].size())
    {
        return data_[pos.row][pos.col];
    }
    else
    {
        return nullptr;
    }
    */
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}