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


/**
 * @brief enum class to define file operation
 */
enum class portig_file {
  NONE,
  READ,
  WRITE,
  READ_WRITE
};

/**
 * @brief enum class to state about open file
 */
enum class open_file {
  OPEN_ERR,
  OPEN_SUCCESS,
  HASH_ERR,
  HASH_OK
};


/**
 * @name WorkWithFile
 *
 * @brief superclass for work with a file that meets the terms of reference
 */
class WorkWithFile {
 public:

 /**
 * Create a new WorkWithFile object with param.
 * @brief Constructor WorkWithFile
 * @param name_file set name file @default std::string "Test.txt"
 * @param max_file_size_ set max file @default uint64_t 5'000'000 bytes
 * @param hash on/off hash to file @default boll false
 */
  explicit WorkWithFile(std::string name_file = "Test.txt", uint64_t max_file_size_ = 5'000'000 , bool hash = false) :
  filename(std::move(name_file)), max_file_size(max_file_size_), hash_option(hash), size_file(0) {

  };

  ~WorkWithFile() {
    of_strm.close();
  }

  open_file OpenFile();
  void StartHandlerReader();
  void StartHandlerWriter();

 private:
  uint32_t hash_calc(bool write_to_file, bool detected_hash);
  uint32_t hash_read_from_file();
  bool check_hash();

  std::string filename; //!< file name
  uint64_t max_file_size; //!< max file size file
  bool hash_option; //!< flag on/off hash
  uint32_t size_file; //!< current size file
  std::string str_buf_out; //!< buffer write file
  std::string str_buf_in; //!< buffer read file
  std::ofstream of_strm; //!< Stream write file



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
