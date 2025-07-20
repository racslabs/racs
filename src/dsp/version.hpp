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

#ifndef RACS_VERSION_HPP
#define RACS_VERSION_HPP

#include <iostream>
#include <iomanip>

#include "runtime/cpu_info.hpp"

namespace racs {

#define RACS_VERSION_MAJOR 1
#define RACS_VERSION_MINOR 0
#define RACS_VERSION_PATCH 0
#define RACS_VERSION_TAG "beta"

#define RACS_RELEASE_YEAR 2024
#define RACS_RELEASE_MONTH 6
#define RACS_RELEASE_DAY 22

#define RACS_VERSION(major, minor, patch, tag) \
    std::to_string(major) + "." +               \
    std::to_string(minor) + "." +               \
    std::to_string(patch) + "-" +               \
    tag

#define RACS_RELEASE_DATE(year, month, day) \
    (std::ostringstream{} << year << "-"     \
                          << std::setfill('0') << std::setw(2) << month << "-" \
                          << std::setfill('0') << std::setw(2) << day).str()


RACS_INLINE std::string version() {
    std::ostringstream oss;

    oss << "racs \"v"
        << RACS_VERSION(RACS_VERSION_MAJOR, RACS_VERSION_MINOR, RACS_VERSION_PATCH, RACS_VERSION_TAG)
        << "\" " << RACS_RELEASE_DATE(RACS_RELEASE_YEAR, RACS_RELEASE_MONTH, RACS_RELEASE_DAY)
        << std::endl << "arch: " << runtime::arch()
        << std::endl << runtime::simd();

    return oss.str();
}

}

#endif //RACS_VERSION_HPP
