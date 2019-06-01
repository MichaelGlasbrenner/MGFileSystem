#include "filesystem_data.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

filesystem_data mg_filesystem_data;

filesystem_data::filesystem_data()
{
   simple_file mg_file_1;

   mg_file_1._path = "/mg_file_1";
   mg_file_1._name = "mg_file_1";
   mg_file_1._mode = S_IFREG | 0777;
   mg_file_1._content = "this is the content";
   mg_file_1._last_access_time = time(NULL);
   mg_file_1._last_modification_time = time(NULL);
   mg_file_1._user = getuid();
   mg_file_1._group = getgid();

   _the_files.push_back(mg_file_1);

   simple_directory root_dir;

   root_dir._directory_file._path = "/";
   root_dir._directory_file._name = "/";
   root_dir._directory_file._mode = S_IFDIR | 0777;
   root_dir._directory_file._content = "./\n..\nmg_file_1\n";
   root_dir._directory_file._last_access_time = time(NULL);
   root_dir._directory_file._last_modification_time = time(NULL);
   root_dir._directory_file._user = getuid();
   root_dir._directory_file._group = getgid();

   root_dir._contained_files.push_back( &_the_files[0] );

   simple_directory* mg_dir_1 = new simple_directory;

   mg_dir_1->_directory_file._path = "/mg_dir_1";
   mg_dir_1->_directory_file._name = "mg_dir_1";
   mg_dir_1->_directory_file._mode = S_IFDIR | 0777;
   mg_dir_1->_directory_file._content = "./\n..\nmg_file_1\n";

   _the_directories.push_back( *mg_dir_1 );

   root_dir._contained_directories.push_back( mg_dir_1 );

   _the_directories.push_back(root_dir);
}


void filesystem_data::list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler)
{
   int directory_index = this->get_index_for_dirname(path);

   this->debug();

   for(int i=0; i < _the_directories.at(directory_index)._contained_directories.size(); ++i)
   {
      printf("listing contained directory : %s \n", _the_directories.at(directory_index)._contained_directories.at(i)->_directory_file._name.c_str());
      filler(buffer, _the_directories.at(directory_index)._contained_directories.at(i)->_directory_file._name.c_str(), NULL, 0 );
   }
   
   for(int i=0; i < _the_files.size(); ++i)
   {
      filler(buffer, _the_files[i]._name.c_str(), NULL, 0 );
   }
}


char* filesystem_data::read_file_content(const char *path)
{
   printf("calling filesystem_data::read_file_content ********************************** \n");

   int index = this->get_index_for_filename(path);

   assert(index >= 0);

   char* the_content = new char[_the_files[index]._content.length()];
           
   strcpy(the_content, _the_files[index]._content.c_str());

   return the_content;
}


void filesystem_data::get_attributes(const char* path, struct stat* st)
{
   simple_file* fileptr = this->get_fileptr_for_filename(path);

   if(fileptr == NULL) return;

   st->st_mode = fileptr->_mode;
   st->st_nlink = 1; // FIXME
   st->st_size = fileptr->_content.length();
   
   st->st_uid = fileptr->_user; 
   st->st_gid = fileptr->_group; 
   st->st_atime = fileptr->_last_access_time;
   st->st_mtime = fileptr->_last_modification_time;
}


void filesystem_data::set_access_and_modification_times(const char* path, const struct timespec tv[2])
{
   int index = this->get_index_for_filename(path);

   assert(index >= 0);

   _the_files[index]._last_access_time = tv[0].tv_sec;
   _the_files[index]._last_modification_time =  tv[1].tv_sec;
}


void filesystem_data::change_ownership(const char* path, uid_t new_user, gid_t new_group)
{
   int index = this->get_index_for_filename(path);

   assert(index >= 0);

   _the_files[index]._user = new_user;
   _the_files[index]._group = new_group;
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

   assert(index >= 0);

   _the_files[index]._mode = new_mode;
}


void filesystem_data::write_file_content(const char* path, const char* new_data, size_t size, off_t offset)
{
   printf("calling filesystem_data::write_file_content with data: \n\n %s \n\n", new_data);
   fflush(stdout);
   int index = this->get_index_for_filename(path);

   assert(index >= 0);

   std::string full_input = std::string(new_data);
   std::string string_for_writing = full_input.substr (0, size);

   if(offset > _the_files[index]._content.size())
   {
      printf("inserting content with resizing\n");
      _the_files[index]._content.resize(offset);
      _the_files[index]._content.insert(offset, string_for_writing); // FIXME : is this insertion what is supposed to be done ?

   }
   else
   {
      printf("inserting content without resizing\n");
      _the_files[index]._content.insert(offset, string_for_writing); // FIXME : is this insertion what is supposed to be done ?
   }
}


void filesystem_data::truncate_file(const char* path, off_t new_length)
{
   printf("calling filesystem_data::truncate_file with new_length: \n\n %d \n\n", (int)new_length);
   int index = this->get_index_for_filename(path);

   assert(index >= 0);

   _the_files[index]._content = _the_files[index]._content.substr(0, new_length);
}


