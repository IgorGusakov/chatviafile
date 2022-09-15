//
// Created by Igor Gusakov on 31.08.2022.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../WorkWithFile.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <future>
#include <utility>

using namespace testing;
using namespace std::chrono_literals;


/*
 * We open the file with a check for the integrity of the file, during the test,
 * an invalid hash is used that is distinguishable from the hash of the text, when opening,
 * there will be a comparison error
 */
TEST(UnitTest, CheckOpenFileWithHashHundler)
{
//Arrange
  //name file
  std::string name_test_file = "unit_test_o_hash.txt";
  std::ofstream test_file(name_test_file,std::ios::out);
  std::string test_text = {"<;hash:5c576ce6;>"
                           "The UNIX core concepts have remained more-or-less the same\n"
                           "since Ritchie and Thompson published their CACM paper. The\n"
                           "process model and the file system model have remained the\n"
                           "same."};
  test_file << test_text;
  test_file.flush();
  test_file.close();
  //max size file
  uint64_t max_size_file = 3'000; //size 3kb
  bool flag_hash = true;
  //create object
  WorkWithFile unit_test(name_test_file,max_size_file,flag_hash,portig_file::READ_WRITE);

//Act
  open_file state_open = unit_test.OpenFile();

//Assert
  //check return oversize status
  EXPECT_EQ(state_open, open_file::OPEN_HASH_ERR);
  //delete file
  std::filesystem::remove(name_test_file);
}

/*
 *Initially, the file is missing, when opening if the file is not present,
 *the thread creates it and opens and checks the opening
 */
TEST(UnitTest, CheckOpenFileHundler)
{
//Arrange
  //name file
  std::string name_test_file = "unit_test_open.txt";
  //max size file
  uint64_t max_size_file = 3'000; //size 3kb
  bool flag_hash = false;
  //create object
  WorkWithFile unit_test(name_test_file,max_size_file,flag_hash,portig_file::READ_WRITE);

//Act
  open_file state_open = unit_test.OpenFile();

//Assert
  //check return oversize status
  EXPECT_EQ(state_open, open_file::OPEN_SUCCESS);
  //delete file
  std::filesystem::remove(name_test_file);
}


/*
 * Emulation of data recording, through emulation of a std:: cin stream,
 * and writing 10 bytes of information in loop, initialization the file size is set to 3kb,
 * after recording, we read the file size and compare it at the output
 *
 */
TEST(UnitTest, CheckSizeWrite3kFile)
{
//Arrange
  std::istringstream iss("unit-test");
  std::cin.rdbuf(iss.rdbuf());  // iss -> cin(mock)
  //name file
  std::string name_test_file = "unit_test_write3k.txt";
  //max size file
  uint64_t max_size_file = 3'000; //size 3kb
  bool flag_hash = false;
  //create object
  WorkWithFile unit_test(name_test_file,max_size_file,flag_hash,portig_file::READ_WRITE);
  unit_test.OpenFile();

//Act
  unit_test.StartHandlerWriter();

//Assert
  //check size file
  EXPECT_EQ(std::filesystem::file_size(name_test_file), 3000);
  //delete file
  std::filesystem::remove(name_test_file);
}


/*
 * Opening a file in which there is already 3kb of information,
 * the logic is that when re-opening the handler should close successfully,
 * since the amount of data is more than initialized
 */
TEST(UnitTest, CheckOpenFileOversize)
{
//Arrange
//name
  std::string name_test_file = "unit_test_oversize.txt";
  std::ofstream test_file(name_test_file,std::ios::out);
  std::string test_text = {"In our modern world, there are many factors that place the wellbeing of the planet in jeopardy. While some people have the opinion that environmental problems are just a natural occurrence, others believe that human beings have a huge impact on the environment. Regardless of your viewpoint, take into consideration the following factors that place our environment as well as the planet Earth in danger.\n"
                           "Global warming or climate change is a major contributing factor to environmental damage. Because of global warming, we have seen an increase in melting ice caps, a rise in sea levels, and the formation of new weather patterns. These weather patterns have caused stronger storms, droughts, and flooding in places that they formerly did not occur.\n"
                           "Air pollution is primarily caused as a result of excessive and unregulated emissions of carbon dioxide into the air. Pollutants mostly emerge from the burning of fossil fuels in addition to chemicals, toxic substances, and improper waste disposal. Air pollutants are absorbed into the atmosphere, and they can cause smog, a combination of smoke and fog, in valleys as well as produce acidic precipitation in areas far away from the pollution source.\n"};
  test_file << test_text;
  test_file.flush();
  test_file.close();
  //max size file
  uint64_t max_size_file = 1000; //size 3kb
  bool flag_hash = false;
  //create object
  WorkWithFile unit_test(name_test_file,max_size_file,flag_hash,portig_file::READ_WRITE);
  unit_test.OpenFile();

//Act
  write_state state = unit_test.StartHandlerWriter();

//Assert
  //check return oversize status
  EXPECT_EQ(state, write_state::STOP_OK_HANDLER);
  //delete file
  std::filesystem::remove(name_test_file);
}



/*
 * This text has 3 hash checks to test 3 methods
 * 1 step - hash_read_from_file() create hash in file
 * 2 step - hash_read_from_file() read hash from file
 * 3 step - check_hash from file read and calc after compare
 */

/*
 * When we open a file that does not have a hash, the first thing we do is add a hash to the file,
 * having previously calculated the hash_test hash for the text test_text
 */
