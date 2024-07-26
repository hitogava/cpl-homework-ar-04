#include "vector.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

VECTOR(char)

typedef struct {
    vector_char *bit_fields;
} BitSet;

void BitSet_init(BitSet *bset, size_t sz) {
    if (!bset) {
        return;
    }
    bset->bit_fields = halloc_vector_char(sz);
    bset->bit_fields->len = bset->bit_fields->cap;
}

bool BitSet_validate(BitSet *b) {
    return (b && b->bit_fields);
}

void BitSet_destroy(BitSet *bset) {
    if (!bset) {
        return;
    }
    vector_char_destroy(bset->bit_fields);
    free(bset->bit_fields);
}

void BitSet_add(BitSet *bset, unsigned long val) {
    if (!bset) {
        return;
    }
    size_t index = val >> 3;
    for (size_t i = bset->bit_fields->len; i < index + 1; i++) {
        vector_char_add_last(bset->bit_fields, 0);
    }
    char *ch = at(bset->bit_fields, index);
    assert(ch);
    *ch |= (char) (1 << (val % 8));
}

void BitSet_remove(BitSet *bset, unsigned long val) {
    if (val > (bset->bit_fields->len << 3)) {
        return;
    }
    char *ch = at(bset->bit_fields, (val >> 3));
    assert(ch);
    *ch &= (char) (~(1 << (val % 8)));
}

bool BitSet_contains(BitSet *bset, unsigned long val) {
    if (!bset || !bset->bit_fields) {
        return false;
    }
    char *ch;
    if ((ch = at(bset->bit_fields, val >> 3)) != NULL) {
        return (bool) (*ch & (1 << val % 8));
    }
    return false;
}

void BitSet_union(BitSet *b1, BitSet *b2, BitSet *result) {
    if (!b1 || !b2) {
        return;
    }
    size_t max = MAX(b1->bit_fields->len, b2->bit_fields->len);
    BitSet_init(result, max);

    for (size_t i = 0; i < max; i++) {
        char *tmp = at(b1->bit_fields, i);
        char val = 0;
        if (tmp)
            val = *tmp;
        tmp = at(b2->bit_fields, i);
        if (tmp)
            val |= *tmp;
        char *v3 = at(result->bit_fields, i);
        assert(v3);
        *v3 = val;
    }
}

void BitSet_intersect(BitSet *b1, BitSet *b2, BitSet *result) {
    if (!b1 || !b2) {
        return;
    }
    size_t min = MIN(b1->bit_fields->len, b2->bit_fields->len);
    BitSet_init(result, min);

    for (size_t i = 0; i < min; i++) {
        char *tmp = at(b1->bit_fields, i);
        char val = 0;
        if (tmp)
            val = *tmp;
        tmp = at(b2->bit_fields, i);
        if (tmp)
            val &= *tmp;
        else
            val = 0;
        char *v3 = at(result->bit_fields, i);
        assert(v3);
        *v3 = val;
    }
}

void BitSet_print(BitSet *bset) {
    bool flag = true;
    for (size_t i = 0; i < bset->bit_fields->len; i++) {
        for (size_t j = 0; j < 8; j++) {
            unsigned long val = (i << 3) + j;
            if (BitSet_contains(bset, val)) {
                if (flag) {
                    printf("%lu", val);
                    flag = false;
                } else {
                    printf(" %lu", val);
                }
            }
        }
    }
    printf("\n");
}

void BitSet_diff(BitSet *b1, BitSet *b2, BitSet *result) {
    size_t max = MAX(b1->bit_fields->len, b2->bit_fields->len);
    BitSet_init(result, max);
    for (size_t i = 0; i < max; i++) {
        char *t = at(b1->bit_fields, i);
        char x = (t) ? *t : 0;
        t = at(b2->bit_fields, i);
        char y = (t) ? *t : 0;
        t = at(result->bit_fields, i);
        assert(t);
        *t = ~(~x | y);
    }
}

#define SZ 4

int main(void) {
    BitSet *sets[SZ];

    size_t n[SZ];
    for (size_t i = 0; i < SZ; i++) {
        if (scanf("%zu", &n[i]) != 1) {
            puts("Input error");
            exit(1);
        }
        sets[i] = malloc(sizeof(BitSet));
        alloc_guard(sets[i]);
        BitSet_init(sets[i], 0);
    }

    for (size_t i = 0; i < SZ; i++) {
        for (size_t j = 0; j < n[i]; j++) {
            unsigned long x;
            if (scanf("%lu", &x) != 1) {
                puts("Input error");
                exit(1);
            }
            BitSet_add(sets[i], x);
        }
    }

    BitSet *un = malloc(sizeof(BitSet));
    BitSet *intersection = malloc(sizeof(BitSet));
    BitSet *difference = malloc(sizeof(BitSet));

    BitSet_union(sets[0], sets[1], un);
    BitSet_intersect(un, sets[2], intersection);
    BitSet_diff(intersection, sets[3], difference);

    BitSet_print(difference);

    for (size_t i = 0; i < SZ; i++) {
        BitSet_destroy(sets[i]);
        free(sets[i]);
    }

    BitSet_destroy(un);
    BitSet_destroy(intersection);
    BitSet_destroy(difference);

    free(un);
    free(intersection);
    free(difference);
}
