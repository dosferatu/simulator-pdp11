#include <fstream>
#include <iostream>
#include <vector>
#include <QtGui>
#include <QQmlComponent>
#include <QtQml>
#include "qtquick2applicationviewer.h"
#include "cpu.h"
#include "memoryViewModel.h"
#include "programViewModel.h"

/******************************************************************************
 *
 *                            PDP 11/20 SIMULATOR
 *
 *****************************************************************************/
// Architecture modules
Memory *memory;
CPU *cpu; 

/*
 * Logistical data structures used by the simulator
 */
std::vector<std::string> *source;
std::fstream *macFile;

/*
 * Data structures used by the GUI
 */
std::vector<int> *breakPoints;	// Will be PC values

/******************************************************************************
 *
 *                                BEGIN MAIN
 *
 *****************************************************************************/
Q_DECL_EXPORT int main(int argc, char *argv[])
{
  int sourceArg = -1;
  bool GUImode = false;
  Verbosity verbosity = Verbosity::off;
  macFile = new std::fstream();

  //Parse command line arguments[>{{{<]
  if (argc > 4)
  {
    std::cout << "Usage: simulator {OPTIONAL}<-V or -v> {OPTIONAL}<-g> {REQUIRED}<ascii file>" << std::endl;
    return 0;
  }

  // Support only one .ascii file
  switch(argc)
  {
    case 2:
      {
        if (static_cast<std::string>(argv[1]).find(".ascii") != std::string::npos)
        {
          sourceArg = 1;
          break;
        }

        else if (static_cast<std::string>(argv[1]).find(".PCascii") != std::string::npos)
        {
          sourceArg = 1;
          break;
        }
      }

    case 3: case 4:
      {
        for (int i = 1; i < argc; ++i)
        {
          if(static_cast<std::string>(argv[i]).compare("-v") == 0)
          {
            if (verbosity == Verbosity::off)
            {
              verbosity = Verbosity::minimal;
            }

            else
            {
              std::cout << "Conflicting verbosity arguments!" << std::endl;
              std::cout << "Usage: simulator {OPTIONAL}<-V or -v> {OPTIONAL}<-g> {REQUIRED}<ascii file>" << std::endl;
              return 0;
            }
          }

          else if(static_cast<std::string>(argv[i]).compare("-V") == 0)
          {
            if (verbosity == Verbosity::off)
            {
              verbosity = Verbosity::verbose;
            }

            else
            {
              std::cout << "Conflicting verbosity arguments!" << std::endl;
              std::cout << "Usage: simulator {OPTIONAL}<-V or -v> {OPTIONAL}<-g> {REQUIRED}<ascii file>" << std::endl;
              return 0;
            }
          }

          else if(static_cast<std::string>(argv[i]).compare("-g") == 0)
          {
            if (!GUImode)
            {
              GUImode = true;
            }

            else
            {
              std::cout << "Conflicting GUI arguments!" << std::endl;
              std::cout << "Usage: simulator {OPTIONAL}<-V or -v> {OPTIONAL}<-g> {REQUIRED}<ascii file>" << std::endl;
              return 0;
            }
          }

          else if (static_cast<std::string>(argv[i]).find(".ascii") != std::string::npos)
          {
            sourceArg = i;
          }

          else if (static_cast<std::string>(argv[i]).find(".PCascii") != std::string::npos)
          {
            sourceArg = i;
          }

          else
          {
            std::cout << "Usage: simulator {OPTIONAL}<-V or -v> {OPTIONAL}<-g> {REQUIRED}<ascii file>" << std::endl;
            return 0;
          }
        }

        break;

        default:
        {
          std::cout << "Usage: simulator {OPTIONAL}<-V or -v> {OPTIONAL}<-g> {REQUIRED}<ascii file>" << std::endl;
          return 0;
        }
      }
  }
  /*}}}*/

  /*
   * Ideally would perform validation on input name and check to see if the file exists
   * so we can print the appropriate error if something goes wrong. This isn't critical
   * right now so it isn't implemented.
   */

  // Parse in .ascii file and retrieve instructions until EOF/*{{{*/
  try
  {
    macFile->open(argv[sourceArg]);
    source = new std::vector<std::string>;
    std::string buffer;

    // Read in a line and store it in to our instruction source vector
    while (!macFile->eof())
    {
      std::getline(*macFile, buffer);
      source->push_back(buffer.c_str());
    }

    // File IO is finished, so close the file
    macFile->close();
  }

  catch (const std::ios_base::failure &e)
  {
    // Implement file not found, access denied, and !success eventually

    // Ghetto file not found version
    std::cout << "Error parsing Macro11 assembler source file!" << std::endl;
    return 0;
  }
  // Remove the last item in the vector that contains only a '\n' character
  source->pop_back();
  /*}}}*/


  // Simulator declarations/*{{{*/
  memory = new Memory(source);
  memory->SetDebugMode(verbosity);
  cpu = new CPU(memory);
  cpu->SetDebugMode(verbosity);/*}}}*/

  // GUI execution block/*{{{*/
  if (GUImode)
  {
    /*
     * Declare the UI ViewModels
     */
    programViewModel *programVM = new programViewModel(cpu, memory);
    memoryViewModel *memoryVM = new memoryViewModel();

    /*
     * Register the ViewModels for use in the QML file
     */
    qmlRegisterType<programViewModel>("ProgramViewModel", 1, 0, "programViewModel");
    qmlRegisterType<memoryViewModel>("MemoryViewModel", 1, 0, "memoryViewModel");

    // Set the context for the GUI and launch
    QGuiApplication app(argc, argv);
    QtQuick2ApplicationViewer viewer;
    viewer.rootContext()->setContextProperty("programViewModel", programVM);
    viewer.rootContext()->setContextProperty("memoryViewModel", memoryVM);
    viewer.setMainQmlFile(QStringLiteral("simulator.qml"));

    // Set this to be conditional for an optional GUI
    viewer.showExpanded();
    return app.exec();
  }/*}}}*/

  // Console execution block/*{{{*/
  else
  {
    /*
     * RUN THIS ONLY IF IN CONSOLE MODE
     */

    // Loop the CPU which will handle state changes internally.
    // Need to make sure program halting is handled in CPU.
    int status = 0;
    do
    {
      status = cpu->FDE();
      memory->IncrementPC();

      if (memory->RetrievePC() % 2 != 0)
      {
        std::cout << "Warning: program counter is not an even number!" << std::endl;
      }

      if (status == 0)
      {
        std::cout << "PDP 11/20 received HALT instruction\n" << std::endl;

        /* The HALT results in a process halt but can be resumed after the user
         *  presses continue on the console.  In this case we are using the
         *  enter key to denote the continue key on the console.
         */
        //std::cout << "Press Enter to continue\n" << std::endl;
        //std::cin.get();
        //status = 0;  // Reset status to allow process to continue.
      }
    } while (status > 0);
  }/*}}}*/

  // Garbage collection/*{{{*/
  delete breakPoints;
  delete cpu;
  delete macFile;
  delete source;
  /*}}}*/

  return 0;
}
