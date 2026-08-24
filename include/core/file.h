#ifndef FILE_H
#define FILE_H

#include <cstdint>
#include <string>
#include <vector>

namespace virtualfilesystem {

using Byte = std::uint8_t;
using Data = std::vector<Byte>;

enum class FileType {
    Regular,
    Directory
};

struct FileMetadata {
    std::string name;
    FileType type = FileType::Regular;
    std::uint64_t size = 0;
};

/**
 * @class File
 * @brief Represents a regular file and its stored data.
 *
 * The File class stores the name and content of a regular file. File
 * content is represented as a sequence of bytes and can be replaced
 * or extended through the provided interface.
 */
class File {
public:
    explicit File(std::string name);

    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] std::uint64_t size() const noexcept;
    [[nodiscard]] const Data& data() const noexcept;

    void setData(Data data);
    void append(const Data& data);

private:
    std::string name_;
    Data data_;
};

} // namespace virtualfilesystem

#endif // FILE_H
