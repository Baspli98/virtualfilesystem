#ifndef INODEFILESYSTEM_H
#define INODEFILESYSTEM_H

#include "filesystem.h"
#include "inode.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace virtualfilesystem {

/**
 * @class InodeFileSystem
 * @brief Implements a virtual filesystem based on inode metadata.
 *
 * InodeFileSystem manages files and directories using inode structures.
 * Each filesystem object is represented by an Inode containing its
 * metadata and references to the storage blocks assigned to it.
 *
 * The class implements the common IFileSystem interface and can therefore
 * be used interchangeably with other filesystem implementations through
 * the VirtualFS abstraction.
 */
class InodeFileSystem final : public FileSystem {
public:
    explicit InodeFileSystem(std::size_t blockCount = 1024,
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
    [[nodiscard]] const char* typeName() const noexcept override { return "Inode"; }

    [[nodiscard]] const std::map<std::uint64_t, Inode>& inodes() const noexcept;

private:
    struct Node {
        std::uint64_t inodeId = 0;
        bool directory = false;
        std::string name;
        std::map<std::string, std::unique_ptr<Node>> children;
    };

    std::unique_ptr<Node> root_;
    std::map<std::uint64_t, Inode> inodes_;
    std::vector<bool> usedBlocks_;
    std::uint64_t nextInodeId_ = 1;

    Node* findNode(const Path& path);
    const Node* findNode(const Path& path) const;
    Node* findParent(const Path& path);

    void removeRecursive(Node& node);
    void writeNodeData(Node& node, const Data& data);
    [[nodiscard]] const Inode& inodeOf(const Node& node) const;
    [[nodiscard]] Inode& inodeOf(Node& node);
    [[nodiscard]] std::size_t allocateBlock();
    void freeBlocks(const Inode& inode);
};

} // namespace virtualfilesystem


#endif // INODEFILESYSTEM_H
