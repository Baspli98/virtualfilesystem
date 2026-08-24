#ifndef BLOCKDEVICE_H
#define BLOCKDEVICE_H

#include <core/file.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace virtualfilesystem {

/**
 * @class BlockDevice
 * @brief Represents a virtual block-based storage device.
 *
 * The BlockDevice class provides an abstraction for a storage device
 * divided into equally sized blocks. Concrete filesystem implementations
 * use this class to read and write raw data independently of the
 * underlying filesystem structure.
 */
class BlockDevice {
public:
    BlockDevice(std::size_t blockCount, std::size_t blockSize);
    virtual ~BlockDevice() = default;

    [[nodiscard]] std::size_t blockCount() const noexcept;
    [[nodiscard]] std::size_t blockSize() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool isReadOnly() const noexcept;

    virtual Data readBlock(std::size_t index) const;
    virtual void writeBlock(std::size_t index, const Data& data);
    virtual void clear();

protected:
    std::vector<Byte> storage_;
    std::size_t blockCount_;
    std::size_t blockSize_;
    bool readOnly_ = false;

    void validateBlock(std::size_t index) const;
};

/**
 * @class CDROM
 * @brief Represents a virtual CD-ROM storage device.
 *
 * The CDROM class simulates a writable optical medium that becomes
 * read-only after it has been burned. It extends BlockDevice and can
 * therefore be used through the same block-based storage interface.
 *
 * Once the medium has been burned, write operations are no longer
 * permitted.
 */
class CDROM final : public BlockDevice {
public:
    CDROM(std::size_t blockCount = 1375, std::size_t blockSize = 512);

    void burn();
    void eject();
    [[nodiscard]] bool isBurned() const noexcept;
    [[nodiscard]] bool isInserted() const noexcept;

    void writeBlock(std::size_t index, const Data& data) override;

private:
    bool burned_ = false;
    bool inserted_ = true;
};

} // namespace virtualfilesystem

#endif // BLOCKDEVICE_H
