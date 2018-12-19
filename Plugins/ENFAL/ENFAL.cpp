#include <s2e/cpu.h>
#include <s2e/opcodes.h>

#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>
#include <s2e/ConfigFile.h>
#include <s2e/S2E.h>
#include <s2e/S2EExecutor.h>
#include <s2e/Utils.h>

#include <iostream>

#include "ENFAL.h"

#define DEBUG_PRINT(inst) if (debug) {inst}

namespace s2e {
namespace plugins {

using namespace llvm;

S2E_DEFINE_PLUGIN(ENFAL, "ENFAL malware analysis plugin", "ENFAL", "ProcessExecutionDetector");

void ENFAL::initialize() {
    thread_address = 0;
    debug = true;// s2e()->getConfig()->getBool(getConfigKey() + ".debug");

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();
    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
            sigc::mem_fun(*this, &ENFAL::onTranslateInstruction));
}

void ENFAL::handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize) {
    EnfalGuestCommand command;

    if (guestDataSize != sizeof(command)) {
        getWarningsStream(state) << "mismatched S2E_HOOK_PLUGIN_COMMAND size\n";
        exit(-1);
    }

    if (!state->mem()->read(guestDataPtr, &command, guestDataSize)) {
        getWarningsStream(state) << "could not read transmitted data\n";
        exit(-1);
    }

    switch (command.cmd) {
        case NOTIFY_THREAD_ADDRESS: {
            this->thread_address = command.thread_address.address;

            DEBUG_PRINT (
                    std::ostringstream message;
                    message << "Thread_address received: 0x" << std::hex << this->thread_address << "\n";
                    getInfoStream(state) << message.str();
            )
        } break;

        default: { getWarningsStream(state) << "unknown command\n"; } break;
    }
}

void ENFAL::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
        TranslationBlock *tb, uint64_t pc) {
    if (thread_address == 0) return;
    if (pc == thread_address) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
}

void ENFAL::do_killState(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    std::ostringstream os;
    os << "State at: 0x" << std::hex << pc << " was terminated by ENFAL plugin\n";
    s2e()->getExecutor()->terminateStateEarly(*state, os.str());
}


} // namespace plugins
} // namespace s2e
