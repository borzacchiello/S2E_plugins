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
#define OFFSET_TARGET -1
#define OFFSET_END_LOOP 0x1099

// COMMANDS
#define OFFSET_ECHO 0x76A
#define OFFSET_IPOP_LOAD_CHECK 0x88D
#define OFFSET_PING 0x8D0
#define OFFSET_SEND_FILE 0x8E2
#define OFFSET_RECV_FILE 0x93E
#define OFFSET_CMDEXEC 0x9AF
#define OFFSET_DELETE_FILE 0x9D8
#define OFFSET_MOVE_FILE 0xA01
#define OFFSET_LS 0xA52
#define OFFSET_INTERACTIVE_MODE 0xC8A
#define OFFSET_MKDIR 0xCDD
#define OFFSET_RMDIR 0xD8D
#define OFFSET_TERMINATE_PROCESS 0xDB6
#define OFFSET_WINEXEC_NETBN 0xD08
#define OFFSET_WINEXEC_NETDC 0xDFA
#define OFFSET_WINEXEC_NFAL 0xE83

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
    else if (pc == this->thread_address + OFFSET_ECHO)
        return "ECHO COMMAND";
    else if (pc == this->thread_address + OFFSET_IPOP_LOAD_CHECK)
        return "IPOP LOAD CHECK COMMAND";
    else if (pc == this->thread_address + OFFSET_PING)
        return "PING COMMAND";
    else if (pc == this->thread_address + OFFSET_SEND_FILE)
        return "SEND FILE COMMAND";
    else if (pc == this->thread_address + OFFSET_RECV_FILE)
        return "RECV FILE COMMAND";
    else if (pc == this->thread_address + OFFSET_CMDEXEC)
        return "CMDEXEC COMMAND";
    else if (pc == this->thread_address + OFFSET_DELETE_FILE)
        return "DELETE FILE COMMAND";
    else if (pc == this->thread_address + OFFSET_MOVE_FILE)
        return "MOVE FILE COMMAND";
    else if (pc == this->thread_address + OFFSET_LS)
        return "LS COMMAND";
    else if (pc == this->thread_address + OFFSET_INTERACTIVE_MODE)
        return "INTERACTIVE COMMAND";
    else if (pc == this->thread_address + OFFSET_MKDIR)
        return "MKDIR COMMAND";
    else if (pc == this->thread_address + OFFSET_RMDIR)
        return "RMDIR COMMAND";
    else if (pc == this->thread_address + OFFSET_TERMINATE_PROCESS)
        return "TERMINATE PROCESS COMMAND";
    else if (pc == this->thread_address + OFFSET_WINEXEC_NETBN)
        return "WINEXEC NETBN COMMAND";
    else if (pc == this->thread_address + OFFSET_WINEXEC_NETDC)
        return "WINEXEC NETDC COMMAND";
    else if (pc == this->thread_address + OFFSET_WINEXEC_NFAL)
        return "WINEXEC NFAL COMMAND";
    else
        return "UNKNOWN";
}

void ENFAL::initialize() {
    thread_address = 0;
    debug = true;// s2e()->getConfig()->getBool(getConfigKey() + ".debug");

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();
    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
            sigc::mem_fun(*this, &ENFAL::onTranslateInstruction));
    s2e()->getCorePlugin()->onStateFork.connect(
            sigc::mem_fun(*this, &ENFAL::onStateFork));
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
        signal->connect(sigc::mem_fun(*this, &ENFAL::otherCommandsCallback));
    }
    if (pc == thread_address + OFFSET_END_LOOP) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::update_loopCount));
    }
//    if (pc == thread_address + OFFSET_TARGET) {
//        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
//    }
    if (pc == thread_address + OFFSET_ECHO) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::echoCallback));
    }
    // ****** COMMANDS ******
//    if (pc == thread_address + OFFSET_IPOP_LOAD_CHECK) {
//        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
//    }
    if (pc == thread_address + OFFSET_PING) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_SEND_FILE) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_RECV_FILE) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
//    if (pc == thread_address + OFFSET_CMDEXEC) {
//        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
//    }
    if (pc == thread_address + OFFSET_DELETE_FILE) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_MOVE_FILE) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_LS) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_INTERACTIVE_MODE) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_MKDIR) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_RMDIR) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_TERMINATE_PROCESS) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_WINEXEC_NETBN) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_WINEXEC_NETDC) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
    if (pc == thread_address + OFFSET_WINEXEC_NFAL) {
        signal->connect(sigc::mem_fun(*this, &ENFAL::do_killState));
    }
}

void ENFAL::onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
                 const std::vector<klee::ref<klee::Expr>> &) {
    if (!loopCount.count(oldState)) return;

    int count = loopCount[oldState];

    std::vector<S2EExecutionState *>::const_iterator it;
    for (it = newStates.begin(); it != newStates.end(); ++it)
        loopCount[*it] = count;
}

void ENFAL::do_killState(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    std::ostringstream os;
    os << "The state was terminated by ENFAL plugin\n" <<
        "    Address: " << std::hex << pc <<
        "    Message: " << addrToMessage(pc) << "\n";
    s2e()->getExecutor()->terminateStateEarly(*state, os.str());
}

void ENFAL::update_loopCount(S2EExecutionState *state, uint64_t pc) {
    if (!loopCount.count(state))
        loopCount[state] = 1;
    else
        loopCount[state] += 1;

    DEBUG_PRINT (
            std::ostringstream message;
            message << "Updating loopCount of state 0x" << std::hex << state <<
                " to value: " << loopCount[state] << "\n";
            getInfoStream(state) << message.str();
    )

    if (loopCount[state] > 2)
        do_killState(state, pc);
}

void ENFAL::otherCommandsCallback(S2EExecutionState *state, uint64_t pc) {
    if (!loopCount.count(state) || loopCount[state] < 2) {
        do_killState(state, pc);
        return;
    }
    DEBUG_PRINT (
        std::ostringstream message;
        message << "state 0x" << std::hex << state <<
                " in _real_ command dispatcher\n";
        getInfoStream(state) << message.str();
    )
}

void ENFAL::echoCallback(S2EExecutionState *state, uint64_t pc) {
    if (loopCount.count(state) && loopCount[state] >= 2) {
        do_killState(state, pc);
        return;
    }
    DEBUG_PRINT(
        std::ostringstream message;
        message << "state 0x" << std::hex << state <<
                " in ECHO command\n";
        getInfoStream(state) << message.str();
    )
}


} // namespace plugins
} // namespace s2e
