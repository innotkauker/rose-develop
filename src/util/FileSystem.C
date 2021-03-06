#include "FileSystem.h"
#include <boost/foreach.hpp>
#include <set>

#if BOOST_FILESYSTEM_VERSION == 2                       // FIXME[Robb P. Matzke 2014-11-18]: Remove version 2 support
#include <LinearCongruentialGenerator.h>
#endif

namespace rose {
namespace FileSystem {

const char *tempNamePattern = "rose-%%%%%%%-%%%%%%%";

bool
baseNameMatches::operator()(const Path &path) {
#if BOOST_FILESYSTEM_VERSION == 2
    return boost::regex_match(path.filename(), re_);
#else
    return boost::regex_match(path.filename().string(), re_);
#endif
}

bool
isExisting(const Path &path) {
    return boost::filesystem::exists(path);
}

bool
isFile(const Path &path) {
    return boost::filesystem::is_regular_file(path);
}

bool
isDirectory(const Path &path) {
    return boost::filesystem::is_directory(path);
}

bool
isSymbolicLink(const Path &path) {
    return boost::filesystem::is_symlink(path);
}

bool
isNotSymbolicLink(const Path &path) {
    return !boost::filesystem::is_symlink(path);
}

Path
createTemporaryDirectory() {
#if BOOST_FILESYSTEM_VERSION == 2                       // FIXME[Robb P. Matzke 2014-11-18]: Remove version 2 support
#ifdef _MSC_VER
    Path dirName = "/tmp";                              // FIXME[Robb P. Matzke 2014-11-18]: is this right for Windows?
#else
    Path dirName = "/tmp";
    if (0 != geteuid() && NULL != getenv("TMPDIR")) {
        dirName = getenv("TMPDIR");
    } else {
#ifdef P_tmpdir
        dirName = P_tmpdir;
#endif
    }
#endif
    std::string base = tempNamePattern;
    LinearCongruentialGenerator lcg;
    for (size_t i=0; i<base.size(); ++i) {
        if ('%'==base[i])
            base[i] = "0123456789abcdef"[lcg()%16];
    }
    dirName /= base;
#else
    Path dirName = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path(tempNamePattern);
#endif
    boost::filesystem::create_directory(dirName);
    return dirName;
}

Path
makeNormal(const Path &path) {
    std::vector<Path> components;
    for (boost::filesystem::path::const_iterator i=path.begin(); i!=path.end(); ++i) {
#if BOOST_FILESYSTEM_VERSION == 2                       // FIXME[Robb P. Matzke 2014-11-18]: Remove version 2 support
        if (0 == i->compare("..") && !components.empty()) {
            components.pop_back();
        } else if (0 != i->compare(".")) {
            components.push_back(*i);
        }
#else
        if (0 == i->string().compare("..") && !components.empty()) {
            components.pop_back();
        } else if (0 != i->string().compare(".")) {
            components.push_back(*i);
        }
#endif
    }
    Path result;
    BOOST_FOREACH (const Path &component, components)
        result /= component;
    return result;
}

Path
makeAbsolute(const Path &path, const Path &root) {
#if BOOST_FILESYSTEM_VERSION == 2                       // FIXME[Robb P. Matzke 2014-11-18]: Remove version 2 support
    Path retval;
    if (path.has_root_name()) {
        if (path.has_root_directory()) {
            retval = path;
        } else {
            retval = Path(path.root_name()) / makeAbsolute(root).root_directory() /
                     makeAbsolute(root).relative_path() / path.relative_path();
        }
    } else {
        Path absoluteRoot;
        if (root.has_root_directory()) {
            absoluteRoot = root;
        } else {
            absoluteRoot = makeAbsolute(root);
        }

        if (path.has_root_directory()) {
            retval = absoluteRoot.root_name() / path;
        } else {
            retval = absoluteRoot / path;
        }
    }
    return makeNormal(retval);
#else
    return makeNormal(path.is_absolute() ? path : absolute(root / path));
#endif
}

Path
makeRelative(const Path &path_, const Path &root_) {
    Path path = makeAbsolute(path_);
    Path root = makeAbsolute(root_);

    boost::filesystem::path::const_iterator rootIter = root.begin();
    boost::filesystem::path::const_iterator pathIter = path.begin();

    // Skip past common prefix
    while (rootIter!=root.end() && pathIter!=path.end() && *rootIter==*pathIter) {
        ++rootIter;
        ++pathIter;
    }

    // Return value must back out of remaining A components
    Path retval;
    while (rootIter!=root.end()) {
        if (*rootIter++ != ".")
            retval /= "..";
    }

    // Append path components
    while (pathIter!=path.end())
        retval /= *pathIter++;
    return retval;
}

std::vector<Path>
findNames(const Path &root) {
    return findNames(root, isExisting);
}

std::vector<Path>
findNamesRecursively(const Path &root) {
    return findNamesRecursively(root, isExisting, isDirectory);
}

// Copies files to dstDir so that their name relative to dstDir is the same as their name relative to root
void
copyFiles(const std::vector<Path> &fileNames, const Path &root, const Path &dstDir) {
    std::set<Path> dirs;
    BOOST_FOREACH (const Path &fileName, fileNames) {
        Path dirName = dstDir / makeRelative(fileName.parent_path(), root);
        if (dirs.insert(dirName).second)
            boost::filesystem::create_directories(dirName);
        boost::filesystem::copy_file(fileName, dirName / fileName.filename());
    }
}

std::vector<Path>
findRoseFilesRecursively(const Path &root) {
    return findNamesRecursively(root, baseNameMatches(boost::regex("rose_.*")), isDirectory);
}

// Don't use this if you can help it!
std::string
toString(const Path &path) {
#if BOOST_FILESYSTEM_VERSION == 2                       // FIXME[Robb P. Matzke 2014-11-18]: Remove version 2 support
    return path.string();
#else
    return path.generic_string();
#endif
}

} // namespace
} // namespace
