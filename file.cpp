#include "file.h"

namespace virtualfilesystem {

File::File(std::string name)
    : name_(std::move(name)) {}

const std::string& File::name() const noexcept {
    return name_;
}

std::uint64_t File::size() const noexcept {
    return data_.size();
}

const Data& File::data() const noexcept {
    return data_;
}

void File::setData(Data data) {
    data_ = std::move(data);
}

void File::append(const Data& data) {
    data_.insert(data_.end(), data.begin(), data.end());
}

} // namespace virtualfilesystem
