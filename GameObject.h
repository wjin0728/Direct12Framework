#pragma once
#include "Mesh.h"
#include "Camera.h"
#include"UploadBuffer.h"
#include"Material.h"

#define ENEMY_BULLETS 5

class CPlayer;

class CGameObject
{
public:
	CGameObject() = default;
	CGameObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::string_view fileName);
	virtual ~CGameObject();

public:
	bool active = true;
	std::string frameName;

	std::shared_ptr<CMesh> mesh;
	std::vector<std::shared_ptr<CMaterial>> materials;

	std::shared_ptr<CGameObject> parent;
	std::vector<std::shared_ptr<CGameObject>> childs;

	XMFLOAT4X4 worldMat = Matrix4x4::Identity();
	XMFLOAT4X4 transformMat = Matrix4x4::Identity();
	XMFLOAT4 color;

	BoundingOrientedBox oobb = BoundingOrientedBox();

	CGameObject* collidedObject = nullptr;

	XMFLOAT3 movingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float movingSpeed = 0.0f;
	float movingRange = 0.0f;

	float movingTime = 0.f;
	XMFLOAT3 randomDir = {1.f, 0.f, 0.f};

private:
	void InitObjectFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::ifstream& inFile);
	void CreateMaterialsFromFile(std::ifstream& inFile);

public:
	static std::shared_ptr<CGameObject> CreateObjectFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::string_view fileName);

public:
	void SetActive(bool bActive) { active = bActive; }
	virtual void SetMesh(std::shared_ptr<CMesh>& _mesh) { mesh = _mesh; }
	virtual void AddChild(const std::shared_ptr<CGameObject>& _child);
	virtual void AddMaterial(const XMFLOAT4& albedo);

	void SetRotationTransform(const XMFLOAT4X4 *pmxf4x4Transform);

	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetRandomMAterial();

	void SetMovingDirection(const XMFLOAT3& xmf3MovingDirection) { movingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { movingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { movingRange = fRange; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(const XMFLOAT3& vDirection, float fSpeed);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(const XMFLOAT3& xmf3Axis, float fAngle);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetRight() const;

	float GetSize() const { if(mesh) return Vector3::Length(mesh->oobb.Extents); };

	void LookTo(const XMFLOAT3& xmf3LookTo, const XMFLOAT3& xmf3Up);
	void LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up);

	void UpdateBoundingBox();
	float GetBoundingBoxSize() { return Vector3::Length(mesh->oobb.Extents); }

	std::shared_ptr<CGameObject>& GetParent() { return parent; }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(std::string_view pstrFrameName);

	void Render(ID3D12GraphicsCommandList* cmdList, XMFLOAT4X4* worldMat, std::shared_ptr<CMesh>& pMesh);
	void Render(ID3D12GraphicsCommandList* cmdList, XMFLOAT4X4* worldMat, std::shared_ptr<CMaterial>& material, std::shared_ptr<CMesh>& pMesh);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* cmdList);
	void ReleaseUploadBuffers();

	virtual void OnUpdateTransform() { }
	virtual void Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera);

	void GenerateRayForPicking(const XMFLOAT3& xmvPickPosition, const XMMATRIX& xmmtxView,
		XMFLOAT3& xmvPickRayOrigin, XMFLOAT3& xmvPickRayDirection);
	int PickObjectByRayIntersection(const XMFLOAT3& xmPickPosition, const XMMATRIX& xmmtxView, float* pfHitDistance);


};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	XMFLOAT3 rotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float rotationSpeed = 0.0f;

	void SetRotationAxis(const XMFLOAT3& xmf3RotationAxis) { rotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { rotationSpeed = fSpeed; }
	virtual void Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent = nullptr);
};

class CExplosiveObject : public CRotatingObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool blowingUp = false;

	XMFLOAT4X4 transformMats[EXPLOSION_DEBRISES];

	float deltaTime = 0.0f;
	float duration = 2.0f;
	float explosionSpeed = 25.0f;
	float explosionRotation = 720.0f;

	virtual void Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera);

public:
	static std::shared_ptr<CMesh> m_pExplosionMesh;
	static XMFLOAT3 m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};

class CWallsObject : public CGameObject
{
public:
	CWallsObject();
	virtual ~CWallsObject();

public:
	BoundingOrientedBox oobbPlayerMoveCheck = BoundingOrientedBox();
	XMFLOAT4 m_pxmf4WallPlanes[6];

	virtual void Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera);
};

class CBulletObject : public CRotatingObject
{
public:
	CBulletObject(float fEffectiveRange);
	virtual ~CBulletObject();

public:
	virtual void Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent = nullptr);

	float bulletRange = 50.0f;
	float movingDistance = 0.0f;
	float rotationAngle = 0.0f;
	XMFLOAT3 firePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float deltaTimeAfterFire = 0.0f;
	float lockingDelayTime = 0.2f;
	float lockingTime = 4.0f;
	std::shared_ptr<CGameObject> lockedObject;

	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	void Reset();
};

