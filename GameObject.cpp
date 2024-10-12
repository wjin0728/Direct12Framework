#include "stdafx.h"
#include "GameObject.h"
#include"Player.h"


CGameObject::CGameObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::string_view fileName)
{
	std::ifstream file{ fileName.data(), std::ios::binary };
	std::string token;

	while (true)
	{
		BinaryReader::ReadDateFromFile(file, token);

		if (token == "<Hierarchy>:")
		{
			InitObjectFromFile(device, cmdList, file);
		}
		else if (token == "</Hierarchy>")
		{
			break;
		}
	}
}

CGameObject::~CGameObject(void)
{
}


void CGameObject::InitObjectFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::ifstream& inFile)
{
	std::string token;

	int frameNum = 0;
	std::string name;

	while(true) 
	{
		BinaryReader::ReadDateFromFile(inFile, token);
		if (token == "<Frame>:")
		{
			BinaryReader::ReadDateFromFile(inFile, frameNum);
			BinaryReader::ReadDateFromFile(inFile, name);

			frameName = name;
		}
		else if (token == "<Transform>:")
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;

			BinaryReader::ReadDateFromFile(inFile, xmf3Position);
			BinaryReader::ReadDateFromFile(inFile, xmf3Rotation);
			BinaryReader::ReadDateFromFile(inFile, xmf3Scale);
			BinaryReader::ReadDateFromFile(inFile, xmf4Rotation);
		}
		else if (token == "<TransformMatrix>:")
		{
			BinaryReader::ReadDateFromFile(inFile, transformMat);
		}
		else if (token == "<Mesh>:")
		{
			mesh = CMesh::CreateMeshFromFile(inFile);
		}
		else if (token == "<Materials>:")
		{
			int nMaterials = 0;
			BinaryReader::ReadDateFromFile(inFile, nMaterials);

			if (nMaterials > 0)
			{
				materials.resize(nMaterials);
				for (auto& material : materials) {
					material = std::make_shared<CMaterial>();
				}

				CreateMaterialsFromFile(inFile);
			}
		}
		else if (token == "<Children>:")
		{
			int nChilds = 0;
			BinaryReader::ReadDateFromFile(inFile, nChilds);

			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					std::shared_ptr<CGameObject> child = std::make_shared<CGameObject>();
					child->InitObjectFromFile(device, cmdList, inFile);
					childs.emplace_back(child);
				}
			}
		}
		else if (token == "</Frame>")
		{
			break;
		}
	}
}

void CGameObject::CreateMaterialsFromFile(std::ifstream& inFile)
{
	std::string token;
	int nMaterial;

	float smoothness = 0.0f;
	float specularHigh = 0.0f;
	float metalic = 0.0f;
	float glossyReflect = 0.0f;

	for (; ; )
	{
		BinaryReader::ReadDateFromFile(inFile, token);

		if (token == "<Material>:")
		{
			BinaryReader::ReadDateFromFile(inFile, nMaterial);
		}
		else if (token == "<AlbedoColor>:")
		{
			XMFLOAT4 albedo;
			BinaryReader::ReadDateFromFile(inFile, albedo);
			materials[nMaterial]->albedoColor = albedo;
		}
		else if (token == "<EmissiveColor>:")
		{
			inFile.read((char*)&(materials[nMaterial]->emissiveColor), sizeof(XMFLOAT4));
		}
		else if (token == "<SpecularColor>:")
		{
			inFile.read((char*)&(materials[nMaterial]->specularColor), sizeof(XMFLOAT4));
		}
		else if (token == "<Glossiness>:")
		{
			float glossness = 0;
			BinaryReader::ReadDateFromFile(inFile, glossness);

			materials[nMaterial]->specularColor.w = glossness;
		}
		else if (token == "<Smoothness>:")
		{
			BinaryReader::ReadDateFromFile(inFile, smoothness);
		}
		else if (token == "<Metallic>:")
		{
			BinaryReader::ReadDateFromFile(inFile, metalic);
		}
		else if (token == "<SpecularHighlight>:")
		{
			BinaryReader::ReadDateFromFile(inFile, specularHigh);
		}
		else if (token == "<GlossyReflection>:")
		{
			BinaryReader::ReadDateFromFile(inFile, glossyReflect);
		}
		else if (token == "</Materials>")
		{
			break;
		}
	}
}


