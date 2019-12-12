//
// Created by luca on 02/10/19.
//

#ifndef SOURCE_PROFILER_H
#define SOURCE_PROFILER_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/Core/BaseInstructions.h>
#include <s2e/S2EExecutionState.h>
#include <time.h>

namespace s2e {
    namespace plugins {

        typedef enum ProfilerCommandType {
            NOTIFY_INIT,
            NOTIFY_FORCE_LLVM,
            NOTIFY_START_TIMER,
            NOTIFY_END_TIMER
        } ProfilerCommandType;

        typedef struct CommandForceLLVMData {
            unsigned long long buffer;
            unsigned long long data;
            unsigned long size;
        } CommandForceLLVMData;

        typedef struct CommandInitData {
            void* address; // 8byte address
        } CommandInitData;

        typedef struct ProfilerGuestCommand {
            ProfilerCommandType cmd;
            union {
                CommandInitData initData;
                CommandForceLLVMData forceLLVMData;
            };
        } ProfilerGuestCommand;

        class ProcessExecutionDetector;
        class Profiler : public Plugin, public BaseInstructions::IPluginInvoker {
            S2E_PLUGIN
        public:

            Profiler(S2E *s2e) : Plugin(s2e) {
            }
            void initialize();

            // INVOKER STUFF
            virtual void handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize);
            // *************

        private:

            std::string timerdata_path;
            struct timespec timer_begin;
            bool debug;

            int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);
            void killState(S2EExecutionState *state, std::string mex);
        };

    } // namespace plugins
} // namespace s2e

#endif //SOURCE_PROFILER_H
