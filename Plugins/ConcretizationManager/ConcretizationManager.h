//
// Created by luca on 08/01/19.
//

#ifndef SOURCE_CONCRETIZATIONMANAGER_H
#define SOURCE_CONCRETIZATIONMANAGER_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>

namespace s2e {
namespace plugins {

class ProcessExecutionDetector;

class ConcretizationManager : public Plugin {
    S2E_PLUGIN

public:
    // 1 for true, 0 for false
    int concWrite;
    int concRead;
    int writeToMax;
    int readToMax;

    ConcretizationManager(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

private:
    ProcessExecutionDetector *m_procDetector;

    void onBeforeSymbolicDataMemoryAccess(S2EExecutionState *state, klee::ref<klee::Expr> addr,
                                          klee::ref<klee::Expr> value, bool isWrite);

    void do_concretizeAddress(S2EExecutionState *state, klee::ref<klee::Expr> addr, bool toMax);
};

} // namespace plugins
} // namespace s2e

#endif //SOURCE_CONCRETIZATIONMANAGER_H
