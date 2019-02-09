//
// Created by luca on 07/02/19.
//

#ifndef S2E_FOLLOWTRACE_H
#define S2E_FOLLOWTRACE_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <list>

#define ENFAL 1
#define CREATE_REMOTE_THREAD_ADDR 0x0402446

#define DIFF_TRESHOLD 2

namespace s2e {
namespace plugins {

class ProcessExecutionDetector;

class Trace: public PluginState {
private:
    std::list<uint64_t> trace;
    int diff_count = 0;

public:
    Trace() {}
    Trace(const std::list<uint64_t>& _trace) {
        trace = _trace;
    }
    ~Trace() {}

    static PluginState *factory(Plugin*, S2EExecutionState*) {
        return new Trace();
    }

    Trace *clone() const {
        return new Trace(this->trace);
    }

    bool is_trace_empty() {
        return trace.empty();
    }

    void set_trace(const std::list<uint64_t>& _trace) {
        trace = _trace;
    }

    uint64_t get_front() {
        return trace.front();
    }

    bool consume(uint64_t pc) {
        if (trace.empty()) return false;

        if (trace.front() == pc) {
            trace.pop_front();
            diff_count = 0;
            return true;
        } else {
            int i = 0;
            for (auto const& el: trace) {

                if (el == pc) {
                    for (int j=0; j<=i; ++j)
                        trace.pop_front();
                    return true;
                }
                if (i++ > DIFF_TRESHOLD) return false;
            }
            if (++diff_count >= DIFF_TRESHOLD)
                return false;
            return true;
        }
    }

};


class FollowTrace : public Plugin {
    S2E_PLUGIN

public:
    FollowTrace(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

private:
    ProcessExecutionDetector *m_procDetector;
    std::string log_filename;
    uint64_t begin_address;
    uint64_t end_address;

    bool initialized;

    void onTranslateInstruction(ExecutionSignal *signal, S2EExecutionState *state,
                                TranslationBlock *tb, uint64_t pc);

    void killState(S2EExecutionState *state, uint64_t pc, std::string mex);
    void follow(S2EExecutionState *state, uint64_t pc);
    void setTrace(Trace* t);
    void updateBeginEndENFAL(S2EExecutionState *state, uint64_t pc);
};
}
}


#endif //S2E_FOLLOWTRACE_H
