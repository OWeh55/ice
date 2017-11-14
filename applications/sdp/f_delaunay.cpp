#include <image.h>

#include "gpointset.h"
#include "ggraph.h"

#include "f_delaunay.h"

void DelaunayPL::get_data()
{
  const GPoint *src = dynamic_cast<const GPoint *>(getInputData(0));
  int limit = getInputInt(1, 0);

// cout << "delaunay " << src->getPointList().size() << " Punkte" << endl;

  GGraph *tresult = new GGraph;
  result[0] = tresult;
  Delaunay delaunay(src->getVector());
  for (int i = 0; i < src->getSize(); ++i)
    tresult->push_back((*src)[i]);
// cout << src->getPointList().size() << " Punkte" << endl;
  vector<vector<int> > indexList;
  delaunay.getEdgesI(indexList, limit);
  for (int i = 0; i < (int)indexList.size(); ++i)
    {
      int idx1 = indexList[i][0];
      int idx2 = indexList[i][1];
//cout << idx1 << " - " << idx2 << endl;
      tresult->addEdge(idx1, idx2);
    }
}

