//
// Created by luca on 30/10/19.
//

#ifndef SOURCE_FORKDISABLER_H
#define SOURCE_FORKDISABLER_H

#include <s2e/Plugin.h>
#include <s2e/CorePlugin.h>
#include <s2e/S2EExecutionState.h>

namespace s2e {
    namespace plugins {
        
        class TerminateForkedStates : public Plugin {
                S2E_PLUGIN
            
            public:
                TerminateForkedStates(S2E *s2e) : Plugin(s2e) { }
            
                void initialize();
                void handleStateFork(S2EExecutionState* /* original state */,
                                     const std::vector<S2EExecutionState*>& /* new states */,
                                     const std::vector<klee::ref<klee::Expr>>& /* new conditions */);
            };
        
        } // namespace plugins
    } // namespace s2e


#endif //SOURCE_FORKDISABLER_H
