#include <s2e/cpu.h>
#include <s2e/opcodes.h>
#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>
#include <s2e/ConfigFile.h>
#include <s2e/S2E.h>
#include <s2e/S2EExecutor.h>
#include <s2e/Utils.h>
#include <iostream>
#include "NetWire.h"
#include "commands.h"

#define DEBUG_PRINT(inst) if (debug) {inst}

#define ADDR_CMD_SWITCH 0x04010C7
#define AVOID_1 0x04020DE
#define AVOID_2 0x040D3B6
#define AFTER_SWITCH 0x040211E
#define CHECK_FAIL 0x040212F

namespace s2e {
namespace plugins {

using namespace llvm;

S2E_DEFINE_PLUGIN(NetWire, "NetWire plugin", "NetWire", "ProcessExecutionDetector");

std::string NetWire::addrToMessage(uint64_t pc) {
    if (pc == ADDR_CMD_SWITCH) return "Address command switch";
    if (pc == AVOID_1) return "AVOID_1";
    if (pc == AVOID_2) return "AVOID_2";
    if (pc == AFTER_SWITCH) return "AFTER_SWITCH";
    if (pc == CHECK_FAIL) return "CHECK_FAIL";
    if (pc == CMD_00) return "CMD_00";
    if (pc == CMD_04) return "CMD_04";
    if (pc == CMD_05) return "CMD_05";
    if (pc == CMD_06) return "CMD_06";
    if (pc == CMD_08) return "CMD_08";
    if (pc == CMD_09) return "CMD_09";
    if (pc == CMD_10) return "CMD_10";
    if (pc == CMD_11) return "CMD_11";
    if (pc == CMD_12) return "CMD_12";
    if (pc == CMD_13) return "CMD_13";
    if (pc == CMD_15) return "CMD_15";
    if (pc == CMD_17) return "CMD_17";
    if (pc == CMD_19) return "CMD_19";
    if (pc == CMD_20) return "CMD_20";
    if (pc == CMD_21) return "CMD_21";
    if (pc == CMD_22) return "CMD_22 and 30";
    if (pc == CMD_23) return "CMD_23";
    if (pc == CMD_24) return "CMD_24";
    if (pc == CMD_25) return "CMD_25";
    if (pc == CMD_26) return "CMD_26";
    if (pc == CMD_27) return "CMD_27";
    if (pc == CMD_28) return "CMD_28";
    if (pc == CMD_29) return "CMD_29";
    if (pc == CMD_31) return "CMD_31";
    if (pc == CMD_32) return "CMD_32";
    if (pc == CMD_33) return "CMD_33";
    if (pc == CMD_35) return "CMD_35";
    if (pc == CMD_37) return "CMD_37";
    if (pc == CMD_39) return "CMD_39";
    if (pc == CMD_41) return "CMD_41";
    if (pc == CMD_42) return "CMD_42";
    if (pc == CMD_43) return "CMD_43";
    if (pc == CMD_44) return "CMD_44";
    if (pc == CMD_46) return "CMD_46";
    if (pc == CMD_47) return "CMD_47";
    if (pc == CMD_48) return "CMD_48";
    if (pc == CMD_49) return "CMD_49";
    if (pc == CMD_50) return "CMD_50";
    if (pc == CMD_53) return "CMD_53";
    if (pc == CMD_55) return "CMD_55";
    if (pc == CMD_56) return "CMD_56";
    if (pc == CMD_57) return "CMD_57";
    if (pc == CMD_60) return "CMD_60 and 61";
    if (pc == CMD_62) return "CMD_62 and 63";
    if (pc == CMD_64) return "CMD_64 and 65";
    if (pc == CMD_66) return "CMD_66";
    if (pc == CMD_67) return "CMD_67";
    if (pc == CMD_69) return "CMD_69";
    if (pc == CMD_71) return "CMD_71";
    if (pc == CMD_72) return "CMD_72";
    if (pc == CMD_75) return "CMD_75";
    if (pc == CMD_DF) return "CMD_DF";
    else return "UNKNOWN";
}

void NetWire::initialize() {
    debug = true;// s2e()->getConfig()->getBool(getConfigKey() + ".debug");

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();
    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
            sigc::mem_fun(*this, &NetWire::onTranslateInstruction));
    s2e()->getCorePlugin()->onStateFork.connect(
            sigc::mem_fun(*this, &NetWire::onStateFork));
}

void NetWire::onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
                        const std::vector<klee::ref<klee::Expr>> &) {
    if (!loopCount.count(oldState)) return;

    int count = loopCount[oldState];

    std::vector<S2EExecutionState *>::const_iterator it;
    for (it = newStates.begin(); it != newStates.end(); ++it)
        loopCount[*it] = count;
}

void NetWire::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
        TranslationBlock *tb, uint64_t pc) {
    if (pc == ADDR_CMD_SWITCH) signal->connect(sigc::mem_fun(*this, &NetWire::update_loopCount));
    if (pc == AVOID_1) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == AVOID_2) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == AFTER_SWITCH) signal->connect(sigc::mem_fun(*this, &NetWire::do_checkValidity));
    if (pc == CHECK_FAIL) signal->connect(sigc::mem_fun(*this, &NetWire::do_checkValidity2));
    if (pc == CMD_00) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_04) signal->connect(sigc::mem_fun(*this, &NetWire::do_checkValidity));
    if (pc == CMD_05) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_06) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_08) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_09) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_10) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_11) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_12) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_13) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_15) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_17) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_19) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_20) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_21) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_22) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_23) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_24) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_25) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_26) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_27) signal->connect(sigc::mem_fun(*this, &NetWire::do_logName));
    if (pc == CMD_28) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_29) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_30) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_31) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_32) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_33) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_35) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_37) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_39) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_41) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_42) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_43) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_44) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_46) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_47) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_48) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_49) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_50) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_53) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_55) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_56) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_57) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_60) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_61) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_62) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_63) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_64) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_65) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_66) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_67) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_69) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_71) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_72) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_75) signal->connect(sigc::mem_fun(*this, &NetWire::do_killState));
    if (pc == CMD_DF) signal->connect(sigc::mem_fun(*this, &NetWire::do_logName));
}

void NetWire::update_loopCount(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

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

void NetWire::do_checkValidity(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;
    DEBUG_PRINT (
            std::ostringstream message;
            message << "In checkvalidity of state 0x" << std::hex << state <<
                    "    Message: " << addrToMessage(pc) << "\n";
            getInfoStream(state) << message.str();
    )
    if (!loopCount.count(state) || loopCount[state] > 1)
        do_killState(state, pc);
}

void NetWire::do_checkValidity2(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    DEBUG_PRINT (
        std::ostringstream message;
        message << "In checkvalidity2 of state 0x" << std::hex << state << "\n" <<
            "    Message: " << addrToMessage(pc) << "\n";
        getInfoStream(state) << message.str();
    )
    if (loopCount.count(state))
        do_killState(state, pc);
}

void NetWire::do_killState(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    std::ostringstream os;
    os << "The state was terminated by NetWire plugin\n" <<
        "    Address: " << std::hex << pc <<
        "    Message: " << addrToMessage(pc) << "\n";
    s2e()->getExecutor()->terminateStateEarly(*state, os.str());
}

void NetWire::do_logName(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    std::ostringstream os;
    os << addrToMessage(pc) << "\n";
    getInfoStream(state) << os.str();
}

} // namespace plugins
} // namespace s2e
