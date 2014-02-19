#include <iostream>
#include <sstream>
#include "memoryViewModel.h"

memoryViewModel::memoryViewModel()
{
  this->_memoryReference = new QStringList();
}

memoryViewModel::memoryViewModel(Memory *memory)
{
  this->_memoryReference = new QStringList();
  this->memory = memory;
}

memoryViewModel::~memoryViewModel()
{
  delete this->_memoryReference;
}

void memoryViewModel::displayMemory(QString address)
{
  std::cout << "Entered memory dump function!" << std::endl;
  std::cout << "Address passed in: " << address.toStdString() << std::endl;

  this->_memoryReference->clear();
  // Create the memory list according to the entered address
  std::stringstream stream;
  
  // Create the memory reference model
  for (int i = 0; i < 10; ++i)
  {
    stream << i;
    QString buffer;
    buffer.fromStdString(stream.str());
    this->_memoryReference->append(buffer);
    stream.clear();
  }
}

QStringList memoryViewModel::memoryReference()
{
  return *this->_memoryReference;
}

void memoryViewModel::setMemoryReference(QStringList _memoryReference)
{
  *this->_memoryReference = _memoryReference;
}
