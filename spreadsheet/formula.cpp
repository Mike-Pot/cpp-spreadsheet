#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

namespace {
    class Formula : public FormulaInterface {
    public:
        // Реализуйте следующие методы:
        explicit Formula(std::string expression)
            try : ast_(ParseFormulaAST(expression)) {}
        catch (...)
        {     
            throw FormulaException("");            
        }
        Value Evaluate(const SheetInterface& sheet) const override
        {
            Value res;
            try
            {
                res = ast_.Execute(sheet);
            }
            catch(FormulaError& fe)
            {
                res = fe;
            }
            return res;
        }
        std::string GetExpression() const override
        {           
            std::ostringstream out;
            ast_.PrintFormula(out);            
            return out.str();
        }
        std::vector<Position> GetReferencedCells() const override
        {            
            std::set<Position> pos_uniques(ast_.GetCells().begin(), ast_.GetCells().end());
            return std::vector<Position>(pos_uniques.begin(), pos_uniques.end());
        }
    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}