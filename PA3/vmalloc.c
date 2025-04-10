#include "vm.h"
#include "vmlib.h"

void *vmalloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  size_t aligned_size =
      (size + sizeof(struct block_header) + (BLKALIGN - 1)) & ~(BLKALIGN - 1);
  struct block_header *current = heapstart;
  struct block_header *best_fit = NULL;
  size_t best_size = (size_t)-1;

  while (current->size_status != VM_ENDMARK) {
    size_t block_size = current->size_status & VM_BLKSZMASK;

    if (!(current->size_status & VM_BUSY) && block_size >= aligned_size) {
      if (block_size == aligned_size) {
        best_fit = current;
        break;
      }

      if (block_size < best_size) {
        best_fit = current;
        best_size = block_size;
      }
    }

    current = (struct block_header *)((char *)current + block_size);
  }

  if (!best_fit) {
    return NULL;
  }

  size_t best_fit_size = best_fit->size_status & VM_BLKSZMASK;

  if (best_fit_size > aligned_size + sizeof(struct block_header)) {
    struct block_header *new_block =
        (struct block_header *)((char *)best_fit + aligned_size);

    new_block->size_status =
        (best_fit_size - aligned_size) | (best_fit->size_status & 2);

    struct block_footer *new_footer =
        (struct block_footer *)((char *)new_block +
                                (new_block->size_status & VM_BLKSZMASK) -
                                sizeof(struct block_footer));

    new_footer->size = new_block->size_status & VM_BLKSZMASK;

    best_fit->size_status =
        aligned_size | VM_BUSY | (best_fit->size_status & 2);
  } else {
    best_fit->size_status |= VM_BUSY;
  }

  struct block_header *next =
      (struct block_header *)((char *)best_fit +
                              (best_fit->size_status & VM_BLKSZMASK));

  if (next->size_status != VM_ENDMARK) {
    next->size_status |= VM_PREVBUSY;
  }

  return (void *)((char *)best_fit + sizeof(struct block_header));
}
