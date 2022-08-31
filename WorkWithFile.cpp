//
// Created by Igor Gusakov on 31.08.2022.
//
#include "WorkWithFile.h"

void WorkWithFile::StartHandlerWriter() {
  if(of_strm.is_open()) {
//      size_file = std::filesystem::file_size(filename);
    while (size_file <= max_file_size) {
      getline(std::cin >> std::ws, str_buf_out);
      str_buf_out.push_back('\n'); // if you want every message is starting from new line
      of_strm << str_buf_out;
      of_strm.flush();
      std::cout << "Size messages: " << str_buf_out.length() << "byte \n";
      size_file += str_buf_out.length();
      str_buf_out.clear();
      std::cout << "Size file: " << size_file << "byte \n";
    }
    of_strm.close();
    std::cout << "File size more 5Mb and Exit" << std::endl;
  }
}


void WorkWithFile::StartHandlerReader() {
  if (if_strm.is_open())
  {
    ///todo: it will provide to detected was changed file and create callback in future, maybe
//    uint32_t size = std::filesystem::file_size(filename);
//    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(filename);
//    std::time_t cftime = to_time_t(ftime);
//    std::cout<<"change file:"<< cftime;

    auto size_old = std::filesystem::file_size(filename);

    ///Fixme: this While(true) is overload work processor
    while (true) {
      if (size_old != std::filesystem::file_size(filename)) {
//        std::cout << "Size other \n";
        if_strm.close();

        {
          ///todo: did not work my_file.clear();???
          if_strm.open(filename);
          if_strm.seekg(size_old);
          uint32_t messages_size = std::filesystem::file_size(filename) - size_old;
          size_old = std::filesystem::file_size(filename);
          if (size_old >= max_file_size) {
            std::cout << "File size more 5Mb and Exit" << std::endl;
            exit(0);
          }
          while (getline(if_strm, str_buf_in)) {
            std::cout << "Read new message from file: " << str_buf_in << std::endl;
            std::cout << "Size new message from file: " << messages_size << "bytes"<< std::endl;
            if_strm.clear(); //< Now we can read again if_stream, but...
          }
        }
      }
    }
    std::cout << "if_stream Close file \n";
    if_strm.close();
  }
  else
    ///Fixme: open file if did not open
    std::cout << "Unable to open file \n";
}
