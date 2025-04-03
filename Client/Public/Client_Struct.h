#pragma once


namespace Client
{
    struct OBJECT_DESC
    {
        virtual ~OBJECT_DESC() {}
    };

    struct PLAYER_DESC : public OBJECT_DESC
    {
        int     iCurrentHP;
        int     iMaxHP;
        int     iAttackPoint;
        float   fSpeed;
        float   fEffectiveRange;
        bool    bStunned;

        PLAYER_DESC(int currentHP, int maxHP, int attackPoint, float speed, float effectiveRange, bool stunned = false)
            : iCurrentHP(currentHP), iMaxHP(maxHP), iAttackPoint(attackPoint),
            fSpeed(speed), fEffectiveRange(effectiveRange), bStunned(stunned) {
        }

        ~PLAYER_DESC() override = default;
    };

}
