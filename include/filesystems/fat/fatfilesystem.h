#ifndef FATFILESYSTEM_H
#define FATFILESYSTEM_H

#include <core/filesystem.h>
#include <filesystems/fat/fat_table.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace virtualfilesystem {

/**
 * @class FATFileSystem
 * @brief Implements a virtual filesystem based on the FAT allocation model.
 *
 * FATFileSystem stores file data on a BlockDevice and uses a FATTable
 * to manage block allocation. Files are represented as chains of blocks,
 * where each block references the next block through the file allocation
 * table.
 *
 * The class implements the common IFileSystem interface and can therefore
 * be used interchangeably with other filesystem implementations through
 * the VirtualFS abstraction.
 */
class FATFileSystem final : public FileSystem {
public:
    explicit FATFileSystem(std::size_t blockCount = 1024,
                           std::size_t blockSize = 512);

    void format() override;

    void createFile(const Path& path) override;
    void deleteFile(const Path& path) override;

    void createDirectory(const Path& path) override;
    void deleteDirectory(const Path& path) override;

    void writeFile(const Path& path, const Data& data) override;
    Data readFile(const Path& path) const override;

    void rename(const Path& oldPath, const std::string& newName) override;
    std::vector<std::string> list(const Path& path) const override;

    StorageStatistics statistics() const override;
    [[nodiscard]] const char* typeName() const noexcept override { return "FAT"; }

    [[nodiscard]] const FATTable& fat() const noexcept;

private:
    struct Node {
        bool directory = false;
        std::string name;
        Data empty;
        std::map<std::string, std::unique_ptr<Node>> children;
        std::size_t firstBlock = FATTable::FREE;
        std::size_t size = 0;
    };

    std::unique_ptr<Node> root_;
    FATTable fat_;

    Node* findNode(const Path& path);
    const Node* findNode(const Path& path) const;
    Node* findParent(const Path& path);
    static std::unique_ptr<Node> makeDirectory(std::string name);
    static std::unique_ptr<Node> makeFile(std::string name);
    void releaseNodeBlocks(Node& node);
    void writeNodeData(Node& node, const Data& data);
};

} // namespace virtualfilesystem


#endif // FATFILESYSTEM_H
