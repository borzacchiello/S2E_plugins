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

#include "WrapRead.h"

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(WrapRead,
"Wrapper of read syscall", "");

void WrapRead::initialize() {
    /* This is for syscall monitoring */
    s2e()->getCorePlugin()->onTranslateInstructionEnd.connect(
            sigc::mem_fun(*this, &WrapRead::onTranslateInstructionEnd));
}

void WrapRead::onTranslateInstructionEnd(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb,
                                             uint64_t pc, uint64_t addend) {
    // see _ struct TranslationBlock _
    if (*(tb->tc_ptr + pc) != 0xcd || *(tb->tc_ptr + pc + 1) != 0x80)
        return;

    signal->connect(sigc::mem_fun(*this, &WrapRead::onInterrupt));
}

void WrapRead::onInterrupt(S2EExecutionState *state, uint64_t pc) {
    // target_ulong syscallid = state->regs()->read<target_ulong>(CPU_OFFSET(regs[R_EAX]));
    getWarningsStream(state) << "INT 0x80 captured\n";
}

} // namespace plugins
} // namespace s2e