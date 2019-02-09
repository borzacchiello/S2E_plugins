//
// Created by luca on 07/02/19.
//

#ifndef S2E_INSTRUCTIONTRACER_H
#define S2E_INSTRUCTIONTRACER_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>

namespace s2e {
namespace plugins {

    class ProcessExecutionDetector;

    class InstructionTracer : public Plugin {
        S2E_PLUGIN

    public:
        InstructionTracer(S2E *s2e) : Plugin(s2e) {
        }

        void initialize();

    private:
        ProcessExecutionDetector *m_procDetector;
        std::string log_filename;
        std::ofstream log;
        uint64_t begin_address;
        uint64_t end_address;

        void onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                    TranslationBlock *tb, uint64_t pc);
        void onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
                         const std::vector<klee::ref<klee::Expr>> &);

        void logOnFile(S2EExecutionState *state, uint64_t pc);
    };
}
}


#endif //S2E_INSTRUCTIONTRACER_H
