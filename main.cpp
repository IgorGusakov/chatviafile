#include <iostream>
#include <string>
#include <thread>

#include "WorkWithFile.h"



int main(int argc, char *argv[]) {
  //name file
  std::string name_test_file = "Test.txt";
  //max size file
  uint64_t max_size_file = 5'000'000;
  //-r -w -rw
  portig_file flag_open = portig_file::NONE;
  bool flag_hash = false;
  open_file status_open = open_file::OPEN_ERR;


  for (int i = 0; i < argc; ++i) { //!< Work with CLI
    if(!std::string(argv[i]).empty()) { //!<  CLI detect
      std::basic_string option = std::string(argv[i]); //!<  argument
      //read only
      if (option == "-r") {    //!<parser  argument -r
        flag_open = portig_file::READ;
        std::cout << "turn on option only read file\n";
      }
      //write only
      else if(option == "-w") {    //!<parser  argument -w
        flag_open = portig_file::WRITE;
        std::cout << "turn on option only write file\n";
      }
      //read and write only
      else if(option == "-rw") {  //!<parser  argument -rw
        flag_open = portig_file::READ_WRITE;
        std::cout << "turn on option read and write file\n";
      }
      //CRC file
      if (option == "-crc") { //!<parser  argument -crc
        std::cout << "turn on option -crc\n";
        flag_hash = true;
      }
    }
  }

  WorkWithFile file(name_test_file,max_size_file,flag_hash);
  status_open = file.OpenFile();

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
