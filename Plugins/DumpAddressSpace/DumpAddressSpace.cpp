//
// Created by luca on 07/01/19.
//

#include <s2e/S2E.h>

#include <s2e/Plugins/OSMonitors/Support/ProcessExecutionDetector.h>

#include <cctype>
#include <sstream>

#include "DumpAddressSpace.h"

#define SUCCESS_ADDRESS 0x080484a8


namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(DumpAddressSpace,
"Plugin for debugging purposes", "",
"ProcessExecutionDetector");

void DumpAddressSpace::initialize() {

    m_procDetector = s2e()->getPlugin<ProcessExecutionDetector>();

    s2e()->getCorePlugin()->onBeforeSymbolicDataMemoryAccess.connect(
            sigc::mem_fun(*this, &DumpAddressSpace::onBeforeSymbolicDataMemoryAccess));
}

void DumpAddressSpace::onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                     TranslationBlock *tb, uint64_t pc) {
    if (!m_procDetector->isTracked(state)) return;

    if (pc == SUCCESS_ADDRESS) {
        signal->connect(sigc::mem_fun(*this, &DumpAddressSpace::dumpMemory));
    }
}

void DumpAddressSpace::dumpMemory(S2EExecutionState *state, uint64_t pc) {

    auto address_space = state->mem()->m_addressSpace;
    klee::ImmutableMap<const klee::MemoryObject *, ObjectHolder, LT> objects = address_space->objects;

    std::ostringstream os;
    auto it = objects.begin();
    for (; it!=objects.end; ++it)
        os << it->first << " => " << it->second << '\n';
    getInfoStream(state) << os.str();

    s2e()->getExecutor()->terminateStateEarly(*state, "");
}

} // namespace plugins
} // namespace s2e