void CGameObject::SetPosition(float x, float y, float z)
{
	transformMat._41 = x;
	transformMat._42 = y;
	transformMat._43 = z;

	UpdateTransform();
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	transformMat = Matrix4x4::Multiply(mtxScale, transformMat);

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(const XMFLOAT3& xmf3Position)
{
	transformMat._41 = xmf3Position.x;
	transformMat._42 = xmf3Position.y;
	transformMat._43 = xmf3Position.z;

	UpdateTransform();
}

XMFLOAT3 CGameObject::GetPosition() const
{
	return XMFLOAT3(transformMat._41, transformMat._42, transformMat._43);
}
 
XMFLOAT3 CGameObject::GetLook() const
{
	XMFLOAT3 xmf3LookAt(transformMat._31, transformMat._32, transformMat._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);

	return xmf3LookAt;
}

XMFLOAT3 CGameObject::GetUp() const
{
	XMFLOAT3 xmf3Up(transformMat._21, transformMat._22, transformMat._23);
	xmf3Up = Vector3::Normalize(xmf3Up);

	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight() const
{
	XMFLOAT3 xmf3Right(transformMat._11, transformMat._12, transformMat._13);
	xmf3Right = Vector3::Normalize(xmf3Right);

	return(xmf3Right);
}



std::shared_ptr<CGameObject> CGameObject::CreateObjectFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::string_view fileName)
{
	std::ifstream file{ fileName.data(), std::ios::binary };
	std::string token;

	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();

	while(true)
	{
		BinaryReader::ReadDateFromFile(file, token);

		if (token == "<Hierarchy>:")
		{
			object->InitObjectFromFile(device, cmdList, file);
		}
		else if (token == "</Hierarchy>")
		{
			break;
		}
	}

	return object;
}

void CGameObject::AddChild(const std::shared_ptr<CGameObject>& _child)
{
	if (_child) {
		//_child->parent.reset(this);
	}
	childs.push_back(_child);

	
}

void CGameObject::AddMaterial(const XMFLOAT4& albedo)
{
	std::shared_ptr<CMaterial> m = std::make_shared<CMaterial>(albedo);
	materials.push_back(m);
}

void CGameObject::SetRotationTransform(const XMFLOAT4X4* pmxf4x4Transform)
{
	transformMat._11 = pmxf4x4Transform->_11; transformMat._12 = pmxf4x4Transform->_12; transformMat._13 = pmxf4x4Transform->_13;
	transformMat._21 = pmxf4x4Transform->_21; transformMat._22 = pmxf4x4Transform->_22; transformMat._23 = pmxf4x4Transform->_23;
	transformMat._31 = pmxf4x4Transform->_31; transformMat._32 = pmxf4x4Transform->_32; transformMat._33 = pmxf4x4Transform->_33;
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::SetRandomMAterial()
{
	auto color = GetRandomColor();
	std::shared_ptr<CMaterial> m = std::make_shared<CMaterial>(color);
	materials.push_back(m);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	transformMat = Matrix4x4::Multiply(mtxRotate, transformMat);

	UpdateTransform();
}

void CGameObject::Rotate(const XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	transformMat = Matrix4x4::Multiply(mtxRotate, transformMat);

	UpdateTransform();
}

void CGameObject::Move(const XMFLOAT3& vDirection, float fSpeed)
{
	transformMat._41 += vDirection.x * fSpeed;
	transformMat._42 += vDirection.y * fSpeed;
	transformMat._43 += vDirection.z * fSpeed;

	UpdateTransform();
}

void CGameObject::LookTo(const XMFLOAT3& xmf3LookTo, const XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookToLH(GetPosition(), xmf3LookTo, xmf3Up);
	transformMat._11 = xmf4x4View._11; transformMat._12 = xmf4x4View._21; transformMat._13 = xmf4x4View._31;
	transformMat._21 = xmf4x4View._12; transformMat._22 = xmf4x4View._22; transformMat._23 = xmf4x4View._32;
	transformMat._31 = xmf4x4View._13; transformMat._32 = xmf4x4View._23; transformMat._33 = xmf4x4View._33;

	UpdateTransform();
}

void CGameObject::LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	transformMat._11 = xmf4x4View._11; transformMat._12 = xmf4x4View._21; transformMat._13 = xmf4x4View._31;
	transformMat._21 = xmf4x4View._12; transformMat._22 = xmf4x4View._22; transformMat._23 = xmf4x4View._32;
	transformMat._31 = xmf4x4View._13; transformMat._32 = xmf4x4View._23; transformMat._33 = xmf4x4View._33;

	UpdateTransform();
}

void CGameObject::UpdateBoundingBox()
{
	if (mesh)
	{
		mesh->oobb.Transform(oobb, XMLoadFloat4x4(&worldMat));
		XMStoreFloat4(&oobb.Orientation, XMQuaternionNormalize(XMLoadFloat4(&oobb.Orientation)));
	}
}

void CGameObject::Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent)
{
	if (movingSpeed != 0.0f) Move(movingDirection, movingSpeed * deltaTime);
	UpdateTransform(pxmf4x4Parent);
	
	for (auto& child : childs) {
		if (child) {
			child->Update(deltaTime, &worldMat);
		}
	}
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	worldMat = (pxmf4x4Parent) ? Matrix4x4::Multiply(transformMat, *pxmf4x4Parent) : transformMat;
	UpdateBoundingBox();
}

