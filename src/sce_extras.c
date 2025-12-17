#include "sce_extras.h"

#include "maac_align.h"

maac_const
MAAC_PUBLIC
size_t
maac_sce_size(void) {
    return sizeof(maac_sce);
}

struct maac_sce_aligner {
    char c;
    maac_sce s;
};

maac_const
MAAC_PUBLIC
size_t
maac_sce_alignof(void) {
    return offsetof(struct maac_sce_aligner, s);
}

maac_pure
MAAC_PUBLIC
maac_sce*
maac_sce_align(void* p) {
    return (maac_sce*)maac_align(p, maac_sce_alignof());
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_sce_state(const maac_sce* s) {
    return (maac_u32)s->state;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_sce_element_instance_tag(const maac_sce* s) {
    return (maac_u32)s->element_instance_tag;
}

static const char* MAAC_SCE_STATE_INVALID_STR = "INVALID";
static size_t MAAC_SCE_STATE_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_sce_state_str_tbl[2] = {
    "TAG",
    "ICS"
};

static const size_t maac_sce_state_len_tbl[2] = {
    sizeof("TAG") - 1,
    sizeof("ICS") - 1
};


maac_const
MAAC_PUBLIC
const char*
maac_sce_state_name(const maac_u32 state) {
    return state > MAAC_SCE_STATE_TAG ?
      MAAC_SCE_STATE_INVALID_STR
      :
      maac_sce_state_str_tbl[state];
}

maac_const
MAAC_PUBLIC
size_t
maac_sce_state_name_len(const maac_u32 state) {
    return state > MAAC_SCE_STATE_TAG ?
      MAAC_SCE_STATE_INVALID_LEN
      :
      maac_sce_state_len_tbl[state];
}

