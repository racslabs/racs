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

#ifndef RACS_GRAPH_SESSION_HPP
#define RACS_GRAPH_SESSION_HPP

#include "thread_pool.hpp"

namespace racs::runtime {

template<typename U>
using task_list = std::vector<std::function<U()>>;

template<typename U>
using callback = std::vector<std::future<U>>;

template<typename T>
class session {

public:
    session();

    callback<T> execute_tasks(task_list<T>&& tasks);

private:
    thread_pool<T>& thread_pool_;

};

}

#endif //RACS_GRAPH_SESSION_HPP