CGameObject* CGameObject::FindFrame(std::string_view pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if(pstrFrameName == frameName)
		return (this);

	for (auto& child : childs) {
		pFrameObject = child->FindFrame(pstrFrameName);
		if (pFrameObject)
			return(pFrameObject);
	}

	return nullptr;
}

void CGameObject::Render(ID3D12GraphicsCommandList* cmdList, XMFLOAT4X4* worldMat, std::shared_ptr<CMesh>& pMesh)
{
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Transpose(*worldMat);

	cmdList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

	for (int i = 0; i < materials.size(); i++) {
		materials[i]->UpdateShaderVariables(cmdList);
		if (pMesh)
		{
			pMesh->Render(cmdList, i);
		}
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList* cmdList, XMFLOAT4X4* worldMat, std::shared_ptr<CMaterial>& material, std::shared_ptr<CMesh>& pMesh)
{
	if (pMesh)
	{
		material->UpdateShaderVariables(cmdList);

		XMFLOAT4X4 xmf4x4World = Matrix4x4::Transpose(*worldMat);

		cmdList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

		pMesh->Render(cmdList);
	}
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* cmdList)
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (mesh)
	{
		mesh->ReleaseUploadBuffers();
	}
	for (auto& child : childs)
		child->ReleaseUploadBuffers();
}

void CGameObject::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	if (pCamera->IsInFrustum(oobb)) 
	{
		CGameObject::Render(cmdList, &worldMat, mesh);
	}

	for (auto& child : childs)
		child->Render(cmdList, pCamera);
}

void CGameObject::GenerateRayForPicking(const XMFLOAT3& xmvPickPosition, const XMMATRIX& xmmtxView, XMFLOAT3& xmvPickRayOrigin, XMFLOAT3& xmvPickRayDirection)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMat) * xmmtxView);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	xmvPickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmmtxToModel);
	xmvPickRayDirection = Vector3::TransformCoord(xmvPickPosition, xmmtxToModel);
	xmvPickRayDirection = Vector3::Normalize(xmvPickRayDirection - xmvPickRayOrigin);
}

