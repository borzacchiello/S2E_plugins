//
// Created by luca on 19/12/18.
//

#ifndef LIBS2EPLUGINS_ENFAL_H
#define LIBS2EPLUGINS_ENFAL_H
#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/Core/BaseInstructions.h>
#include <s2e/S2EExecutionState.h>

namespace s2e {
namespace plugins {

typedef struct CMDS {
    bool cmd_01;
    bool cmd_02;
    bool cmd_03;
    bool cmd_04;
    bool cmd_05;
    bool cmd_06;
    bool cmd_07;
    bool cmd_08;
    bool cmd_09;
    bool cmd_10;
    bool cmd_11;
    bool cmd_12;
    bool cmd_13;
    bool cmd_14;
    bool cmd_15;
} CMDS;

typedef enum BORZ_ENFAL_COMMAND {
    NOTIFY_THREAD_ADDRESS
} BORZ_ENFAL_COMMAND;

typedef struct THREAD_ADDRESS {
    unsigned int address; // 4byte address
} THREAD_ADDRESS;

typedef struct EnfalGuestCommand {
    BORZ_ENFAL_COMMAND cmd;
    union {
        THREAD_ADDRESS thread_address;
    };
} EnfalGuestCommand;

class ProcessExecutionDetector;
class ENFAL : public Plugin, public BaseInstructionsPluginInvokerInterface {
    S2E_PLUGIN
public:

    ENFAL(S2E *s2e) : Plugin(s2e) {
    }
    void initialize();

    // INVOKER STUFF
    virtual void handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize);
    // *************

private:
    bool  debug;
    unsigned int thread_address;
    ProcessExecutionDetector *m_procDetector;
    std::map<S2EExecutionState*, int> loopCount;

    CMDS flags;

    bool limit_loop;

    void onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb, uint64_t pc);
    void onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
            const std::vector<klee::ref<klee::Expr>> &);

    void do_killState(S2EExecutionState *state, uint64_t pc);
    void do_logName(S2EExecutionState *state, uint64_t pc);
    void update_loopCount(S2EExecutionState *state, uint64_t pc);
    void otherCommandsCallback(S2EExecutionState *state, uint64_t pc);
    void echoCallback(S2EExecutionState *state, uint64_t pc);
    std::string addrToMessage(uint64_t pc);
};

} // namespace plugins
} // namespace s2e

#endif //LIBS2EPLUGINS_ENFAL_H
