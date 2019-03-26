#include "filesystem_data.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

filesystem_data mg_filesystem_data;

filesystem_data::filesystem_data()
{
   simple_file mg_file_1;

   mg_file_1._path = "/mg_file_1";
   mg_file_1._name = "mg_file_1";
   mg_file_1._mode = S_IFREG | 0777;
   mg_file_1._content = "this is the content";

   _the_files.push_back(mg_file_1);

   simple_directory root_dir;

   root_dir._directory_file._path = "/";
   root_dir._directory_file._name = "/";
   root_dir._directory_file._mode = S_IFDIR | 0777;
   root_dir._directory_file._content = "./\n..\nmg_file_1\n";

   root_dir._contained_files.push_back( &_the_files[0] );

   _the_directories.push_back(root_dir);

   simple_directory mg_dir_1;

   mg_dir_1._directory_file._path = "/mg_dir_1";
   mg_dir_1._directory_file._name = "mg_dir_1";
   mg_dir_1._directory_file._mode = S_IFDIR | 0777;
   mg_dir_1._directory_file._content = "./\n..\nmg_file_1\n";

   _the_directories.push_back(mg_dir_1);

   root_dir._contained_directories.push_back( &mg_dir_1);
}


void filesystem_data::list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler)
{
   for(int i=0; i < _the_files.size(); ++i)
   {
      filler(buffer, _the_files[i]._name.c_str(), NULL, 0 );
   }
}


char* filesystem_data::read_file_content(const char *path)
{
   printf("calling filesystem_data::read_file_content ********************************** \n");

   int index = this->get_index_for_filename(path);

   char* the_content = new char[_the_files[index]._content.length() + 1];
           
   strcpy(the_content, _the_files[index]._content.c_str());

   return the_content;
}


void filesystem_data::get_attributes(const char* path, struct stat* st)
{
   int index = this->get_index_for_filename(path);

   st->st_mode = _the_files[index]._mode;
}


bool filesystem_data::file_exists(const char* path)
{
   int index = this->get_index_for_filename(path);
   
   printf("determining if file with path %s exists **************************\n", path);
   printf("answer : %s ************************\n", index > 0 ? "true" : "false" );

   return (index >= 0 ? true : false);
}


void filesystem_data::set_file_mode(const char* path, mode_t new_mode)
{
   int index = this->get_index_for_filename(path);

   _the_files[index]._mode = new_mode;
}


void filesystem_data::write_file_content(const char* path, const char* new_data, size_t size, off_t offset)
{
   int index = this->get_index_for_filename(path);

   std::string full_input = std::string(new_data);
   std::string string_for_writing = full_input.substr (0, size);

   _the_files[index]._content.insert(offset, string_for_writing); // FIXME : is this insertion what is supposed to be done ?
}


void filesystem_data::create_directory(const char* path, mode_t new_mode)
{

    simple_directory new_dir;

    new_dir._directory_file._path = std::string(path);

    _the_directories.push_back(new_dir);
}


void filesystem_data::rename_file(const char* path, const char* new_path)
{
   int index = this->get_index_for_filename(path);

   _the_files[index]._path = std::string(new_path);

   std::string new_name = std::string(new_path).erase(0 , 1); // remove beginning "/"

   _the_files[index]._name = new_name;
}


void filesystem_data::remove_file(const char* path)
{
   int index = this->get_index_for_filename(path);

   this->debug();

   if(index >= 0)
   {
      _the_files.erase(_the_files.begin() + index);
   }

   printf("AFTER REMOVAL =========================================\n");

   this->debug();

}


int filesystem_data::get_index_for_filename(const char* path)
{
   for(int i=0; i < _the_files.size(); ++i)
   {
       printf("comparing %s to %s \n", path, _the_files[i]._path.c_str() );
       if(strcmp( path, _the_files[i]._path.c_str() ) == 0)
       {
           printf("the are equal !!!!\n");
           return i;
       }
   }

   return -1; // FIXME : error handling
}


void filesystem_data::debug()
{
   printf("\n\n===============  MG_FILESYSTEM DEBUG START =================================\n\n");

   printf("files in filesystem_data : \n\n");
   for(int i=0; i < _the_files.size(); ++i)
   {
       printf("path : %s ; name : %s ; content : %s \n", _the_files[i]._path.c_str(), _the_files[i]._name.c_str(), 
                                                         _the_files[i]._content.c_str());
   }


   printf("\n\n===============  MG_FILESYSTEM DEBUG END   =================================\n\n");
}
