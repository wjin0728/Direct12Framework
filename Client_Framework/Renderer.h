#pragma once
#include "Component.h"

class CMaterial;
class CCamera;

class CRenderer : public CComponent {
protected:
    std::vector<std::shared_ptr<CMaterial>> m_materials;

    BoundingSphere mWorldBS{};
	BoundingOrientedBox mWorldOOBB{};

    bool isDirty{};
    int mCbvIdx = -1;
    UINT mCbvOffset{};

public:
    CRenderer();
    CRenderer(const CRenderer& other) : m_materials(other.m_materials), CComponent(other) {}
    virtual ~CRenderer();

    virtual void Awake();
    virtual void Start() override {}

    virtual void Update() override {}
    virtual void LateUpdate() override {}

    virtual void Render(std::shared_ptr<CCamera> camera, int pass = 0) = 0;

    virtual std::shared_ptr<CComponent> Clone() override = 0;

public:
    void SetCBVIndex();
    void ReturnCBVIndex();

public:
    void AddMaterial(const std::shared_ptr<CMaterial>& material);
    void AddMaterial(const std::string& name);


    bool IsIntersect(const BoundingOrientedBox& oobb) const
    {
        return mWorldOOBB.Intersects(oobb);
    }
    bool IsIntersect(const BoundingSphere& sphere) const
    {
        return mWorldOOBB.Intersects(sphere);
    }
    bool IsIntersect(const Vec3& point) const
    {
        return mWorldOOBB.Intersects(point);
    }
    bool IsIntersect(const Ray& ray, float& dist) const
    {
        return mWorldOOBB.Intersects(ray.position, ray.direction, dist);
    }

    std::shared_ptr<CMaterial> GetMaterial(UINT idx = 0) const { return m_materials[idx]; }
};