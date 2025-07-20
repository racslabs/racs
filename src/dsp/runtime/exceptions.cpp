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

#include "exceptions.hpp"

#include <utility>

namespace racs::runtime {

zf100_exception::zf100_exception(std::string message) :
        message_(std::move(message)) {
    snprintf(buff_, 256, "[ZF100E] Invalid shape: %s", message_.c_str());
    std::cerr << buff_ << std::endl;
}

char *zf100_exception::what() {
    return buff_;
}

zf101_exception::zf101_exception(std::string message) :
        message_(std::move(message)) {
    snprintf(buff_, 256, "[ZF101E] Incompatible shape: %s", message_.c_str());
    std::cerr << buff_ << std::endl;
}

char *zf101_exception::what() {
    return buff_;
}

zf102_exception::zf102_exception(std::string message) :
        message_(std::move(message)) {
    snprintf(buff_, 256, "[ZF102E] Out of range: %s", message_.c_str());
    std::cerr << buff_ << std::endl;
}

char *zf102_exception::what() {
    return buff_;
}

zf103_exception::zf103_exception(std::string message) :
        message_(std::move(message)) {
    snprintf(buff_, 256, "[ZF103E] Unsupported operation: %s", message_.c_str());
    std::cerr << buff_ << std::endl;
}

char *zf103_exception::what() {
    return buff_;
}

zf104_exception::zf104_exception(std::string message) :
        message_(std::move(message)) {
    snprintf(buff_, 256, "[ZF104E] Invalid dtype: %s", message_.c_str());
    std::cerr << buff_ << std::endl;
}

char *zf104_exception::what() {
    return buff_;
}

zf105_exception::zf105_exception(std::string message) :
        message_(std::move(message)) {
    snprintf(buff_, 256, "[ZF105E] IO Exception: %s", message_.c_str());
    std::cerr << buff_ << std::endl;
}

char *zf105_exception::what() {
    return buff_;
}

}