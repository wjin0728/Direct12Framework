#pragma once
#include "Object.h"
#include "PlayerState.h"

class PlayerCharacter : public Object
{
public:
	Vec3				_look_dir; // 캐릭터가 보고있는 방향

	S_PLAYER_CLASS	    _class;
	int					_hp;
	S_ITEM_TYPE			_skill;
	S_PLAYER_STATE		_state;

	int					_barrier;
	bool				_on_FireEnchant;
	bool				_on_GrassWeaken;

	PlayerStateMachine*		currentState;	
	PlayerStateMachine*		previousState;

	PlayerCharacter() :
		Object(S_OBJECT_TYPE::S_PLAYER),
		_look_dir(Vec3(1, 1, 1)), 
		_class(S_PLAYER_CLASS::end),
		_barrier(0),
		_on_FireEnchant(false),	
		_on_GrassWeaken(false),
		currentState(&PlayerState::IdleState::GetInstance()),
		previousState(nullptr) {};
	~PlayerCharacter() {};

	void SetClass(S_PLAYER_CLASS class_type) {
		_class = class_type;
		if (S_PLAYER_CLASS::FIGHTER == _class) {
			_hp = MAX_HP_FIGHTER;
		}
		else if (S_PLAYER_CLASS::ARCHER == _class) {
			_hp = MAX_HP_ARCHER_MAGE;
		}
		else if (S_PLAYER_CLASS::MAGE == _class) {
			_hp = MAX_HP_ARCHER_MAGE;
		}
	};
	int PlayerMaxHp() {
		if (S_PLAYER_CLASS::FIGHTER == _class)
			return (_class == S_PLAYER_CLASS::FIGHTER) 
			? MAX_HP_FIGHTER : MAX_HP_ARCHER_MAGE;
	}

	void SetLookDir(float x, float y, float z) { _look_dir = Vec3(x, y, z); };
	void SetLookDir(Vec3 dir) { _look_dir = dir; };

	void OnSkillFireEnchant();
	void OnSkillFireExplosion();
	void OnSkillWaterHeal();
	void OnSkillWaterShield();
	void OnSkillGrassWeaken();
	void OnSkillGrassVine();

	void SetState(PlayerStateMachine* newState);
	void SetState(uint8_t newState);

	bool HasMoveInput() {
		return (_velocity != Vec3::Zero);
	}

	void Update();
	void TakeDamage(int damage);

	bool OnFighterBasicAttack(BoundingOrientedBox& monster_box);
};

