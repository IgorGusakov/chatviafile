//
// Created by Igor Gusakov on 31.08.2022.
//

#include "WorkWithFile.h"
#include <deque>
#include <utility>

class WorkWithFile::impl
{
  std::string filename; // file name
  uint64_t max_file_size; // max file size file
  bool hash_option; // flag on/off hash
  uint32_t size_file; // current size file
  std::string str_buf_out; // buffer write file
  std::string str_buf_in; // buffer read file
  std::string str_last_message; // last input message
  std::ofstream of_strm; // Stream write file


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
 */
  uint32_t hash_calc(bool write_to_file, bool detected_hash)
  {
    std::ifstream readfile(filename, std::ios::in);
    std::string str_file;//object copy file for rewrite new hash
    std::stringstream buffer_from_file;
    uint32_t hash{0};
    readfile.seekg(0);// pos begin file
    //read file
    buffer_from_file << readfile.rdbuf();
    str_file = buffer_from_file.str();
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
      hash = std::hash<std::string>{}(str_file);
    }

    readfile.close();
    return hash;
  }

 public:

  /**
  * @brief Method for opening a file into a stream, checks if the file is open,
  * if the file is not found, the stream will create a new one,
  * the Hash is also read if it is included in the options.
  */
  open_file OpenFile(WorkWithFile& w)
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
      bool hash_check = w.check_hash();
      if(hash_check)
        status = open_file::OPEN_HASH_OK;
      else
        status = open_file::OPEN_HASH_ERR;
    }

    size_file = std::filesystem::file_size(filename); // read file size
    std::cout << "Current size in open file  " <<std::dec << size_file << " byte \n";

    return status;
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
  * need file change callback to silently report it, perhaps synchronization via sockets or shared mem
  * @warning 2. if_strm.clear() should update the stream this allowed
  * std::filesystem::file_size to use if_strm.tellg()
  */
  void StartHandlerReader(WorkWithFile& w)
  {
    std::ifstream if_strm(filename, std::ios_base::in);
    if (if_strm.is_open())
    {
      ///todo: it will provide to detected was changed file and create callback in future, maybe ->
//    uint32_t size = std::filesystem::file_size(filename);
//    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(filename);
//    std::time_t cftime = to_time_t(ftime);
//    std::cout<<"change file:"<< cftime;
      std::cout<<"filename : " <<filename;
      auto size_old = std::filesystem::file_size(filename);
      ///Fixme: this While(true) is overload work processor
      while (true) {

        auto size_current = std::filesystem::file_size(filename);
        if (size_old != size_current && size_current > 0 || size_old >= max_file_size) { //when we full erase file size_current > 0
          if_strm.close();
          if(hash_option)
            w.check_hash();
          {
            ///todo: did not work if_strm.clear();???
            if_strm.open(filename);
            if_strm.seekg(static_cast<long long>(size_old));
            uint32_t messages_size = size_current - size_old;
            size_old = size_current;
            if (size_old >= max_file_size) {
              std::cout << "File size more "<<max_file_size<<" byte and Exit" << std::endl;
              break;
            }
            while (getline(if_strm, str_buf_in)) {
              str_last_message.clear();
              str_last_message.append(str_buf_in);
              std::cout << "\nRead new message from file: " << str_last_message << std::endl;
              std::cout << "Size read new message from file: " << messages_size << " bytes"<< std::endl;
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
  * @brief Handler for writing to a file, if the -crc file is disabled,
  * then the data is written line by line to the end, if the crc flag is enabled,
  * then the hash is calculated along with the input data from the stream and written at
  * the beginning of the file, thereby overwriting the entire file
  * The handler does not load the processor, as it expects from the stream through getline,
  * the output for the handler will be the threshold file size
  * that can be set through the object constructor,
  * you can also set the file name through the constructor and hash calculation control
  */
  write_state StartHandlerWriter(WorkWithFile& w)
  {
    write_state state = write_state::START_HANDLER;
    if(of_strm.is_open()) { //double check open file
      //loop while size file < value max file size
      while (size_file < max_file_size) {
        //read line from cin >>
        getline(std::cin >> std::ws, str_buf_out);

        //check on.off work with hash
        if(hash_option) {
          uint32_t hash = hash_calc(true,true);
          std::cout<<"hash write file changed : " <<std::hex << hash <<"\n";
        }
        else { //just write to the end file
          //todo: will add check update file
          of_strm << str_buf_out << "\n";
          of_strm.flush();
        }
        //between size message
        uint64_t between_s = str_buf_out.length();
        ++between_s; //add \n
        //sum size_file to maintain file size
        size_file += between_s;
        std::cout << "Size write messages: " <<std::dec<< between_s <<  " byte \n";
        std::cout << "Size file: " <<std::dec<< size_file << " byte \n";
      }
      //if size file > 5Mb close handler
      std::cout << "File size more "<<max_file_size<<" byte and Exit" << std::endl;
      state = write_state::STOP_OK_HANDLER;
      return state;
    }
    else {
      std::cout << "Unable to open of_strm \n";
      state = write_state::STOP_ERR_HANDLER;
      return state;
    }
  }


  /**
  * @brief return read hash from file or if hash did not detect will calc and add hash
  * @return uint32_t hash from file or new calc hash
  */
  uint32_t hash_read_from_file(WorkWithFile& w)
  {
    std::ifstream readfile(filename, std::ios::in);
    std::string str_buf;
    uint32_t hash{0};
    readfile.seekg(0); //lock for from start file
    readfile >> str_buf; //read first line <;hash:20982937;>
    size_t symbol = str_buf.find("<;hash:");//lock for hash mark
    if(!symbol) {
      symbol += 7; //--> <;hash:
      hash  = std::stol(str_buf.substr(symbol, 8), nullptr, 16);
    }
    else {
      //if we did not detected hash we created in beginner file
      hash = hash_calc(true,false);
    }
    readfile.close();
    return hash;
  }



/**
 * @brief checks hash read from file and hash calculated from data in file
 * @return bool,
 * true - both hashes matches
 * false - both hashes does not match
 */
  bool check_hash(WorkWithFile& w)
  {
    std::cout <<"\nCheck READ HASH"<<"\n";
    //read hash from file
    uint32_t hash_read = w.hash_read_from_file();
    //read hash data
    uint32_t hash_calc_ = hash_calc(false,true);
    std::cout <<std::hex<< hash_calc_<< " : "<<hash_read<<std::dec;
    //compare
    if(hash_calc_ == hash_read) {
      std::cout <<" Hash Read Successfully"<<"\n";
      return true;
    }
    else {
      std::cout <<" Hash Read Error"<<"\n";
      return false;
    }
  }

  std::string get_input_buf() {
    return str_last_message;
  }

  impl(std::string name_file, uint64_t max_file_size_ , bool hash) :
      filename(std::move(name_file)), max_file_size(max_file_size_), hash_option(hash), size_file(0) {}
};


open_file WorkWithFile::OpenFile() { return pImpl->OpenFile(*this); }
void WorkWithFile::StartHandlerReader() { pImpl->StartHandlerReader(*this); }
write_state WorkWithFile::StartHandlerWriter() { return pImpl->StartHandlerWriter(*this); }
uint32_t WorkWithFile::hash_read_from_file() {return pImpl->hash_read_from_file(*this);}
bool WorkWithFile::check_hash() {return pImpl->check_hash(*this);}

WorkWithFile::WorkWithFile() = default;
WorkWithFile::WorkWithFile(const std::string& name_file, uint64_t max_file_size_ , bool hash) :
    pImpl{std::make_unique<impl>(name_file,max_file_size_,hash)} {}
std::string WorkWithFile::get_input_buf() { return pImpl->get_input_buf();};
[[maybe_unused]] WorkWithFile::WorkWithFile(WorkWithFile&&) noexcept = default;
WorkWithFile::~WorkWithFile() = default;
WorkWithFile& WorkWithFile::operator=(WorkWithFile&&) noexcept = default;
