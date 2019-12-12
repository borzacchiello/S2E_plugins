//
// Created by luca on 19/12/18.
//

#ifndef LIBS2EPLUGINS_KILLAFTERN_H
#define LIBS2EPLUGINS_KILLAFTERN_H
#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/Core/BaseInstructions.h>
#include <s2e/S2EExecutionState.h>

namespace s2e {
namespace plugins {

typedef enum BORZ_KILLAFTERN_COMMAND {
    START_COUNT
} BORZ_KILLAFTERN_COMMAND;


typedef struct KillAfterNGuestCommand {
    BORZ_KILLAFTERN_COMMAND cmd;
    union {
        int num_instructions;
    };
} KillAfterNGuestCommand;

class ProcessExecutionDetector;
class KillAfterN : public Plugin, public BaseInstructions::IPluginInvoker {
    S2E_PLUGIN
public:

    KillAfterN(S2E *s2e) : Plugin(s2e) {
    }
    void initialize();

    // INVOKER STUFF
    virtual void handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize);
    // *************

private:
    bool debug;
    bool counting;
    int num_instructions;
    ProcessExecutionDetector *m_procDetector;
    std::map<int, int> instructionCount;

    void onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb, uint64_t pc);
    void onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
                     const std::vector<klee::ref<klee::Expr>> &);
    void do_killState(S2EExecutionState *state, uint64_t pc);
};

} // namespace plugins
} // namespace s2e

#endif //LIBS2EPLUGINS_KILLAFTERN_H
