#include <filesystems/fat/fatfilesystem.h>
#include <error_handling/errors.h>
#include <filesystems/inode/inodefilesystem.h>

#include <iomanip>
#include <iostream>
#include <string>

using namespace virtualfilesystem;

static Data bytes(const std::string& text) {
    return Data(text.begin(), text.end());
}

static std::string text(const Data& data) {
    return std::string(data.begin(), data.end());
}

static void printStats(const IFileSystem& fs) {
    const auto s = fs.statistics();

    std::cout << "\n--- " << fs.typeName() << " statistics ---\n";
    std::cout << "Total blocks:      " << s.totalBlocks << '\n';
    std::cout << "Used blocks:       " << s.usedBlocks << '\n';
    std::cout << "Free blocks:       " << s.freeBlocks << '\n';
    std::cout << "Utilization:       "
              << std::fixed << std::setprecision(1)
              << s.utilization * 100.0 << "%\n";
    std::cout << "Fragmentation:     "
              << s.fragmentation * 100.0 << "%\n";
}

static void demo(IFileSystem& fs) {
    std::cout << "\n==============================\n";
    std::cout << "Demo: " << fs.typeName() << '\n';
    std::cout << "==============================\n";

    fs.format();

    fs.createDirectory(Path("/documents"));
    fs.createDirectory(Path("/images"));
    fs.createFile(Path("/documents/report.txt"));

    fs.writeFile(
        Path("/documents/report.txt"),
        bytes("Das ist eine Datei im VirtualFS.")
        );

    std::cout << "\nRoot directory:\n";
    for (const auto& entry : fs.list(Path("/"))) {
        std::cout << "  " << entry << '\n';
    }

    std::cout << "\n/documents:\n";
    for (const auto& entry : fs.list(Path("/documents"))) {
        std::cout << "  " << entry << '\n';
    }

    std::cout << "\nFile contents:\n";
    std::cout << "  " << text(fs.readFile(Path("/documents/report.txt"))) << '\n';

    fs.rename(Path("/documents/report.txt"), "bericht.txt");

    std::cout << "\nAfter rename:\n";
    for (const auto& entry : fs.list(Path("/documents"))) {
        std::cout << "  " << entry << '\n';
    }

    printStats(fs);
}

int main() {
    try {
        FATFileSystem fat(256, 64);
        demo(fat);

        InodeFileSystem inode(256, 64);
        demo(inode);

        std::cout << "\nVirtualFS demo completed successfully.\n";
        return 0;
    } catch (const VirtualFSError& e) {
        std::cerr << "VirtualFS error: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << '\n';
        return 1;
    }
}
