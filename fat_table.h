#ifndef FAT_TABLE_H
#define FAT_TABLE_H

#include <cstddef>
#include <vector>

namespace virtualfilesystem {

/**
 * @class FATTable
 * @brief Manages block allocation using a File Allocation Table.
 *
 * FATTable implements the allocation mechanism used by FAT-based
 * filesystems. Each block contains a reference to the next block in
 * a file's block chain.
 *
 * A block can be marked as free, as the end of a chain, or as pointing
 * to another block. This allows files to be stored across multiple
 * non-contiguous blocks.
 */
class FATTable {
public:
    static constexpr std::size_t FREE = static_cast<std::size_t>(-1);
    static constexpr std::size_t END = static_cast<std::size_t>(-2);

    explicit FATTable(std::size_t blockCount = 0);

    void reset(std::size_t blockCount);
    [[nodiscard]] std::size_t allocate();
    [[nodiscard]] std::vector<std::size_t> allocateChain(std::size_t count);
    void freeChain(std::size_t firstBlock);

    void setNext(std::size_t block, std::size_t next);
    [[nodiscard]] std::size_t next(std::size_t block) const;

    [[nodiscard]] bool used(std::size_t block) const;
    [[nodiscard]] std::size_t usedBlocks() const;
    [[nodiscard]] std::size_t freeBlocks() const;
    [[nodiscard]] std::vector<std::size_t> chain(std::size_t firstBlock) const;

private:
    std::vector<std::size_t> table_;
};

} // namespace virtualfilesystem


#endif // FAT_TABLE_H
