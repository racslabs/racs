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

#include "thread_pool.hpp"

namespace racs::runtime {

template<typename T>
std::future<T> thread_pool<T>::submit(std::function<T()>&& task) {
    packaged_task p_task(std::move(task));

    auto future = p_task.get_future();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.emplace(std::move(p_task));
    }

    condition_.notify_one();

    return future;
}

template<typename T>
thread_pool<T>::thread_pool(size_t num_threads) {
    threads_.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back(&thread_pool<T>::thread_main, this);
    }
}

template<typename T>
thread_pool<T>::thread_pool() : thread_pool(MAX_THREADS) {}

template<typename T>
void thread_pool<T>::thread_main() {
    while (!shutdown_) {
        packaged_task task;

        {
            std::unique_lock<std::mutex> lock(mutex_);

            condition_.wait(lock, [this] {
                return !tasks_.empty() || shutdown_;
            });

            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
            }
        }

        if (task.valid()) {
            task();
        }
    }
}

template<typename T>
thread_pool<T>::~thread_pool() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
    }

    condition_.notify_all();

    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

template<typename T>
thread_pool<T>& thread_pool_singleton<T>::instance() {
    std::call_once(once_flag_, [&]() {
        instance_ = std::make_unique<thread_pool<T>>();
    });

    return *instance_;
}

template class thread_pool<tensor<1, float>>;
template class thread_pool_singleton<tensor<1, float>>;
template class thread_pool<tensor<1, std::complex<float>>>;
template class thread_pool_singleton<tensor<1, std::complex<float>>>;

template class thread_pool<tensor<2, float>>;
template class thread_pool_singleton<tensor<2, float>>;
template class thread_pool<tensor<2, std::complex<float>>>;
template class thread_pool_singleton<tensor<2, std::complex<float>>>;


}