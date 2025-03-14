#include "modding.h"
#include "global.h"

struct CollisionPoly* D_801ED8B0; // 1 func
extern s32 D_801ED8B4;            // 2 funcs

s32 func_800B7678(PlayState* play, Actor* actor, Vec3f* pos, s32 updBgCheckInfoFlags);

RECOMP_PATCH void Actor_UpdateBgCheckInfo(PlayState* play, Actor* actor, f32 wallCheckHeight, f32 wallCheckRadius,
                             f32 ceilingCheckHeight, u32 updBgCheckInfoFlags) {
    f32 sp94 = actor->world.pos.y - actor->prevPos.y;
    s32 pad;
    Vec3f pos;

    // @mod Fix actors clipping through dynapolys.
    f32 dynapoly_y_disp = 0;
    if ((actor->floorBgId != BGCHECK_SCENE) && (actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
        f32 og_y = actor->world.pos.y;
        bool wasUpdated;

        wasUpdated = DynaPolyActor_TransformCarriedActor(&play->colCtx, actor->floorBgId, actor);

        if (wasUpdated) {
            dynapoly_y_disp = actor->world.pos.y - og_y;
        }
    }

    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_1) {
        s32 bgId;

        actor->bgCheckFlags &= ~BGCHECKFLAG_PLAYER_1000;
        if ((!(updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_80) &&
             (BgCheck_EntitySphVsWall3(&play->colCtx, &pos, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                       &actor->wallPoly, &bgId, actor, wallCheckHeight))) ||
            ((updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_80) &&
             (BgCheck_EntitySphVsWall4(&play->colCtx, &pos, &actor->world.pos, &actor->prevPos, wallCheckRadius,
                                       &actor->wallPoly, &bgId, actor, wallCheckHeight)))) {
            CollisionPoly* sp7C = actor->wallPoly;

            actor->bgCheckFlags |= BGCHECKFLAG_WALL;
            if ((updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_200) && (actor->bgCheckFlags & BGCHECKFLAG_PLAYER_1000) &&
                ((s32)sp7C->normal.y > 0) && (sqrtf(SQXYZ(actor->colChkInfo.displacement)) < 10.0f)) {
                actor->bgCheckFlags &= ~BGCHECKFLAG_WALL;
            } else if (actor->bgCheckFlags & BGCHECKFLAG_WALL) {
                Math_Vec3f_Copy(&actor->world.pos, &pos);
            }

            actor->wallYaw = Math_Atan2S_XY(sp7C->normal.z, sp7C->normal.x);
            actor->wallBgId = bgId;
        } else {
            actor->bgCheckFlags &= ~BGCHECKFLAG_WALL;
        }
    }

    pos.x = actor->world.pos.x;
    pos.z = actor->world.pos.z;
    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_2) {
        f32 y;

        pos.y = actor->prevPos.y + 4.0f;
        if (BgCheck_EntityCheckCeiling(&play->colCtx, &y, &pos, (ceilingCheckHeight + sp94) - 4.0f, &D_801ED8B0,
                                       &D_801ED8B4, actor)) {
            actor->bgCheckFlags |= BGCHECKFLAG_CEILING;
            actor->world.pos.y = (y + sp94) - 4.0f;
        } else {
            actor->bgCheckFlags &= ~BGCHECKFLAG_CEILING;
        }
    }

    if ((updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_4)) {
        WaterBox* waterbox;
        f32 y;

        // @mod Fix actors clipping through dynapolys.
        pos.y = actor->prevPos.y + dynapoly_y_disp;
        func_800B7678(play, actor, &pos, updBgCheckInfoFlags);
        y = actor->world.pos.y;

        if (WaterBox_GetSurface1(play, &play->colCtx, actor->world.pos.x, actor->world.pos.z, &y, &waterbox)) {
            actor->depthInWater = y - actor->world.pos.y;
            if (actor->depthInWater <= 0.0f) {
                actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            } else if (!(actor->bgCheckFlags & BGCHECKFLAG_WATER)) {
                actor->bgCheckFlags |= (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
                if (!(updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_40)) {
                    Vec3f sp64;

                    sp64.x = actor->world.pos.x;
                    sp64.y = y;
                    sp64.z = actor->world.pos.z;

                    EffectSsGRipple_Spawn(play, &sp64, 100, 500, 0);
                    EffectSsGRipple_Spawn(play, &sp64, 100, 500, 4);
                    EffectSsGRipple_Spawn(play, &sp64, 100, 500, 8);
                }
            } else {
                actor->bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
            }
        } else {
            actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            actor->depthInWater = BGCHECK_Y_MIN;
        }
    }

    if (updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_400) {
        WaterBox* waterbox;
        f32 y = actor->world.pos.y;

        if (WaterBox_GetSurface1(play, &play->colCtx, actor->world.pos.x, actor->world.pos.z, &y, &waterbox)) {
            actor->depthInWater = y - actor->world.pos.y;

            if (actor->depthInWater < 0.0f) {
                actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            } else if (!(actor->bgCheckFlags & BGCHECKFLAG_WATER)) {
                actor->bgCheckFlags |= (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
                if (!(updBgCheckInfoFlags & UPDBGCHECKINFO_FLAG_40)) {
                    Vec3f sp50;

                    sp50.x = actor->world.pos.x;
                    sp50.y = y;
                    sp50.z = actor->world.pos.z;

                    EffectSsGRipple_Spawn(play, &sp50, 100, 500, 0);
                    EffectSsGRipple_Spawn(play, &sp50, 100, 500, 4);
                    EffectSsGRipple_Spawn(play, &sp50, 100, 500, 8);
                }
            } else {
                actor->bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
            }
        } else {
            actor->bgCheckFlags &= ~(BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH);
            actor->depthInWater = BGCHECK_Y_MIN;
        }
    }
}