#ifndef _DOCUMENTIO_H_
#define _DOCUMENTIO_H_

#include <QVariant>

class Document;

typedef QMap<QString, QVariant> VariantMap;
typedef QList<QVariant> VariantList;

class DocumentIo
{
 public:
  DocumentIo(Document *doc);
  virtual ~DocumentIo();

  QVariant save(QString &error);
  bool load(const QVariant &data, QString &error);

  virtual int version() const = 0;

  virtual QVariant serialize(QString &error) const = 0;
  virtual bool deserialize(const VariantMap &data, QString &error) = 0;

 protected:
  Document *document_;
};

class DocumentIoV1 : public DocumentIo
{
 public:
  DocumentIoV1(Document *doc);
  ~DocumentIoV1();

  int version() const { return 1; }

  QVariant serialize(QString &error) const;
  bool deserialize(const VariantMap &data, QString &error);

 private:
  VariantList serializeColors(QString &error) const;
  VariantList serializeStitches(QString &error) const;

  bool deserializeStitches(const VariantList &list, QString &error);
};

class DocumentFactory
{
 public:
  static DocumentIo* defaultSerializer(Document *d);
  static Document* load(const QString &path, QString &error);
  static bool save(Document *doc, const QString &path, QString &error);
};

#endif
