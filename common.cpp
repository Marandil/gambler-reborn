//
// Created by marandil on 30.09.16.
//

#include <cstdio>
#include <string>
#include <fstream>
#include <streambuf>

std::string system_output(std::string cmd)
{
    std::string tmpfile = tmpnam(nullptr);
    cmd = cmd + " > " + tmpfile;
    system(cmd.c_str());
    std::ifstream t(tmpfile);
    std::string buffer;
    
    t.seekg(0, std::ios::end);
    buffer.reserve(t.tellg());
    t.seekg(0, std::ios::beg);
    
    buffer.assign((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
    
    return buffer;
}
