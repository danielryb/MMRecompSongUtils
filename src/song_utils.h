#include "global.h"

#include "modding.h"

RECOMP_IMPORT("*", void recomp_printf(char* temp, ...));

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
RECOMP_IMPORT("*", double recomp_get_config_double(const char* key));

enum reprise_skip_mode {
    REPRISE_SKIP_ALWAYS,
    REPRISE_SKIP_BUTTON,
    REPRISE_SKIP_NEVER,
};

#define CFG_REPRISE_SKIP_MODE recomp_get_config_u32("reprise_skip_mode")
#define CFG_FIX_NOTE_POSITIONS !(bool)recomp_get_config_u32("fix_note_positions")
#define CFG_DISABLE_OCARINA_INPUT_LIMIT !(bool)recomp_get_config_u32("disable_ocarina_input_limit")
#define CFG_ELEGY_ANYWHERE !(bool)recomp_get_config_u32("elegy_anywhere")
#define CFG_BLOCK_PLACEMENT_SPEED recomp_get_config_double("stone_tower_block_placement_speed")
#define CFG_BLOCK_RETURN_SPEED recomp_get_config_double("stone_tower_block_return_speed")