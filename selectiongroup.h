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
  SelectionGroup(Document *doc);
  SelectionGroup(Document *doc, const QRect &region);
  SelectionGroup(Document *doc, const QPoint &initialPosition);
  SelectionGroup(Document *doc, QByteArray &array);
  ~SelectionGroup();

  const QPoint& initialPosition() { return initialPosition_; }

  QByteArray serialize() const;

 private:
  void deserialize(QByteArray &array);

 private:
  QPoint initialPosition_;
  SparseMap *map_;
};

#endif
