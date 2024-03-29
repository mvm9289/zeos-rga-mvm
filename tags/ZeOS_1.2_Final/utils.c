#include <utils.h>
#include <types.h>
#include <mm_address.h>

void copy_data(void *start, void *dest, int size) {
    DWord *p = start, *q = dest;
    Byte *p1, *q1;
    while(size > 4) {
        *q++ = *p++;
        size -= 4;
    }
    p1=(Byte*)p;
    q1=(Byte*)q;
    while(size > 0) {
        *q1++ = *p1++;
        size --;
    }
}

/* Copia de espacio de usuario a espacio de kernel, devuelve 0 si ok y -1 si error*/
int copy_from_user(void *start, void *dest, int size) {
    DWord *p = start, *q = dest;
    Byte *p1, *q1;
    while(size > 4) {
        *q++ = *p++;
        size -= 4;
    }
    p1=(Byte*)p;
    q1=(Byte*)q;
    while(size > 0) {
        *q1++ = *p1++;
        size --;
    }
    return 0;
}

/* Copia de espacio de kernel a espacio de usuario, devuelve 0 si ok y -1 si error*/
int copy_to_user(void *start, void *dest, int size) {
    DWord *p = start, *q = dest;
    Byte *p1, *q1;
    while(size > 4) {
        *q++ = *p++;
        size -= 4;
    }
    p1=(Byte*)p;
    q1=(Byte*)q;
    while(size > 0) {
        *q1++ = *p1++;
        size --;
    }
    return 0;
}

/* access_ok: Checks if a user space pointer is valid
 * @type:  Type of access: %VERIFY_READ or %VERIFY_WRITE. Note that
 *         %VERIFY_WRITE is a superset of %VERIFY_READ: if it is safe
 *         to write to a block, it is always safe to read from it
 * @addr:  User space pointer to start of block to check
 * @size:  Size of block to check
 * Returns true (nonzero) if the memory block may be valid,
 *         false (zero) if it is definitely invalid
 */
int access_ok(int type, const void * addr, unsigned long size) {
    unsigned long pag_start = NUM_PAGE_ADDR((unsigned long)addr);
    unsigned long pag_end = NUM_PAGE_ADDR(((unsigned long)addr+size));

    if(pag_start < NUM_PAG_KERNEL) return 0;
    else if(pag_end >= NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA) return 0;
    else if(pag_start < (NUM_PAG_KERNEL+NUM_PAG_CODE) && type==WRITE) return 0; 
    else return 1;
}

