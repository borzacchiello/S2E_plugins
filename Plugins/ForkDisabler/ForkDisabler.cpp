//
// Created by luca on 30/10/19.
//
#include "ForkDisabler.h"
#include <s2e/S2E.h>

namespace s2e {
namespace plugins {

    S2E_DEFINE_PLUGIN(TerminateForkedStates,
                      "Terminate forked states immediately",
                      "TerminateForkedStates");

    void TerminateForkedStates::initialize() {
            s2e()->getCorePlugin()->onStateFork.connect(
                        sigc::mem_fun(*this, &TerminateForkedStates::handleStateFork));
        }

    void TerminateForkedStates::handleStateFork(S2EExecutionState* original,
                                                const std::vector<S2EExecutionState*>& newStates,
                                                const std::vector<klee::ref<klee::Expr>>& newConditions)
    {
        for (auto state : newStates) {
            if (state == original)
                continue;

            s2e()->getExecutor()->terminateStateEarly(
                *state, "Concolic execution - only following the original path");
        }
    }

} // namespace plugins
} // namespace s2e
