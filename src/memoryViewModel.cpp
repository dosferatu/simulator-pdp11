#include <iostream>
#include <sstream>
#include <QtQml>
#include "memoryViewModel.h"

memoryViewModel::memoryViewModel(QObject *parent) : QObject(parent)
{
}

memoryViewModel::memoryViewModel(Memory *memory, QQuickView *view, QObject *parent) : QObject(parent)/*{{{*/
{
  this->memory = memory;
  this->view = view;

  this->view->rootContext()->setContextProperty("memoryModel", QVariant::fromValue(this->memoryModel));

  // Initialize the UI register fields
  std::stringstream stream;
  unsigned short value;

  value = this->memory->ReadAddress(R0);
  stream << value;
  this->_R0 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadAddress(R1);
  stream << value;
  this->_R1 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadAddress(R2);
  stream << value;
  this->_R2 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadAddress(R3);
  stream << value;
  this->_R3 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadAddress(R4);
  stream << value;
  this->_R4 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadAddress(R5);
  stream << value;
  this->_R5 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadAddress(SP);
  stream << value;
  this->_SP = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadAddress(PC);
  stream << value;
  this->_PC = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadPS();
  stream << ((value & 0x8) >> 3);
  this->_N = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  value = this->memory->ReadPS();
  stream << ((value & 0x4) >> 2);
  this->_Z = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  stream << ((value & 0x2) >> 1);
  this->_V = stream.str().c_str();
  stream.str(std::string());
  stream.clear();

  stream << (value & 0x1);
  this->_C = stream.str().c_str();
}/*}}}*/

memoryViewModel::~memoryViewModel()
{
}

void memoryViewModel::displayMemory(QString address)
{
  this->memoryModel.clear();
  this->currentMemoryAddress = address;

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

void memoryViewModel::refreshFields()/*{{{*/
{
  this->displayMemory(this->currentMemoryAddress);

  // Refresh all the register values when called
  std::stringstream stream;
  unsigned short value;

  value = this->memory->ReadAddress(R0);
  stream << value;
  this->_R0 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyR0(_R0);

  value = this->memory->ReadAddress(R1);
  stream << value;
  this->_R1 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyR1(_R1);

  value = this->memory->ReadAddress(R2);
  stream << value;
  this->_R2 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyR2(_R2);

  value = this->memory->ReadAddress(R3);
  stream << value;
  this->_R3 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyR3(_R3);

  value = this->memory->ReadAddress(R4);
  stream << value;
  this->_R4 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyR4(_R4);

  value = this->memory->ReadAddress(R5);
  stream << value;
  this->_R5 = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyR5(_R5);

  value = this->memory->ReadAddress(SP);
  stream << value;
  this->_SP = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifySP(_SP);

  value = this->memory->ReadAddress(PC);
  stream << value;
  this->_PC = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyPC(_PC);

  value = this->memory->ReadPS();
  stream << ((value & 0x8) >> 3);
  this->_N = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyN(_N);

  stream << ((value & 0x4) >> 2);
  this->_Z = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyZ(_Z);

  stream << ((value & 0x2) >> 1);
  this->_V = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyV(_V);

  stream << (value & 0x1);
  this->_C = stream.str().c_str();
  stream.str(std::string());
  stream.clear();
  this->notifyC(_C);


  return;
}/*}}}*/

// Set/*{{{*/
void memoryViewModel::setR0(QString value)
{
  this->memory->WriteAddress(R0, value.toUShort());
  //this->view->rootContext()->setContextProperty("memoryViewModel._R0", value);
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
