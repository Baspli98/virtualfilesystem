#include "filesystem.h"

namespace virtualfilesystem {

FileSystem::FileSystem(std::unique_ptr<BlockDevice> device)
    : device_(std::move(device)) {}

BlockDevice& FileSystem::device() noexcept {
    return *device_;
}

const BlockDevice& FileSystem::device() const noexcept {
    return *device_;
}

std::vector<std::string> FileSystem::splitParent(const Path& path) {
    return Path(path.parent()).components();
}

std::string FileSystem::parentPath(const Path& path) {
    return path.parent();
}

} // namespace virtualfilesystem
