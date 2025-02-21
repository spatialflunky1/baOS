#include <string.h>

void* memcpy(void* sourcep, void* destp, uint32_t nbytes) {
    /*
     * When both pointers and number of bytes are word aligned, copy "word-at-a-time" instead of "byte-at-a-time"... Wise words I see
     */

    // "word-at-a-time"
    if ((uint64_t)destp % sizeof(word) == 0 &&
        (uint64_t)sourcep % sizeof(word) == 0 &&
        nbytes % sizeof(word) == 0) 
        {
            const word* source = (word*)sourcep;
            word*       dest   = (word*)destp;
            for (uint32_t i = 0; i < nbytes/sizeof(word); i++) {
                dest[i] = source[i];
            }
    }
    // "byte-at-a-time"
    else {
        const uint8_t* source = (uint8_t*)sourcep;
        uint8_t*       dest   = (uint8_t*)destp;
        for (uint32_t i = 0; i < nbytes; i++) {
            dest[i] = source[i];
        }
    }
    return destp;
}

void* memset(void *destp, int constant, uint32_t nbytes) {
    uint8_t* dest = (uint8_t*)destp;
    for (uint32_t i = 0; i < nbytes; i++) {
        dest[i] = constant;
    }
    return destp;
}
