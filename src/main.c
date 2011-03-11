#include <stdio.h>

#include "vm.h"
#include "bytecode.h"
#include "message.h"

int main(int argc, char** argv) {
    SkVM *vm = sk_vm_new();
    SkObject *avalanche;
    int i;

    if(argc == 1) {
        printf("skelde vm\nUsage:\tskelde FILE [FILE ...]\n\n\tIf FILE is -, the bytecode is read from stdin.\n\n");
        return 1;
    }
    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-") == 0) {
	    avalanche = sk_bytecode_parse_stream(vm, stdin);
	    i--; // TODO: maybe do that nicer. WE ARE DOING IT FOREVAAAA.
	} else {
            avalanche = sk_bytecode_parse_filename(vm, argv[i]);
        }
        sk_vm_setup_thread(vm);
        sk_message_dispatch_avalanche(avalanche);
    }

    pthread_exit(NULL);
}
