#ifndef _FSMANAGER_
#define _FSMANAGER_

#include <vector>
#include <string>

// FIXME : common base class for client_node and storage_node ??

class client_node
{
    public: // FIXME
       std::string _ip;
       bool _reachable;
       bool _responsive;
};


class storage_node
{
    public: // FIXME
       std::string _ip;
       bool _reachable;
       bool _responsive;
};


class  FSmanager
{

   private:
       std::vector<client_node> _client_nodes;
       std::vector<storage_node> _storages_nodes;
};


#endif
