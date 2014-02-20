#ifndef MEMORYVIEWMODEL_H
#define MEMORYVIEWMODEL_H

#include <QObject>
#include <QQuickView>
#include <QStandardItemModel>
#include "memory.h"

class memoryViewModel : public QObject
{
  Q_OBJECT

  public:
    // Define any Q_PROPERTY values here for proper UI binding if I have time
    
    explicit memoryViewModel(QObject *parent = 0);
    explicit memoryViewModel(Memory *memory,QQuickView *view, QObject *parent = 0);
    ~memoryViewModel();

    public slots:
      void displayMemory(QString address);

  private:
    QStringList memoryModel;
    Memory *memory;
    QQuickView *view;
};

#endif // MEMORYVIEWMODEL_H
