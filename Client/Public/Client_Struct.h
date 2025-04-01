#pragma once

namespace Client
{
	typedef struct tagPlayerDesc
	{
		int     _currentHP;
        int     _maxHP;
        int     _attackPoint;      // 보통 1 per 플레이어 하트 1/2
        float   _speed;
        float   _effectiveRange;   // 무기별 공격 유효 사거리
        bool    _bStunned  = false;

	}PLAYER_DESC;
}
