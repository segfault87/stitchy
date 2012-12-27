// Copyright (c) 2012 Park Joon-Kyu

#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <vector>

#include "color.h"

class ColorManager;

namespace Comparator
{
  struct Red {
    bool operator()(const Color *lhs, const Color *rhs) {
      return lhs->red() < rhs->red();
    }
  };

  struct Green {
    bool operator()(const Color *lhs, const Color *rhs) {
      return lhs->green() < rhs->green();
    }
  };

  struct Blue {
    bool operator()(const Color *lhs, const Color *rhs) {
      return lhs->blue() < rhs->blue();
    }
  };
}

typedef std::vector<const Color *> ColorList;
typedef ColorList::iterator ColorListItr;

class KdNode
{
 private:
  KdNode(ColorListItr begin, ColorListItr end, int depth = 0);
  ~KdNode();
  
  const Color *color_;
  KdNode *left_;
  KdNode *right_;

  friend class KdTree;
};

class KdTree
{
 public:
  KdTree(ColorManager *manager, const Color *transparentColor);
  KdTree(ColorListItr begin, ColorListItr end);
  ~KdTree();

  const Color* nearest(const QColor &color);

 private:
  static int distance(byte a[], const Color *b);
  const Color* nearest(KdNode *node, byte color[],
		       const Color *&min, int depth = 0);

  KdNode *root_;
};

#endif
