//
// Created by luca on 04/12/18.
//

#include <s2e/S2E.h>

#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>

#include <klee/util/ExprTemplates.h>

#include <cctype>
#include <sstream>

#include "Explore.h"

uint64_t SUCCESS_ADDRESS;
uint64_t FAILURE_ADDRESS;

namespace s2e {
    namespace plugins {

        S2E_DEFINE_PLUGIN(Explore,
        "Plugin for CrackMe-type binaries", "",
        "ProcessExecutionDetector");

        void Explore::initialize() {

            // Address read from config file
            SUCCESS_ADDRESS = (uint64_t) s2e()->getConfig()->getInt(getConfigKey() + ".successAddress");
            FAILURE_ADDRESS = (uint64_t) s2e()->getConfig()->getInt(getConfigKey() + ".failureAddress");
            // We need to use the ProcessExecutionDetector plugin to filter out all processes other than the interesting one
            m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();

            s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
                    sigc::mem_fun(*this, &Explore::onTranslateInstruction));
        }

        void Explore::onSymbolicVariableCreation(S2EExecutionState *state, const std::string &name,
                                                 const std::vector<klee::ref<klee::Expr>> &expr,
                                                 const klee::MemoryObject *mo, const klee::Array *array) {

            getWarningsStream(state) << "Creation of Symbolic Variable\n";
        }

        void Explore::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                             TranslationBlock *tb, uint64_t pc) {
            if (pc == SUCCESS_ADDRESS) {
                signal->connect(sigc::mem_fun(*this, &Explore::onSuccess));
            } else if (pc == FAILURE_ADDRESS) {
                signal->connect(sigc::mem_fun(*this, &Explore::onFailure));
            }
        }

        void Explore::onSuccess(S2EExecutionState *state, uint64_t pc) {
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

        void Explore::onFailure(S2EExecutionState *state, uint64_t pc) {
            // Ensure that the unbreakable process is executing
            if (!m_procDetector->isTracked(state)) {
                return;
            }

            // There is no reason to continue execution any further. So kill the state
            s2e()->getExecutor()->terminateStateEarly(*state, "Invalid path");
        }

    } // namespace plugins
} // namespace s2e