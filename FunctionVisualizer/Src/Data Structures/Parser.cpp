#include "../../Hdr/Data Structures/Parser.hpp"
#include "../../Hdr/Data Structures/Stack.hpp"
#include "../../Hdr/Data Structures/String.hpp"
//Parser is shit, make better later.




//Public
bool Parser::IsValidAxisRange(_In_ PCTSTR pctszRange)
{
    assert(pctszRange != nullptr);

    if (_tcslen(pctszRange) == 0)
    {
        return false;
    }

    return (pctszRange[0] != __TEXT('0'));
}

bool Parser::IsValidParseTarget(_In_ PCTSTR pctszInfix)
{
    String Infix(pctszInfix);
    Infix.Lower();
    Infix.ReplaceAll(__TEXT(" "), __TEXT(""));
    for (UINT i = 0; i < _countof(Parser::ValidFunctionTokenPairs); i++)
    {
        Infix.ReplaceAll(Parser::ValidFunctionTokenPairs[i].pctszFunctionName, Parser::ValidFunctionTokenPairs[i].pctszFunctionToken);
    }


    //Check for invalid symbol
    for (UINT i = 0; i < _tcslen(Infix.Get()); i++)
    {
        if (!Parser::IsValidSymbol(Infix.Get()[i]))
        {
            return false;
        }
    }

    //Validate parenthesis
    if (!Parser::IsValidParenthesis(pctszInfix))
    {
        return false;
    }
   
    return TRUE;
}

void Parser::GenerateInfixTokens(_In_ PCTSTR pctszInfix, _Out_ List<PTSTR>& Tokens)
{
    assert(pctszInfix != nullptr);

    //Setup format
    Tokens.SetCleanupDynamicElementsType(List<PTSTR>::DynamicElementType::Array);

    String Infix(pctszInfix);
    Infix.Lower();
    Infix.ReplaceAll(__TEXT(" "), __TEXT(""));
    for (UINT i = 0; i < _countof(Parser::ValidFunctionTokenPairs); i++)
    {
        Infix.ReplaceAll(Parser::ValidFunctionTokenPairs[i].pctszFunctionName, Parser::ValidFunctionTokenPairs[i].pctszFunctionToken);
    }

    for (UINT i = 0; i < _tcslen(Infix.Get()); i++)
    {
        if (Infix.Get()[i] >= __TEXT('0') && Infix.Get()[i] <= __TEXT('9'))
        {
            UINT j = i;
            for (; j < _tcslen(Infix.Get()) && (Infix.Get()[j] >= __TEXT('0') && Infix.Get()[j] <= __TEXT('9')); j++)
            {
            }

            PTSTR Token = new TCHAR[static_cast<ULONGLONG>(j) - static_cast<ULONGLONG>(i) + 1]{};
            for (UINT k = 0; k < (j - i + 1); k++)
            {
                Token[k] = Infix.Get()[i + k];
            }
            Token[j - i] = __TEXT('\0');

            Tokens.Append(static_cast<PTSTR>(Token));
            i += (j - i - 1);
        }
        else
        {
            PTSTR Token = new TCHAR[2]{};
            Token[0] = Infix.Get()[i];
            Token[1] = __TEXT('\0');

            Tokens.Append(static_cast<PTSTR>(Token));
        }
    }
}

