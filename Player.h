#pragma once
#include "stdafx.h"

class Player
{
	int id;
	int ob_id;
	Vec3 position;
	Vec3 rotation;

public:
	Player() : id(-1) {}
	Player(int id) : id(id) {}
	Player(const Player& other) : id(other.id) {}

	~Player() {}

	int GetID() const { return id; }
	void SetID(int _id) { id = _id; }

	int GetObjectID() const { return ob_id; }
	void SetObjectID(int _ob_id) { ob_id = _ob_id; }

	Vec3 GetPosition() const { return position; }
	void SetPosition(const Vec3& _position) { position = _position; }

	Vec3 GetRotation() const { return rotation; }
	void SetRotation(const Vec3& _rotation) { rotation = _rotation; }
};