//
// Created by luca on 07/12/18.
//

#ifndef SOURCE_WRAPREAD_H
#define SOURCE_WRAPREAD_H
namespace s2e {
namespace plugins {

class WrapRead : public Plugin {
    S2E_PLUGIN

public:
    WrapRead(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

private:

    void onTranslateInstructionEnd(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb,
         uint64_t pc, uint64_t addend);

    void onInterrupt(S2EExecutionState *state, uint64_t pc);
};


} // namespace plugins
} // namespace s2e

#endif //SOURCE_WRAPREAD_H
