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

#define OFFSET_AVOID 0x599
#define OFFSET_AVOID_2 0x799
#define OFFSET_AVOID_3 0x7D5
#define OFFSET_AVOID_4 0xF79
#define OFFSET_DISPATCH 0x573
#define OFFSET_OTHER_COMMANDS 0x873
#define OFFSET_TARGET 0x7E4
#define OFFSET_END_LOOP 0x1099

namespace s2e {
namespace plugins {

using namespace llvm;

S2E_DEFINE_PLUGIN(ENFAL, "ENFAL malware analysis plugin", "ENFAL", "ProcessExecutionDetector");

std::string ENFAL::addrToMessage(uint64_t pc) {
    if (pc == this->thread_address + OFFSET_AVOID)
        return "AVOID ADDRESS";
    else if (pc == this->thread_address + OFFSET_DISPATCH)
        return "DISPATCH ADDRESS";
    else if (pc == this->thread_address + OFFSET_OTHER_COMMANDS)
        return "OTHER COMMANDS DISPATCH";
    else if (pc == this->thread_address + OFFSET_AVOID_2)
        return "AVOID ADDRESS 2 - \"@@\" check failed";
    else if (pc == this->thread_address + OFFSET_AVOID_3)
        return "AVOID ADDRESS 3 - \"8040$(\" check failed";
    else if (pc == this->thread_address + OFFSET_AVOID_4)
        return "AVOID ADDRESS 4 - You shouldn't arrive here at this point";
    else if (pc == this->thread_address + OFFSET_TARGET)
        return "TARGET REACHED";
    else if (pc == this->thread_address + OFFSET_END_LOOP)
        return "LOOP END";
    else
        return "UNKNOWN";
}

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
    if (pc == thread_address + OFFSET_AVOID) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_AVOID_2) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_AVOID_3) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_AVOID_4) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_OTHER_COMMANDS) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_END_LOOP) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
}

void ENFAL::do_killState(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    std::ostringstream os;
    os << "The state was terminated by ENFAL plugin\n" <<
        "    Address: " << std::hex << pc <<
        "    Message: " << addrToMessage(pc) << "\n";
    s2e()->getExecutor()->terminateStateEarly(*state, os.str());
}


} // namespace plugins
} // namespace s2e
