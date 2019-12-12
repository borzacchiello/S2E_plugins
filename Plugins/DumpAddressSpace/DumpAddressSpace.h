//
// Created by luca on 07/01/19.
//

#ifndef SOURCE_DUMPADDRESSSPACE_H
#define SOURCE_DUMPADDRESSSPACE_H
#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>

namespace s2e {
namespace plugins {

class ProcessExecutionDetector;

class DumpAddressSpace : public Plugin {
    S2E_PLUGIN

public:
    DumpAddressSpace(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

private:
    ProcessExecutionDetector *m_procDetector;

    void onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb, uint64_t pc);
    void dumpMemory(S2EExecutionState *state, uint64_t pc);
};

} // namespace plugins
} // namespace s2e

#endif //SOURCE_DUMPADDRESSSPACE_H
