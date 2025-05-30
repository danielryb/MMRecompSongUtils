#include "song_utils.h"

RECOMP_IMPORT("mm_recomp_message_hooks", void mh_Message_DrawMain_set_return_flag(void))

Actor* activeSongEffect = NULL;

extern s16 sOcarinaEffectActorIds[];
extern s32 sOcarinaEffectActorParams[];

RECOMP_PATCH void Message_SpawnSongEffect(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);

    //! FAKE:
    if (1) {}
    if ((msgCtx->songPlayed <= OCARINA_SONG_SCARECROW_SPAWN) &&
        (msgCtx->songPlayed != OCARINA_SONG_GORON_LULLABY_INTRO) &&
        !((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
          (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU))) {
        msgCtx->ocarinaSongEffectActive = true;
        if (msgCtx->songPlayed != OCARINA_SONG_SCARECROW_SPAWN) {
            // @mod Store pointer to song VFX.
            activeSongEffect = Actor_Spawn(&play->actorCtx, play, sOcarinaEffectActorIds[msgCtx->songPlayed], player->actor.world.pos.x,
                        player->actor.world.pos.y, player->actor.world.pos.z, 0, 0, 0,
                        sOcarinaEffectActorParams[msgCtx->songPlayed]);
        } else {
            // @mod Store pointer to song VFX.
            activeSongEffect = Actor_Spawn(&play->actorCtx, play, ACTOR_OCEFF_WIPE4, player->actor.world.pos.x, player->actor.world.pos.y,
                        player->actor.world.pos.z, 0, 0, 0, 0);
        }
    }
}

extern s16 sLastPlayedSong;
void Message_ResetOcarinaButtonState(PlayState* play);


RECOMP_CALLBACK("mm_recomp_message_hooks", mh_on_Message_Update) void on_Message_Update(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Input* input = CONTROLLER1(&play->state);

    u8 msgMode = msgCtx->msgMode;
    if ((msgMode >= MSGMODE_SONG_PLAYED) &&
        (msgMode <= MSGMODE_16)) {
        // Allow skipping song reprise.
        if (CFG_REPRISE_SKIP_MODE != REPRISE_SKIP_NEVER) {
            if (CFG_REPRISE_SKIP_MODE == REPRISE_SKIP_ALWAYS || CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B)) {
                if ((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
                    (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                    Message_CloseTextbox(play);
                    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                } else {
                    if (activeSongEffect == NULL) {
                        Message_SpawnSongEffect(play);
                    }

                    msgCtx->ocarinaStaff->state = 0;

                    play->msgCtx.ocarinaMode = OCARINA_MODE_ACTIVE;
                    if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) {
                        msgCtx->ocarinaAction = OCARINA_ACTION_FREE_PLAY_DONE;
                    }
                    if (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOTIME) {
                        msgCtx->ocarinaAction = OCARINA_ACTION_CHECK_NOTIME_DONE;
                    }
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                    Message_ResetOcarinaButtonState(play);
                    msgCtx->msgMode = MSGMODE_18;
                    msgCtx->stateTimer = 1;

                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);

                    if (activeSongEffect != NULL) {
                        while (activeSongEffect->update) {
                            activeSongEffect->update(activeSongEffect, play);
                        }

                        activeSongEffect = NULL;
                    }
                }

                mh_Message_DrawMain_set_return_flag();
            }
        }
    } else if (msgMode == MSGMODE_18) {
        activeSongEffect = NULL;
    }
}

RECOMP_CALLBACK("mm_recomp_message_hooks", mh_on_Message_DrawMain) void on_Message_DrawMain(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;

    u8 msgMode = msgCtx->msgMode;
    if (msgMode == MSGMODE_18) {
        if (CFG_ELEGY_ANYWHERE) {
            if ((msgCtx->stateTimer == 1) && (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY_DONE) && (sLastPlayedSong == OCARINA_SONG_ELEGY)) {
                msgCtx->stateTimer--;
                Message_CloseTextbox(play);
                play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;

                mh_Message_DrawMain_set_return_flag();
            }
        }
    }
}

extern s16 sOcarinaButtonAPrimR;
extern s16 sOcarinaButtonAPrimB;
extern s16 sOcarinaButtonAPrimG;
extern s16 sOcarinaButtonAEnvR;
extern s16 sOcarinaButtonAEnvB;
extern s16 sOcarinaButtonAEnvG;
extern s16 sOcarinaButtonCPrimR;
extern s16 sOcarinaButtonCPrimB;
extern s16 sOcarinaButtonCPrimG;
extern s16 sOcarinaButtonCEnvR;
extern s16 sOcarinaButtonCEnvB;
extern s16 sOcarinaButtonCEnvG;

void Message_ResetOcarinaButtonAlphas(void);

RECOMP_PATCH void Message_ResetOcarinaButtonState(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_A] = 189;
    // @mod Fix mismatched note positions.
    if (CFG_FIX_NOTE_POSITIONS) {
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_DOWN] = 183;
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_RIGHT] = 175;
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_LEFT] = 172;
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_UP] = 165;
    } else {
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_DOWN] = 184;
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_RIGHT] = 179;
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_LEFT] = 174;
        msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_UP] = 169;
    }

    Message_ResetOcarinaButtonAlphas();

    sOcarinaButtonAPrimR = 80;
    sOcarinaButtonAPrimG = 150;
    sOcarinaButtonAPrimB = 255;
    sOcarinaButtonAEnvR = 10;
    sOcarinaButtonAEnvG = 10;
    sOcarinaButtonAEnvB = 10;
    sOcarinaButtonCPrimR = 255;
    sOcarinaButtonCPrimG = 255;
    sOcarinaButtonCPrimB = 50;
    sOcarinaButtonCEnvR = 10;
    sOcarinaButtonCEnvG = 10;
    sOcarinaButtonCEnvB = 10;
}