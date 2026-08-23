#include "inodefilesystem.h"
#include "errors.h"

#include <algorithm>

namespace virtualfilesystem {

InodeFileSystem::InodeFileSystem(std::size_t blockCount, std::size_t blockSize)
    : FileSystem(std::make_unique<BlockDevice>(blockCount, blockSize)),
      root_(std::make_unique<Node>()),
      usedBlocks_(blockCount, false) {
    format();
}

void InodeFileSystem::format() {
    device_->clear();
    root_ = std::make_unique<Node>();
    root_->directory = true;
    root_->name = "/";
    root_->inodeId = 0;

    inodes_.clear();
    inodes_.emplace(0, Inode{0, true, "/", 0, {}});
    usedBlocks_.assign(device_->blockCount(), false);
    nextInodeId_ = 1;
}

InodeFileSystem::Node* InodeFileSystem::findNode(const Path& path) {
    Node* current = root_.get();
    for (const auto& part : path.components()) {
        const auto it = current->children.find(part);
        if (it == current->children.end()) {
            return nullptr;
        }
        current = it->second.get();
    }
    return current;
}

const InodeFileSystem::Node* InodeFileSystem::findNode(const Path& path) const {
    const Node* current = root_.get();
    for (const auto& part : path.components()) {
        const auto it = current->children.find(part);
        if (it == current->children.end()) {
            return nullptr;
        }
        current = it->second.get();
    }
    return current;
}

InodeFileSystem::Node* InodeFileSystem::findParent(const Path& path) {
    return findNode(Path(path.parent()));
}

std::size_t InodeFileSystem::allocateBlock() {
    for (std::size_t i = 0; i < usedBlocks_.size(); ++i) {
        if (!usedBlocks_[i]) {
            usedBlocks_[i] = true;
            return i;
        }
    }
    throw StorageError("Inode filesystem: no free blocks available.");
}

void InodeFileSystem::freeBlocks(const Inode& inode) {
    for (const auto block : inode.blocks) {
        usedBlocks_.at(block) = false;
    }
}

const Inode& InodeFileSystem::inodeOf(const Node& node) const {
    return inodes_.at(node.inodeId);
}

Inode& InodeFileSystem::inodeOf(Node& node) {
    return inodes_.at(node.inodeId);
}

void InodeFileSystem::createFile(const Path& path) {
    if (path.isRoot()) {
        throw AlreadyExistsError(path.string());
    }

    Node* parent = findParent(path);
    if (!parent) {
        throw DirectoryNotFoundError(path.parent());
    }
    if (!parent->directory) {
        throw NotDirectoryError(path.parent());
    }
    if (parent->children.contains(path.filename())) {
        throw AlreadyExistsError(path.string());
    }

    auto node = std::make_unique<Node>();
    node->inodeId = nextInodeId_++;
    node->name = path.filename();
    node->directory = false;

    inodes_.emplace(node->inodeId,
                    Inode{node->inodeId, false, node->name, 0, {}});
    parent->children[node->name] = std::move(node);
}

void InodeFileSystem::deleteFile(const Path& path) {
    Node* node = findNode(path);
    if (!node) {
        throw FileNotFoundError(path.string());
    }
    if (node->directory) {
        throw NotFileError(path.string());
    }

    Node* parent = findParent(path);
    freeBlocks(inodeOf(*node));
    inodes_.erase(node->inodeId);
    parent->children.erase(path.filename());
}

void InodeFileSystem::createDirectory(const Path& path) {
    if (path.isRoot()) {
        return;
    }

    Node* parent = findParent(path);
    if (!parent) {
        throw DirectoryNotFoundError(path.parent());
    }
    if (!parent->directory) {
        throw NotDirectoryError(path.parent());
    }
    if (parent->children.contains(path.filename())) {
        throw AlreadyExistsError(path.string());
    }

    auto node = std::make_unique<Node>();
    node->inodeId = nextInodeId_++;
    node->name = path.filename();
    node->directory = true;

    inodes_.emplace(node->inodeId,
                    Inode{node->inodeId, true, node->name, 0, {}});
    parent->children[node->name] = std::move(node);
}

void InodeFileSystem::removeRecursive(Node& node) {
    for (auto& [_, child] : node.children) {
        removeRecursive(*child);
    }

    if (node.inodeId != 0) {
        freeBlocks(inodeOf(node));
        inodes_.erase(node.inodeId);
    }
}

void InodeFileSystem::deleteDirectory(const Path& path) {
    if (path.isRoot()) {
        throw VirtualFSError("The root directory cannot be deleted.");
    }

    Node* node = findNode(path);
    if (!node) {
        throw DirectoryNotFoundError(path.string());
    }
    if (!node->directory) {
        throw NotDirectoryError(path.string());
    }
    if (!node->children.empty()) {
        throw VirtualFSError("Directory is not empty: " + path.string());
    }

    Node* parent = findParent(path);
    inodes_.erase(node->inodeId);
    parent->children.erase(path.filename());
}

void InodeFileSystem::writeNodeData(Node& node, const Data& data) {
    Inode& inode = inodeOf(node);
    freeBlocks(inode);
    inode.blocks.clear();
    inode.size = 0;

    if (data.empty()) {
        return;
    }

    const std::size_t blockSize = device_->blockSize();
    const std::size_t required =
        (data.size() + blockSize - 1) / blockSize;

    std::vector<std::size_t> allocated;
    allocated.reserve(required);

    try {
        for (std::size_t i = 0; i < required; ++i) {
            allocated.push_back(allocateBlock());
        }
    } catch (...) {
        for (const auto block : allocated) {
            usedBlocks_[block] = false;
        }
        throw;
    }

    inode.blocks = allocated;
    inode.size = data.size();

    for (std::size_t i = 0; i < allocated.size(); ++i) {
        const auto offset = i * blockSize;
        const auto length = std::min(blockSize, data.size() - offset);
        Data chunk(data.begin() + static_cast<std::ptrdiff_t>(offset),
                   data.begin() + static_cast<std::ptrdiff_t>(offset + length));
        device_->writeBlock(allocated[i], chunk);
    }
}

void InodeFileSystem::writeFile(const Path& path, const Data& data) {
    Node* node = findNode(path);
    if (!node) {
        throw FileNotFoundError(path.string());
    }
    if (node->directory) {
        throw NotFileError(path.string());
    }

    writeNodeData(*node, data);
}

Data InodeFileSystem::readFile(const Path& path) const {
    const Node* node = findNode(path);
    if (!node) {
        throw FileNotFoundError(path.string());
    }
    if (node->directory) {
        throw NotFileError(path.string());
    }

    const Inode& inode = inodeOf(*node);
    Data result;
    result.reserve(inode.size);

    for (const auto block : inode.blocks) {
        const auto chunk = device_->readBlock(block);
        result.insert(result.end(), chunk.begin(), chunk.end());
    }

    result.resize(inode.size);
    return result;
}

void InodeFileSystem::rename(const Path& oldPath, const std::string& newName) {
    if (newName.empty() || newName.find('/') != std::string::npos ||
        newName == "." || newName == "..") {
        throw InvalidPathError(newName);
    }

    Node* parent = findParent(oldPath);
    Node* node = findNode(oldPath);

    if (!node) {
        throw FileNotFoundError(oldPath.string());
    }
    if (!parent) {
        throw DirectoryNotFoundError(oldPath.parent());
    }
    if (parent->children.contains(newName)) {
        throw AlreadyExistsError(newName);
    }

    auto moved = std::move(parent->children.at(oldPath.filename()));
    parent->children.erase(oldPath.filename());

    moved->name = newName;
    inodeOf(*moved).name = newName;
    parent->children[newName] = std::move(moved);
}

std::vector<std::string> InodeFileSystem::list(const Path& path) const {
    const Node* node = findNode(path);
    if (!node) {
        throw DirectoryNotFoundError(path.string());
    }
    if (!node->directory) {
        throw NotDirectoryError(path.string());
    }

    std::vector<std::string> result;
    for (const auto& [name, child] : node->children) {
        result.push_back(child->directory ? name + "/" : name);
    }
    return result;
}

StorageStatistics InodeFileSystem::statistics() const {
    StorageStatistics stats;
    stats.totalBlocks = device_->blockCount();

    for (const bool used : usedBlocks_) {
        if (used) {
            ++stats.usedBlocks;
        }
    }

    stats.freeBlocks = stats.totalBlocks - stats.usedBlocks;
    stats.utilization = stats.totalBlocks == 0
        ? 0.0
        : static_cast<double>(stats.usedBlocks) / stats.totalBlocks;

    if (stats.usedBlocks > 0) {
        std::size_t runs = 0;
        bool previousUsed = false;
        for (const bool used : usedBlocks_) {
            if (used && !previousUsed) {
                ++runs;
            }
            previousUsed = used;
        }
        stats.fragmentation =
            std::max(0.0, 1.0 -
                static_cast<double>(runs) /
                static_cast<double>(stats.usedBlocks));
    }

    return stats;
}

const std::map<std::uint64_t, Inode>& InodeFileSystem::inodes() const noexcept {
    return inodes_;
}

} // namespace virtualfilesystem