void filesystem_data::create_directory(const char* path, mode_t new_mode)
{
    simple_directory new_dir;

    new_dir._directory_file._path = std::string(path);

    _the_directories.push_back(new_dir);
}


void filesystem_data::rename_file(const char* path, const char* new_path)
{
   int index_new_path = this->get_index_for_filename(new_path);

   if(index_new_path >= 0) // mv destination already exists -> delete this file
   {
      this->remove_file(new_path);
   }
   
   int index = this->get_index_for_filename(path);

   assert(index >= 0);

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


void filesystem_data::create_file(const char* path, mode_t new_mode)
{
   printf("create_file with path : %s \n", path);
   simple_file new_file;

   new_file._path = std::string(path); 
  
   std::string path_string = std::string(path);
   std::size_t found = path_string.find_last_of("/");

   new_file._name = path_string.substr(found+1);
   new_file._mode = new_mode;
   new_file._content = "";
   new_file._last_access_time = time(NULL);
   new_file._last_modification_time = time(NULL);
   
   new_file._user = fuse_get_context()->uid;
   new_file._group = fuse_get_context()->gid;
   //new_file._user = getuid(); // user-id of user who mounted the filesystem
   //new_file._group = getgid(); // group-id of user who mounted the filesystem

   _the_files.push_back(new_file);

   
   std::string the_dir_name = this->get_dir_from_path( std::string(path) );

   int dir_index = this->get_index_for_dirname(the_dir_name.c_str());
   assert(dir_index >= 0);
   assert(dir_index < _the_directories.size());

   simple_directory& the_dir = _the_directories[ dir_index ];

   the_dir._contained_files.push_back( &(_the_files.back()) );
}


/*int filesystem_data::get_directory_index_from_path(const char* path)
{
   printf("entering filesystem_data::get_directory_index_from_path with path = %s", path);
   
   std::string path_string = std::string(path);
   std::size_t position_of_last_slash = path_string.find_last_of("/");
   std::string dir_path = path_string.substr(0, position_of_last_path);
   
   printf("dir_path = %s", dir_path.c_str());
   
   for(int i=0; i < _the_directories.size(); ++i)
   {
       printf("comparing %s to %s \n", dir_path.c_str(), _the_directories[i]._directory_file._path.c_str() );
       if(strcmp( dir_path.c_str() == _the_directories[i]._directory_file._path.c_str() ) == 0)
       {
           printf("the are equal !!! -> result : %d \n", i);
           return i;
       }
   }

   printf("did not find directory - returning -1");
   return -1; // FIXME : error handling
   
}*/


int filesystem_data::get_index_for_filename(const char* path)
{
   printf("entering filesystem_data::get_index_for_filename with path = %s", path);
   for(int i=0; i < _the_files.size(); ++i)
   {
       printf("comparing %s to %s \n", path, _the_files[i]._path.c_str() );
       if(strcmp( path, _the_files[i]._path.c_str() ) == 0)
       {
           printf("the are equal !!! -> result : %d \n", i);
           return i;
       }
   }

   printf("did not find file - returning -1");
   return -1; // FIXME : error handling
}



int filesystem_data::get_index_for_dirname(const char* path)
{
   printf("entering filesystem_data::get_index_for_dirname with path = %s \n", path);
   for(int i=0; i < _the_directories.size(); ++i)
   {
       printf("comparing %s to %s \n", path, _the_directories[i]._directory_file._path.c_str() );
       if(strcmp( path, _the_directories[i]._directory_file._path.c_str() ) == 0)
       {
           printf("the are equal !!! -> result : %d \n", i);
           return i;
       }
   }

   printf("did not find directory - returning -1");
   return -1; // FIXME : error handling
}


std::string filesystem_data::get_dir_from_path(const std::string& path)
{
  printf("calling filesystem_data::get_dir_from_path with path = %s \n", path.c_str());
  std::size_t found = path.find_last_of("/");
  printf("result : %s \n", path.substr(0,found+1).c_str());
  return path.substr(0,found+1);
}


std::string filesystem_data::get_filename_from_path(const std::string& path)
{
  printf("calling filesystem_data::get_filename_from_path with path = %s \n", path.c_str());
  std::size_t found = path.find_last_of("/");

  printf("result : %s \n", path.substr(found+1).c_str());
  return path.substr(found+1);
}
 

filesystem_data::simple_file* filesystem_data::get_fileptr_for_filename(const std::string& path)
{
    int file_index = this->get_index_for_filename(path.c_str());

    if(file_index >= 0)
    {
       return &(_the_files.at(file_index)); 
    }

    int dir_index = this->get_index_for_dirname(path.c_str());

    if(dir_index >= 0)
    {
       return &(_the_directories.at(dir_index)._directory_file); 

    }
    else
    {
       return NULL;
    }
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

   printf("directories : \n\n");
   for(int i=0; i < _the_directories.size(); ++i)
   {
       simple_file& dirfile = _the_directories.at(i)._directory_file;

       printf("path : %s, name: %s ; content : %s \n", dirfile._path.c_str(), dirfile._name.c_str(), dirfile._content.c_str());
   }

   printf("\n\n===============  MG_FILESYSTEM DEBUG END   =================================\n\n");
}
