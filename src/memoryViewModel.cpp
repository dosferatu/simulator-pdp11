#include <iostream>
#include <sstream>
#include <QtQml>
#include "memoryViewModel.h"

memoryViewModel::memoryViewModel(QObject *parent) : QObject(parent)
{
  //this->_memoryReference = new QStringList();
}

memoryViewModel::memoryViewModel(Memory *memory, QQuickView *view, QObject *parent) : QObject(parent)
{
  //this->_memoryReference = new QStringList();
  this->memory = memory;
  this->view = view;

  this->view->rootContext()->setContextProperty("memoryModel", QVariant::fromValue(this->memoryModel));
}

memoryViewModel::~memoryViewModel()
{
  //delete this->_memoryReference;
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

//QStringList memoryViewModel::memoryReference()
//{
//return *this->_memoryReference;
//}

//void memoryViewModel::setMemoryReference(QStringList _memoryReference)
//{
//*this->_memoryReference = _memoryReference;
//}
