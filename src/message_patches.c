#include "modding.h"
#include "global.h"

RECOMP_IMPORT("mm_recomp_message_hooks", void set_return_flag(void))

Actor* activeSongEffect;

void Message_ResetOcarinaButtonState(PlayState* play);

RECOMP_CALLBACK("mm_recomp_message_hooks", on_Message_Update) void on_Message_Update(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Input* input = CONTROLLER1(&play->state);

    u8 msgMode = msgCtx->msgMode;
    if ((msgMode >= MSGMODE_SONG_PLAYED) &&
        (msgMode <= MSGMODE_16)) {
        // Allow skipping song playback.
        if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B)) {
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

            if (msgCtx->ocarinaSongEffectActive) {
                Actor_Kill(activeSongEffect);
            }
        }

        set_return_flag();
    }
}

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