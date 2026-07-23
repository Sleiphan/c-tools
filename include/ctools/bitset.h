#ifndef CTOOLS_BITSET
#define CTOOLS_BITSET

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#define _BITSET_INDEX unsigned int
#define _BITSET_ENTRY uint64_t

struct bitset {
    _BITSET_ENTRY* entries;
    _BITSET_INDEX bit_count;
};

int bitset_create(struct bitset* bs, const _BITSET_INDEX bit_count) {
    const _BITSET_INDEX entry_count = bit_count / (sizeof(_BITSET_ENTRY) * 8) + 1;

    _BITSET_ENTRY* entries = (_BITSET_ENTRY*) malloc(entry_count * sizeof(_BITSET_ENTRY));
    if (!entries)
        return -1;

    // Assign a common default value to all bits
    memset(entries, 0, entry_count * sizeof(_BITSET_ENTRY));

    bs->entries = entries;
    bs->bit_count = bit_count;

    return 0;
}

static inline void bitset_destroy(struct bitset* bs) {
    free(bs->entries);
}

static inline int bitset_assign(struct bitset* bs, const _BITSET_INDEX index, int value) {
    if (index < 0 || index >= bs->bit_count) {
        errno = EINVAL;
        return -1;
    }

    const int value_normalized = value ? 1 : 0;

    const _BITSET_INDEX entry_idx_maj = index / (sizeof(_BITSET_ENTRY) * 8);
    const _BITSET_INDEX entry_idx_min = index & (sizeof(_BITSET_ENTRY) * 8 - 1);
    const _BITSET_ENTRY bit_mask = ((_BITSET_ENTRY)1) << entry_idx_min;

    bs->entries[entry_idx_maj] &= ~bit_mask;
    bs->entries[entry_idx_maj] |= bit_mask * value_normalized;

    return 0;
}

static inline int bitset_get(struct bitset* bs, const _BITSET_INDEX index) {
    if (index < 0 || index >= bs->bit_count) {
        errno = EINVAL;
        return -1;
    }

    const _BITSET_INDEX entry_idx_maj = index / (sizeof(_BITSET_ENTRY) * 8);
    const _BITSET_INDEX entry_idx_min = index & (sizeof(_BITSET_ENTRY) * 8 - 1);
    const _BITSET_ENTRY bit_mask = ((_BITSET_ENTRY)1) << entry_idx_min;

    return (bs->entries[entry_idx_maj] & bit_mask) != 0;
}

int bitset_search_up(const struct bitset* bs, _BITSET_INDEX* dst, const _BITSET_INDEX from, const _BITSET_INDEX to) {
    int invalid_argument =
        from < 0 ||
        from >= bs->bit_count ||
        to < 0 ||
        to > bs->bit_count ||
        from >= to;

    if (invalid_argument) {
        errno = EINVAL;
        return -1;
    }

    const _BITSET_INDEX entry_count = bs->bit_count / (sizeof(_BITSET_ENTRY) * 8) + 1;
    const _BITSET_INDEX from_idx_maj = from / (sizeof(_BITSET_ENTRY) * 8);
    const _BITSET_INDEX from_idx_min = from & (sizeof(_BITSET_ENTRY) * 8 - 1);
    const _BITSET_INDEX   to_idx_maj =   to / (sizeof(_BITSET_ENTRY) * 8);
    const _BITSET_INDEX   to_idx_min =   to & (sizeof(_BITSET_ENTRY) * 8 - 1);

    // Backup the entry we're searching from
    const _BITSET_ENTRY from_entry_bckp = bs->entries[from_idx_maj];

    // Filter away the bits we should not check
    bs->entries[from_idx_maj] &= ~((1ULL << from_idx_min) - 1);

    // Find the next entry containing a set bit
    _BITSET_INDEX entry_idx = from_idx_maj;
    for (; entry_idx < to_idx_maj && bs->entries[entry_idx] == 0; entry_idx++);

    // Avoid assigning `dst` if no set bit was found
    if (entry_idx == entry_count) {
        // Remove the filter
        bs->entries[from_idx_maj] = from_entry_bckp;

        // Indicate no findings
        return 1;
    }

    // Find the lowest set bit
    const int bit = __builtin_ctzll(bs->entries[entry_idx]);

    // Remove the filter
    bs->entries[from_idx_maj] = from_entry_bckp;

    // Cancel if the discovered bit is beyond the upper bound
    int outside_upper_bound = entry_idx == to_idx_maj && bit >= to_idx_min;
    if (outside_upper_bound)
        return 1;

    // Return the set bit's index to the caller
    *dst = bit + entry_idx * sizeof(_BITSET_ENTRY) * 8;

    return 0;
}

#undef _BITSET_INDEX
#undef _BITSET_ENTRY
#endif // CTOOLS_BITSET
