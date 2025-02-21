#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell(const  SheetInterface& sheet) : sheet_(sheet)
{}

Cell::~Cell()
{
	delete cell_ptr_;
}

void Cell::Set(std::string text)
{
	if (cell_ptr_ && GetText() == text)
	{
		return;
	}
	CellT* new_ptr;
	if (text.size() > 1 && text[0] == FORMULA_SIGN)
	{
		new_ptr = new FormCellT(text, sheet_); 
		std::vector<Position> refs = new_ptr->GetRefs();
		for (Position pos : refs)
		{
			Cell* cell = (Cell*)sheet_.GetCell(pos);
			if (!cell)
			{
				const_cast<SheetInterface&>(sheet_).SetCell(pos, "");
				cell = (Cell*)sheet_.GetCell(pos);
			}
			cell->CheckCircs(this);
			cell->AddBackRef(this);
		}
	}
	else
	{
		new_ptr = new TxtCellT(text);
		std::vector<Position> refs = new_ptr->GetRefs();
		for (Position pos : refs)
		{
			((Cell*)sheet_.GetCell(pos))->RemoveBackRef(this);
		}
	}
	for (Cell* cell : back_refs_)
	{
		cell->Invalidate();
	}
	std::swap(cell_ptr_, new_ptr);
	delete new_ptr;
}

void Cell::Clear()
{	
	Set("");	
}

Cell::Value Cell::GetValue() const
{	
	return cell_ptr_->GetVal(sheet_);
}

std::string Cell::GetText() const
{	
	return cell_ptr_->GetTxt();	
}

void Cell::AddBackRef(Cell* ref)
{
	back_refs_.insert(ref);
}

void Cell::RemoveBackRef(Cell* ref)
{
	back_refs_.erase(ref);
}

void Cell::Invalidate() 
{
	if (cell_ptr_->IsValid())
	{
		cell_ptr_->Invalidate();
		for (auto cell : back_refs_)
		{
			cell->Invalidate();
		}
	}
}

std::vector<Position> Cell::GetReferencedCells() const
{
	return cell_ptr_->GetRefs();
}

void Cell::CheckCircs(Cell* token)
{
	if (token == this)
	{
		throw CircularDependencyException("");
	}
	for (Position pos : GetReferencedCells())
	{
		((Cell*)sheet_.GetCell(pos))->CheckCircs(token);
	}		
}

bool CellT::IsValid() const
{
	return is_valid_;;
}

void CellT::Invalidate()
{
	is_valid_ = false;
}

TxtCellT::TxtCellT(std::string s) : CellT(s)
{
	txt_val_ =
		(!s.empty() && s[0] == ESCAPE_SIGN) ?
		(s.size() > 1 ? s.substr(1) : "")
		: s;	
}

Cell::Value TxtCellT::GetVal(const  SheetInterface&) const
{
	is_valid_ = true;
	return txt_val_;
}

std::string TxtCellT::GetTxt() const
{
	return cell_txt_;
}

FormCellT::FormCellT(std::string s, const  SheetInterface& sheet) : CellT(s)
{
	auto form = ParseFormula(s.substr(1));
	forw_refs_ = form->GetReferencedCells();
	form_.swap(form);
}

Cell::Value FormCellT::GetVal(const SheetInterface& sheet) const
{	
	if (!is_valid_)
	{
		form_val_ = form_->Evaluate(sheet);
		is_valid_ = true;
	}
	
	if (std::holds_alternative<double>(form_val_))
	{
		return std::get<double>(form_val_);
	}
	else
	{
		return std::get<FormulaError>(form_val_);
	}	
}

std::string FormCellT::GetTxt() const
{
	return ("=" + form_->GetExpression());
}

const std::vector<Position>& CellT::GetRefs() const
{
	return forw_refs_;	
}
