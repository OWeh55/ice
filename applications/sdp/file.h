#ifndef SDP_FILE_H
#define SDP_FILE_H

#include <vector>
#include <string>

bool fileexists(const std::string& fn);
void splitFileName(const std::string& fname,
                   std::string& path, std::string& basename, std::string& extension);
std::string makeFileName(const std::string& path,
                         const std::string& basename,
                         const std::string& extension);
std::string findFile(const std::string& name, std::vector<std::string>& pathlist);

#endif
