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

	void SetItemType(S_ITEM_TYPE item_type) { 
		_item_type = item_type; 

		if (_item_type <= S_ITEM_TYPE::S_GRASS_WEAKEN) {
			_boundingbox.Center = XMFLOAT3(-0.02260171f, 0.01106288f, -0.005101735f);
			_boundingbox.Extents = XMFLOAT3(0.806778f, 0.7830477f, 0.8548302f);
			_orignalboundingbox.Center = XMFLOAT3(-0.02260171f, 0.01106288f, -0.005101735f);
			_orignalboundingbox.Extents = XMFLOAT3(0.806778f, 0.7830477f, 0.8548302f);
		}
		else {
			//_boundingbox.Extents = XMFLOAT3(0.806778f, 0.7830477f, 0.8548302f);
			//_boundingbox.Extents = XMFLOAT3(1.f, 1.f, 1.f);
		}	
	};
};

