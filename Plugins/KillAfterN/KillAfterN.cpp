#include <s2e/cpu.h>
#include <s2e/opcodes.h>
#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>
#include <s2e/ConfigFile.h>
#include <s2e/S2E.h>
#include <s2e/S2EExecutor.h>
#include <s2e/Utils.h>
#include <iostream>
#include "KillAfterN.hpp"

#define DEBUG_PRINT(inst) if (debug) {inst}

namespace s2e {
namespace plugins {

using namespace llvm;

S2E_DEFINE_PLUGIN(KillAfterN, "Kill states after N instructions", "KillAfterN", "ProcessExecutionDetector");


void KillAfterN::initialize() {
    debug = true;// s2e()->getConfig()->getBool(getConfigKey() + ".debug");
    num_instructions = s2e()->getConfig()->getInt(getConfigKey() + ".num_instructions");
    counting = false;

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();
    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
            sigc::mem_fun(*this, &KillAfterN::onTranslateInstruction));
    s2e()->getCorePlugin()->onStateFork.connect(
            sigc::mem_fun(*this, &KillAfterN::onStateFork));
}

void KillAfterN::handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize) {
    KillAfterNGuestCommand command;

    if (guestDataSize != sizeof(command)) {
        getWarningsStream(state) << "mismatched S2E_HOOK_PLUGIN_COMMAND size\n";
        exit(-1);
    }

    if (!state->mem()->read(guestDataPtr, &command, guestDataSize)) {
        getWarningsStream(state) << "could not read transmitted data\n";
        exit(-1);
    }

    switch (command.cmd) {
        case START_COUNT: {
            counting = true;
            num_instructions = command.num_instructions==0 ? num_instructions : command.num_instructions;
            DEBUG_PRINT (
                std::ostringstream message;
                message << "Counting started at: 0x" << std::hex << state->regs()->getPc(); << "\n";
                getInfoStream(state) << message.str();
            )
        } break;

        default: { getWarningsStream(state) << "unknown command\n"; } break;
    }
}

void KillAfterN::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                   TranslationBlock *tb, uint64_t pc) {
    if (!counting) return;

    if (!instructionCount.count(state))
        instructionCount[state] = 1;
    else
        instructionCount[state] += 1;

    if (instructionCount[state] > num_instructions)
        do_killState(state, pc);
}

void KillAfterN::onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
                        const std::vector<klee::ref<klee::Expr>> &) {
    if (!instructionCount.count(oldState)) return;

    int count = instructionCount[oldState];

    std::vector<S2EExecutionState *>::const_iterator it;
    for (it = newStates.begin(); it != newStates.end(); ++it)
        instructionCount[*it] = count;
}

void KillAfterN::do_killState(S2EExecutionState *state, uint64_t pc) {

    std::ostringstream os;
    os << "The state was terminated by KillAfterN plugin\n" <<
       "    Address: " << std::hex << pc << "\n";
    s2e()->getExecutor()->terminateStateEarly(*state, os.str());
}

} // namespace plugins
} // namespace s2e
