//
// Created by Igor Gusakov on 31.08.2022.
//

#ifndef TESTFILE__WORKWITHFILE_H_
#define TESTFILE__WORKWITHFILE_H_
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <chrono>
#include <sstream>

enum class portig_file {
  NONE,
  READ = 1,
  WRITE,
  READ_WRITE
};


class WorkWithFile {
 public:
  explicit WorkWithFile(std::string name_file = "Test.txt", uint64_t max_file_size_ = 5'000'000 , bool hash = false) :
  filename(std::move(name_file)), max_file_size(max_file_size_), hash_option(hash), size_file(0) { };

  ~WorkWithFile() {
    of_strm.close();
  }
  void OpenFile(portig_file flag);
  void StartHandlerReader();
  void StartHandlerWriter();

 private:
  uint32_t hash_calc(bool write_to_file, bool detected_hash);
  uint32_t hash_read_from_file();
  bool check_hash();

  std::string filename;
  uint64_t max_file_size;
  bool hash_option;
  uint32_t size_file;
  std::string str_buf_out;
  std::string str_buf_in;
  std::ofstream of_strm;
//  std::ifstream if_strm;



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
