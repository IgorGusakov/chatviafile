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
#include <memory>
#include <experimental/propagate_const>
#include "Poco/DirectoryWatcher.h"
#include "Poco/Delegate.h"


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
  OPEN_HASH_ERR,
  OPEN_HASH_OK
};

/**
 * @brief enum class to state about open file
 */
enum class write_state {
  START_HANDLER,
  STOP_OK_HANDLER,
  STOP_ERR_HANDLER
};



/**
 * @name WorkWithFile
 *
 * @brief Pointer implementation superclass for work with a file that meets the terms of reference
 */
class WorkWithFile {

  class impl;
  std::experimental::propagate_const<std::unique_ptr<impl>> pImpl;
 public:

  WorkWithFile(); //default constr
/**
 * Create a new WorkWithFile object with param.
 * @brief Constructor WorkWithFile
 * @param name_file set name file @default std::string "Test.txt"
 * @param max_file_size_ set max file @default uint64_t 5'000'000 bytes
 * @param hash on/off hash to file @default boll false
 */
  explicit WorkWithFile(const std::string& name_file, uint64_t max_file_size_ , bool hash);
  ~WorkWithFile(); // default des
  [[maybe_unused]] WorkWithFile(WorkWithFile&&) noexcept ; // move
  WorkWithFile(const WorkWithFile&) = delete; // copy
  WorkWithFile& operator=(WorkWithFile&&) noexcept ; //move =
  WorkWithFile& operator=(const WorkWithFile&) = delete; //copy =

  open_file OpenFile();
  write_state StartHandlerWriter();
  uint32_t hash_read_from_file();
  bool check_hash();
  std::string get_input_buf();


};

#endif //TESTFILE__WORKWITHFILE_H_
