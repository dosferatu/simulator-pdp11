#ifndef PROGRAMVIEWMODEL_H
#define PROGRAMVIEWMODEL_H

#include <QObject>
#include <QQuickView>
#include <vector>
#include <string>
#include "cpu.h"
#include "memory.h"
#include "memoryViewModel.h"

class programViewModel : public QObject
{
  Q_OBJECT
  public:
    // Define any Q_PROPERTY values here for proper UI binding if I have time

    explicit programViewModel(QObject *parent = 0);
    explicit programViewModel(CPU *cpu, Memory *memory, memoryViewModel *memoryVM, QQuickView *view, std::vector<std::string> *source, QObject *parent = 0);
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
    memoryViewModel *memoryVM;
    QQuickView *view;
    QStringList instructionModel;
    std::vector<std::string> pcList;
};

#endif // PROGRAMVIEWMODEL_H
