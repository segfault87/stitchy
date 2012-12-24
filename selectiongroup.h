#ifndef _SELECTIONGROUP_H_
#define _SELECTIONGROUP_H_

#include <QByteArray>
#include <QGraphicsItemGroup>
#include <QPoint>

class Document;
class SparseMap;

class SelectionGroup : public QGraphicsItemGroup
{
 public:
  static const char* mimeType();

  SelectionGroup(Document *doc);
  SelectionGroup(Document *doc, const QRect &region, bool move = false);
  SelectionGroup(Document *doc, const QPoint &position);
  SelectionGroup(Document *doc, QByteArray &array);
  ~SelectionGroup();

  const QPoint& position() { return position_; }
  const SparseMap* map() { return map_; }

  void moveTo(const QPoint &p);
  void moveRel(const QPoint &delta);

  QByteArray serialize() const;

 private:
  void deserialize(QByteArray &array);
  void initialize(Document *doc, const QRect &region, bool move = false);

 private:
  QPoint position_;
  SparseMap *map_;
};

#endif
