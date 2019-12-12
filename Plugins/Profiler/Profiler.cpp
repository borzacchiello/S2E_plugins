//
// Created by luca on 02/10/19.
//

#include <s2e/cpu.h>
#include <s2e/opcodes.h>
#include <s2e/ConfigFile.h>
#include <s2e/S2E.h>
#include <s2e/S2EExecutor.h>
#include <klee/util/ExprTemplates.h>
#include <s2e/Utils.h>
#include <iostream>
#include <fstream>
#include "Profiler.h"
#include "monitor_inst.h"

#define DEBUG_PRINT(inst) if (debug) {inst}

namespace s2e {
    namespace plugins {

        using namespace llvm;

        S2E_DEFINE_PLUGIN(Profiler, "Profiler plugin", "Profiler",);

        void Profiler::initialize() {
            debug = s2e()->getConfig()->getBool(getConfigKey() + ".debug");
            timerdata_path = s2e()->getConfig()->getString(getConfigKey() + ".timer_path");

            init_count();
        }

        void Profiler::handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize) {
            ProfilerGuestCommand command;

            if (guestDataSize != sizeof(command)) {
                getWarningsStream(state) << "mismatched S2E_HOOK_PLUGIN_COMMAND size\n";
                exit(-1);
            }

            if (!state->mem()->read(guestDataPtr, &command, guestDataSize)) {
                getWarningsStream(state) << "could not read transmitted data\n";
                exit(-1);
            }

            switch (command.cmd) {
                case NOTIFY_INIT: {

                    DEBUG_PRINT (
                            std::ostringstream message;
                            message << "NOTIFY_INIT received" << "\n";
                            getInfoStream(state) << message.str();
                    )
                } break;

                case NOTIFY_START_TIMER: {

                    clock_gettime(CLOCK_MONOTONIC, &timer_begin);

                    DEBUG_PRINT (
                            std::ostringstream message;
                            message << "NOTIFY_START_TIMER received." << "\n";
                            getInfoStream(state) << message.str();
                    )
                    start_count();
                } break;

                case NOTIFY_END_TIMER: {

                    struct timespec timer_end;
                    clock_gettime(CLOCK_MONOTONIC, &timer_end);
                    stop_count();

                    unsigned long long perf_count;
                    read_count(&perf_count);

                    std::ofstream timer_file;
                    timer_file.open (timerdata_path, std::ios_base::app);
                    timer_file << timespecDiff(&timer_end, &timer_begin) << ";" <<
                    perf_count << "\n";
                    timer_file.close();

                    DEBUG_PRINT (
                            std::ostringstream message;
                            message << "NOTIFY_END_TIMER received. \n\tElapsed time: "
                                    << timespecDiff(&timer_end, &timer_begin) << "\n\t" <<
                                    "Executed instructions (perf): " << perf_count << "\n";
                            getInfoStream(state) << message.str();
                    )
                } break;

                case NOTIFY_FORCE_LLVM: {
                    assert(false && "deprecated");

                    unsigned long long buff = command.forceLLVMData.buffer;
                    unsigned long long data = command.forceLLVMData.data;
                    unsigned long size = command.forceLLVMData.size;

                    DEBUG_PRINT (
                            std::ostringstream message;
                            message << "NOTIFY_FORCE_LLVM received data: \n"   <<
                                       "\t buff: " << std::hex << buff << "\n" <<
                                       "\t data: " << std::hex << data << "\n" <<
                                       "\t size: " << size << "\n";
                            getInfoStream(state) << message.str();
                    )

                    std::vector<uint8_t> concreteData;
                    for (unsigned i = 0; i < size; ++i) {
                        uint8_t byte = 0;
                        if (!state->mem()->read<uint8_t>(data + i, &byte, VirtualAddress, false)) {
                            getWarningsStream(state) << "FORCE_LLVM_BUFFER Can not concretize/read symbolic value at "
                                                     << hexval(data + i)
                                                     << ". System state not modified\n";
                            return;
                        }
                        concreteData.push_back(byte);
                    }

                    std::vector<klee::ref<klee::Expr>> symb;
                    symb = state->createConcolicArray("FORCE_LLVM_BUFFER", size, concreteData, 0);

                    for (unsigned i = 0; i < size; ++i) {
                        if (!state->mem()->write(buff + i, symb[i], VirtualAddress)) {
                            getWarningsStream(state) << "FORCE_LLVM_BUFFER Can not insert symbolic value at "
                                                     << hexval(buff + i)
                                                     << ": can not write to memory\n";
                        }
                        state->addConstraint(E_EQ(symb[i], E_CONST(concreteData[i], klee::Expr::Int8)));
                    }

                    DEBUG_PRINT (
                            std::ostringstream message;
                            message << "NOTIFY_FORCE_LLVM completed. Fake symbolic array at "
                                    << std::hex << buff << " injected" << "\n";
                            getInfoStream(state) << message.str();
                    )

                }

                default: { getWarningsStream(state) << "unknown command\n"; } break;
            }
        }

        int64_t Profiler::timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p) {
            return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
                   ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
        }

        void Profiler::killState(S2EExecutionState *state, std::string mex) {
            std::ostringstream os;
            os << "The state was terminated by Profiler plugin\n" <<
               "    Message: " << mex << "\n";
            s2e()->getExecutor()->terminateStateEarly(*state, os.str());
        }

    } // namespace plugins
} // namespace s2e
