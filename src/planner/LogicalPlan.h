/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef PLANNER_LOGICALPLAN_H_
#define PLANNER_LOGICALPLAN_H_

#include <memory>
#include <vector>

#include "planner/PlanNode.h"

namespace nebula {
namespace graph {

class ExecutionPlan;
class Executor;

class LogicalPlan {
public:
    LogicalPlan(std::unique_ptr<PlanNode> start);

    // Convert logical plan to physical(execution) plan
    std::vector<std::shared_ptr<ExecutionPlan>> createExecutionPlans();

private:
    static std::shared_ptr<Executor> generateExecuteNode(const PlanNode &planNode);

    void traverseLogicalPlanReversely(const PlanNode &node,
                                      ExecutionPlan *currExecPlan,
                                      std::vector<std::shared_ptr<ExecutionPlan>> *allPlans);
    std::shared_ptr<PlanNode> getLastDummyNode(const std::shared_ptr<PlanNode> &node) const;
    std::shared_ptr<PlanNode> LogicalPlan::getLastDummyNodeInner(
            const std::shared_ptr<PlanNode> &node,
            std::vector<PlanNode *> &visited) const;
    void makeUnionExecNode(const PlanNode &node,
                           ExecutionPlan *currExecPlan,
                           std::vector<std::shared_ptr<ExecutionPlan>> *allPlans);
    void makeIntersectExecNode(const PlanNode &node,
                               ExecutionPlan *currExecPlan,
                               std::vector<std::shared_ptr<ExecutionPlan>> *allPlans);
    void makeMinusExecNode(const PlanNode &node,
                           ExecutionPlan *currExecPlan,
                           std::vector<std::shared_ptr<ExecutionPlan>> *allPlans);

    // The unique start plan node of logical plan graph
    std::shared_ptr<PlanNode> start_;
}   // namespace graph
}   // namespace graph
#endif   // PLANNER_LOGICALPLAN_H_
