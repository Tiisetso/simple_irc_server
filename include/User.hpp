#pragma once

#include <string>

class User
{
public:
    int Fd;
    std::string readBuffer;

    User(int fd = -1);
    ~User();
};