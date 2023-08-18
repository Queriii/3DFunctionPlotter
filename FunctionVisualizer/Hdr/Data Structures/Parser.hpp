#pragma once

#include "List.hpp"

#include <Windows.h>
#include <tchar.h>





class Parser
{
public:
    static bool IsValidAxisRange(_In_ PCTSTR pctszRange);

    static bool IsValidParseTarget(_In_ PCTSTR pctszInfix); 

    static void GenerateInfixTokens(_In_ PCTSTR pctszInfix, _Out_ List<PTSTR>& Tokens);
    static void InfixToPostfix(_In_ List<PTSTR>& InfixTokens, _Out_ List<PTSTR>& PostfixTokens);

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


    static bool IsValidSymbol(TCHAR c);
    static bool IsValidParenthesis(PCTSTR pctszInfixNotation);

    static bool IsNumber(PTSTR c);
    static bool IsFunction(PTSTR c);
    static bool IsOperator(PTSTR c);

    static Parser::OperatorInformation GetOperatorInformation(PTSTR c);
};