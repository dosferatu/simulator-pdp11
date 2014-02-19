#ifndef MEMORYVIEWMODEL_H
#define MEMORYVIEWMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <string>
#include "memory.h"

class memoryViewModel : public QObject
{
  Q_OBJECT

  public:
    Q_PROPERTY(QStringList memoryReference\
        READ memoryReference\
        WRITE setMemoryReference)

      memoryViewModel();
      memoryViewModel(Memory *memory);
    ~memoryViewModel();

    QStringList memoryReference();

    void setMemoryReference(QStringList memoryReference);

    public slots:
      void displayMemory(QString address);

  private:
      QStringList *_memoryReference;
      Memory *memory;
};

#endif // MEMORYVIEWMODEL_H
