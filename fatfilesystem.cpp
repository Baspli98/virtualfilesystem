#include "fatfilesystem.h"
#include "errors.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace virtualfilesystem {

FATFileSystem::FATFileSystem(std::size_t blockCount, std::size_t blockSize)
    : FileSystem(std::make_unique<BlockDevice>(blockCount, blockSize)),
      root_(makeDirectory("/")),
      fat_(blockCount) {}

void FATFileSystem::format() {
    device_->clear();
    fat_.reset(device_->blockCount());
    root_ = makeDirectory("/");
}

std::unique_ptr<FATFileSystem::Node> FATFileSystem::makeDirectory(std::string name) {
    auto node = std::make_unique<Node>();
    node->directory = true;
    node->name = std::move(name);
    return node;
}

std::unique_ptr<FATFileSystem::Node> FATFileSystem::makeFile(std::string name) {
    auto node = std::make_unique<Node>();
    node->directory = false;
    node->name = std::move(name);
    return node;
}

FATFileSystem::Node* FATFileSystem::findNode(const Path& path) {
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

const FATFileSystem::Node* FATFileSystem::findNode(const Path& path) const {
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

FATFileSystem::Node* FATFileSystem::findParent(const Path& path) {
    return findNode(Path(path.parent()));
}

void FATFileSystem::createFile(const Path& path) {
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

    parent->children[path.filename()] = makeFile(path.filename());
}

void FATFileSystem::deleteFile(const Path& path) {
    Node* node = findNode(path);
    if (!node) {
        throw FileNotFoundError(path.string());
    }
    if (node->directory) {
        throw NotFileError(path.string());
    }

    Node* parent = findParent(path);
    releaseNodeBlocks(*node);
    parent->children.erase(path.filename());
}

void FATFileSystem::createDirectory(const Path& path) {
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

    parent->children[path.filename()] = makeDirectory(path.filename());
}

void FATFileSystem::deleteDirectory(const Path& path) {
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
    parent->children.erase(path.filename());
}

void FATFileSystem::releaseNodeBlocks(Node& node) {
    if (node.firstBlock != FATTable::FREE) {
        fat_.freeChain(node.firstBlock);
        node.firstBlock = FATTable::FREE;
    }
    node.size = 0;
}

void FATFileSystem::writeNodeData(Node& node, const Data& data) {
    releaseNodeBlocks(node);

    if (data.empty()) {
        return;
    }

    const std::size_t blockSize = device_->blockSize();
    const std::size_t required =
        (data.size() + blockSize - 1) / blockSize;

    const auto blocks = fat_.allocateChain(required);
    node.firstBlock = blocks.front();
    node.size = data.size();

    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const auto offset = i * blockSize;
        const auto length = std::min(blockSize, data.size() - offset);
        Data chunk(data.begin() + static_cast<std::ptrdiff_t>(offset),
                   data.begin() + static_cast<std::ptrdiff_t>(offset + length));
        device_->writeBlock(blocks[i], chunk);
    }
}

void FATFileSystem::writeFile(const Path& path, const Data& data) {
    Node* node = findNode(path);
    if (!node) {
        throw FileNotFoundError(path.string());
    }
    if (node->directory) {
        throw NotFileError(path.string());
    }
    writeNodeData(*node, data);
}

Data FATFileSystem::readFile(const Path& path) const {
    const Node* node = findNode(path);
    if (!node) {
        throw FileNotFoundError(path.string());
    }
    if (node->directory) {
        throw NotFileError(path.string());
    }

    Data result;
    result.reserve(node->size);

    for (const auto block : fat_.chain(node->firstBlock)) {
        const auto chunk = device_->readBlock(block);
        result.insert(result.end(), chunk.begin(), chunk.end());
        if (result.size() >= node->size) {
            break;
        }
    }

    result.resize(node->size);
    return result;
}

void FATFileSystem::rename(const Path& oldPath, const std::string& newName) {
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
    parent->children[newName] = std::move(moved);
}

std::vector<std::string> FATFileSystem::list(const Path& path) const {
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

StorageStatistics FATFileSystem::statistics() const {
    StorageStatistics stats;
    stats.totalBlocks = device_->blockCount();
    stats.usedBlocks = fat_.usedBlocks();
    stats.freeBlocks = fat_.freeBlocks();
    stats.utilization = stats.totalBlocks == 0
        ? 0.0
        : static_cast<double>(stats.usedBlocks) / stats.totalBlocks;

    if (stats.usedBlocks > 0) {
        std::size_t runs = 0;
        bool previousUsed = false;
        for (std::size_t i = 0; i < stats.totalBlocks; ++i) {
            const bool currentUsed = fat_.used(i);
            if (currentUsed && !previousUsed) {
                ++runs;
            }
            previousUsed = currentUsed;
        }
        stats.fragmentation =
            stats.usedBlocks == 0
                ? 0.0
                : std::max(0.0, 1.0 -
                    static_cast<double>(runs) /
                    static_cast<double>(stats.usedBlocks));
    }

    return stats;
}

const FATTable& FATFileSystem::fat() const noexcept {
    return fat_;
}

} // namespace virtualfilesystem
