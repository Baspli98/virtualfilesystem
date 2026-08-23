#include "path.h"
#include "errors.h"

#include <algorithm>
#include <sstream>

namespace virtualfilesystem {

Path::Path(std::string path)
    : value_(normalize(path)) {}

const std::string& Path::string() const noexcept {
    return value_;
}

std::vector<std::string> Path::components() const {
    std::vector<std::string> result;
    std::stringstream stream(value_);
    std::string item;

    while (std::getline(stream, item, '/')) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

bool Path::isRoot() const noexcept {
    return value_ == "/";
}

std::string Path::filename() const {
    if (isRoot()) {
        return "/";
    }

    const auto pos = value_.find_last_of('/');
    return value_.substr(pos + 1);
}

std::string Path::parent() const {
    if (isRoot()) {
        return "/";
    }

    const auto pos = value_.find_last_of('/');
    if (pos == 0) {
        return "/";
    }

    return value_.substr(0, pos);
}

Path Path::child(const std::string& name) const {
    if (name.empty() || name.find('/') != std::string::npos ||
        name == "." || name == "..") {
        throw InvalidPathError(name);
    }

    return Path(isRoot() ? "/" + name : value_ + "/" + name);
}

std::string Path::normalize(const std::string& path) {
    if (path.empty() || path[0] != '/') {
        throw InvalidPathError(path);
    }

    std::vector<std::string> stack;
    std::stringstream stream(path);
    std::string item;

    while (std::getline(stream, item, '/')) {
        if (item.empty() || item == ".") {
            continue;
        }

        if (item == "..") {
            if (!stack.empty()) {
                stack.pop_back();
            }
            continue;
        }

        stack.push_back(item);
    }

    std::string result = "/";
    for (std::size_t i = 0; i < stack.size(); ++i) {
        result += stack[i];
        if (i + 1 < stack.size()) {
            result += '/';
        }
    }

    return result;
}

bool Path::operator==(const Path& other) const noexcept {
    return value_ == other.value_;
}

bool Path::operator<(const Path& other) const noexcept {
    return value_ < other.value_;
}

} // namespace virtualfilesystem
