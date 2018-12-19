#ifndef ENFAL_H
#define ENFAL_H

typedef enum BORZ_ENFAL_COMMAND {
    NOTIFY_THREAD_ADDRESS
} BORZ_ENFAL_COMMAND;

typedef struct THREAD_ADDRESS {
    unsigned int address;
} THREAD_ADDRESS;

typedef struct EnfalGuestCommand {
    BORZ_ENFAL_COMMAND cmd;
    union {
        THREAD_ADDRESS thread_address;
    };
} EnfalGuestCommand;

#endif
