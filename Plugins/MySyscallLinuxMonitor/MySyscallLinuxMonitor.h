//
// Created by luca on 07/12/18.
//

#ifndef SOURCE_MYLINUXMONITOR_H
#define SOURCE_MYLINUXMONITOR_H

namespace s2e {
namespace plugins {

class MySyscallLinuxMonitor : public Plugin {
    S2E_PLUGIN

public:
    MySyscallLinuxMonitor(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

private:
    ProcessExecutionDetector *m_procDetector;

    void onTranslateSoftInterruptStart(ExecutionSignal *signal, S2EExecutionState *state, TranslationBlock *tb,
            uint64_t pc, unsigned vector);

    void onSyscallInt(S2EExecutionState *state, uint64_t pc);
};

} // namespace plugins
} // namespace s2e


#endif //SOURCE_MYLINUXMONITOR_H
