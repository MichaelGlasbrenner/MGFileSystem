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

    public:
       filesystem_data();
       void list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler);
};


#endif
