#ifndef INODE_H
#define INODE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace virtualfilesystem {

struct Inode {
    std::uint64_t id = 0;
    bool directory = false;
    std::string name;
    std::uint64_t size = 0;
    std::vector<std::size_t> blocks;
};

} // namespace virtualfilesystem


#endif // INODE_H
