//
// Created by luca on 05/12/18.
//

#ifndef LIBS2EPLUGINS_CUSTOMSEARCHER_H
#define LIBS2EPLUGINS_CUSTOMSEARCHER_H
#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/OSMonitors/Support/ModuleExecutionDetector.h>
#include <s2e/S2EExecutionState.h>

#include <klee/Searcher.h>

#include <vector>

namespace s2e {
namespace plugins {

class CustomSearcher : public Plugin, public klee::Searcher {
    S2E_PLUGIN
public:

    CustomSearcher(S2E *s2e) : Plugin(s2e) {
    }
    void initialize();

    virtual klee::ExecutionState &selectState();

    virtual void update(klee::ExecutionState *current, const klee::StateSet &addedStates,
                        const klee::StateSet &removedStates);

    virtual bool empty();

private:
    std::vector<S2EExecutionState*> fringe;
    bool debug;
};

} // namespace plugins
} // namespace s2e

#endif //LIBS2EPLUGINS_CUSTOMSEARCHER_H
