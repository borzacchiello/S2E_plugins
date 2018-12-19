#include "ENFAL/ENFAL.h"
#include "s2elib/s2e.h"

void foo() {
    printf("CIAO\n");
}

void bar() {
    printf("HOLA\n");
}

int main(int argc, char const *argv[])
{
    EnfalGuestCommand command;
    command.cmd = NOTIFY_THREAD_ADDRESS;
    command.thread_address.address = (unsigned int)&foo;
    s2e_invoke_plugin("ENFAL", &command, sizeof(command));

    char dummy;
    s2e_make_concolic(&dummy, 1, "dummy");
    if (dummy)
        foo();
    else
        bar();
    return 0;
}