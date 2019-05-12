#ifndef _MGFILESYSTEM_DATA_
#define _MGFILESYSTEM_DATA_

#include <vector>
#include <string>
#include <map>
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
           time_t      _last_access_time;
           time_t      _last_modification_time;
           uid_t       _user;
           gid_t       _group;
       };

       std::vector<simple_file> _the_files;

       struct simple_directory
       {
           simple_file _directory_file; // containing path, name, mode and (within _content) a list of contained files/directories

           std::vector<simple_file*> _contained_files;
           std::vector<simple_directory*> _contained_directories;
       };

       std::vector<simple_directory> _the_directories;

       std::map<std::string, size_t> _file_lookup;
       std::map<std::string, size_t> _directory_lookup;

    public:
       filesystem_data();

       void list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler);
       char* read_file_content(const char *path);
       void get_attributes(const char* path, struct stat* st);
       bool file_exists(const char* path);

       void set_file_mode(const char* path, mode_t new_mode);
       void write_file_content(const char* path, const char* data, size_t size, off_t offset);
       void rename_file(const char* path, const char* new_path);
       void create_directory(const char* path, mode_t new_mode);
       void remove_file(const char* path);
       void create_file(const char* path, mode_t new_mode);
       void set_access_and_modification_times(const char* path, const struct timespec tv[2]);
       void truncate_file(const char* path, off_t new_length);
       void change_ownership(const char* path, uid_t new_user, gid_t new_group);

       void debug();

    private:
       int get_index_for_filename(const char* path);
       int get_index_for_dirname(const char* path);
       std::string get_dir_from_path(const std::string& path);
       std::string get_filename_from_path(const std::string& path);
};


#endif
