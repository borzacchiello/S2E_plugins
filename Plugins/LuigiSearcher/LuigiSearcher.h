//
// Created by luca on 11/02/19.
//

#ifndef S2E_LUIGISEARCHER_H
#define S2E_LUIGISEARCHER_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/OSMonitors/Support/ModuleExecutionDetector.h>
#include <s2e/S2EExecutionState.h>

#include <klee/Searcher.h>

#include <vector>

namespace s2e {
    namespace plugins {

        class LuigiSearcher : public Plugin, public klee::Searcher {
            S2E_PLUGIN
        public:

            LuigiSearcher(S2E *s2e) : Plugin(s2e) {
            }
            void initialize();

            virtual klee::ExecutionState &selectState();

            virtual void update(klee::ExecutionState *current, const klee::StateSet &addedStates,
                                const klee::StateSet &removedStates);

            virtual bool empty();

            void goLuigiGo(S2EExecutionState* state);

        private:
            S2EExecutionState *currentState = nullptr;
            std::vector<S2EExecutionState*> fringe;
            std::vector<S2EExecutionState*> deferred;

            bool star;
            bool debug;
        };

    } // namespace plugins
} // namespace s2e

#endif //S2E_LUIGISEARCHER_H
