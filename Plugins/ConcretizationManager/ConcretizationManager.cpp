//
// Created by luca on 08/01/19.
//
#include <s2e/S2E.h>
#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>

#include "ConcretizationManager.h"

namespace s2e {
namespace plugins {

using namespace klee;

S2E_DEFINE_PLUGIN(ConcretizationManager,
"Manage addresses concretization strategy", "",
"ProcessExecutionDetector");

void ConcretizationManager::initialize() {

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();

    // 1 for true, 0 for false
    concWrite = (int) s2e()->getConfig()->getInt(getConfigKey() + ".writeConcretization");
    // 1 for true, 0 for false
    concRead  = (int) s2e()->getConfig()->getInt(getConfigKey() + ".readConcretization");
    // 1 for true, 0 for false
    writeToMax = (int) s2e()->getConfig()->getInt(getConfigKey() + ".writeToMax");
    // 1 for true, 0 for false
    readToMax = (int) s2e()->getConfig()->getInt(getConfigKey() + ".readToMax");

    s2e()->getCorePlugin()->onBeforeSymbolicDataMemoryAccess.connect(
            sigc::mem_fun(*this, &ConcretizationManager::onBeforeSymbolicDataMemoryAccess));
}

void ConcretizationManager::onBeforeSymbolicDataMemoryAccess(S2EExecutionState *state, ref<Expr> addr,
                                             ref<Expr> value, bool isWrite) {
    if (!m_procDetector->isTracked(state)) return;

    getInfoStream(state) << "onBeforeSymbolicDataMemoryAccess"
                          << ", EIP=" << hexval(state->regs()->getPc()) << ", isWrite=" << isWrite << "\n";
    getInfoStream(state) << "  addr=" << addr << "\n";
    getInfoStream(state) << "  value=" << value << "\n";

    if (isWrite) {
        // handle symbolic write
        if (concWrite)
            do_concretizeAddress(state, addr, writeToMax);
    } else {
        // handle symbolic read
        if (concRead)
            do_concretizeAddress(state, addr, readToMax);
    }
}

void ConcretizationManager::do_concretizeAddress(S2EExecutionState *state, ref<Expr> addr, bool toMax) {
    if (!m_procDetector->isTracked(state)) return;

    ref<ConstantExpr> ris;
    if (!toMax)
        ris = s2e()->getExecutor()->toConstant(*state, addr, "");
    else {
        Solver *solver = s2e()->getExecutor()->getSolver(*state);
        Query query(state->constraints, addr);
        ris = dyn_cast<ConstantExpr>(solver->getRange(query).second);
        state->addConstraint(EqExpr::create(addr, ris));
    }

    std::ostringstream message;
    message << "Concretizing at 0x" << std::hex << state->regs()->getPc() << " to " << ris << "\n";
    getInfoStream(state) << message.str();
}


} // namespace plugins
} // namespace s2e