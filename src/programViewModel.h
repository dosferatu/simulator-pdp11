#ifndef PROGRAMVIEWMODEL_H
#define PROGRAMVIEWMODEL_H

#include <vector>
#include <QObject>
#include "cpu.h"
#include "memory.h"

class programViewModel : public QObject
{
  Q_OBJECT
public:
  explicit programViewModel(QObject *parent = 0);
  explicit programViewModel(CPU *cpu, Memory *memory, QObject *parent = 0);
  ~programViewModel();

signals:

public slots:
  // Program Execution
  void continueExecution();
  void run();
  void step();
  void stop();

  // Program control
  void setBreak();
  void clearBreak();
  void clearAllBreaks();

private:
  std::vector<unsigned short> *breakPoints;
  unsigned short currentInstruction;
  unsigned short nextBreak;
  int status;
  CPU *cpu;
  Memory *memory;
};

#endif // PROGRAMVIEWMODEL_H
