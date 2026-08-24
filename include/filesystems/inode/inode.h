#ifndef INODE_H
#define INODE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace virtualfilesystem {

/**
 * @struct Inode
 * @brief Stores metadata and block references for a filesystem object.
 *
 * An Inode represents the metadata of a file or directory in the
 * inode-based filesystem. It stores information such as the object
 * identifier, name, type, size, and the blocks allocated to the object.
 *
 * File data is stored separately on the underlying BlockDevice, while
 * the Inode contains the metadata required to locate and manage it.
 */
struct Inode {
    std::uint64_t id = 0;
    bool directory = false;
    std::string name;
    std::uint64_t size = 0;
    std::vector<std::size_t> blocks;
};

} // namespace virtualfilesystem


#endif // INODE_H
