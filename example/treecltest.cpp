#include <image.h>
#include <treecl.h>

std::vector<ClassSample> tl =
{
  ClassSample(0, {1,  1, 1.1}),
  ClassSample(1, {2,  2, 1.0}),
  ClassSample(2, {3,  3, 1.1}),
  ClassSample(0, {1.1, 0.9, 1.0}),
  ClassSample(1, {2.2, 1.8, 1.1}),
  ClassSample(2, {3.3, 2.7, 1.0})
};

int main(int argc, char** argv)
{
  TreeCl tree(3, 3); // 3 classes , 3 features
  // Eintrag von Werten

  ClassSample s;

  cout << "Inserting values" << endl;
  for (int i = 0; i < tl.size(); ++i)
    {
      tree.insertValue(tl[i]);
    }

  cout << "Organize" << endl;
  tree.organize();

  cout << "Get Statistics" << endl;
  tree.statistics();
  for (int i = 0; i < tl.size(); ++i)
    {
      cout << tree.find(tl[i].features) << endl;
    }

}
