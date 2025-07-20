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

#ifndef RACS_THREAD_POOL_HPP
#define RACS_THREAD_POOL_HPP

#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include "../core/tensor.hpp"
#include "../simd/constants.hpp"

namespace racs::runtime {

#define MAX_THREADS std::thread::hardware_concurrency()

template<typename T>
class thread_pool {

public:
    using packaged_task = std::packaged_task<T()>;

    thread_pool();

    explicit thread_pool(size_t num_threads);

    virtual ~thread_pool();

    std::future<T> submit(std::function<T()>&& task);

    void thread_main();

private:
    std::queue<packaged_task> tasks_;

    std::mutex mutex_;

    std::condition_variable condition_;

    std::vector<std::thread> threads_;

    std::atomic<bool> shutdown_{};
};

template <typename T>
class thread_pool_singleton {

public:
    static thread_pool<T>& instance();

private:
    static std::unique_ptr<thread_pool<T>> instance_;

    static std::once_flag once_flag_;
};

template <typename T>
std::unique_ptr<thread_pool<T>> thread_pool_singleton<T>::instance_;

template <typename T>
std::once_flag thread_pool_singleton<T>::once_flag_;

}

#endif //RACS_THREAD_POOL_HPP
