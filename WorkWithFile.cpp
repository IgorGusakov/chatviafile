//
// Created by Igor Gusakov on 31.08.2022.
//

#include "WorkWithFile.h"
#include <deque>


/**
 * @brief Determine if the three lengths provided could form a valid Triangle object.
 */
open_file WorkWithFile::OpenFile()
{
  open_file status = open_file::OPEN_ERR;
  //open write stream and create file if did not create
  of_strm.open(filename, std::ios::out | std::ios::app);
  if(of_strm.is_open()) {
    status = open_file::OPEN_SUCCESS;
  }
  else {
    status = open_file::OPEN_ERR;
  }
  if(hash_option) {
    //read hash if hash is not present in file so create
    std::cout << "Hash is working..."<<'\n';
    uint32_t hash_file = hash_read_from_file();
    std::cout << "Current hash : "<<std::hex<<hash_file<<std::dec<<'\n';
  }

  size_file = std::filesystem::file_size(filename); // read file size
  std::cout << "Current size in open file  " <<std::dec << size_file << "byte \n";

  return status;
}



/**
 * @brief Handler for writing to a file, if the -crc file is disabled,
 * then the data is written line by line to the end, if the crc flag is enabled,
 * then the hash is calculated along with the input data from the stream and written at
 * the beginning of the file, thereby overwriting the entire file
 * The handler does not load the processor, as it expects from the stream through getline,
 * the output for the handler will be the threshold file size
 * that can be set through the object constructor,
 * you can also set the file name through the constructor and hash calculation control
 */
void WorkWithFile::StartHandlerWriter() {
  if(of_strm.is_open()) { //double check open file
    //loop while size file < value max file size
    while (size_file <= max_file_size) {
      //read line from cin >>
      getline(std::cin >> std::ws, str_buf_out);

      str_buf_out.push_back('\n'); // if you want every message is starting from new line

      //check on.off work with hash
      if(hash_option) {
        uint32_t hash = hash_calc(true,true);
        std::cout<<"hash file changed : " <<std::hex << hash <<"\n";
      }
      else { //just write to the end file
        //todo: will add check update file
        of_strm << str_buf_out;
        of_strm.flush();
      }

      std::cout << "Size messages: " <<std::dec<< str_buf_out.length() << "byte \n";
      //sum size_file to maintain file size
      size_file += str_buf_out.length();
//      str_buf_out.clear();
      std::cout << "Size file: " <<std::dec<< size_file << "byte \n";
    }
    //if size file > 5Mb close handler
    std::cout << "File size more 5Mb and Exit" << std::endl;
  }
  else
    std::cout << "Unable to open of_strm \n";
}


/**
 * @brief Handler for reading the file when its size is changed.
 * If the new read size differs from the previous one, then a new stream
 * is opened with the new data and reads the latest data.
 * It also has a flag for calculating the hash, when the flag is enabled,
 * after changing the file size,
 * the data hash is checked and success or error hash is output to the console.


 * This method requires a refactoring for the following reasons
 * @warning 1. infinite loop checking file size is CPU intensive,
 * need file change callback to silently report it
 * @warning 2. if_strm.clear() should update the stream this allowed
 * std::filesystem::file_size to use if_strm.tellg()
 * @warning 3. perhaps synchronization via sockets or shared mem is needed to work with hash,
 * since overwriting a large file may take longer and an intermediate file change during writing
 * may give a false file change
 */
void WorkWithFile::StartHandlerReader() {
  std::ifstream if_strm(filename, std::ios_base::in);
  if (if_strm.is_open())
  {
    ///todo: it will provide to detected was changed file and create callback in future, maybe ->
//    uint32_t size = std::filesystem::file_size(filename);
//    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(filename);
//    std::time_t cftime = to_time_t(ftime);
//    std::cout<<"change file:"<< cftime;



    auto size_old = std::filesystem::file_size(filename);

    ///Fixme: this While(true) is overload work processor
    while (true) {
      if (size_old != std::filesystem::file_size(filename)) {
        if_strm.close();
        if(hash_option)
          check_hash();
        {
          ///todo: did not work if_strm.clear();???
          if_strm.open(filename);
          if_strm.seekg(static_cast<long long>(size_old));
          uint32_t messages_size = std::filesystem::file_size(filename) - size_old;
          size_old = std::filesystem::file_size(filename);
          if (size_old >= max_file_size) {
            std::cout << "File size more 5Mb and Exit" << std::endl;
            break;
          }
          while (getline(if_strm, str_buf_in)) {
//            hash_calc(false,true);
            std::cout << "\nRead new message from file: " << str_buf_in << std::endl;
            std::cout << "Size new message from file: " << messages_size << "bytes"<< std::endl;
//            if_strm.clear(); //< Now we can read again if_stream, but...
          }
        }
      }
    }
  }
  else
    std::cout << "Unable to open if_strm \n";
}


