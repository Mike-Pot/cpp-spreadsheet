#pragma once

#include <unordered_set>
#include "common.h"
#include "formula.h"

class CellT;
class FormCellT;
class TxtCellT;

class Cell : public CellInterface {
public:
    Cell(const  SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    void AddBackRef(Cell* ref);
    void RemoveBackRef(Cell* ref);
    void Invalidate();     
    void CheckCircs(Cell* token);
    
private:    
    CellT* cell_ptr_ = nullptr;
    std::unordered_set<Cell*> back_refs_;
    const  SheetInterface& sheet_;
};

class CellT
{
public:
    virtual ~CellT() = default;
    CellT(std::string s) : cell_txt_(s) {}

    virtual std::string GetTxt() const = 0;
    virtual Cell::Value GetVal(const  SheetInterface& sheet) const = 0;

    bool IsValid() const;
    void Invalidate();   
    const std::vector<Position>& GetRefs() const;
    
protected: 
    std::vector<Position> forw_refs_;
    std::string cell_txt_; 
    mutable bool is_valid_ = false; 
};

class TxtCellT : public CellT
{
public:
    TxtCellT(std::string s);
    Cell::Value GetVal(const  SheetInterface&) const override;
    std::string GetTxt() const override;
private:
    std::string txt_val_;    
};

class FormCellT : public CellT
{
public:
    FormCellT(std::string s, const  SheetInterface& sheet);
    Cell::Value GetVal(const  SheetInterface& sheet) const override;  
    std::string GetTxt() const override;
private:
    std::unique_ptr<FormulaInterface> form_;
    mutable FormulaInterface::Value form_val_;  
};