void Parser::InfixToPostfix(_In_ List<PTSTR>& InfixTokens, _Out_ List<PTSTR>& PostfixTokens)
{
    assert(InfixTokens.Size() != 0);


    Stack<TCHAR> OperatorStack;

    //Convert to postfix
    for (UINT i = 0; i < InfixTokens.Size(); i++)
    {
        if (Parser::IsNumber(InfixTokens[i]))
        {
            PTSTR ptszCopy = new TCHAR[_tcslen(InfixTokens[i]) + 1]{};
            _tcscpy_s(ptszCopy, _tcslen(InfixTokens[i]) + 1, InfixTokens[i]);
            PostfixTokens.Append(static_cast<PTSTR>(ptszCopy));
        }
        else if (Parser::IsFunction(InfixTokens[i]))
        {
            OperatorStack.Push(InfixTokens[i][0]);
        }
        else if (Parser::IsOperator(InfixTokens[i]))
        {
            for (;;)
            {
                if (!OperatorStack.Size())
                {
                    break;
                }

                if (OperatorStack.Peek() == __TEXT('('))
                {
                    break;
                }
                Parser::OperatorInformation CurrentOperatorInfo = Parser::GetOperatorInformation(InfixTokens[i]);
                TCHAR TempOperator[2] = { OperatorStack.Peek(), __TEXT('\0') };
                Parser::OperatorInformation TopStackOperatorInfo = Parser::GetOperatorInformation(TempOperator);
                if ((TopStackOperatorInfo.pctszOperator[0] != __TEXT('(')) && ((TopStackOperatorInfo.uiPrecedence > CurrentOperatorInfo.uiPrecedence) || ((TopStackOperatorInfo.uiPrecedence == CurrentOperatorInfo.uiPrecedence) && CurrentOperatorInfo.bLeftAssociativity)))
                {
                    PTSTR DynOperator = new TCHAR[2]{};
                    DynOperator[0] = OperatorStack.Pop();
                    DynOperator[1] = __TEXT('\0');
                    PostfixTokens.Append(static_cast<PTSTR>(DynOperator));
                }
                else
                {
                    break;
                }
            }

            OperatorStack.Push(InfixTokens[i][0]);
        }
        else if (InfixTokens[i][0] == __TEXT(','))
        {
            for (;;)
            {
                if (!OperatorStack.Size())
                {
                    break;
                }

                if (OperatorStack.Peek() != __TEXT('('))
                {
                    PTSTR DynOperator = new TCHAR[2]{};
                    DynOperator[0] = OperatorStack.Pop();
                    DynOperator[1] = __TEXT('\0');
                    PostfixTokens.Append(static_cast<PTSTR>(DynOperator));
                }
                else
                {
                    break;
                }
            }
        }
        else if (InfixTokens[i][0] == __TEXT('('))
        {
            OperatorStack.Push(InfixTokens[i][0]);
        }
        else if (InfixTokens[i][0] == __TEXT(')'))
        {
            for (;;)
            {
                if (!OperatorStack.Size())
                {
                    assert(1 == 0);
                }

                if (OperatorStack.Peek() != __TEXT('('))
                {
                    PTSTR DynOperator = new TCHAR[2]{};
                    DynOperator[0] = OperatorStack.Pop();
                    DynOperator[1] = __TEXT('\0');
                    PostfixTokens.Append(static_cast<PTSTR>(DynOperator));
                }
                else
                {
                    break;
                }
            }

            OperatorStack.Pop();

            if (OperatorStack.Size())
            {
                TCHAR TempOperator[2] = { OperatorStack.Peek(), __TEXT('\0') };
                if (Parser::IsFunction(TempOperator))
                {
                    PTSTR DynOperator = new TCHAR[2]{};
                    DynOperator[0] = OperatorStack.Pop();
                    DynOperator[1] = __TEXT('\0');
                    PostfixTokens.Append(static_cast<PTSTR>(DynOperator));
                }
            }
        }
        else
        {
            assert(1 == 0);
        }
    }
}




//Private
bool Parser::IsValidSymbol(TCHAR c)
{
    //Check numbers
    if (c >= __TEXT('0') && c <= __TEXT('9'))
    {
        return true;
    }

    //Check parenthesis
    if (c == __TEXT('(') || c == __TEXT(')'))
    {
        return true;
    }

    //Check functions
    for (UINT i = 0; i < _countof(Parser::ValidFunctionTokenPairs); i++)
    {
        if (c == Parser::ValidFunctionTokenPairs[i].pctszFunctionToken[0])
        {
            return true;
        }
    }

    //Check operators
    for (UINT i = 0; i < _countof(Parser::ValidOperators); i++) 
    {
        if (c == Parser::ValidOperators[i].pctszOperator[0])
        {
            return true;
        }
    }

    //Check allowed variables
    if (c == __TEXT('x') || c == __TEXT('z'))
    {
        return true;
    }

    return false;
}

bool Parser::IsValidParenthesis(PCTSTR pctszInfixNotation)
{
    assert(pctszInfixNotation != nullptr);

    Stack<TCHAR> Parenthesis;
    for (UINT i = 0; i < _tcslen(pctszInfixNotation); i++)
    {
        if (pctszInfixNotation[i] == __TEXT('('))
        {
            Parenthesis.Push(__TEXT('('));
        }
        else if (pctszInfixNotation[i] == __TEXT(')'))
        {
            if (!Parenthesis.Size())
            {
                return false;
            }
            else
            {
                Parenthesis.Pop();
            }
        }
    }

    return (Parenthesis.Size() == 0);
}

bool Parser::IsNumber(PTSTR c)
{
    for (UINT i = 0; i < _tcslen(c); i++)
    {
        if (!(c[i] >= __TEXT('0') && c[i] <= __TEXT('9')))
        {
            return false;
        }
    }

    return true;
}

bool Parser::IsFunction(PTSTR c)
{
    if (_tcslen(c) > 1)
    {
        return false;
    }

    for (UINT i = 0; i < _countof(Parser::ValidFunctionTokenPairs); i++)
    {
        if (c[0] == Parser::ValidFunctionTokenPairs[i].pctszFunctionToken[0])
        {
            return true;
        }
    }

    return false;
}

bool Parser::IsOperator(PTSTR c)
{
    if (_tcslen(c) > 1)
    {
        return false;
    }

    for (UINT i = 0; i < _countof(Parser::ValidOperators); i++)
    {
        if (c[0] == Parser::ValidOperators[i].pctszOperator[0])
        {
            return true;
        }
    }

    return false;
}

Parser::OperatorInformation Parser::GetOperatorInformation(PTSTR c)
{
    for (UINT i = 0; i < _countof(Parser::ValidOperators); i++)
    {
        if (c[0] == Parser::ValidOperators[i].pctszOperator[0])
        {
            return Parser::ValidOperators[i];
        }
    }

    assert(1 == 0);
    return {};
}