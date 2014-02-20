#include <iostream>
#include <sstream>
#include <QtQml>
#include "memoryViewModel.h"

memoryViewModel::memoryViewModel(QObject *parent) : QObject(parent)
{
}

memoryViewModel::memoryViewModel(Memory *memory, QQuickView *view, QObject *parent) : QObject(parent)
{
  this->memory = memory;
  this->view = view;

  this->view->rootContext()->setContextProperty("memoryModel", QVariant::fromValue(this->memoryModel));
}

memoryViewModel::~memoryViewModel()
{
}

void memoryViewModel::displayMemory(QString address)
{
  this->memoryModel.clear();

  // Perform validation on address here

  // Create the memory list according to the entered address
  std::stringstream stream;
  unsigned short addressVal;

  // Convert the string to a number
  stream << std::oct << address.toStdString();
  stream >> addressVal;

  // Create the memory reference model
  for (int i = 0; i < 20; i = i + 2)  // Only access words
  {
    std::stringstream converter;
    converter << std::oct << this->memory->ReadAddress(addressVal + i);
    this->memoryModel.append(converter.str().c_str());
  }

  // Notify the view that the model is updated
  this->view->rootContext()->setContextProperty("memoryModel", QVariant::fromValue(this->memoryModel));
}

// Set/*{{{*/
void memoryViewModel::setR0(QString value)
{
  this->memory->WriteAddress(R0, value.toUShort());
  return;
}

void memoryViewModel::setR1(QString value)
{
  this->memory->WriteAddress(R1, value.toUShort());
  return;
}

void memoryViewModel::setR2(QString value)
{
  this->memory->WriteAddress(R2, value.toUShort());
  return;
}

void memoryViewModel::setR3(QString value)
{
  this->memory->WriteAddress(R3, value.toUShort());
  return;
}

void memoryViewModel::setR4(QString value)
{
  this->memory->WriteAddress(R4, value.toUShort());
  return;
}

void memoryViewModel::setR5(QString value)
{
  this->memory->WriteAddress(R5, value.toUShort());
  return;
}

void memoryViewModel::setSP(QString value)
{
  this->memory->WriteAddress(SP, value.toUShort());
  return;
}

void memoryViewModel::setPC(QString value)
{
  this->memory->WriteAddress(PC, value.toUShort());
  return;
}/*}}}*/
