#ifndef MEMORYVIEWMODEL_H
#define MEMORYVIEWMODEL_H

#include <QObject>
#include <QQuickView>
#include <QStandardItemModel>
#include "memory.h"

class memoryViewModel : public QObject
{
  Q_OBJECT

  // Register states
  Q_PROPERTY(QString _R0 READ getR0 WRITE setR0 NOTIFY notifyR0)
  Q_PROPERTY(QString _R1 READ getR1 WRITE setR1 NOTIFY notifyR1)
  Q_PROPERTY(QString _R2 READ getR2 WRITE setR2 NOTIFY notifyR2)
  Q_PROPERTY(QString _R3 READ getR3 WRITE setR3 NOTIFY notifyR3)
  Q_PROPERTY(QString _R4 READ getR4 WRITE setR4 NOTIFY notifyR4)
  Q_PROPERTY(QString _R5 READ getR5 WRITE setR5 NOTIFY notifyR5)
  Q_PROPERTY(QString _SP READ getSP WRITE setSP NOTIFY notifySP)
  Q_PROPERTY(QString _PC READ getPC WRITE setPC NOTIFY notifyPC)

  // Processor status flags
  Q_PROPERTY(QString _N READ getN NOTIFY notifyN)
  Q_PROPERTY(QString _Z READ getZ NOTIFY notifyZ)
  Q_PROPERTY(QString _V READ getV NOTIFY notifyV)
  Q_PROPERTY(QString _C READ getC NOTIFY notifyC)

  public:
  explicit memoryViewModel(QObject *parent = 0);
  explicit memoryViewModel(Memory *memory,QQuickView *view, QObject *parent = 0);
  ~memoryViewModel();

  // Get/*{{{*/
  QString getR0() const { return _R0; }
  QString getR1() const { return _R1; }
  QString getR2() const { return _R2; }
  QString getR3() const { return _R3; }
  QString getR4() const { return _R4; }
  QString getR5() const { return _R5; }
  QString getSP() const { return _SP; }
  QString getPC() const { return _PC; }
  QString getN() const { return _N; }
  QString getZ() const { return _Z; }
  QString getV() const { return _V; }
  QString getC() const { return _C; }
  /*}}}*/

  public slots:
    void displayMemory(QString address);
  void refreshFields();

  // Set/*{{{*/
  void setR0(QString value);
  void setR1(QString value);
  void setR2(QString value);
  void setR3(QString value);
  void setR4(QString value);
  void setR5(QString value);
  void setSP(QString value);
  void setPC(QString value);
  /*}}}*/

signals:

  // Notify/*{{{*/
  void notifyR0(QString value);
  void notifyR1(QString value);
  void notifyR2(QString value);
  void notifyR3(QString value);
  void notifyR4(QString value);
  void notifyR5(QString value);
  void notifySP(QString value);
  void notifyPC(QString value);
  void notifyN(QString value);
  void notifyZ(QString value);
  void notifyV(QString value);
  void notifyC(QString value);
  /*}}}*/

  private:
  QStringList memoryModel;
  Memory *memory;
  QQuickView *view;
  QString _R0;
  QString _R1;
  QString _R2;
  QString _R3;
  QString _R4;
  QString _R5;
  QString _SP;
  QString _PC;
  QString _N;
  QString _Z;
  QString _V;
  QString _C;
};

#endif // MEMORYVIEWMODEL_H