int CGameObject::PickObjectByRayIntersection(const XMFLOAT3& xmvPickPosition, const XMMATRIX& xmmtxView, float* pfHitDistance)
{
	int nIntersected = 0;
	if (mesh)
	{
		XMFLOAT3 xmvPickRayOrigin, xmvPickRayDirection;
		GenerateRayForPicking(xmvPickPosition, xmmtxView, xmvPickRayOrigin, xmvPickRayDirection);
		nIntersected = mesh->CheckRayIntersection(xmvPickRayOrigin, xmvPickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRotatingObject::CRotatingObject()
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent)
{
	if (rotationSpeed != 0.0f) Rotate(rotationAxis, rotationSpeed * deltaTime);

	CGameObject::Update(deltaTime, pxmf4x4Parent);

	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallsObject::CWallsObject()
{
}

CWallsObject::~CWallsObject()
{
}

void CWallsObject::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	CGameObject::Render(cmdList, &worldMat, mesh);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
std::shared_ptr<CMesh> CExplosiveObject::m_pExplosionMesh;

CExplosiveObject::CExplosiveObject()
{
}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::PrepareExplosion(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	//m_pExplosionMesh = std::make_shared<CCubeMesh>(device, cmdList, XMFLOAT3(2.f, 2.f, 2.f));
}

void CExplosiveObject::Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent)
{
	if (blowingUp)
	{
		deltaTime += deltaTime;
		if (deltaTime <= duration)
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				transformMats[i] = Matrix4x4::Identity();
				transformMats[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * explosionSpeed * deltaTime;
				transformMats[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * explosionSpeed * deltaTime;
				transformMats[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * explosionSpeed * deltaTime;
				transformMats[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], explosionRotation * deltaTime), transformMats[i]);
			}
		}
		else
		{
			blowingUp = false;
			active = true;
			deltaTime = 0.0f;

			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				transformMats[i] = Matrix4x4::Identity();
				transformMats[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * explosionSpeed * deltaTime;
				transformMats[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * explosionSpeed * deltaTime;
				transformMats[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * explosionSpeed * deltaTime;
				transformMats[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], explosionRotation * deltaTime), transformMats[i]);
			}
		}
	}
	else
	{
		CRotatingObject::Update(deltaTime, pxmf4x4Parent);
	}
}

void CExplosiveObject::Render(ID3D12GraphicsCommandList* cmdList, CCamera* pCamera)
{
	if (blowingUp)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			CGameObject::Render(cmdList, &transformMats[i], materials[0], m_pExplosionMesh);
		}
	}
	else
	{
		CGameObject::Render(cmdList, pCamera);
	}
}


CBulletObject::CBulletObject(float fEffectiveRange) : bulletRange(fEffectiveRange)
{
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(const XMFLOAT3 xmf3FirePosition)
{
	firePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Reset()
{
	lockedObject = nullptr;
	deltaTimeAfterFire = 0;
	movingDistance = 0;
	rotationAngle = 0.0f;

	active = false;
}

void CBulletObject::Update(float deltaTime, XMFLOAT4X4* pxmf4x4Parent)
{
	deltaTimeAfterFire += deltaTime;

	float fDistance = movingSpeed * deltaTime;

	if ((deltaTimeAfterFire > lockingDelayTime) && lockedObject)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		XMFLOAT3 xmf3LockedObjectPosition = lockedObject->GetPosition();
		XMFLOAT3 xmvToLockedObject = Vector3::Subtract(xmf3LockedObjectPosition, xmf3Position);

		xmvToLockedObject = Vector3::Normalize(xmvToLockedObject);

		movingDirection = Vector3::VectorLerp(movingDirection, xmvToLockedObject, 0.25f);
		movingDirection = Vector3::Normalize(movingDirection);
	}

	XMFLOAT3 xmf3Position = GetPosition();

	rotationAngle += rotationSpeed * deltaTime;
	if (rotationAngle > 360.0f) rotationAngle -= 360.0f;

	XMFLOAT4X4 mtxRotate1 = Matrix4x4::RotationYawPitchRoll(0.0f, 0.f, rotationAngle);

	transformMat = Matrix4x4::Multiply(mtxRotate1, transformMat);
	transformMat._31 = movingDirection.x, transformMat._32 = movingDirection.y, transformMat._33 = movingDirection.z;

	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(movingDirection, fDistance, false);
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);
	movingDistance += fDistance;

	CGameObject::Update(deltaTime, pxmf4x4Parent);

	if ((movingDistance > bulletRange) || (deltaTimeAfterFire > lockingTime)) Reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
