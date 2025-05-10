#pragma once
#include "stdafx.h"
#include "Object.h"

class Item : public Object
{
public:
	S_ITEM_TYPE _item_type;

	Item() : Object(S_OBJECT_TYPE::S_ITEM), 
		_item_type(S_ITEM_TYPE::S_item_end) {};
	Item(S_ITEM_TYPE item_type) : Object(S_OBJECT_TYPE::S_ITEM), 
		_item_type(item_type) {};
	~Item() {};

	void SetItemType(S_ITEM_TYPE item_type);
};

