//
// Created by luca on 07/02/19.
//

#include <s2e/S2E.h>

#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>

#include <klee/util/ExprTemplates.h>

#include <cctype>
#include <sstream>

#include "InstructionTracer.h"

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(InstructionTracer,
"Print instruction trace", "",
"ProcessExecutionDetector");

void InstructionTracer::initialize() {

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();

    log_filename   = s2e()->getConfig()->getString(getConfigKey() + ".log_filename");
    begin_address  = s2e()->getConfig()->getInt(getConfigKey() + ".begin_address");
    end_address    = s2e()->getConfig()->getInt(getConfigKey() + ".end_address");

    log.open(log_filename, std::ios::out | std::ios::trunc);

    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
            sigc::mem_fun(*this, &InstructionTracer::onTranslateInstruction));
    s2e()->getCorePlugin()->onStateFork.connect(
            sigc::mem_fun(*this, &InstructionTracer::onStateFork));
}

void InstructionTracer::onStateFork(S2EExecutionState *oldState, const std::vector<S2EExecutionState *> &newStates,
         const std::vector<klee::ref<klee::Expr>> &) {

    log << "fork," << oldState->getGuid();
    for (auto it = newStates.begin(); it != newStates.end(); ++it)
        log << "," << (*it)->getGuid();
    log << "\n";
    log.flush();

}

void InstructionTracer::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                     TranslationBlock *tb, uint64_t pc) {
    if (begin_address <= pc && pc <= end_address)
        signal->connect(sigc::mem_fun(*this, &InstructionTracer::logOnFile));
}

void InstructionTracer::logOnFile(S2EExecutionState *state, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    log << state->getGuid() << "," << pc << "\n";
    log.flush();
}

} // namespace plugins
} // namespace s2e