#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Contur.h"
namespace ice
{
  Contur Dijkstra(const Image& img, IPoint start, Image& end);
  Contur Dijkstra(const Image& img, IPoint start, IPoint end);
}
#endif
