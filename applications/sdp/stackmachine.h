#ifndef STACKMACHINE_H
#define STACKMACHINE_H

#include <string>
#include <vector>

class Stack1
{
public:
  int pop()
  {
    int h = stack.back();
    stack.pop_back();
    return h;
  }

  void push(int i)
  {
    stack.push_back(i);
  }

  int &top()
  {
    return stack.back();
  }

  void clear()
  {
    stack.clear();
  }

  int get(int i)
  {
    int last = stack.size() - 1;
    return stack[last - i];
  }
private:
  std::vector<int> stack;
};

class Stack
{
public:
  Stack(): sp(-1) {}

  int pop()
  {
    return stack[sp--];
  }

  void push(int i)
  {
    stack[++sp] = i;
  }

  int &top()
  {
    return stack[sp];
  }

  void clear()
  {
    sp = -1;
  }

  int &get(int i)
  {
    return stack[sp - i];
  }
private:
  int stack[128];
  int sp;
};

class StackMachine
{
public:
  StackMachine(const std::string &expression);
  int evaluate(const std::vector<int> &variables, int x, int y, int z) const;

protected:
  std::vector<int> code;
  // mutable Stack stack;
};
#endif
