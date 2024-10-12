#include "modding.h"
#include "global.h"
#include "rt64_extended_gbi.h"

#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((EnTorch2*)thisx)

void EnTorch2_Init(Actor* thisx, PlayState* play);
void EnTorch2_Destroy(Actor* thisx, PlayState* play);
void EnTorch2_Update(Actor* thisx, PlayState* play);
void EnTorch2_Draw(Actor* thisx, PlayState* play2);

void EnTorch2_UpdateIdle(Actor* thisx, PlayState* play);
void EnTorch2_UpdateDeath(Actor* thisx, PlayState* play);

RECOMP_PATCH void EnTorch2_Update(Actor* thisx, PlayState* play) {
    EnTorch2* this = THIS;
    u16 targetAlpha;
    u16 remainingFrames;
    s32 pad[2];

    if (this->state == TORCH2_STATE_IDLE) {
        this->actor.update = EnTorch2_UpdateIdle;
        return;
    }

    this->actor.gravity = -1.0f;
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 20.0f, 70.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    // @mod Disable Elegy of Emptiness' delay.
    // if (this->framesUntilNextState == 0) {
    //     remainingFrames = 0;
    // } else {
    //     remainingFrames = --this->framesUntilNextState;
    // }

    // @mod Disable Elegy of Emptiness' delay.
    // if (remainingFrames == 0) {

    u16 stepSize = 8;
    if (this->state == TORCH2_STATE_INITIALIZED) {
        // Spawn in
        if (this->alpha == 0) {
            Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
            this->actor.shape.rot.y = this->actor.home.rot.y;
            this->state = TORCH2_STATE_FADING_IN;
        }
        stepSize = 32;
        targetAlpha = 0;
    } else if (this->state == TORCH2_STATE_FADING_IN) {
        // Stay semitransparent until the player moves away
        if ((this->actor.xzDistToPlayer > 32.0f) || (fabsf(this->actor.playerHeightRel) > 70.0f)) {
            this->state = TORCH2_STATE_SOLID;
        }
        targetAlpha = 60;
    } else {
        // Once the player has moved away, update collision and become opaque
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        targetAlpha = 255;
    }
    Math_StepToS(&this->alpha, targetAlpha, stepSize);

    // @mod Disable Elegy of Emptiness' delay.
    // }
}