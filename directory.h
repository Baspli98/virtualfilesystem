#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "file.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace virtualfilesystem {

/**
 * @class Directory
 * @brief Represents a directory in the virtual filesystem.
 *
 * A Directory stores filesystem entries and provides access to files
 * and subdirectories contained within it. It represents the hierarchical
 * structure used to organize objects in the virtual filesystem.
 */
class Directory {
public:
    explicit Directory(std::string name = "/");

    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] bool contains(const std::string& name) const noexcept;

    void addFile(std::shared_ptr<File> file);
    void addDirectory(std::shared_ptr<Directory> directory);

    [[nodiscard]] std::shared_ptr<File> file(const std::string& name) const;
    [[nodiscard]] std::shared_ptr<Directory> directory(const std::string& name) const;

    void removeFile(const std::string& name);
    void removeDirectory(const std::string& name);

    [[nodiscard]] std::vector<std::string> entries() const;

private:
    std::string name_;
    std::map<std::string, std::shared_ptr<File>> files_;
    std::map<std::string, std::shared_ptr<Directory>> directories_;
};

} // namespace virtualfilesystem

#endif // DIRECTORY_H
