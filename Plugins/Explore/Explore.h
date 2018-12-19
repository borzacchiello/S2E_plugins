//
// Created by luca on 04/12/18.
//

#ifndef SOURCE_EXPLORE_H
#define SOURCE_EXPLORE_H
#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>

namespace s2e {
    namespace plugins {

        class ProcessExecutionDetector;

        class Explore : public Plugin {
            S2E_PLUGIN

        public:
            Explore(S2E *s2e) : Plugin(s2e) {
            }

            void initialize();

        private:
            ProcessExecutionDetector *m_procDetector;

            void onSymbolicVariableCreation(S2EExecutionState *state, const std::string &name,
                                            const std::vector<klee::ref<klee::Expr>> &expr, const klee::MemoryObject *mo,
                                            const klee::Array *array);
            void onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb, uint64_t pc);
            void onSuccess(S2EExecutionState *state, uint64_t pc);
            void onFailure(S2EExecutionState *state, uint64_t pc);
        };

    } // namespace plugins
} // namespace s2e

#endif