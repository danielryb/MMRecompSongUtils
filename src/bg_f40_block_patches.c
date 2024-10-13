#include "modding.h"
#include "global.h"

// @mod Custom speed for Stone Tower Blocks.
const float block_placement_speed = 80.0f; // 20.0f
const float block_return_speed = 100.0f; // 40.0f

#include "overlays/actors/ovl_Bg_F40_Block/z_bg_f40_block.h"

extern CollisionHeader gStoneTowerBlockCol;

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((BgF40Block*)thisx)

void BgF40Block_Init(Actor* thisx, PlayState* play);
void BgF40Block_Destroy(Actor* thisx, PlayState* play);
void BgF40Block_Update(Actor* thisx, PlayState* play);
void BgF40Block_Draw(Actor* thisx, PlayState* play);

void func_80BC41AC(BgF40Block* this, PlayState* play);
void func_80BC4228(BgF40Block* this, PlayState* play);
void func_80BC4344(BgF40Block* this, PlayState* play);
void func_80BC4380(BgF40Block* this, PlayState* play);
void func_80BC43CC(BgF40Block* this, PlayState* play);
void func_80BC4448(BgF40Block* this, PlayState* play);
void func_80BC44F4(BgF40Block* this, PlayState* play);
void func_80BC4530(BgF40Block* this, PlayState* play);
void func_80BC457C(BgF40Block* this, PlayState* play);

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

s32 func_80BC3A2C(BgF40Block* this, PlayState* play);
s32 func_80BC3980(BgF40Block* this, PlayState* play);
s32 func_80BC3B00(BgF40Block* this);
s32 func_80BC3D08(BgF40Block* this, PlayState* play, s32 arg2);

RECOMP_PATCH void BgF40Block_Init(Actor* thisx, PlayState* play) {
    BgF40Block* this = THIS;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gStoneTowerBlockCol);

    if (BGF40BLOCK_GET_PATH_INDEX(&this->dyna.actor) != BGF40BLOCK_PATH_INDEX_NONE) {
        this->path = &play->setupPathList[BGF40BLOCK_GET_PATH_INDEX(&this->dyna.actor)];
    } else {
        this->path = NULL;
    }

    if (this->path != NULL) {
        if (Flags_GetSwitch(play, BGF40BLOCK_GET_SWITCH_FLAG(&this->dyna.actor))) {
            this->actionFunc = func_80BC4530;
            // @mod Use custom speed for Stone Tower Blocks.
            this->dyna.actor.speed = block_return_speed;
            func_80BC3A2C(this, play);
        } else {
            this->actionFunc = func_80BC4380;
            // @mod Use custom speed for Stone Tower Blocks.
            this->dyna.actor.speed = block_placement_speed;
            func_80BC3980(this, play);
        }
    } else {
        this->actionFunc = func_80BC457C;
    }
    this->unk_168 = 6;
}


RECOMP_PATCH void func_80BC4228(BgF40Block* this, PlayState* play) {
    if (func_80BC3B00(this)) {
        // @mod Use custom speed for Stone Tower Blocks.
        this->dyna.actor.speed = block_placement_speed;
        if (this->unk_160 < (this->path->count - 1)) {
            this->unk_164 = this->unk_160 + 1;
        } else {
            this->actionFunc = func_80BC4530;
            CutsceneManager_Stop(this->dyna.actor.csId);
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_IKANA_BLOCK_STOP_C);
        }
    }

    if (func_80BC3D08(this, play, 0)) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        this->actionFunc = func_80BC41AC;
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_IKANA_BLOCK_STOP_F);
        return;
    }

    switch (this->unk_168) {
        case 0:
        case 3:
            Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_IKANA_BLOCK_MOVE_X - SFX_FLAG);
            break;

        case 1:
        case 4:
            Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_IKANA_BLOCK_MOVE_Y - SFX_FLAG);
            break;

        case 2:
        case 5:
            Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_IKANA_BLOCK_MOVE_Z - SFX_FLAG);
            break;
    }
}

RECOMP_PATCH void func_80BC4448(BgF40Block* this, PlayState* play) {
    if (func_80BC3B00(this)) {
        // @mod Use custom speed for Stone Tower Blocks.
        this->dyna.actor.speed = block_return_speed;
        if (this->unk_160 > 0) {
            this->unk_164 = this->unk_160 - 1;
        } else {
            this->actionFunc = func_80BC4380;
            CutsceneManager_Stop(this->dyna.actor.csId);
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_IKANA_BLOCK_STOP_C);
        }
    }

    if (func_80BC3D08(this, play, 0)) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        this->actionFunc = func_80BC43CC;
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_IKANA_BLOCK_STOP_F);
    }
}