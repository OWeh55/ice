#include <iostream>
#include "parser.h"

#include "stackmachine.h"

using namespace std;

StackMachine::StackMachine(const string& expression)
{
  int stacksize = 0;
  Parser exp(expression);
  if (exp.empty())
    throw SdpException("evaluate", "empty expression");

  // compile expression
  while (!exp.empty())
    {
      // cout << "stacksize: " << stacksize<< endl;
      Token next = exp.nextToken();
      if (next.type == Token::identifier)
        {
          exp.skipToken();

          if (next.token.size() == 1)  // variables
            {
              char operand = next.token[0];
              if ((operand >= 'a' && operand <= 'm') ||
                  operand == 'x' || operand == 'y' || operand == 'z')
                code.push_back(operand);
              else
                throw SdpException("evaluate", "unknown variable " + next.token);
              stacksize++;
            }
          else if (next.token == "min")
            {
              code.push_back('I');
              stacksize--; // two operands -> one result
            }
          else if (next.token == "max")
            {
              code.push_back('A');
              stacksize--; // two operands -> one result
            }
          else if (next.token == "if")
            {
              code.push_back('F');
              stacksize -= 2; // three operands -> one result
            }
          else if (next.token == "dup")
            {
              code.push_back('D');
              stacksize++; // one operands -> two result
            }
          else if (next.token == "swap")
            {
              code.push_back('S');
            }
          else if (next.token == "over")
            {
              code.push_back('O');
              stacksize++;
            }
          else if (next.token == "abs")
            {
              code.push_back('V');
            }
          else
            throw SdpException("evaluate", "unexpected identifier " + next.token);
        }
      else if (next.type == Token::integer)
        {
          exp.skipToken();
          code.push_back('L');
          code.push_back(next.getInt());
          stacksize++;
        }
      else if (next.type == Token::singlecharacter)
        {
          exp.skipToken();
          char oper = next.token[0];
          switch (oper)
            {
            // binary operators represent themself
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
            case '<':
            case '=':
            case '>':
              code.push_back(oper);
              stacksize--;
              break;
            // unary operators represent themself
            case '!':
              // logical negation
              code.push_back(oper);
              break;

            default:
              throw SdpException("evaluate", "unknown operator " + next.token);
            }
        }
      else
        throw SdpException("evaluate", "unexpected token " + next.token);
      // after first token stack must not be empty
      if (stacksize <= 0)
        throw SdpException("evaluate", "stack underflow");
    }

  if (stacksize > 1)
    throw SdpException("evaluate", "stack not empty");
}


int StackMachine::evaluate(const vector<int>& variables,
                           int x, int y, int z) const
{
  Stack stack;
  stack.clear();
  int codesteps = code.size();
  for (int i = 0; i < codesteps; ++i)
    {
      int cc = code[i];
      int oper, oper2;
      switch (cc)
        {
        case 'a':
          stack.push(variables[0]);
          break;
        case 'b':
          stack.push(variables[1]);
          break;
        case 'c':
          stack.push(variables[2]);
          break;
        case 'd':
          stack.push(variables[3]);
          break;
        case 'e':
          stack.push(variables[4]);
          break;
        case 'x':
          stack.push(x);
          break;
        case 'y':
          stack.push(y);
          break;
        case 'z':
          stack.push(z);
          break;

        case 'L':
          ++i;
          stack.push(code[i]);
          break;

        case '+':
          oper = stack.pop();
          stack.top() += oper;
          break;
        case '-':
          oper = stack.pop();
          stack.top() -= oper;
          break;

        case '*':
          oper = stack.pop();
          stack.top() *= oper;
          break;
        case '/':
          oper = stack.pop();
          if (oper != 0)
            stack.top() /= oper;
          else
            throw SdpException("evaluate", "division by zero");
          break;
        case '%':
          oper = stack.pop();
          if (oper != 0)
            stack.top() %= oper;
          else
            throw SdpException("evaluate", "division by zero");
          break;

        case '<':
          oper = stack.pop();
          stack.top() = stack.top() < oper ? true : false;
          break;

        case '>':
          oper = stack.pop();
          stack.top() = stack.top() > oper ? true : false;
          break;

        case '=':
          oper = stack.pop();
          stack.top() = stack.top() == oper ? true : false;
          break;

        case '!':
          stack.top() = ! stack.top();
          break;

        case 'A':
          oper = stack.pop();
          oper2 = stack.top();
          stack.top() = oper > oper2 ? oper : oper2;
          break;

        case 'I':
          oper = stack.pop();
          oper2 = stack.top();
          stack.top() = oper < oper2 ? oper : oper2;
          break;

        case 'F':
          oper = stack.pop();
          oper2 = stack.pop();
          stack.top() = stack.top() ? oper2 : oper ;
          break;

        case 'D':
          stack.push(stack.top());
          break;

        case 'S':
          oper = stack.pop();
          oper2 = stack.pop();
          stack.push(oper);
          stack.push(oper2);
          break;

        case 'O':
        {
          oper = stack.get(1); // get element 1 under top
          stack.push(oper);
        }
        break;

        case 'V':
        {
          stack.top() = abs(stack.top());
        }
        break;
        }
    }
  return stack.top();
}
