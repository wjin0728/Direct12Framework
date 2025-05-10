#include "Item.h"

void Item::SetItemType(S_ITEM_TYPE item_type)
{
	_item_type = item_type;

	if (_item_type <= S_ITEM_TYPE::S_GRASS_WEAKEN) {
		_boundingbox = BoundingOrientedBox(Vec3(0.f), Vec3(0.58f) / 2.f, Quaternion::Identity);
		_orignalboundingbox = _boundingbox;
	}
	else {
		//_boundingbox.Extents = XMFLOAT3(0.806778f, 0.7830477f, 0.8548302f);
		//_boundingbox.Extents = XMFLOAT3(1.f, 1.f, 1.f);
	}
}
