#include <iostream>
#include <QStandardItemModel>
#include "memoryViewModel.h"

memoryViewModel::memoryViewModel()
{
  this->_memoryReference = new QVector<std::string>();
}

memoryViewModel::~memoryViewModel()
{

}

void memoryViewModel::displayMemory(QString address)
{
  std::cout << "Entered memory dump function!" << std::endl;
  std::cout << "Address passed in: " << address.toStdString() << std::endl;
}

QVector<std::string> memoryViewModel::memoryReference()
{
  return *this->_memoryReference;
}

void memoryViewModel::setMemoryReference(QVector<std::string> __memoryReference)
{
  _memoryReference = &__memoryReference;
}
