#include "blockdevice.h"
#include "errors.h"

#include <algorithm>

namespace virtualfilesystem {

BlockDevice::BlockDevice(std::size_t blockCount, std::size_t blockSize)
    : storage_(blockCount * blockSize, 0),
      blockCount_(blockCount),
      blockSize_(blockSize) {
    if (blockCount == 0 || blockSize == 0) {
        throw StorageError("Block count and block size must be greater than zero.");
    }
}

std::size_t BlockDevice::blockCount() const noexcept {
    return blockCount_;
}

std::size_t BlockDevice::blockSize() const noexcept {
    return blockSize_;
}

std::size_t BlockDevice::capacity() const noexcept {
    return storage_.size();
}

bool BlockDevice::isReadOnly() const noexcept {
    return readOnly_;
}

void BlockDevice::validateBlock(std::size_t index) const {
    if (index >= blockCount_) {
        throw StorageError("Block index out of range: " + std::to_string(index));
    }
}

Data BlockDevice::readBlock(std::size_t index) const {
    validateBlock(index);

    const auto begin = storage_.begin() +
        static_cast<std::ptrdiff_t>(index * blockSize_);
    return Data(begin, begin + static_cast<std::ptrdiff_t>(blockSize_));
}

void BlockDevice::writeBlock(std::size_t index, const Data& data) {
    validateBlock(index);

    if (readOnly_) {
        throw ReadOnlyError("Block device is read-only.");
    }

    if (data.size() > blockSize_) {
        throw StorageError("Data does not fit into one block.");
    }

    const auto offset = index * blockSize_;
    std::fill(storage_.begin() + static_cast<std::ptrdiff_t>(offset),
              storage_.begin() + static_cast<std::ptrdiff_t>(offset + blockSize_),
              0);

    std::copy(data.begin(), data.end(),
              storage_.begin() + static_cast<std::ptrdiff_t>(offset));
}

void BlockDevice::clear() {
    if (readOnly_) {
        throw ReadOnlyError("Block device is read-only.");
    }
    std::fill(storage_.begin(), storage_.end(), 0);
}

CDROM::CDROM(std::size_t blockCount, std::size_t blockSize)
    : BlockDevice(blockCount, blockSize) {}

void CDROM::burn() {
    if (!inserted_) {
        throw StorageError("CD is not inserted.");
    }
    burned_ = true;
    readOnly_ = true;
}

void CDROM::eject() {
    inserted_ = false;
}

bool CDROM::isBurned() const noexcept {
    return burned_;
}

bool CDROM::isInserted() const noexcept {
    return inserted_;
}

void CDROM::writeBlock(std::size_t index, const Data& data) {
    if (!inserted_) {
        throw StorageError("CD is not inserted.");
    }
    if (burned_) {
        throw ReadOnlyError("CD has been burned and is now read-only.");
    }
    BlockDevice::writeBlock(index, data);
}

} // namespace virtualfilesystem
