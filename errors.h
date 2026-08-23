#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
#include <string>

namespace virtualfilesystem {

/**
 * @class VirtualFSError
 * @brief Base exception class for all VirtualFS-related errors.
 *
 * This exception serves as the common base class for all errors that can
 * occur while working with the VirtualFS library. Specific filesystem
 * exceptions should inherit from this class.
 */
class VirtualFSError : public std::runtime_error {
public:
    explicit VirtualFSError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class FileNotFoundError
 * @brief Exception thrown when a requested file does not exist.
 *
 * This exception is used when an operation attempts to access a file
 * that cannot be found at the specified path.
 */
class FileNotFoundError : public VirtualFSError {
public:
    explicit FileNotFoundError(const std::string& path)
        : VirtualFSError("File not found: " + path) {}
};

/**
 * @class DirectoryNotFoundError
 * @brief Exception thrown when a requested directory does not exist.
 *
 * This exception is used when an operation requires a directory that
 * cannot be found at the specified path.
 */
class DirectoryNotFoundError : public VirtualFSError {
public:
    explicit DirectoryNotFoundError(const std::string& path)
        : VirtualFSError("Directory not found: " + path) {}
};

/**
 * @class AlreadyExistsError
 * @brief Exception thrown when an object already exists.
 *
 * This exception is typically thrown when attempting to create a file
 * or directory with a name that already exists in the target location.
 */
class AlreadyExistsError : public VirtualFSError {
public:
    explicit AlreadyExistsError(const std::string& path)
        : VirtualFSError("Already exists: " + path) {}
};

/**
 * @class NotDirectoryError
 * @brief Exception thrown when a directory operation is performed on a file.
 *
 * This exception is used when an operation expects a directory but the
 * specified path refers to a regular file.
 */
class NotDirectoryError : public VirtualFSError {
public:
    explicit NotDirectoryError(const std::string& path)
        : VirtualFSError("Not a directory: " + path) {}
};

/**
 * @class NotFileError
 * @brief Exception thrown when a file operation is performed on a directory.
 *
 * This exception is used when an operation expects a regular file but
 * the specified path refers to a directory.
 */
class NotFileError : public VirtualFSError {
public:
    explicit NotFileError(const std::string& path)
        : VirtualFSError("Not a file: " + path) {}
};

/**
 * @class InvalidPathError
 * @brief Exception thrown when an invalid filesystem path is used.
 *
 * This exception indicates that a path does not meet the requirements
 * of the VirtualFS path representation.
 */
class InvalidPathError : public VirtualFSError {
public:
    explicit InvalidPathError(const std::string& path)
        : VirtualFSError("Invalid path: " + path) {}
};

/**
 * @class StorageError
 * @brief Exception thrown when a storage device operation fails.
 *
 * This exception is used for errors related to reading from, writing to,
 * or managing the underlying virtual storage device.
 */
class StorageError : public VirtualFSError {
public:
    explicit StorageError(const std::string& message)
        : VirtualFSError(message) {}
};

/**
 * @class ReadOnlyError
 * @brief Exception thrown when attempting to modify read-only storage.
 *
 * This exception is used when a write operation is attempted on a
 * storage device that does not allow modifications.
 */
class ReadOnlyError : public VirtualFSError {
public:
    explicit ReadOnlyError(const std::string& message)
        : VirtualFSError(message) {}
};

} // namespace virtualfilesystem

#endif // ERRORS_H
