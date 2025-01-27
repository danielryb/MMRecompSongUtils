#include "modding.h"
#include "global.h"

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

void AudioOcarina_StartDefault(u32 ocarinaFlags);

void AudioOcarina_CheckIfStartedSong(void);
void AudioOcarina_UpdateCurOcarinaSong(void);

/**
 * Checks for ocarina songs from user input with no music staff prompt.
 * Includes ocarina actions such as free play, no warp
 */
RECOMP_FORCE_PATCH void AudioOcarina_CheckSongsWithoutMusicStaff(void) {
    u32 pitch;
    u8 ocarinaStaffPlayingPosStart;
    u8 songIndex;
    u8 j;
    u8 k;

    // @mod Fix instrument animation not playing if L is held.
    // if (CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_L) &&
    //     CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_A | BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP)) {
    //     AudioOcarina_StartDefault(sOcarinaFlags);
    //     return;
    // }

    AudioOcarina_CheckIfStartedSong();

    if (!sOcarinaHasStartedSong) {
        return;
    }


    ocarinaStaffPlayingPosStart = sOcarinaStaffPlayingPos;
    if ((sPrevOcarinaPitch != sCurOcarinaPitch) && (sCurOcarinaPitch != OCARINA_PITCH_NONE)) {
        sOcarinaStaffPlayingPos++;

        if (sOcarinaStaffPlayingPos > ARRAY_COUNT(sCurOcarinaSongWithoutMusicStaff)) {
            sOcarinaStaffPlayingPos = 1;
        }

        // @mod Fix instrument animation not playing if L is held.
        if (CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_L)) {
            u8 tmp = sOcarinaStaffPlayingPos;
            AudioOcarina_StartDefault(sOcarinaFlags);
            sOcarinaStaffPlayingPos = tmp;
            return;
        }

        AudioOcarina_UpdateCurOcarinaSong();

        if ((ABS_ALT(sCurOcarinaBendIndex) > 20) && (ocarinaStaffPlayingPosStart != sOcarinaStaffPlayingPos)) {
            sCurOcarinaSongWithoutMusicStaff[sOcarinaWithoutMusicStaffPos - 1] = OCARINA_PITCH_NONE;
        } else {
            sCurOcarinaSongWithoutMusicStaff[sOcarinaWithoutMusicStaffPos - 1] = sCurOcarinaPitch;
        }


        // This nested for-loop tests to see if the notes from the ocarina are identical
        // to any of the songIndex from sFirstOcarinaSongIndex to sLastOcarinaSongIndex

        // Loop through each of the songs
        for (songIndex = sFirstOcarinaSongIndex; songIndex < sLastOcarinaSongIndex; songIndex++) {
            // Checks to see if the song is available to be played
            if ((u32)sOcarinaAvailableSongFlags & (1 << songIndex)) {
                // Loops through all possible starting indices?
                // Loops through the notes of the song?
                for (j = 0, k = 0; (j < gOcarinaSongButtons[songIndex].numButtons) && (k == 0) &&
                                   (sOcarinaWithoutMusicStaffPos >= gOcarinaSongButtons[songIndex].numButtons);) {

                    pitch = sCurOcarinaSongWithoutMusicStaff[(sOcarinaWithoutMusicStaffPos -
                                                              gOcarinaSongButtons[songIndex].numButtons) +
                                                             j];

                    if (pitch == sButtonToPitchMap[gOcarinaSongButtons[songIndex].buttonIndex[j]]) {
                        j++;
                    } else {
                        k++;
                    }
                }

                // This conditional is true if songIndex = i is detected
                if (j == gOcarinaSongButtons[songIndex].numButtons) {
                    sPlayedOcarinaSongIndexPlusOne = songIndex + 1;
                    sIsOcarinaInputEnabled = false;
                    sOcarinaFlags = 0;
                }
            }
        }
    }
}