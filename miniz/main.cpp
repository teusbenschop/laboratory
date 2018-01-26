#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include <string.h>
#include "miniz.h"


using namespace std;


string filter_archive_unzip_miniz_internal (string zipfile)
{
  string folder = "/tmp";

  bool error = false;
  mz_bool status;
  mz_zip_archive zip_archive;
  memset (&zip_archive, 0, sizeof (zip_archive));
  status = mz_zip_reader_init_file(&zip_archive, zipfile.c_str(), 0);
  if (status) {

    int filecount = mz_zip_reader_get_num_files (&zip_archive);
    for (int i = 0; i < filecount; i++) {
      if (error) continue;
      mz_zip_archive_file_stat file_stat;
      status = mz_zip_reader_file_stat (&zip_archive, i, &file_stat);
      if (status) {
        
        string filename = file_stat.m_filename;
        if (mz_zip_reader_is_file_a_directory (&zip_archive, i)) {
          
        } else {
          
          size_t filesize = file_stat.m_uncomp_size;
          cout << filename << " " << filesize << endl;
          void * buff = operator new (filesize);
          if (buff) {

            status = mz_zip_reader_extract_to_mem (&zip_archive, i, buff, filesize, 0);
            if (status) {

              string contents (static_cast<const char*>(buff), filesize);
              cout << "contents size " << contents.size () << endl;
              
            } else {
              cout << "mz_zip_reader_extract_to_mem failure for " << filename << " in " << zipfile << endl;
              error = true;
            }
            
            operator delete (buff);
          
          } else {
            cout << "failure to allocate buffer for file extraction" << endl;
            error = true;
          }
          
        }
        
      } else {
        cout << "mz_zip_reader_file_stat failed for " << zipfile << endl;
        error = true;
      }
      
    }
    
    mz_zip_reader_end (&zip_archive);
    
  } else {
    cout << "mz_zip_reader_init_file failed for " << zipfile << endl;
    error = true;
  }

  if (error) folder.clear ();
  
  return folder;
}


int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  filter_archive_unzip_miniz_internal ("template.odt");
  return EXIT_SUCCESS;
}
