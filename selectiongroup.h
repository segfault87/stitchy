#ifndef _SELECTIONGROUP_H_
#define _SELECTIONGROUP_H_

#include <QByteArray>
#include <QGraphicsItemGroup>
#include <QPoint>
#include <QRect>

class Document;
class SparseMap;

class SelectionGroup : public QGraphicsItemGroup
{
 public:
  static const char* mimeType();

  SelectionGroup(Document *doc);
  SelectionGroup(Document *doc, const QRect &region, bool move = false);
  SelectionGroup(Document *doc, const QRect &region);
  SelectionGroup(Document *doc, const QByteArray &array);
  ~SelectionGroup();

  QPoint position() { return region_.topLeft(); }
  const QRect& region() { return region_; }
  const SparseMap* map() { return map_; }

  void moveTo(const QPoint &p);
  void moveRel(const QPoint &delta);

  QByteArray serialize() const;

 private:
  void deserialize(Document *doc, const QByteArray &array);
  void initialize(Document *doc, const QRect &region, bool move = false);

 private:
  QRect region_;
  SparseMap *map_;
};

#endif
