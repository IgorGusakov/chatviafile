#include <iostream>
#include <string>
#include <thread>

#include "WorkWithFile.h"



int main(int argc, char *argv[]) {
  std::string name_test_file = "Test.txt";
  WorkWithFile file(name_test_file,200);
  portig_file flag_open = portig_file::NONE;

  for (int i = 0; i < argc; ++i) {
    if(!std::string(argv[i]).empty()) {
      std::basic_string option = std::string(argv[i]);
      if (option == "-r") {
        flag_open = portig_file::READ;
        std::cout << "turn on option only read file\n";
      }
      else if(option == "-w") {
        flag_open = portig_file::WRITE;
        std::cout << "turn on option only write file\n";
      }
      else if(option == "-rw") { //default run how if(option == "-rw")
        flag_open = portig_file::READ_WRITE;
        std::cout << "turn on option read and write file\n";
      }
      if (option == "-crc") {
        std::cout << "turn on option -crc\n";
      }
    }
  }

  file.OpenFile(flag_open);

  switch (flag_open) {
    case portig_file::READ: {
      file.StartHandlerReader();
      break;
    }
    case portig_file::WRITE: {
      file.StartHandlerWriter();
      break;
    }
    case portig_file::READ_WRITE: {
      std::thread in_file (&WorkWithFile::StartHandlerReader, &file);
      in_file.detach();
      file.StartHandlerWriter();
      break;
    }
    case portig_file::NONE: {
      std::cout << "You need choose -r -w -rw\n";
    }
  }

  return 0;
}