/**
 * @brief private method for calculating the hash of a file,
 * the method removes the current hash from the read file and then
 * calculates its own hash from the input data of the file. The method has two options
 * @param write_to_file bool after calculation overwrite file with new hash
 * true - calc hash and rewrite all data with new hash,
 * false - just calc hash and return
 * @param detected_hash bool an option that helps to calculate the correct run in case
 * you opened it in a file that did not use -crc.
 * true - file did not use calc hash and you need add hash label,
 * false - hash label has in file
 * @return uint32_t hash.
 *
 * @warning after the test, it was noticed that when writing quickly,
 * the queue from the stream produces empty data and the calculated hash is not valid
 */
uint32_t WorkWithFile::hash_calc(bool write_to_file, bool detected_hash) {
  std::ifstream readfile(filename);
  std::string str_buf; //buffer read data if_stream
  //to quickly copy input from a stream, since copying string to string takes more time than emplace back
  std::deque<std::string> queue_;
  std::string str_file;//object copy file for rewrite new hash
  uint32_t hash{0};
  readfile.seekg(0);// pos begin file
  //read file
  if(readfile.is_open()) {
    while (getline(readfile, str_buf)) {
      str_buf.push_back('\n');
      //str to fast emplace_back
      queue_.emplace_back(str_buf);
    }
    //copy deque to str
    for(const std::string& str : queue_) {
      str_file.append(str);
    }
  }
  //if hash is has
  if(detected_hash) {
    str_file.erase(0, 18); //delete hash from begin
  }

  //rewrite new hash with all information to file
  if(write_to_file) {
    //added data from cin>>
    if(str_buf_out.length()) {
      str_file.append(str_buf_out);
    }
    //calc hash
    hash = std::hash<std::string>{}(str_file);
    //transformation hash to code <;hash:00000000;>
    std::stringstream hash_str;
    hash_str <<"<;hash:" <<std::setfill ('0')<<std::setw(8)<<std::hex<< hash <<";>\n";
    str_file.insert(0,hash_str.str(),0,18);

    //erase all file and rewrite buffer to file
    std::ofstream writefile(filename, std::ofstream::out | std::ofstream::trunc);
    writefile<<str_file;
    writefile.flush();
    writefile.close();
  }
    //Only calc hash
  else {
//    std::cout<<"Only calc hash2 "<< str_file<<"\n";
    hash = std::hash<std::string>{}(str_file);
  }

  readfile.close();
  return hash;
}


/**
 * @brief return read hash from file or if hash did not detect will calc and add hash
 * @return uint32_t hash from file or new calc hash
 *
 */
uint32_t WorkWithFile::hash_read_from_file() {
  std::ifstream readfile(filename);
  std::string str_buf;
  uint32_t hash{0};
  readfile.seekg(0); //lock for from start file
  while (getline(readfile, str_buf,'>')) { //lock for > from <;hash:20982937;>
//    std::cout<<"str_buf "<<str_buf<<"\n";
    size_t symbol = str_buf.find("<;hash:");//lock for hash mark
    if(!symbol) {
      symbol += 7; //--> <;hash:
//      std::cout<<"str_buf.substr(symbol, 8) :  "<<str_buf.substr(symbol, 8)<<"\n";
      hash  = std::stol(str_buf.substr(symbol, 8), nullptr, 16);
      break;
    }
    else {
      //if we did not detected hash we created in beginner file
      hash = hash_calc(true,false);
    }
  };
  readfile.close();
  return hash;
}


/**
 * @brief checks hash read from file and hash calculated from data in file
 * @return bool,
 * true - both hashes matches
 * false - both hashes does not match
 */
bool WorkWithFile::check_hash() {
  //read hash data
  uint32_t hash_calc_ = hash_calc(false,true);
  //read hash from file
  uint32_t hash_read = hash_read_from_file();
  std::cout <<std::hex<< hash_calc_<< " : "<<hash_read<<std::dec<<"\n";
  //compare
  if(hash_calc_ == hash_read) {
//    std::cout <<std::hex<< "Hash Read Successfully"<<std::dec<<"\n";
    return true;
  }
  else {
//    std::cout <<std::hex<< "Hash Read Error"<<std::dec<<"\n";
    return false;
  }
}
