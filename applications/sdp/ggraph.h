#ifndef IM_G_GRAPH_H
#define IM_G_GRAPH_H

#include <vector>

#include <image.h>
#include "exceptions.h"
#include "gpointset.h"

class GGraph: public GPoint
{
public:
  virtual DType getType() const
  {
    return DType(DType::graph, datavector.size());
  }

  virtual ~GGraph() {};

  virtual GData* clone() const
  {
    return new GGraph(*this);
  }

  virtual void addEdge(int i1, int i2)
  {
    try
      {
        checkIndex(i1);
        checkIndex(i2);
        edges.push_back(std::pair<int, int>(i1, i2));
      }
    catch (SdpException& ex)
      {
        ex.setWhere("DGraph");
        throw (ex);
      }
  }

  virtual int nEdges() const
  {
    return edges.size();
  }

  virtual void getEdge(int idx, Point& p1, Point& p2) const
  {
    if (idx < 0 || idx >= (int)edges.size())
      throw SdpException("Graph::getEdge", "Edge index invalid");
    p1 = datavector[edges[idx].first];
    p2 = datavector[edges[idx].second];
  }
protected:
  std::vector<std::pair<int, int> > edges;
};

#endif
