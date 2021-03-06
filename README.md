*** Installation (tested on Ubuntu 18.04.3 LTS): ***

---installation of fuse, tclap and boost ---

sudo apt-get install fuse libfuse-dev\
sudo apt-get install libtclap-dev
sudo apt-get install libboost-all-dev

---installation of libssh-library:---

Download and extract libssh\
mkdir build\
cd build/\
sudo apt-get install libssl-dev\
sudo apt-get install libcmocka-dev\
cmake -DUNIT_TESTING=ON -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ..\
make\
sudo make install # -> puts headers to /usr/include/libssh/ and binary to /usr/lib/x86_64-linux-gnu/

---installation of MGfilesystem---

cmake .\
make

****************************************************

*** Running the program ***

mounting the in-RAM filesystem:
./bin/MGfilesystem --mount-point=./mountdir --backend=ram

mounting via the ssh backend:
./bin/MGfilesystem --mount-point=./mountdir --backend=ssh --auth=pw --ssh=root@192.168.122.252:/root/testdir


****************************************************


Supported Filesystem Operations:\
    - list files with ls/ll\
    - read file content with cat\
    - overwrite file with echo "..." > \
    - append content to file with echo "..." >> \
    - open file in VI\
    - modify and save file in VI\
    - rename files with mv\
    - remove files with rm\
    - change file permissions with chmod\

