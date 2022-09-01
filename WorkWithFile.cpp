//
// Created by Igor Gusakov on 31.08.2022.
//

#include "WorkWithFile.h"
#include <deque>

void WorkWithFile::OpenFile(portig_file flag) {
      of_strm.open(filename, std::ios::out | std::ios::app);
//      if_strm.open(filename, std::ios_base::in);

  if(hash_option) {
    //read hash if hash is not present in file so create
    std::cout << "Hash is working..."<<'\n';
    uint32_t hash_file = hash_read_from_file();
    std::cout << "Current hash : "<<std::hex<<hash_file<<std::dec<<'\n';
  }

  size_file = std::filesystem::file_size(filename);
  std::cout << "Current size in open file  " <<std::dec << size_file << "byte \n";
}




void WorkWithFile::StartHandlerWriter() {
  if(of_strm.is_open()) {
    while (size_file <= max_file_size) {
      getline(std::cin >> std::ws, str_buf_out);

      str_buf_out.push_back('\n'); // if you want every message is starting from new line

      if(hash_option) {
        uint32_t hash = hash_calc(true,true);
        std::cout<<"hash file changed : " <<std::hex << hash <<"\n";
      }
      else {
        //todo: will add check update file
        of_strm << str_buf_out;
        of_strm.flush();
      }

      std::cout << "Size messages: " <<std::dec<< str_buf_out.length() << "byte \n";
      size_file += str_buf_out.length();
      str_buf_out.clear();
      std::cout << "Size file: " <<std::dec<< size_file << "byte \n";
    }
    std::cout << "File size more 5Mb and Exit" << std::endl;
  }
  else
    std::cout << "Unable to open of_strm \n";
}



void WorkWithFile::StartHandlerReader() {
  std::ifstream if_strm(filename, std::ios_base::in);
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



uint32_t WorkWithFile::hash_calc(bool write_to_file, bool detected_hash) {
  std::ifstream readfile(filename);
  std::string str_buf; //buffer read data if_stream
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



bool WorkWithFile::check_hash() {
  //read hash data
  uint32_t hash_calc_ = hash_calc(false,true);
  //read hash from file
  uint32_t hash_read = hash_read_from_file();
  std::cout <<std::hex<< hash_calc_<< " : "<<hash_read<<std::dec<<"\n";
  //compare
  if(hash_calc_ == hash_read) {
    std::cout <<std::hex<< "Hash Read Successfully"<<std::dec<<"\n";
    return true;
  }
  else {
    std::cout <<std::hex<< "Hash Read Error"<<std::dec<<"\n";
    return false;
  }
}
