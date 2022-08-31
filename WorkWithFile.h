//
// Created by Igor Gusakov on 31.08.2022.
//

#ifndef TESTFILE__WORKWITHFILE_H_
#define TESTFILE__WORKWITHFILE_H_
#include "string"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <chrono>

enum class portig_file {
  NONE,
  READ = 1,
  WRITE,
  READ_WRITE
};


class WorkWithFile {
 public:
  explicit WorkWithFile(std::string name_file = "Test.txt", uint64_t max_file_size_ = 5'000'000) : filename(std::move(name_file)), max_file_size(max_file_size_), size_file(0) { //default name test.txt
  };

  ~WorkWithFile() {
    std::cout << "~WorkWithFile()" << std::endl;
    of_strm.close();
    if_strm.close();
  }
  void OpenFile(portig_file flag) {
//    try {
    switch (flag) {
      case portig_file::READ: {
        if_strm.open(filename, std::ios::out | std::ios::in);
        if(!if_strm.is_open()) {
          //create file
          std::ofstream outfile (filename);
          outfile.close();
          //reopen
          if_strm.open(filename, std::ios::out | std::ios::app);
        }
        break;
      }
      case portig_file::WRITE: {
        of_strm.open(filename, std::ios::out | std::ios::app);
        break;
      }
      case portig_file::READ_WRITE: {
        of_strm.open(filename, std::ios::out | std::ios::app);
        if_strm.open(filename, std::ios_base::in);
        break;
      }
      case portig_file::NONE: {

        break;
      }
    }

      size_file = std::filesystem::file_size(filename);
      std::cout << "Current size in open file  " << size_file << "byte \n";
  }
  void StartHandlerReader();
  void StartHandlerWriter();

 private:
  std::string filename;
  uint64_t max_file_size;
  uint32_t size_file;
  std::string str_buf_out;
  std::string str_buf_in;
  std::ofstream of_strm;
  std::ifstream if_strm;

  //for future(for translate std::filesystem::file_time_type to unix time)
  //now we can not use this
  template <typename TP>
  std::time_t to_time_t(TP tp)
  {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
  }
};

#endif //TESTFILE__WORKWITHFILE_H_
