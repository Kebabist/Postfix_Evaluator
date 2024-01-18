#pragma once

#include <cmath>
#include <cstddef>
#include <map>
#include <string>

#include "Queue.h"
#include "stack.h"

#define PI 3.14159265358979323846
#define E 2.71828182845904523536

typedef long double ldouble;

inline ldouble _pow(ldouble x, ldouble y, bool aoe) { return (ldouble)pow(x, y); }
inline ldouble _mod(ldouble x, ldouble y, bool aoe) { return (ldouble)((int)x % (int)y); }
inline ldouble _add(ldouble x, ldouble y, bool aoe) { return (ldouble)(x + y); }
inline ldouble _mul(ldouble x, ldouble y, bool aoe) { return (ldouble)(x * y); }
inline ldouble _div(ldouble x, ldouble y, bool aoe) { return (ldouble)(x / y); }
inline ldouble _sub(ldouble x, ldouble y, bool aoe) { return (ldouble)(x - y); }
inline ldouble _unary_minus(ldouble x, bool aoe) { return (ldouble)(-x); }
inline ldouble _unary_minus_wrapper(ldouble x, ldouble, bool aoe) { return _unary_minus(x, aoe); }

ldouble _factorial(ldouble x, ldouble y, bool aoe);

enum token_type { OPERATOR, OPERAND, SYMBOL, FUNCTION ,CONSTAN };

struct Token {
    std::string value;
    token_type type;
};

struct me_SepValues {
    Queue<Token> infixValues;
    std::string infix;
};

struct me_RPN : public me_SepValues {
    std::string RPN;
    Queue<Token> RPNValues;
};

template <typename T>
class me_List : public Queue<T> {
public:
    size_t getIndex(const T);
};

template <class T>
size_t me_List<T>::getIndex(const T data) {
    for (int i = 0; (size_t)i < this->size(); i++) {
        if (this->getData(i) == data) {
            return i;
        }
    }

    return 0;
}

class MathEvaluator {
public:
    MathEvaluator();
    MathEvaluator(bool);
    void appendVariable(const std::string, double&);
    void deleteVariable(const std::string);
    ldouble eval(const std::string);
    me_RPN getRPN() { return rpn; };
    std::map<std::string, double*> getExternalVariables() { return externalVariablesMap; };

private:

    me_SepValues seperate(std::string);
    me_RPN compile(const std::string);
    void populateArrays();

    double pi = (double)PI;
    double e = (double)E;

    me_RPN rpn;
    std::map<std::string, ldouble(*)(ldouble, ldouble)> multipleParameterFunction;
    std::map<std::string, double*> externalVariablesMap = { {"pi", &pi}, {"e", &e} };
    std::map<std::string, double (*)(double)> functionsMap = {
    {"sin", sin},   {"cos", cos},   {"tan", tan},
    {"asin", asin}, {"acos", acos}, {"atan", atan},
    {"sqrt", sqrt} };
    std::map<std::string, std::string> operatorTranslationTable = {
    {"!", "factorial"}, {"^", "pow"}, {"*", "mul"}, {"/", "div"}, {"+", "add"}, {"-", "sub"}, {"%", "mod"},{"(-)", "unary_minus"} };
    std::map<std::string, int> functionParameters = {
    {"sin", 1}, {"cos", 1},   {"tan", 1},
    {"asin", 1}, {"acos", 1}, {"atan", 1},
    {"pow", 2}, {"mul", 2}, {"div", 2},
    {"add", 2}, {"sub", 2}, {"mod", 2},
    {"sqrt", 1}, {"factorial", 1}, { "unary_minus", 1 }};
    std::map<std::string, ldouble(*)(ldouble, ldouble, bool)> operatorMap = {
    {"!", _factorial}, {"^", _pow}, {"+", _add}, {"-", _sub}, {"*", _mul}, {"/", _div}, {"%", _mod}, {"(-)", _unary_minus_wrapper}
    };


    std::map<std::string, int> operatorPrecedence = {
        {"!", 5}, {"^", 4}, {"*", 3}, {"/", 3}, {"%", 3}, {"+", 2}, {"-", 2}, {"(-)", 6}
    };

    std::map<std::string, int> operatorAssociative = {
        {"!", 1}, {"^", 1}, {"*", 0}, {"/", 0}, {"+", 0}, {"-", 0}, {"%", 0}, {"(-)", 1}
    };

    me_List<std::string> externalVariables;
    Queue<std::string> functions;
    Queue<std::string> operators;
    Queue<std::string> symbols;

    bool aoe;
};

ldouble evaluate(const std::string);