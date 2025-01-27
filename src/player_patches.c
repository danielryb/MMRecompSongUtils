#include "modding.h"
#include "global.h"
#include "rt64_extended_gbi.h"

#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"

// elegy of emptiness
RECOMP_PATCH void func_80848640(PlayState* play, Player* this) {
    EnTorch2* torch2;
    Actor* effChange;

    torch2 = play->actorCtx.elegyShells[this->transformation];
    if (torch2 != NULL) {
        Math_Vec3f_Copy(&torch2->actor.home.pos, &this->actor.world.pos);
        torch2->actor.home.rot.y = this->actor.shape.rot.y;
        torch2->state = 0;
        // @mod Disable Elegy of Emptiness' delay.
        // torch2->framesUntilNextState = 20;
    } else {
        torch2 = (EnTorch2*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TORCH2, this->actor.world.pos.x,
                                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                                        this->transformation);
    }

    if (torch2 != NULL) {
        play->actorCtx.elegyShells[this->transformation] = torch2;
        Play_SetupRespawnPoint(play, this->transformation + 3, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B));
    }

    effChange = Actor_Spawn(&play->actorCtx, play, ACTOR_EFF_CHANGE, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                            (GET_PLAYER_FORM << 3) | this->transformation);
    if (effChange != NULL) {
        //! @bug: This function should only pass Player*: it uses *(this + 0x153), which is meant to be
        //! player->currentMask, but in this case is garbage in the skelAnime
        Player_PlaySfx((Player*)effChange, NA_SE_PL_TRANSFORM);
    }
}

void func_8085B384(Player* this, PlayState* play);

RECOMP_PATCH void Player_Action_88(Player* this, PlayState* play) {
    // @mod Disable Elegy of Emptiness' delay and cutscene.
    // if (this->av2.actionVar2++ > 90) {
    //     play->msgCtx.ocarinaMode = OCARINA_MODE_END;
    //     func_8085B384(this, play);
    // } else if (this->av2.actionVar2 == 10) {
    //     func_80848640(play, this);
    // }
    if (this->av2.actionVar2++ == 0) {
        func_80848640(play, this);
    } else if (this->av2.actionVar2 == 10) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        func_8085B384(this, play);
    }
}