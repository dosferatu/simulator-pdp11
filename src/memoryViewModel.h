#ifndef MEMORYVIEWMODEL_H
#define MEMORYVIEWMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <string>

class memoryViewModel : public QObject
{
  Q_OBJECT

  public:
    Q_PROPERTY(QVector<std::string> memoryReference\
        READ memoryReference\
        WRITE setMemoryReference)

      memoryViewModel();
    ~memoryViewModel();

    QVector<std::string> memoryReference();

    void setMemoryReference(QVector<std::string> __memoryReference);

    public slots:
      void displayMemory(QString address);

  private:
      QVector<std::string> *_memoryReference;
};

#endif // MEMORYVIEWMODEL_H
