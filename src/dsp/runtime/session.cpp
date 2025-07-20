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

#include "session.hpp"

namespace racs::runtime {

template<typename T>
session<T>::session() : thread_pool_(thread_pool_singleton<T>::instance()){}

template<typename T>
callback<T> session<T>::execute_tasks(task_list<T> &&tasks) {
    callback<T> callback;

    for (auto&& task : tasks) {
        auto future = thread_pool_.submit(std::move(task));
        callback.push_back(std::move(future));
    }

    return callback;
}

template class session<tensor<1, float>>;
template class session<tensor<1, std::complex<float>>>;
template class session<tensor<2, float>>;
template class session<tensor<2, std::complex<float>>>;
}