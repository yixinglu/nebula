/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "scheduler/Scheduler.h"

#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/executors/task_queue/UnboundedBlockingQueue.h>

namespace nebula {
namespace graph {

Scheduler::Scheduler(uint32_t numThreads) {
    auto taskQueue =
        std::make_unique<folly::UnboundedBlockingQueue<folly::CPUThreadPoolExecutor::CPUTask>>();
    threadPool_ = std::make_unique<folly::CPUThreadPoolExecutor>(
        numThreads,
        std::move(taskQueue),
        std::make_shared<folly::NamedThreadFactory>("graph-scheduler"));
}

void Scheduler::addTask(std::function<void()> task) {
    threadPool_->add(task);
}

}   // namespace graph
}   // namespace nebula
