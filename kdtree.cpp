#include <algorithm>
#include <queue>

#include "colormanager.h"

#include "kdtree.h"

KdNode::KdNode(ColorListItr begin, ColorListItr end, int depth)
{
  left_ = NULL;
  right_ = NULL;
  color_ = NULL;

  if (begin == end)
    return;
  else if (end - begin == 1)
    return;

  int dim = depth % 3;
  if (dim == 0)
    std::sort(begin, end, Comparator::Red());
  else if (dim == 1)
    std::sort(begin, end, Comparator::Green());
  else
    std::sort(begin, end, Comparator::Blue());

  ColorList::size_type median = (end - begin) / 2;
  color_ = *(begin + median);

  if (begin != (begin + median))
    left_ = new KdNode(begin, begin + median, depth + 1);
  if ((begin + median + 1) != end)
    right_ = new KdNode(begin + median + 1, end, depth + 1);
}

KdNode::~KdNode()
{
  if (left_)
    delete left_;
  if (right_)
    delete right_;
}

KdTree::KdTree(ColorManager *manager, const Color *transparentColor)
{
  std::vector<const Color *> v;
  foreach (const Color *c, manager->colorList()) {
    if (!transparentColor ||
	(transparentColor && c->color() != transparentColor->color()))
      v.push_back(c);
  }
  if (transparentColor)
    v.push_back(transparentColor);

  root_ = new KdNode(v.begin(), v.end());
}

KdTree::KdTree(ColorListItr begin, ColorListItr end)
{
  root_ = new KdNode(begin, end);
}

KdTree::~KdTree()
{
  delete root_;
}

const Color* KdTree::nearest(const QColor &color)
{
  const Color *min = root_->color_;
  byte array[] = { color.red(), color.green(), color.blue() };

  return nearest(root_, array, min);
}

int KdTree::distance(byte a[], const Color *b)
{
  if (!b)
    return INT_MAX;

  int dr = a[0] - b->red();
  int dg = a[1] - b->green();
  int db = a[2] - b->blue();

  return dr * dr + dg * dg + db * db;
}

const Color* KdTree::nearest(KdNode *node, byte color[], const Color *&min, int depth)
{
  if (node) {
    int axis = depth % 3;
    byte mincolors[] = { min->red(), min->green(), min->blue() };
    int dist = color[axis] - mincolors[axis];
    KdNode *near = dist <= 0 ? node->left_ : node->right_;
    KdNode *far = dist <= 0 ? node->right_ : node->left_;

    min = nearest(near, color, min, depth + 1);
    if ((dist * dist) < distance(color, min))
      min = nearest(far, color, min, depth + 1);
    if (distance(color, node->color_) < distance(color, min))
      min = node->color_;
  }

  return min;
}
