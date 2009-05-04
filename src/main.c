#include <stdio.h>

#include "vm.h"
#include "bytecode.h"
#include "message.h"

int main(int argc, char** argv) {
    SkVM *vm = sk_vm_new();
    SkObject *avalanche; 
    int i;
    
    if(argc == 1) {
        printf("skelde vm\nUsage:\tskelde FILE [FILE ...]\n");
        return 1;
    }
    for(i = 1; i < argc; i++) {
        avalanche = sk_bytecode_parse_filename(vm, argv[i]);
        sk_vm_setup_thread(vm);
        sk_message_dispatch_avalanche(avalanche);
    }

    pthread_exit(NULL);
}
