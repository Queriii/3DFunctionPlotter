#pragma once

#include "List.hpp"

#include <Windows.h>
#include <tchar.h>





class Parser
{
public:
    static bool IsValidAxisRange(PCTSTR pctszRange);

    static bool IsValidFunction(PCTSTR pctszInfix);

    static void GenerateInfixTokens(PCTSTR pctszInfix, List<PTSTR>& Tokens);
    static void InfixToPostfix(PCTSTR pctszInfix, List<PTSTR>& PostfixTokens);


private:
    typedef struct FunctionTokenPair
    {
        PCTSTR          pctszFunctionName;
        PCTSTR          pctszFunctionToken;
        unsigned int    uiArgCount;
    }FunctionTokenPair;
    static constexpr FunctionTokenPair ValidFunctionTokenPairs[]
    {
        //Trig
        {__TEXT("sin"), __TEXT("$"), 1}, {__TEXT("cos"), __TEXT("#"), 1}, {__TEXT("tan"), __TEXT("@"), 1},

        //Misc
        {__TEXT("min"), __TEXT("~"), 2}, {__TEXT("max"), __TEXT("&"), 2}
    };

    typedef struct OperatorInformation
    {
        PCTSTR          pctszOperator;
        unsigned int    uiPrecedence;
        bool            bLeftAssociativity;
    }OperatorInformation;
    static constexpr OperatorInformation ValidOperators[]
    {
        {__TEXT("+"), 2, true},
        {__TEXT("-"), 2, true},
        {__TEXT("*"), 3, true},
        {__TEXT("/"), 3, true},
        {__TEXT("^"), 4, false}
    };

    static constexpr TCHAR ValidVariables[] =
    {
        __TEXT('x'), __TEXT('z')
    };


    static bool IsValidSymbol(TCHAR c);
    static bool IsValidParenthesis(PCTSTR pctszInfixNotation);
    static bool IsValidSequence(PCTSTR pctszInfixNotation);

    static bool IsNumber(TCHAR c);
    static bool IsNumber(PTSTR c);
    static bool IsFunction(TCHAR c);
    static bool IsFunction(PTSTR c);
    static bool IsOperator(TCHAR c);
    static bool IsOperator(PTSTR c);
    static bool IsVariable(TCHAR c);
    static bool IsVariable(PTSTR c);

    static Parser::OperatorInformation GetOperatorInformation(TCHAR c);
    static Parser::OperatorInformation GetOperatorInformation(PTSTR c);
};