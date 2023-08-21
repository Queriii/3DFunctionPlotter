#include "../../Hdr/Data Structures/Parser.hpp"
#include "../../Hdr/Data Structures/Stack.hpp"
#include "../../Hdr/Data Structures/String.hpp"




//Public
bool Parser::IsValidAxisRange(PCTSTR pctszRange)
{
    assert(pctszRange != nullptr);

    if (_tcslen(pctszRange) == 0)
    {
        return false;
    }

    return (pctszRange[0] != __TEXT('0'));
}

bool Parser::IsValidFunction(PCTSTR pctszInfix)
{
    assert(pctszInfix != nullptr);

    List<PTSTR> PostfixTokens;
    Parser::InfixToPostfix(pctszInfix, PostfixTokens);
    if (!PostfixTokens.Size())
    {
        return false;
    }

    Stack<PTSTR> Operands;
    for (UINT i = 0; i < PostfixTokens.Size(); i++)
    {
        if (Parser::IsOperator(PostfixTokens[i]))
        {
            if (Operands.Size() < 2)
            {
                return false;
            }

            if (!(Parser::IsNumber(Operands.Peek()) || Parser::IsVariable(Operands.Peek())))
            {
                return false;
            }
            delete[] Operands.Pop();

            if (!(Parser::IsNumber(Operands.Peek()) || Parser::IsVariable(Operands.Peek())))
            {
                return false;
            }
            delete[] Operands.Pop();

            PTSTR DummyOperand = new TCHAR[2]{ __TEXT('0'), __TEXT('\0') };
            Operands.Push(DummyOperand);
        }
        else if (Parser::IsFunction(PostfixTokens[i]))
        {
            if (Operands.Size() < 1)
            {
                return false;
            }

            if (!(Parser::IsNumber(Operands.Peek()) || Parser::IsVariable(Operands.Peek())))
            {
                return false;
            }
            delete[] Operands.Pop();

            PTSTR DummyOperand = new TCHAR[2]{ __TEXT('0'), __TEXT('\0') };
            Operands.Push(DummyOperand);
        }
        else
        {
            size_t  ullOperandLength    = _tcslen(PostfixTokens[i]);
            PTSTR   ptszOperand         = new TCHAR[ullOperandLength + 1]{};
            _tcscpy_s(ptszOperand, ullOperandLength + 1, PostfixTokens[i]);
            Operands.Push(ptszOperand);
        }
    }

    //Cleanup
    while (Operands.Size())
    {
        delete[] Operands.Pop();
    }
    for (UINT i = 0; i < PostfixTokens.Size(); i++)
    {
        delete[] PostfixTokens[i];
    }

    return true;
}

void Parser::GenerateInfixTokens(PCTSTR pctszInfix, List<PTSTR>& Tokens)
{
    assert(pctszInfix != nullptr);

    String Infix(pctszInfix);
    Infix.Lower();
    Infix.ReplaceAll(__TEXT(" "), __TEXT(""));
    for (UINT i = 0; i < _countof(Parser::ValidFunctionTokenPairs); i++)
    {
        Infix.ReplaceAll(Parser::ValidFunctionTokenPairs[i].pctszFunctionName, Parser::ValidFunctionTokenPairs[i].pctszFunctionToken);
    }

    for (UINT i = 0; i < _tcslen(Infix.Get()); i++)
    {
        if (Parser::IsNumber(Infix.Get()[i]))
        {
            UINT j = i;
            for (; j < _tcslen(Infix.Get()) && Parser::IsNumber(Infix.Get()[j]); j++)
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
            PTSTR Token = new TCHAR[2]{Infix.Get()[i], __TEXT('\0')};
            Tokens.Append(static_cast<PTSTR>(Token));
        }
    }
}

