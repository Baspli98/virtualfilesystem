#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>

namespace virtualfilesystem {

/**
 * @class Path
 * @brief Represents and manages a normalized virtual filesystem path.
 *
 * The Path class provides an abstraction for handling paths within the
 * virtual filesystem. It normalizes path strings and provides utility
 * functions for accessing individual path components, the parent path,
 * and the file or directory name.
 */
class Path {
public:
    Path() = default;
    explicit Path(std::string path);

    [[nodiscard]] const std::string& string() const noexcept;
    [[nodiscard]] std::vector<std::string> components() const;
    [[nodiscard]] bool isRoot() const noexcept;
    [[nodiscard]] std::string filename() const;
    [[nodiscard]] std::string parent() const;
    [[nodiscard]] Path child(const std::string& name) const;

    static std::string normalize(const std::string& path);

    bool operator==(const Path& other) const noexcept;
    bool operator<(const Path& other) const noexcept;

private:
    std::string value_ = "/";
};

} // namespace virtualfilesystem

#endif // PATH_H
