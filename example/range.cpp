#include <set>
#include <iostream>

#include <range.h>

using namespace ice;
using namespace std;

#define RANGE 70

template<typename T>
void print(const set<Range<T>>& rangeSet)
{
  for (auto r : rangeSet)
    {
      cout << boolalpha << r.isValid() << " ";
      cout << r.getLowest() << " .. " << r.getHighest() << endl;
    }
}

template<typename T>
void makeDisjunkt(set<Range<T>>& rangeSet)
{
  set<Range<T>> result;
  auto current = rangeSet.begin();
  Range<T> thisRange = *current;
  while (current != rangeSet.end())
    {
      if (current->getLowest() < thisRange.getHighest())
        thisRange.update(current->getHighest());
      else
        {
          result.insert(thisRange);
          thisRange = *current;
        }
      current++;
    }
  result.insert(thisRange);
  rangeSet = result;
}

int main(int ragc, char** argv)
{
  srand(time(NULL));

  set<Range<int>> rangeSet;
  for (int i = 0; i < 10; i++)
    {
      int l = rand() % RANGE;
      rangeSet.insert({l, l + 4});
    }
  print(rangeSet);
  makeDisjunkt(rangeSet);
  cout << "------" << endl;
  print(rangeSet);
}
