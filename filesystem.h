#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "blockdevice.h"
#include "directory.h"
#include "file.h"
#include "path.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace virtualfilesystem {

struct StorageStatistics {
    std::size_t totalBlocks = 0;
    std::size_t usedBlocks = 0;
    std::size_t freeBlocks = 0;
    double utilization = 0.0;
    double fragmentation = 0.0;
};

/**
 * @class IFileSystem
 * @brief Defines the common interface for all virtual filesystem implementations.
 *
 * IFileSystem represents the Virtual File System abstraction of the
 * library. It defines a common set of operations that every concrete
 * filesystem implementation must provide, such as creating, reading,
 * writing, renaming, and deleting files and directories.
 *
 * The graphical user interface and other applications should interact
 * with filesystem implementations through this interface instead of
 * directly depending on a specific filesystem type.
 */
class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    virtual void format() = 0;

    virtual void createFile(const Path& path) = 0;
    virtual void deleteFile(const Path& path) = 0;

    virtual void createDirectory(const Path& path) = 0;
    virtual void deleteDirectory(const Path& path) = 0;

    virtual void writeFile(const Path& path, const Data& data) = 0;
    virtual Data readFile(const Path& path) const = 0;

    virtual void rename(const Path& oldPath, const std::string& newName) = 0;
    virtual std::vector<std::string> list(const Path& path) const = 0;

    virtual StorageStatistics statistics() const = 0;

    [[nodiscard]] virtual const char* typeName() const noexcept = 0;
};

/**
 * @class FileSystem
 * @brief Provides a common base implementation for concrete filesystems.
 *
 * FileSystem extends the IFileSystem interface and contains functionality
 * shared by multiple filesystem implementations. It manages the
 * underlying BlockDevice and provides a common foundation for concrete
 * filesystems such as FATFileSystem and InodeFileSystem.
 */
class FileSystem : public IFileSystem {
public:
    explicit FileSystem(std::unique_ptr<BlockDevice> device);
    ~FileSystem() override = default;

    [[nodiscard]] BlockDevice& device() noexcept;
    [[nodiscard]] const BlockDevice& device() const noexcept;

protected:
    std::unique_ptr<BlockDevice> device_;

    static std::vector<std::string> splitParent(const Path& path);
    static std::string parentPath(const Path& path);
};

} // namespace virtualfilesystem


#endif // FILESYSTEM_H
