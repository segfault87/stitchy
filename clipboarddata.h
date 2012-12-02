#ifndef _CLIPBOARDDATA_H_
#define _CLIPBOARDDATA_H_

#include <QByteArray>
#include <QPoint>

class Document;
class SparseMap;

class ClipboardData
{
 public:
  ClipboardData(Document *doc);
  ClipboardData(Document *doc, const QPoint &initialPosition);
  ClipboardData(Document *doc, QByteArray &array);
  ~ClipboardData();

  const QPoint& initialPosition() { return initialPosition_; }

  QByteArray serialize() const;

 private:
  void deserialize(QByteArray &array);

 private:
  QPoint initialPosition_;
  SparseMap *map_;
};

#endif
