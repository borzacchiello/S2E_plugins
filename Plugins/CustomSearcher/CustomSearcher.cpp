///
/// Copyright (C) 2011-2015, Dependable Systems Laboratory, EPFL
/// Copyright (C) 2016, Cyberhaven
/// All rights reserved.
///
/// Licensed under the Cyberhaven Research License Agreement.
///

/**
 * This plugin implements a cooperative searcher.
 * The current state is run until the running program expicitely
 * asks to schedule another one, akin to cooperative scheduling.
 *
 * This searcher is useful for debugging S2E, becauses it allows
 * to control the sequence of executed states.
 *
 * RESERVES THE CUSTOM OPCODE 0xAB
 */

#include <s2e/cpu.h>
#include <s2e/opcodes.h>

#include <s2e/ConfigFile.h>
#include <s2e/S2E.h>
#include <s2e/S2EExecutor.h>
#include <s2e/Utils.h>

#include <iostream>

#include "CustomSearcher.h"

namespace s2e {
namespace plugins {

using namespace llvm;

S2E_DEFINE_PLUGIN(CustomSearcher, "Custom searcher", "CustomSearcher");

void CustomSearcher::initialize() {
    s2e()->getExecutor()->setSearcher(this);
    debug = true;// s2e()->getConfig()->getBool(getConfigKey() + ".debug");
}

klee::ExecutionState &CustomSearcher::selectState() {
    S2EExecutionState* ris = fringe.front();
    fringe.push_back(ris);
    fringe.erase(fringe.begin());
    if (debug) {
        uint64_t ip = 0x0;
        if (ris->regs())
            ip = ris->regs()->getPc();
        getInfoStream(ris) <<
            "Selecting state ID: " << ris->getID() <<
            " GID: " << ris->getGuid() <<
            " IP: " << ip << "\n";
    }
    return *ris;
}

void CustomSearcher::update(klee::ExecutionState *current, const klee::StateSet &addedStates,
                                 const klee::StateSet &removedStates) {

    S2EExecutionState *s2eCurrent = static_cast<S2EExecutionState *>(current);

    if (debug && (addedStates.size() > 0 || removedStates.size() > 0)) {
        getInfoStream(s2eCurrent) <<
          "Current state ID: " << s2eCurrent->getID() <<
          " GID: " << s2eCurrent->getGuid() <<
          " len(added): " << addedStates.size() <<
          " len(removed): " << removedStates.size() << "\n";
    }

    foreach2 (it, addedStates.begin(), addedStates.end()) {
        S2EExecutionState *state = static_cast<S2EExecutionState *>(*it);
        fringe.push_back(state);

        if (debug) {
            getInfoStream(s2eCurrent) <<
                 "Adding state ID: " << state->getID() <<
                 " GID: " << state->getGuid() << "\n";
        }

    }

    foreach2 (it, removedStates.begin(), removedStates.end()) {
        S2EExecutionState *state = static_cast<S2EExecutionState *>(*it);
        bool ok = false;

        for (std::vector<S2EExecutionState *>::iterator it = fringe.begin(), ie = fringe.end(); it != ie; ++it) {
            if (state == *it) {
                fringe.erase(it);
                ok = true;
                break;
            }
        }
        assert(ok && "invalid state removed");

        if (debug) {
            getInfoStream(s2eCurrent) <<
                "Removing state ID: " << state->getID() <<
                " GID: " << state->getGuid() << "\n";
        }
    }

}

bool CustomSearcher::empty() {
    return fringe.empty();
}

} // namespace plugins
} // namespace s2e
