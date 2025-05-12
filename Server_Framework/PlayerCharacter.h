
#include "Object.h"
#include "PlayerState.h"

class PlayerCharacter : public Object
{
public:
	Vec3				_look_dir; // 캐릭터가 보고있는 방향

	S_PLAYER_CLASS	    _class;
	int					_hp;
	S_ITEM_TYPE			_skill;

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

	void OnSkillFireEnchant();
	void OnSkillFireExplosion();
	void OnSkillWaterHeal();
	void OnSkillWaterShield();
	void OnSkillGrassWeaken();
	void OnSkillGrassVine();

	void SetState(PlayerStateMachine* newState);
	void SetState(uint8_t newState) {
		switch (newState) {
		case (uint8_t)S_PLAYER_STATE::IDLE:
			SetState(&PlayerState::IdleState::GetInstance());
			break;
		case (uint8_t)S_PLAYER_STATE::RUN:
			SetState(&PlayerState::RunState::GetInstance());
			break;
		case (uint8_t)S_PLAYER_STATE::ATTACK:
			SetState(&PlayerState::BasicAttackState::GetInstance());
			break;
		case (uint8_t)S_PLAYER_STATE::MOVE_ATTACK:
			SetState(&PlayerState::RunAttackState::GetInstance());
			break;
		case (uint8_t)S_PLAYER_STATE::JUMP:
			SetState(&PlayerState::JumpState::GetInstance());
			break;
		case (uint8_t)S_PLAYER_STATE::SKILL:
			SetState(&PlayerState::SkillState::GetInstance());
			break;
		default:
			break;
		}
	}

	uint8_t GetStateEnum() {
		if (currentState == &PlayerState::IdleState::GetInstance()) return (uint8_t)S_PLAYER_STATE::IDLE;
		else if (currentState == &PlayerState::RunState::GetInstance()) return (uint8_t)S_PLAYER_STATE::RUN;
		else if (currentState == &PlayerState::BasicAttackState::GetInstance()) return (uint8_t)S_PLAYER_STATE::ATTACK;
		else if (currentState == &PlayerState::RunAttackState::GetInstance()) return (uint8_t)S_PLAYER_STATE::MOVE_ATTACK;
		else if (currentState == &PlayerState::JumpState::GetInstance()) return (uint8_t)S_PLAYER_STATE::JUMP;
		else if (currentState == &PlayerState::SkillState::GetInstance()) return (uint8_t)S_PLAYER_STATE::SKILL;
		return 0;
	}

	bool HasMoveInput() {
		return (_velocity.x != 0 || _velocity.y != 0 || _velocity.z != 0);
	}

	void Update();
	void TakeDamage(int damage);

	bool OnFighterBasicAttack(BoundingOrientedBox& monster_box);
};