void Parser::InfixToPostfix(PCTSTR pctszInfix, List<PTSTR>& PostfixTokens)
{
    assert(pctszInfix != nullptr); 

    List<PTSTR>     InfixTokens;
    Stack<TCHAR>    OperatorStack;

    Parser::GenerateInfixTokens(pctszInfix, InfixTokens);

    for (UINT i = 0; i < InfixTokens.Size(); i++)
    {
        if (Parser::IsNumber(InfixTokens[i]) || Parser::IsVariable(InfixTokens[i]))
        {
            size_t ullTokenLength   = _tcslen(InfixTokens[i]);
            PTSTR ptszToken         = new TCHAR[ullTokenLength + 1]{};
            _tcscpy_s(ptszToken, ullTokenLength + 1, InfixTokens[i]);

            PostfixTokens.Append(static_cast<PTSTR>(ptszToken));
        }
        else if (Parser::IsFunction(InfixTokens[i]))
        {
            OperatorStack.Push(InfixTokens[i][0]);
        }
        else if (Parser::IsOperator(InfixTokens[i]))
        {
            while (OperatorStack.Size() && OperatorStack.Peek() != __TEXT('('))
            {
                Parser::OperatorInformation TopStackOperatorInfo = Parser::GetOperatorInformation(OperatorStack.Peek());
                Parser::OperatorInformation CurrentOperatorInfo = Parser::GetOperatorInformation(InfixTokens[i]);

                if (!((TopStackOperatorInfo.uiPrecedence > CurrentOperatorInfo.uiPrecedence) || ((TopStackOperatorInfo.uiPrecedence == CurrentOperatorInfo.uiPrecedence) && CurrentOperatorInfo.bLeftAssociativity)))
                {
                    break;
                }

                PTSTR Operator = new TCHAR[2]{ OperatorStack.Pop(), __TEXT('\0') };
                PostfixTokens.Append(static_cast<PTSTR>(Operator));
            }

            OperatorStack.Push(InfixTokens[i][0]);
        }
        else if (InfixTokens[i][0] == __TEXT('(') && _tcslen(InfixTokens[i]) == 1)
        {
            OperatorStack.Push(InfixTokens[i][0]);
        }
        else if (InfixTokens[i][0] == __TEXT(')') && _tcslen(InfixTokens[i]) == 1)
        {
            for (;;)
            {
                if (!OperatorStack.Size())
                {
                    PostfixTokens.RemoveAll();
                    return;
                }

                if (OperatorStack.Peek() == __TEXT('('))
                {
                    OperatorStack.Pop();
                    break;
                }

                PTSTR Operator = new TCHAR[2]{ OperatorStack.Pop(), __TEXT('\0') };
                PostfixTokens.Append(static_cast<PTSTR>(Operator));
            }

            if (OperatorStack.Size())
            {
                if (Parser::IsFunction(OperatorStack.Peek()))
                {
                    PTSTR Operator = new TCHAR[2]{ OperatorStack.Pop(), __TEXT('\0') };
                    PostfixTokens.Append(static_cast<PTSTR>(Operator));
                }
            }
        }
        else
        {
            PostfixTokens.RemoveAll();
            return;
        }
    }

    //Cleanup Infix Tokens
    for (UINT i = 0; i < InfixTokens.Size(); i++)
    {
        delete[] InfixTokens[i];
    }

    while (OperatorStack.Size())
    {
        if (OperatorStack.Peek() == __TEXT('('))
        {
            PostfixTokens.RemoveAll();
            return;
        }

        PTSTR Operator = new TCHAR[2]{ OperatorStack.Pop(), __TEXT('\0') };
        PostfixTokens.Append(static_cast<PTSTR>(Operator));
    }
}

bool Parser::IsNumber(TCHAR c)
{
    return (c >= __TEXT('0') && c <= __TEXT('9'));
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

bool Parser::IsFunction(TCHAR c)
{
    for (UINT i = 0; i < _countof(Parser::ValidFunctionTokenPairs); i++)
    {
        if (c == Parser::ValidFunctionTokenPairs[i].pctszFunctionToken[0])
        {
            return true;
        }
    }

    return false;
}

bool Parser::IsFunction(PTSTR c)
{
    if (_tcslen(c) > 1)
    {
        return false;
    }

    for (UINT i = 0; i < _countof(Parser::ValidFunctionTokenPairs); i++)
    {
        if (c[0] == Parser::ValidFunctionTokenPairs[i].pctszFunctionToken[0] && _tcslen(c) == 1)
        {
            return true;
        }
    }

    return false;
}

bool Parser::IsOperator(TCHAR c) 
{
    for (UINT i = 0; i < _countof(Parser::ValidOperators); i++)
    {
        if (c == Parser::ValidOperators[i].pctszOperator[0])
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

bool Parser::IsVariable(TCHAR c)
{
    for (UINT i = 0; i < _countof(Parser::ValidVariables); i++)
    {
        if (c == Parser::ValidVariables[i])
        {
            return true;
        }
    }

    return false;
}

bool Parser::IsVariable(PTSTR c)
{
    if (_tcslen(c) > 1)
    {
        return false;
    }

    for (UINT i = 0; i < _countof(Parser::ValidVariables); i++)
    {
        if (c[0] == Parser::ValidVariables[i])
        {
            return true;
        }
    }
     
    return false; 
}




//Private
Parser::OperatorInformation Parser::GetOperatorInformation(TCHAR c)
{
    for (UINT i = 0; i < _countof(Parser::ValidOperators); i++)
    {
        if (c == Parser::ValidOperators[i].pctszOperator[0])
        {
            return Parser::ValidOperators[i];
        }
    }

    assert(1 == 0);
    return {};
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