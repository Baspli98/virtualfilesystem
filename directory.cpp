#include "directory.h"
#include "errors.h"

namespace virtualfilesystem {

Directory::Directory(std::string name)
    : name_(std::move(name)) {}

const std::string& Directory::name() const noexcept {
    return name_;
}

bool Directory::contains(const std::string& name) const noexcept {
    return files_.contains(name) || directories_.contains(name);
}

void Directory::addFile(std::shared_ptr<File> file) {
    if (!file || contains(file->name())) {
        throw AlreadyExistsError(file ? file->name() : "<null>");
    }
    files_[file->name()] = std::move(file);
}

void Directory::addDirectory(std::shared_ptr<Directory> directory) {
    if (!directory || contains(directory->name())) {
        throw AlreadyExistsError(directory ? directory->name() : "<null>");
    }
    directories_[directory->name()] = std::move(directory);
}

std::shared_ptr<File> Directory::file(const std::string& name) const {
    const auto it = files_.find(name);
    return it == files_.end() ? nullptr : it->second;
}

std::shared_ptr<Directory> Directory::directory(const std::string& name) const {
    const auto it = directories_.find(name);
    return it == directories_.end() ? nullptr : it->second;
}

void Directory::removeFile(const std::string& name) {
    if (!files_.erase(name)) {
        throw FileNotFoundError(name);
    }
}

void Directory::removeDirectory(const std::string& name) {
    if (!directories_.erase(name)) {
        throw DirectoryNotFoundError(name);
    }
}

std::vector<std::string> Directory::entries() const {
    std::vector<std::string> result;
    for (const auto& [name, _] : directories_) {
        result.push_back(name + "/");
    }
    for (const auto& [name, _] : files_) {
        result.push_back(name);
    }
    return result;
}

} // namespace virtualfilesystem
