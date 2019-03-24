#ifndef _MGFILESYSTEM_DATA_
#define _MGFILESYSTEM_DATA_

#include <vector>
#include <string>

#include <fuse.h>

class filesystem_data
{
    public:
       struct simple_file
       {
           std::string _path;
           std::string _name;
           mode_t      _mode;
           std::string _content;
       };

       std::vector<simple_file> _the_files;

       struct simple_directory
       {
           simple_file _directory_file; // containing path, name, mode and (within _content) a list of contained files/directories

           std::vector<simple_file*> _contained_files;
           std::vector<simple_directory*> _contained_directories;
       };

       std::vector<simple_directory> _the_directories;

    public:
       filesystem_data();

       void list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler);
       char* read_file_content(const char *path);
       void get_attributes(const char* path, struct stat* st);
       bool file_exists(const char* path);

       void set_file_mode(const char* path, mode_t new_mode);
       void write_file_content(const char* path, const char* data, size_t size, off_t offset);
       void rename_file(const char* path, const char* new_name);

    private:
       int get_index_for_filename(const char* path);
};


#endif
