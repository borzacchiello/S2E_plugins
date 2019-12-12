///
/// Copyright (C) 2017, Dependable Systems Laboratory, EPFL
/// All rights reserved.
///
/// Copy this file to source/s2e/libs2eplugins/src/s2e/Plugins in your S2E
/// environment.
///

#include <s2e/S2E.h>

#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>

#include <klee/util/ExprTemplates.h>

#include <cctype>
#include <sstream>

#include "CrackMe.h"

static const uint64_t SUCCESS_ADDRESS = 0x80487AF;
static const uint64_t FAILURE_ADDRESS = 0x80487D6;

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(CrackMe,
                  "Solution for the fror a CrackMe", "",
                  "ProcessExecutionDetector");

void CrackMe::initialize() {
    // We need to use the ProcessExecutionDetector plugin to filter out all processes other than the "unbreakable"
    // process
    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();

    s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
        sigc::mem_fun(*this, &CrackMe::onTranslateInstruction));
}

void CrackMe::onSymbolicVariableCreation(S2EExecutionState *state, const std::string &name,
                                                      const std::vector<klee::ref<klee::Expr>> &expr,
                                                      const klee::MemoryObject *mo, const klee::Array *array) {

    getWarningsStream(state) << "Creation of Symbolic Variable\n";
}

void CrackMe::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                                  TranslationBlock *tb, uint64_t pc) {
    if (pc == SUCCESS_ADDRESS) {
        signal->connect(sigc::mem_fun(*this, &CrackMe::onSuccess));
    } else if (pc == FAILURE_ADDRESS) {
        signal->connect(sigc::mem_fun(*this, &CrackMe::onFailure));
    }
}

void CrackMe::onSuccess(S2EExecutionState *state, uint64_t pc) {
    // Ensure that the unbreakable process is executing
    if (!m_procDetector->isTracked(state)) {
        return;
    }

    // `results` is a vector containing pairs of strings and a vector of bytes. The string corresponds to the symbolic
    // variable's name while the vector of bytes is the actual solution
    std::vector<std::pair<std::string, std::vector<unsigned char>>> results;

    // Invoke the constraint solver
    if (!s2e()->getExecutor()->getSymbolicSolution(*state, results)) {
        getWarningsStream(state) << "Unable to generate a solution for the product activation code\n";
        exit(1);
    }

    // Since we only have a single symbolic variable, we will only have a single result. We then iterate over the
    // bytes in this result to print the solution
    std::stringstream ss;
    for (auto c : results[0].second) {
        if (!std::isprint(c)) {
            break;
        }
        ss << (char) c;
    }

    getInfoStream(state) << "Solution = " << ss.str() << "\n";

    // No need to continue running S2E - terminate
    exit(0);
}

void CrackMe::onFailure(S2EExecutionState *state, uint64_t pc) {
    // Ensure that the unbreakable process is executing
    if (!m_procDetector->isTracked(state)) {
        return;
    }

    // There is no reason to continue execution any further. So kill the state
    s2e()->getExecutor()->terminateStateEarly(*state, "Invalid path");
}

} // namespace plugins
} // namespace s2e