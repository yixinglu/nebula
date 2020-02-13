/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "planner/LogicalPlan.h"

#include "planner/ExecutionPlan.h"

namespace nebula {
namespace graph {

LogicalPlan::LogicalPlan(std::shared_ptr<PlanNode> start) : start_(start) {}

// For scheduling execution plans parallelly, Split following plan node to different execution node
// which shares data by memory:
//  - Union
//  - Intersect
//  - Minus
std::vector<std::shared_ptr<ExecutionPlan>> LogicalPlan::createExecutionPlans() {
    std::vector<std::shared_ptr<ExecutionPlan>> execPlans;
    auto last = getLastDummyNode(start_);
    // Actually, the last exec node would be yield or project node
    for (auto &node : last->prevTable()) {
        if (!node || node->kind() != PlanNode::kProject) {
            LOG(FATAL) << "invalid plan node";
        }

        auto plan = std::make_shared<ExecutionPlan>();
        // Add execute node to plan
        plan->append(generateExecuteNode(*n));   // TODO(yee): Implement append function
        allPlans->push_back(plan);
        traverseLogicalPlanReversely(*node, plan.get(), &execPlans);
    }
    return execPlans;
}

void LogicalPlan::traverseLogicalPlanReversely(
        const PlanNode &node,
        ExecutionPlan *currExecPlan,
        std::vector<std::shared_ptr<ExecutionPlan>> *allPlans) {
    if (node.kind() == PlanNode::kUnknown) {
        LOG(FATAL) << "Invalid plan node kind";
    }

    if (node.kind() == PlanNode::kStart) {
        return;
    }

    if (node.kind() == PlanNode::kUnion) {
        makeUnionExecNode(node, currExecPlan, allPlans);
        return;
    }

    if (node.kind() == PlanNode::kIntersect) {
        makeIntersectExecNode(node, currExecPlan, allPlans);
        return;
    }

    if (node.kind() == PlanNode::kMinus) {
        makeMinusExecNode(node, currExecPlan, allPlans);
        return;
    }

    currExecPlan->append(generateExecuteNode(node));
    auto table = node.prevTable();
    // TODO(yee): FindShortestPath don't match this pattern
    DCHECK_EQ(table.size(), 1U);
    traverseLogicalPlanReversely(table.back(), currExecPlan, allPlans);
}

// static
std::shared_ptr<Executor> LogicalPlan::generateExecuteNode(const PlanNode &planNode) {
    return Executor::make(planNode.kind());
}

std::shared_ptr<PlanNode> LogicalPlan::getLastDummyNode(
        const std::shared_ptr<PlanNode> &node) const {
    std::vector<PlanNode *> visited;
    return getLastDummyNodeInner(node, visited);
}

std::shared_ptr<PlanNode> LogicalPlan::getLastDummyNodeInner(
        const std::shared_ptr<PlanNode> &node,
        std::vector<PlanNode *> &visited) const {
    auto table = node->table();
    if (table.empty()) {
        StateTransition trans;
        trans.setTable(StateTransitionTable{node});
        auto dummy = std::make_shared<PlanNode>();
        dummy->setPreTable(trans);
        return dummy;
    }

    // There may be some loops in the plan
    for (auto it = table.begin(); it != table.end(); ++it) {
        if (visited.find(it->get()) == visited.end()) {
            visited.push_back(it->get());
            return getLastDummyNodeInner(*it, visited);
        }
    }

    LOG(FATAL) << "This method can not find a valid plan node";
    return nullptr;
}

void LogicalPlan::makeUnionExecNode(const PlanNode &node,
                                    ExecutionPlan *currExecPlan,
                                    std::vector<std::shared_ptr<ExecutionPlan>> *allPlans) {
    auto prevTable = node.prevTable();
    DCHECK_EQ(prevTable.size(), 2U);

    // union build side
    // TODO(yee): Implement union build side executor
    currExecPlan->append(Executor::make("UnionBuild"));
    traverseLogicalPlanReversely(prevTable.front(), currExecPlan, allPlans);

    // union probe side
    auto plan = std::make_shared<ExecutionPlan>();
    // TODO(yee): implement probe side union executor
    plan->append(Executor::make("UnionProbe"));
    allPlans->push_back(plan);
    traverseLogicalPlanReversely(prevTable.back(), plan.get(), allPlans);

    // Organize dependency relationship of exec plans
    plan->addDependedPlan(currExecPlan);
}

void LogicalPlan::makeIntersectExecNode(const PlanNode &node,
                                        ExecutionPlan *currExecPlan,
                                        std::vector<std::shared_ptr<ExecutionPlan>> *allPlans) {
    auto prevTable = node.prevTable();
    DCHECK_EQ(prevTable.size(), 2U);

    // Intersect build side
    // TODO(yee): Implement intersect build side executor
    currExecPlan->append(Executor::make("Intersect"));
    traverseLogicalPlanReversely(prevTable.front(), currExecPlan, allPlans);

    // Intersect probe side
    auto plan = std::make_shared<ExecutionPlan>();
    // TODO(yee): implement probe side intersect executor
    plan->append(Executor::make("Intersect"));
    allPlans->push_back(plan);
    traverseLogicalPlanReversely(prevTable.back(), plan.get(), allPlans);

    // Organize dependency relationship of exec plans
    plan->addDependedPlan(currExecPlan);
    return;
}

void LogicalPlan::makeMinusExecNode(const PlanNode &node,
                                    ExecutionPlan *currExecPlan,
                                    std::vector<std::shared_ptr<ExecutionPlan>> *allPlans) {
    auto prevTable = node.prevTable();
    DCHECK_EQ(prevTable.size(), 2U);

    // Minus build side
    // TODO(yee): Implement intersect build side executor
    currExecPlan->append(Executor::make("Minus"));
    traverseLogicalPlanReversely(prevTable.front(), currExecPlan, allPlans);

    // Minus probe side
    auto plan = std::make_shared<ExecutionPlan>();
    // TODO(yee): implement probe side minus executor
    plan->append(Executor::make("Minus"));
    allPlans->push_back(plan);
    traverseLogicalPlanReversely(prevTable.back(), plan.get(), allPlans);

    // Organize dependency relationship of exec plans
    plan->addDependedPlan(currExecPlan);
    return;
}

}   // namespace graph
}   // namespace nebula
