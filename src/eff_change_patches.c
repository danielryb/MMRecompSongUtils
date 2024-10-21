#include "modding.h"
#include "global.h"
#include "rt64_extended_gbi.h"

#define PINK_DEKU_FLOWER_LIMB_BACK_RIGHT_PETAL ((0x1BC - 0x198) / sizeof(Vec3s))
extern u8 gameplay_keep_Blob_02900C[];
extern u8 gameplay_keep_Blob_0281DC[];

#include "overlays/actors/ovl_Eff_Change/z_eff_change.h"

#define FLAGS (ACTOR_FLAG_10)

#define THIS ((EffChange*)thisx)

void EffChange_Init(Actor* thisx, PlayState* play);
void EffChange_Destroy(Actor* thisx, PlayState* play);
void EffChange_Update(Actor* thisx, PlayState* play);
void EffChange_Draw(Actor* thisx, PlayState* play);

void EffChange_SetColors(EffChange* this, s32 arg1);
void func_80A4C5CC(EffChange* this, PlayState* play);

RECOMP_PATCH void EffChange_Init(Actor* thisx, PlayState* play) {
    EffChange* this = THIS;

    this->actionFunc = func_80A4C5CC;
    this->actor.draw = EffChange_Draw;
    EffChange_SetColors(this, EFFCHANGE_GET_COLORS(thisx));
    Actor_SetScale(&this->actor, 0.075f);
    this->primColors[3] = 0;
    func_80183430(&this->skeletonInfo, gameplay_keep_Blob_02900C, gameplay_keep_Blob_0281DC, this->jointTable,
                  this->morphTable, NULL);
    func_801834A8(&this->skeletonInfo, gameplay_keep_Blob_0281DC);
    this->step = 0;
    this->actor.shape.rot.y = 0;
    this->skeletonInfo.frameCtrl.unk_C = (2.0f / 3.0f);
    // @mod Disable Elegy of Emptiness' cutscene.
    // CutsceneManager_Queue(CS_ID_GLOBAL_ELEGY);

    // @mod Use further unused actor.params property for storing light adjustment value.
    this->actor.params = 0;
}

#define CUTSCENE_DARKEN_ACCELERATION 52

RECOMP_PATCH void func_80A4C5CC(EffChange* this, PlayState* play) {
    f32 phi_fv0;

    if (func_80183DE0(&this->skeletonInfo)) {
        Actor_Kill(&this->actor);
        // @mod Disable Elegy of Emptiness' cutscene.
        // CutsceneManager_Stop(CS_ID_GLOBAL_ELEGY);
        Environment_AdjustLights(play, 0.0f, 850.0f, 0.2f, 0.0f);
        return;
    }

    this->step++;
    if (this->skeletonInfo.frameCtrl.unk_10 < 20.0f) {
        if ((this->primColors[3]) < 242) {
            this->primColors[3] += 13;
        } else {
            this->primColors[3] = 255;
        }
    } else if (this->skeletonInfo.frameCtrl.unk_10 > 70.0f) {
        if ((this->primColors[3]) >= 14) {
            this->primColors[3] -= 13;
        } else {
            this->primColors[3] = 0;
        }
    } else {
        this->primColors[3] = 255;
    }

    // @mod Manage actor.params value.
    if (CutsceneManager_GetCurrentCsId() <= 0) {
        if (this->actor.params >= 0) {
            if (this->skeletonInfo.frameCtrl.unk_10 < 20.0f) {
                if ((this->actor.params) < 242) {
                    this->actor.params += 13;
                } else {
                    this->actor.params = 255;
                }
            } else if (this->skeletonInfo.frameCtrl.unk_10 > 70.0f) {
                if ((this->actor.params) >= 14) {
                    this->actor.params -= 13;
                } else {
                    this->actor.params = 0;
                }
            } else {
                if ((this->actor.params) < 255 - CUTSCENE_DARKEN_ACCELERATION) {
                    this->actor.params += CUTSCENE_DARKEN_ACCELERATION;
                } else {
                    this->actor.params = 255;
                }
            }
        } else {
            this->actor.params++;
        }
    } else {
        // @mod Disable light adjustment during cutscenes and 1 frame after for cutscene transitions.
        this->actor.params = -1;
    }

    // @mod Calculate light adjustment from actor.params.
    phi_fv0 = this->actor.params * (1.0f / 255.0f);
    if (phi_fv0 > 1.0f) {
        phi_fv0 = 1.0f;
    } else if (phi_fv0 < 0.0f) {
        phi_fv0 = 0.0f;
    }

    Environment_AdjustLights(play, phi_fv0, 850.0f, 0.2f, 0.0f);

    // @mod Disable Elegy of Emptiness' cutscene.
    // if (CutsceneManager_GetCurrentCsId() != CS_ID_GLOBAL_ELEGY) {
    //     if (CutsceneManager_IsNext(CS_ID_GLOBAL_ELEGY)) {
    //         CutsceneManager_Start(CS_ID_GLOBAL_ELEGY, &this->actor);
    //     } else {
    //         CutsceneManager_Queue(CS_ID_GLOBAL_ELEGY);
    //     }
    // }
}