#ifndef CTOOLS_HEAP
#define CTOOLS_HEAP

#include <stddef.h>

void heap_build(void* heap_array, const size_t heap_array_size, const int element_size, int (*comp)(const void* lhs, const void* rhs));

/// @brief Verifies that the given array adheres to the attributes of a max heap.
/// @param heap_array The array containing the candidate heap.
/// @param heap_array_size The size of the containing array.
/// @return The number of violations found, or zero if the array contains a valid heap.
int heap_verify(const void* heap_array, const size_t heap_array_size, const int element_size, int (*comp)(const void* lhs, const void* rhs));

#endif // CTOOLS_HEAP