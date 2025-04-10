#include "vm.h"
#include "vmlib.h"

/**
 * The vmfree() function frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to vmmalloc().
 * Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 */
void vmfree(void *ptr) {
  if (!ptr) {
    return;
  }

  struct block_header *block =
      (struct block_header *)((char *)ptr - sizeof(struct block_header));

  if (!(block->size_status & VM_BUSY)) {
    return;
  }

  block->size_status &= ~VM_BUSY;

  struct block_header *next =
      (struct block_header *)((char *)block +
                              (block->size_status & VM_BLKSZMASK));

  if (next->size_status != VM_ENDMARK) {
    next->size_status &= ~VM_PREVBUSY;
  }

  struct block_footer *footer =
      (struct block_footer *)((char *)block +
                              (block->size_status & VM_BLKSZMASK) -
                              sizeof(struct block_footer));

  footer->size = block->size_status & VM_BLKSZMASK;

  if (!(next->size_status & VM_BUSY)) {
    block->size_status += next->size_status & VM_BLKSZMASK;
    struct block_footer *next_footer =
        (struct block_footer *)((char *)block +
                                (block->size_status & VM_BLKSZMASK) -
                                sizeof(struct block_footer));

    next_footer->size = block->size_status & VM_BLKSZMASK;
  }

  if (!(block->size_status & VM_PREVBUSY)) {
    struct block_footer *prev_footer =
        (struct block_footer *)((char *)block - sizeof(struct block_footer));

    struct block_header *prev =
        (struct block_header *)((char *)block - prev_footer->size);

    prev->size_status += block->size_status & VM_BLKSZMASK;

    struct block_footer *new_footer =
        (struct block_footer *)((char *)prev +
                                (prev->size_status & VM_BLKSZMASK) -
                                sizeof(struct block_footer));

    new_footer->size = prev->size_status & VM_BLKSZMASK;
  }
}
