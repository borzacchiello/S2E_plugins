//
// Created by luca on 07/12/18.
//

#include <s2e/CorePlugin.h>
#include <s2e/S2EExecutionState.h>
#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>
#include <s2e/S2E.h>
#include <s2e/cpu.h>

#include <cctype>
#include <sstream>

#include "MySyscallLinuxMonitor.h"

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(MySyscallLinuxMonitor,
"Linux Syscalls tracing", "", "ProcessExecutionDetector");

void MySyscallLinuxMonitor::initialize() {
    /* This is for syscall monitoring */
    s2e()->getCorePlugin()->onTranslateSoftInterruptStart.connect(
            sigc::mem_fun(*this, &MySyscallLinuxMonitor::onTranslateSoftInterruptStart));

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();
}

void MySyscallLinuxMonitor::onTranslateSoftInterruptStart(ExecutionSignal *signal, S2EExecutionState *state,
        TranslationBlock *tb, uint64_t pc, unsigned vector) {
    // if (!m_procDetector->isTracked(state) || vector != 0x80) {
    if (vector != 0x80) {
        return;
    }
    signal->connect(sigc::mem_fun(*this, &MySyscallLinuxMonitor::onSyscallInt));
}

void MySyscallLinuxMonitor::onSyscallInt(S2EExecutionState *state, uint64_t pc) {
    target_ulong syscallid = state->regs()->read<target_ulong>(CPU_OFFSET(regs[R_EAX]));
    getWarningsStream(state) << "Syscall " << syscallid << " called.\n";
}

} // namespace plugins
} // namespace s2e