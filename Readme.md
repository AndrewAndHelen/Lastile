# Lastile

A tool to tile the .las file

## Dependencies

The code is based on the following prerequisites:
* C++11

##  Compilation
prerequisites: cmake version>=3.0

```
1. git clone https://github.com/AndrewAndJenny/Lastile.git
2. cd Lastile
```

**in the cmake-bash**
* Windows
```
1. mkdir build && cd build
2. cmake .. -A x64
3. open the vs solution, and install
```
* Linux
```
1. mkdir build && cd build
2. cmake ..
3. make
```

## Supported File Format
* .las: ASPRS LAS file which contains LIDAR point data records
* .laz: ASPRS LAZ file which contains LIDAR point data records(compression)

## Functions
- [x] tile .las files according to sub-block size
- [x] tile .laz files according to sub-block size

##  Contact
If you found bugs or have new ideas,please pull requests:smile:   
If you have trouble compiling or using this software,email to 15313326374@163.com

## Good Luck For You!
