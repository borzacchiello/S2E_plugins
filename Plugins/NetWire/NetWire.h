//
// Created by luca on 19/12/18.
//

#ifndef LIBS2EPLUGINS_NetWire_H
#define LIBS2EPLUGINS_NetWire_H
#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/Core/BaseInstructions.h>
#include <s2e/S2EExecutionState.h>
#include "commands.h"


namespace s2e {
namespace plugins {

class InstructionTrackerState: public PluginState
{
private:
    int m_count;

public:
    InstructionTrackerState() {
        m_count = 0;
    }

    ~InstructionTrackerState() {}

    static PluginState *factory(Plugin*, S2EExecutionState*) {
        return new InstructionTrackerState();
    }

    InstructionTrackerState *clone() const {
        return new InstructionTrackerState(*this);
    }

    void increment() { ++m_count; }
    int get() { return m_count; }

};

class ProcessExecutionDetector;
class NetWire : public Plugin {
    S2E_PLUGIN
public:

    NetWire(S2E *s2e) : Plugin(s2e) {
    }
    void initialize();

private:
    bool counting;
    bool debug;
    bool limit_instruction;
    int instruction_threshold;
    cmd_flags flags;
    ProcessExecutionDetector *m_procDetector;

    std::map<S2EExecutionState*, int> loopCount;
    std::map<S2EExecutionState*, int> count_loop_recv;
    void onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb, uint64_t pc);
    void onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
                     const std::vector<klee::ref<klee::Expr>> &);

    void do_checkRecvLoopCout(S2EExecutionState *state, uint64_t pc);
    void do_incrementCounter(S2EExecutionState *state, uint64_t pc);
    void update_loopCount(S2EExecutionState *state, uint64_t pc);
    void do_checkValidity(S2EExecutionState *state, uint64_t pc);
    void do_checkValidity2(S2EExecutionState *state, uint64_t pc);
    void do_killState(S2EExecutionState *state, uint64_t pc);
    void do_logName(S2EExecutionState *state, uint64_t pc);
    std::string addrToMessage(uint64_t pc);
};

} // namespace plugins
} // namespace s2e

#endif //LIBS2EPLUGINS_NetWire_H
