#include <filesystems/fat/fat_table.h>
#include <error_handling/errors.h>

namespace virtualfilesystem {

FATTable::FATTable(std::size_t blockCount)
    : table_(blockCount, FREE) {}

void FATTable::reset(std::size_t blockCount) {
    table_.assign(blockCount, FREE);
}

std::size_t FATTable::allocate() {
    for (std::size_t i = 0; i < table_.size(); ++i) {
        if (table_[i] == FREE) {
            table_[i] = END;
            return i;
        }
    }
    throw StorageError("FAT: no free blocks available.");
}

std::vector<std::size_t> FATTable::allocateChain(std::size_t count) {
    std::vector<std::size_t> blocks;
    blocks.reserve(count);

    try {
        for (std::size_t i = 0; i < count; ++i) {
            blocks.push_back(allocate());
        }
    } catch (...) {
        for (const auto block : blocks) {
            table_[block] = FREE;
        }
        throw;
    }

    for (std::size_t i = 0; i + 1 < blocks.size(); ++i) {
        table_[blocks[i]] = blocks[i + 1];
    }

    return blocks;
}

void FATTable::freeChain(std::size_t firstBlock) {
    if (firstBlock == FREE) {
        return;
    }

    auto current = firstBlock;
    while (current != END && current != FREE) {
        const auto nextBlock = table_.at(current);
        table_[current] = FREE;
        current = nextBlock;
    }
}

void FATTable::setNext(std::size_t block, std::size_t nextBlock) {
    if (block >= table_.size()) {
        throw StorageError("FAT block index out of range.");
    }
    table_[block] = nextBlock;
}

std::size_t FATTable::next(std::size_t block) const {
    if (block >= table_.size()) {
        throw StorageError("FAT block index out of range.");
    }
    return table_[block];
}

bool FATTable::used(std::size_t block) const {
    return table_.at(block) != FREE;
}

std::size_t FATTable::usedBlocks() const {
    std::size_t count = 0;
    for (const auto value : table_) {
        if (value != FREE) {
            ++count;
        }
    }
    return count;
}

std::size_t FATTable::freeBlocks() const {
    return table_.size() - usedBlocks();
}

std::vector<std::size_t> FATTable::chain(std::size_t firstBlock) const {
    std::vector<std::size_t> result;

    if (firstBlock == FREE) {
        return result;
    }

    auto current = firstBlock;
    while (current != END && current != FREE) {
        result.push_back(current);
        current = table_.at(current);
    }

    return result;
}

} // namespace virtualfilesystem
