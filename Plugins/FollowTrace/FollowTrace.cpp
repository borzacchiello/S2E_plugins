//
// Created by luca on 07/02/19.
//
#include <s2e/cpu.h>
#include <s2e/opcodes.h>

#include <s2e/S2E.h>

#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>

#include <klee/util/ExprTemplates.h>

#include <cctype>
#include <sstream>
#include <fstream>

#include "FollowTrace.h"

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(FollowTrace,
"Follow instruction trace", "",
"ProcessExecutionDetector", "TestCaseGenerator");

void FollowTrace::initialize() {

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();
    m_testGen = s2e()->getPlugin<testcases::TestCaseGenerator>();
    initialized = false;

    log_filename   = s2e()->getConfig()->getString(getConfigKey() + ".log_filename");
    begin_address  = s2e()->getConfig()->getInt(getConfigKey()    + ".begin_address");
    end_address    = s2e()->getConfig()->getInt(getConfigKey()    + ".end_address");
    threshold      = s2e()->getConfig()->getInt(getConfigKey()    + ".threshold");
    stop_on_target = s2e()->getConfig()->getBool(getConfigKey()   + ".stop_on_target");

    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
            sigc::mem_fun(*this, &FollowTrace::onTranslateInstruction));
}

void FollowTrace::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                               TranslationBlock *tb, uint64_t pc) {

#if ENFAL
    if (pc == CREATE_REMOTE_THREAD_ADDR) {
        signal->connect(sigc::mem_fun(*this, &FollowTrace::updateBeginEndENFAL));
        return;
    }
#endif
    if (begin_address <= pc && pc <= end_address)
        signal->connect(sigc::mem_fun(*this, &FollowTrace::follow));
}

void FollowTrace::follow(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    DECLARE_PLUGINSTATE(Trace, state);
    if (plgState->is_trace_empty() && !initialized) {
        initialized = true;
        setTrace(plgState);
    }
#if ENFAL
    if (!plgState->consume(pc - (uint32_t)begin_address)) {
#else
    if (!plgState->consume(pc)) {
#endif
        std::ostringstream os;
        uint64_t a = plgState->get_front();
#if ENFAL
        os << "avoid. " << std::hex << pc - (uint32_t)begin_address << " in place of " << std::hex << a;
#else
        os << "avoid. " << std::hex << pc << " in place of " << std::hex << a;
#endif
        killState(state, pc, os.str());
    }
    if (plgState->is_trace_empty()) {
        if (stop_on_target) {
            m_testGen->generateTestCases(state, "target", testcases::TC_LOG);
            killState(state, pc, "target");
        } else
            plgState->unfollow();
    }
}

void FollowTrace::updateBeginEndENFAL(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    uint32_t new_begin;
    state->regs()->read(CPU_OFFSET(regs[R_EAX]), &new_begin, sizeof new_begin);

    getInfoStream(state) << "ENFAL remote thread start address: " << new_begin << "\n";

    begin_address = (uint64_t)new_begin;
    end_address   = (uint64_t)new_begin + 0x100000;
}

void FollowTrace::setTrace(Trace* t) {
    std::ifstream log;
    log.open(log_filename);
    uint64_t value;

    std::list<uint64_t> trace;
    while (log >> value) {
#if ENFAL
        trace.push_back(value - 0x0160000);
#else
        trace.push_back(value);
#endif
    }
    log.close();
    t->set_trace(trace);
    t->set_threshold(threshold);
}

void FollowTrace::killState(S2EExecutionState *state, uint64_t pc, std::string mex) {

    std::ostringstream os;
    os << "The state was terminated by FollowTrace plugin\n" <<
       "    Address: " << std::hex << pc <<
       "    Message: " << mex << "\n";
    s2e()->getExecutor()->terminateStateEarly(*state, os.str());
}

} // namespace plugins
} // namespace s2e