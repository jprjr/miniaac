#include "cpe_extras.h"

#include "maac_align.h"

maac_const
MAAC_PUBLIC
size_t
maac_cpe_size(void) {
    return sizeof(maac_cpe);
}

struct maac_cpe_aligner {
    char c;
    maac_cpe cpe;
};

maac_const
MAAC_PUBLIC
size_t
maac_cpe_alignof(void) {
    return offsetof(struct maac_cpe_aligner, cpe);
}

maac_pure
MAAC_PUBLIC
maac_cpe*
maac_cpe_align(void* p) {
    return (maac_cpe*)maac_align(p, maac_cpe_alignof());
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_state(const maac_cpe* c) {
    return (maac_u32)c->state;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_element_instance_tag(const maac_cpe* c) {
    return (maac_u32)c->element_instance_tag;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_common_window(const maac_cpe* c) {
    return (maac_u32)c->common_window;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_ms_mask_present(const maac_cpe* c) {
    return (maac_u32)c->ms_mask_present;
}

static const char* MAAC_CPE_STATE_INVALID_STR = "INVALID";
static size_t MAAC_CPE_STATE_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_cpe_state_str_tbl[7] = {
    "TAG",
    "COMMON_WINDOW",
    "ICS_INFO",
    "MS_MASK_PRESENT",
    "MS_USED",
    "ICS_LEFT",
    "ICS_RIGHT"
};

static const size_t maac_cpe_state_len_tbl[7] = {
    sizeof("TAG") - 1,
    sizeof("COMMON_WINDOW") - 1,
    sizeof("ICS_INFO") - 1,
    sizeof("MS_MASK_PRESENT") - 1,
    sizeof("MS_USED") - 1,
    sizeof("ICS_LEFT") - 1,
    sizeof("ICS_RIGHT") - 1
};


maac_const
MAAC_PUBLIC
const char*
maac_cpe_state_name(const maac_u32 state) {
    return state > MAAC_CPE_STATE_ICS_RIGHT ?
      MAAC_CPE_STATE_INVALID_STR
      :
      maac_cpe_state_str_tbl[state];
}

maac_const
MAAC_PUBLIC
size_t
maac_cpe_state_name_len(const maac_u32 state) {
    return state > MAAC_CPE_STATE_ICS_RIGHT ?
      MAAC_CPE_STATE_INVALID_LEN
      :
      maac_cpe_state_len_tbl[state];
}


