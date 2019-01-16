#ifndef KILLAFTERN_H
#define KILLAFTERN_H

typedef enum BORZ_KILLAFTERN_COMMAND {
    START_COUNT
} BORZ_KILLAFTERN_COMMAND;

typedef struct KillAfterNGuestCommand {
    BORZ_KILLAFTERN_COMMAND cmd;
    union {
        int num_instructions;
    };
} KillAfterNGuestCommand;

#endif
