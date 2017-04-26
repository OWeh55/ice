/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _COMPONENT_TREE_H
#define _COMPONENT_TREE_H

#include <limits.h>
#include <vector>

#include "region.h"

#define VIRTUAL

#define xy2ind(x,y) (y * xs + x)
#define ind2x(idx) (idx % xs)
#define ind2y(idx) (idx / xs)

namespace ice
{
  class ComponentTreeNode final
  {
    typedef std::vector<ComponentTreeNode*> nodelist_t;

  public:
    // block copy constructor
    ComponentTreeNode(const ComponentTreeNode&) = delete;

    ComponentTreeNode(): value(0), area(0), region(nullptr) {}
    ComponentTreeNode(int valuep): value(valuep), area(1), region(nullptr) {}

    VIRTUAL ~ComponentTreeNode()
    {
      for (auto it = children.begin(); it != children.end(); it++)
        {
          delete *it;
        }
      delete region;
    }

    VIRTUAL void unlink() // alle Verknüpfungen zu Kindern lösen
    {
      children.clear();
    }

  private:
    VIRTUAL void addChilds(const ComponentTreeNode& ch)
    {
      for (auto it = ch.children.begin(); it != ch.children.end(); it++)
        {
          children.push_back(*it);
        }
    }

  public:

    VIRTUAL int Value() const
    {
      return value;
    }

    VIRTUAL void merge(const ComponentTreeNode& ad)
    {
      addChilds(ad);
      area += ad.area;
    }

    VIRTUAL void addAsChild(ComponentTreeNode& ch)
    {
      children.push_back(&ch);
      area += ch.area;
    }

    VIRTUAL Region& getRegion()
    {
      if (region == nullptr)
        {
          region = new Region;
        }
      return *region;
    }

    VIRTUAL const Region& getRegion() const
    {
      if (region == nullptr)
        {
          region = new Region;
        }
      return *region;
    }

    VIRTUAL Region getTotalRegion() const;

    VIRTUAL int getDepth() const; // depth in graph sense

    VIRTUAL int getHeight(int referenceValue) const; // depth in [pixel] value sense
    VIRTUAL int getHeight() const
    {
      return getHeight(value);
    }

    VIRTUAL void absorbChildren();
    VIRTUAL bool cutSmallAreas(int minarea);

    VIRTUAL void reduceSmallLeaves(int minheight);

    VIRTUAL void print(int depth, int lvl = 0) const;
    VIRTUAL void draw(Image& img, int depth = INT_MAX) const;

    VIRTUAL int getBigRegions(std::vector<Region>& regs, int minarea) const;

    VIRTUAL int getNComponents() const;

    VIRTUAL void getLeafs(std::vector<const ComponentTreeNode*>& leafes) const;
    VIRTUAL void getLeafs(std::vector<const ComponentTreeNode*>& leafes,
                          std::vector<const ComponentTreeNode*>& basins) const;

    VIRTUAL void getLeafs(std::vector<Region>& minreg) const;
    VIRTUAL void getLeafs(std::vector<Region>& minreg,
                          std::vector<Region>& basinregs) const;

    VIRTUAL int getArea() const
    {
      return area;
    }
  private:
    int setArea();
    int setHeight(int referenceValue);

    int value; // pixel value
    int height; // difference to extremal leaf
    int area; // !total area including childs

    //    Region region;
    // we use a pointer here to avoid premature creation of region
    mutable Region* region;

    nodelist_t children;

    friend ComponentTreeNode* makeComponentTree(const Image& img, bool maxAsLeaf = false);
  };
  ComponentTreeNode* makeComponentTree(const Image& img, bool maxAsLeaf);
} // namespace ice
#endif
