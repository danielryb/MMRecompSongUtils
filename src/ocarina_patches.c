#include "song_utils.h"

// #include "audio/code_8019AF00.c"

extern u32 sOcarinaInputButtonCur;
extern u32 sOcarinaFlags;
extern u8 sOcarinaHasStartedSong;
extern u8 sOcarinaStaffPlayingPos;
extern u8 sPrevOcarinaPitch;
extern u8 sCurOcarinaPitch;
extern u8 sCurOcarinaSongWithoutMusicStaff[8];
extern s8 sCurOcarinaBendIndex;
extern u8 sOcarinaWithoutMusicStaffPos;
extern u8 sFirstOcarinaSongIndex;
extern u8 sLastOcarinaSongIndex;
extern u32 sOcarinaAvailableSongFlags;
extern u8 sButtonToPitchMap[5];
extern u8 sPlayedOcarinaSongIndexPlusOne;
extern u8 sIsOcarinaInputEnabled;
extern u8 sMusicStaffPrevPitch;

void AudioOcarina_StartDefault(u32 ocarinaFlags);

void AudioOcarina_CheckIfStartedSong(void);
void AudioOcarina_UpdateCurOcarinaSong(void);

bool L_pressed = false;
bool skipped_AudioOcarina_CheckSongsWithoutMusicStaff = false;
u8 sCurOcarinaPitch_tmp = OCARINA_PITCH_NONE;

RECOMP_HOOK("AudioOcarina_CheckSongsWithoutMusicStaff") void on_AudioOcarina_CheckSongsWithoutMusicStaff(void) {
    L_pressed = CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_L);
    if (L_pressed) {
        sOcarinaInputButtonCur &= ~BTN_L;
    }

    AudioOcarina_CheckIfStartedSong();

    if (!sOcarinaHasStartedSong) {
        return;
    }

    if ((sPrevOcarinaPitch != sCurOcarinaPitch) && (sCurOcarinaPitch != OCARINA_PITCH_NONE)) {
        // @mod Fix instrument animation not playing if L is held.
        if (L_pressed) {
            sOcarinaStaffPlayingPos++;

            if (sOcarinaStaffPlayingPos > ARRAY_COUNT(sCurOcarinaSongWithoutMusicStaff)) {
                sOcarinaStaffPlayingPos = 1;
            }

            u8 tmp = sOcarinaStaffPlayingPos;
            AudioOcarina_StartDefault(sOcarinaFlags);
            sOcarinaStaffPlayingPos = tmp;

            // @mod Force the hooked function to go into the second return.
            sCurOcarinaPitch_tmp = sCurOcarinaPitch;
            sCurOcarinaPitch = OCARINA_PITCH_NONE;

            sOcarinaHasStartedSong = false;

            skipped_AudioOcarina_CheckSongsWithoutMusicStaff = true;
        }
    }
}

RECOMP_HOOK_RETURN("AudioOcarina_CheckSongsWithoutMusicStaff") void after_AudioOcarina_CheckSongsWithoutMusicStaff(void) {
    if (L_pressed) {
        sOcarinaInputButtonCur |= BTN_L;
    }

    if (skipped_AudioOcarina_CheckSongsWithoutMusicStaff) {
        sCurOcarinaPitch = sCurOcarinaPitch_tmp;
        sOcarinaHasStartedSong = true;

        skipped_AudioOcarina_CheckSongsWithoutMusicStaff = false;
    }
}

extern u8 sOcarinaDropInputTimer;

RECOMP_HOOK("AudioOcarina_PlayControllerInput") void on_AudioOcarina_PlayControllerInput(u8 isOcarinaSfxSuppressedWhenCancelled) {
    if (CFG_DISABLE_OCARINA_INPUT_LIMIT) {
        sOcarinaDropInputTimer = 0;
    }
}