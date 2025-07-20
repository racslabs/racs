/*
 * Copyright (c) 2024 Computer Music Research Lab (https://cmrlab.ai)
 *
 * This file is part of ZForm.
 *
 * ZForm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZForm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZForm. If not, see <http://www.gnu.org/licenses/>.
 *
 * If you are using this library in a commercial, closed-source, or
 * proprietary project, you must purchase a ZForm license.
 */

#ifndef RACS_EXCEPTIONS_HPP
#define RACS_EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <sstream>
#include <iostream>

namespace racs::runtime {

class zf100_exception : public std::exception {

public:
    explicit zf100_exception(std::string message);

    char * what();

private:
    char buff_[256];

    std::string message_;

};

class zf101_exception : public std::exception {

public:
    explicit zf101_exception(std::string message);

    char * what();

private:
    char buff_[256];

    std::string message_;
};

class zf102_exception : public std::exception {

public:
    explicit zf102_exception(std::string message);

    char * what();

private:
    char buff_[256];

    std::string message_;
};

class zf103_exception : public std::exception {

public:
    explicit zf103_exception(std::string message);

    char * what();

private:
    char buff_[256];

    std::string message_;
};

class zf104_exception : public std::exception {

public:
    explicit zf104_exception(std::string  message);

    char * what();

private:
    char buff_[256];

    std::string message_;
};

class zf105_exception : public std::exception {

public:
    explicit zf105_exception(std::string  message);

    char * what();

private:
    char buff_[256];

    std::string message_;
};

}

#endif //RACS_EXCEPTIONS_HPP
