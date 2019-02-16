//
// Created by luca on 11/02/19.
//

#include "klee/Internal/ADT/RNG.h"

#include <s2e/cpu.h>
#include <s2e/opcodes.h>

#include <s2e/ConfigFile.h>
#include <s2e/S2E.h>
#include <s2e/S2EExecutor.h>
#include <s2e/Utils.h>

#include <iostream>

#include "LuigiSearcher.h"

namespace klee {
    extern RNG theRNG;
}

namespace s2e {
    namespace plugins {

        using namespace llvm;

        S2E_DEFINE_PLUGIN(LuigiSearcher, "Luigi Searcher (UHUH)", "LuigiSearcher");

        void LuigiSearcher::initialize() {
            s2e()->getExecutor()->setSearcher(this);
            debug = true;// s2e()->getConfig()->getBool(getConfigKey() + ".debug");
            star  = false;
        }

        klee::ExecutionState &LuigiSearcher::selectState() {
            S2EExecutionState* ris = fringe.back();

            if (true || !star) {
                if (currentState == nullptr) {
                    currentState = ris;
                }

                // getInfoStream(currentState) << "returning state " << currentState << "\n";
                return *currentState;
            } else {
                // return *fringe[klee::theRNG.getInt32() % fringe.size()];
                return *fringe.front();
            }
        }

        void LuigiSearcher::update(klee::ExecutionState *current, const klee::StateSet &addedStates,
                                    const klee::StateSet &removedStates) {

            foreach2 (it, addedStates.begin(), addedStates.end()) {
                S2EExecutionState* s = static_cast<S2EExecutionState*>(*it);
                fringe.push_back(s);
            }

            foreach2 (it, removedStates.begin(), removedStates.end()) {
                foreach2 (itt, fringe.begin(), fringe.end()) {
                    if (*itt == *it) {
                        fringe.erase(itt);
                        break;
                    }
                }

                if (*it == currentState)
                    currentState = nullptr;
            }
        }

        bool LuigiSearcher::empty() {
            return fringe.empty() && deferred.empty();
        }

        void LuigiSearcher::goLuigiGo(S2EExecutionState* state) {
            star = true;
            currentState = state;
            fringe.clear();
            fringe.push_back(state);
        }

    } // namespace plugins
} // namespace s2e