TEST(UnitTest, CheckAddHash)
{
//Arrange
  //name file
  std::string name_test_file = "unit_test_add_hash.txt";
  std::ofstream test_file(name_test_file, std::ios::out);
  std::string test_text = {"The UNIX core concepts have remained more-or-less the same\n"
                           "since Ritchie and Thompson published their CACM paper. The\n"
                           "process model and the file system model have remained the\n"
                           "same."};
  uint32_t hash_test = std::hash<std::string>{}(test_text);// hash test text above
//  uint32_t hash_test = 0xF0746A71; // hash test text above
  test_file << test_text;
  test_file.flush();
  test_file.close();
  //max size file
  uint64_t max_size_file = 3000; //size 100 b
  bool flag_hash = false;
  //create object
  WorkWithFile unit_test(name_test_file,max_size_file,flag_hash,portig_file::READ_WRITE);
  unit_test.OpenFile();

//Act
  //calculate and added hash to file
  uint32_t hash_add_to_file = unit_test.hash_read_from_file();

//Assert
  //check add to file
  EXPECT_EQ(hash_test, hash_add_to_file);
  //delete file
  std::filesystem::remove(name_test_file);
}

/*
 * When we open a file that does not have a hash, the first thing we do is add a hash to the file,
 * having previously calculated the hash_test hash for the text test_text
 */
TEST(UnitTest, CheckReadHash)
{
//Arrange
  //name file
  std::string name_test_file = "unit_test_read_hash.txt";
  std::ofstream test_file(name_test_file,std::ios::out);
  std::string test_text = {"<;hash:f0746a71;>\n"
                           "The UNIX core concepts have remained more-or-less the same\n"
                           "since Ritchie and Thompson published their CACM paper. The\n"
                           "process model and the file system model have remained the\n"
                           "same."};
  uint32_t hash_test = 0xF0746A71; // hash test text above
  test_file << test_text;
  test_file.flush();
  //max size file
  uint64_t max_size_file = 3000; //size 100 b
  bool flag_hash = false;
  //create object
  WorkWithFile unit_test(name_test_file,max_size_file,flag_hash,portig_file::READ_WRITE);
  unit_test.OpenFile();

//Act
//  read from file
  uint32_t hash_read_to_file = unit_test.hash_read_from_file();

//Assert
  //check read hash
  EXPECT_EQ(hash_test, hash_read_to_file);
  test_file.close();
  //delete file
  std::filesystem::remove(name_test_file);
}
/*
 * After we check private method WorkWithFile::check_hash()
 */
TEST(UnitTest, CheckHash)
{
//Arrange
  //name file
  std::string name_test_file = "unit_test_check_hash.txt";
  std::ofstream test_file(name_test_file,std::ios::out);
  std::string test_text_ = {"The UNIX core concepts have remained more-or-less the same\n"
                            "since Ritchie and Thompson published their CACM paper. The\n"
                            "process model and the file system model have remained the\n"
                            "same."};
  std::string test_text;
  uint32_t hash = std::hash<std::string>{}(test_text_);

  std::stringstream hash_str;
  hash_str <<"<;hash:" <<std::setfill ('0')<<std::setw(8)<<std::hex<< hash <<";>\n";
  test_text.insert(0,hash_str.str(),0,18);
  test_text.append(test_text_);
  test_file << test_text;
  test_file.flush();
  //max size file
  uint64_t max_size_file = 3000; //size 3000 b
  bool flag_hash = false;
  //create object
  WorkWithFile unit_test(name_test_file,max_size_file,flag_hash,portig_file::READ_WRITE);
  unit_test.OpenFile();

//Act
  //read from file
  bool hash_check = unit_test.check_hash();

//Assert
  EXPECT_EQ(true,hash_check);
  test_file.close();
  //delete file
  std::filesystem::remove(name_test_file);
}



/*
 * Integration test(we can use it on your laptop)
 * The input istringstream emulates the text cin
 * Output of WorkWithFile::get_input_buf() last line read from buffer
 * For readability, the file size is set to 500 bytes,
 * this will allow you to write the test string n times
 * At the end, the output is compared with the test input.
 */
//TEST(IntegrationTest, CheckSomeWriteReadFile)
//{
////Arrange
//  std::istringstream iss("Memory Management: Possible increase");
//  std::cin.rdbuf(iss.rdbuf());  // iss -> cin(mock)
//
//  std::string name_test_file = "unit_test_rw_file.txt";
//  static std::mutex mux_r;
//  static std::mutex mux_w;
//  mux_w.lock();
//  //max size file
//  uint64_t max_size_file = 3000; //size 3000 b
//  bool flag_hash = false;
//  std::string str;
//  std::thread in_file([&] {
//    mux_r.lock();
//    WorkWithFile unit_test_wr(name_test_file, max_size_file, flag_hash);
//    unit_test_wr.OpenFile();
//    mux_w.unlock();
//    unit_test_wr.StartHandlerReader();
//    str = unit_test_wr.get_input_buf();
//    mux_r.unlock();
//  });
//  in_file.detach();
////Act
//  std::cout<<"Start Write Handler"<<"\n";
//  std::scoped_lock lock_w(mux_w);
////  std::this_thread::sleep_for(100ms);
//  WorkWithFile unit_test(name_test_file, max_size_file, flag_hash);
//  unit_test.OpenFile();
//  unit_test.StartHandlerWriter();
//  std::scoped_lock lock_r(mux_r);
//
////Assert
//  EXPECT_EQ(str,iss.str());
//  //delete file
//  std::filesystem::remove(name_test_file);
//}
