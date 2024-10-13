#include "stdafx.h"
#include "Scene.h"
#include"SceneStateMachine.h"
#include"Camera.h"
#include"Shader.h"
#include"Player.h"
#include"Enemy.h";
#include"Terrain.h"
#include"LightManager.h"
#include"DX12Manager.h"
#include"ResourceManager.h"
#include"Timer.h"


CScene::CScene(CSceneManager& sceneStateMachine) : sceneStateMachine(sceneStateMachine) {}

CMenuScene::CMenuScene(CSceneManager& sceneStateMachine) : CScene(sceneStateMachine) {}

void CMenuScene::Initialize()
{
	BuildObjects();
}

void CMenuScene::Destroy()
{
	ReleaseObjects();
}

void CMenuScene::InitLights()
{
	lightManager = std::make_unique<CLightManager>();
	lightManager->Initialize();

	XMFLOAT4 lightColor = { 1.f,1.f,1.f,1.f };
	XMFLOAT3 strength = { 1.f,1.f,1.f };
	XMFLOAT3 dir = { 0.f,0.f,1.f };

	std::shared_ptr<CDirectionalLight> dirLight = std::make_shared<CDirectionalLight>(lightColor, strength, dir);

	lightManager->AddDirectionalLight(dirLight);
}

void CMenuScene::BuildObjects()
{
	auto renderTargetSize = INSTANCE(CDX12Manager).GetRenderTargetSize();
	float clientWidth = renderTargetSize.x;
	float clientHeight = renderTargetSize.y;

	camera = std::make_shared<CCamera>();
	camera->SetViewport(0, 0, clientWidth, clientHeight);
	camera->SetScissorRect(0, 0, clientWidth, clientHeight);
	camera->SetLookAt({ 0.f,0.f,-30.f }, { 0.f,0.f,1.f }, { 0.f,1.f,0.f });
	camera->SetFOVAngle(60.0f);
	camera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	camera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, clientWidth, clientHeight);


	float width = 6.f;
	float xOffset = 10.f;

	CRotatingObject* alphabet = new CRotatingObject();

	/*CRotatingObject* alphabet = new CRotatingObject();
	alphabet->SetMesh(s);
	alphabet->SetPosition(-xOffset * 2.f, 0.0f, 0.0f);
	alphabet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	alphabet->SetRotationSpeed(50.0f);
	alphabet->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	alphabet->SetMovingSpeed(0.0f);
	alphabet->SetRandomMAterial();
	
	alphabet = new CRotatingObject();
	alphabet->SetMesh(t);
	alphabet->SetPosition(-xOffset, 0.0f, 0.0f);
	alphabet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	alphabet->SetRotationSpeed(50.0f);
	alphabet->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	alphabet->SetMovingSpeed(0.0f);
	alphabet->SetRandomMAterial();

	alphabet = new CRotatingObject();
	alphabet->SetMesh(a);
	alphabet->SetPosition(0.0f, 0.0f, 0.0f);
	alphabet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	alphabet->SetRotationSpeed(50.0f);
	alphabet->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	alphabet->SetMovingSpeed(0.0f);
	alphabet->SetRandomMAterial();

	alphabet = new CRotatingObject();
	alphabet->SetMesh(r);
	alphabet->SetPosition(xOffset, 0.0f, 0.0f);
	alphabet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	alphabet->SetRotationSpeed(50.0f);
	alphabet->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	alphabet->SetMovingSpeed(0.0f);
	alphabet->SetRandomMAterial();

	alphabet = new CRotatingObject();
	alphabet->SetMesh(t);
	alphabet->SetPosition(xOffset*2.f, 0.0f, 0.0f);
	alphabet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	alphabet->SetRotationSpeed(50.0f);
	alphabet->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	alphabet->SetMovingSpeed(0.0f);
	alphabet->SetRandomMAterial();*/
}

void CMenuScene::ReleaseObjects()
{
}

void CMenuScene::ProcessInput(HWND hWnd)
{
}

void CMenuScene::Update()
{
	float deltaTime = INSTANCE(CGameTimer).GetDeltaTime();
	auto& DX12Mgr = INSTANCE(CDX12Manager);

	CBPassData passData{};
	passData.viewProjMat = camera->GetViewProjMat();
	passData.camPos = camera->GetPosition();
	passData.deltaTime = INSTANCE(CGameTimer).GetDeltaTime();
	passData.totalTime = INSTANCE(CGameTimer).GetTotalTime();
	passData.renderTargetSize = DX12Mgr.GetRenderTargetSize();

	auto passBuffer = CONSTBUFFER(CONSTANT_BUFFER_TYPE::PASS);
	passBuffer->UpdateData(&passData);

	for (auto& [shader, objectList] : objects) {
		for (auto& object : objectList) {
			object->Update(deltaTime);
		}
	}
}

void CMenuScene::Render()
{

}

bool CMenuScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		//sceneStateMachine.AddScene(MAINSTAGE);
		return true;
		break;
	default:
		break;
	}
	return false;
}

bool CMenuScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		//if(wParam == VK_ESCAPE)
		//sceneStateMachine.AddScene(MAINSTAGE);
		return true;
		break;
	default:
		break;
	}
	return false;
}

void CMenuScene::ChangeViewport(int width, int height)
{
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GeneratePerspectiveProjectionMatrix(1.01f, 1000.0f, 60.0f);
	camera->SetFOVAngle(60.0f);

	camera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, width, height);
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CPlayScene::CPlayScene(CSceneManager& sceneStateMachine) : CScene(sceneStateMachine)
{
	
}

void CPlayScene::Initialize()
{

	BuildObjects();
}

void CPlayScene::Destroy()
{
	ReleaseObjects();
}

void CPlayScene::InitLights()
{
	lightManager = std::make_unique<CLightManager>();
	lightManager->Initialize();

	XMFLOAT4 lightColor = { 1.f,1.f,1.f,1.f };
	XMFLOAT3 strength = { 0.5f,0.5f,0.5f };
	XMFLOAT3 dir = { 1.f,-1.f,1.f };

	std::shared_ptr<CDirectionalLight> dirLight = std::make_shared<CDirectionalLight>(lightColor, strength, dir);

	lightManager->AddDirectionalLight(dirLight);

}

void CPlayScene::BuildObjects()
{
	//shaders[SHADER_TYPE::LIGHT_SHADER] = std::make_shared<CLightingShader>();
	//shaders[SHADER_TYPE::LIGHT_SHADER]->InitShader(device, d3dRootSignature.Get());

	//shaders[SHADER_TYPE::WIREFRAME_SHADER] = std::make_shared<CWireframeShader>();
	//shaders[SHADER_TYPE::WIREFRAME_SHADER]->InitShader(device, d3dRootSignature.Get());

	//shaders[SHADER_TYPE::OUTLINE_SHADER] = std::make_shared<COutLineShader>();
	//shaders[SHADER_TYPE::OUTLINE_SHADER]->InitShader(device, d3dRootSignature.Get());

	//shaders[SHADER_TYPE::TERRAIN_SHADER] = std::make_shared<CTerrainShader>();
	//shaders[SHADER_TYPE::TERRAIN_SHADER]->InitShader(device, d3dRootSignature.Get());

	//camera = std::make_shared<CCamera>();
	//camera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	//camera->SetScissorRect(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	//camera->GeneratePerspectiveProjectionMatrix(1.01f, 1000.0f, 60.0f);
	//camera->SetFOVAngle(60.0f);

	//camera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	//camera->CreateShaderVariables(device);

	////敲饭捞绢 按眉 积己
	////std::shared_ptr<CMesh> spaceShipMesh = std::make_shared<CMesh>(device, cmdList, "./Obj/spaceship.obj");
	////std::shared_ptr<CMesh> enemyMesh = std::make_shared<CMesh>(device, cmdList, "./Obj/enemy.obj", XMFLOAT3{ 0.065,0.065,0.065 });
	////std::shared_ptr<CMesh> enemyMesh = std::make_shared<CCubeMesh>(device, cmdList, XMFLOAT3{ 10.f,10.f,10.f });
	//InitLights(device);

	//std::shared_ptr<CGameObject> apache = CGameObject::CreateObjectFromFile(device, cmdList, "./Obj/Apache.bin");

	//m_pPlayer = std::make_shared<CAirplanePlayer>(device, cmdList);
	//m_pPlayer->AddChild(apache);
	//m_pPlayer->SetCamera(camera);
	//m_pPlayer->SetPosition(0.0f, 600.0f, 0.0f);
	//m_pPlayer->SetScale(1.f, 1.f, 1.f);
	//m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 30.0f, -150.0f));
	//m_pPlayer->InitPlayer();

	//float size = m_pPlayer->GetSize();
	//std::shared_ptr<CMesh> shield = std::make_shared<CMesh>(device, cmdList, "./Obj/sphere.obj", XMFLOAT3{ size ,size ,size });

	//((CAirplanePlayer*)m_pPlayer.get())->SetShield(shield, shaders[SHADER_TYPE::WIREFRAME_SHADER]);

	//std::shared_ptr<CSpotLight> playerLight = std::make_shared<CSpotLight>(
	//	XMFLOAT4(1.f, 1.f, 1.f,1.f),
	//	XMFLOAT3(1.f, 1.f, 1.f),
	//	XMFLOAT3(0.f, 0.f, 1.f),
	//	XMFLOAT3(0.f, 0.f, 0.f),
	//	500.f,
	//	20.f,
	//	35.f,
	//	10.f
	//);
	//lightManager->AddSpotLight(playerLight);

	//((CAirplanePlayer*)m_pPlayer.get())->SetPlayerLight(playerLight);
	//
	//float fHalfWidth = 180.0f, fHalfHeight = 120.0f, fHalfDepth = 400.0f;


	////利 按眉 积惑

	//CExplosiveObject::PrepareExplosion(device, cmdList);
	//CEnemy::SetTargetPlayer(m_pPlayer);

	//std::random_device rd;
	//std::default_random_engine dre(rd());
	//std::uniform_real_distribution<float> randTime(0.f, 2.f);
	//std::uniform_real_distribution<float> randX(0.f, fHalfWidth);
	//std::uniform_real_distribution<float> randY(0.f, fHalfHeight);
	//std::uniform_real_distribution<float> randZ(0.f, fHalfDepth);

	//CEnemy* enemy = nullptr;

	//for (int i = 0; i < 5;i++) {
	//	float xPos = randX(dre) * 2.f - fHalfWidth;
	//	float yPos = randY(dre) * 2.f - fHalfHeight;
	//	float zPos = randZ(dre) * 2.f - fHalfDepth;

	//	enemy = new CEnemy(device, cmdList);
	//	enemy->SetFirstFireTime(randTime(dre));
	//	//enemy->AddChild(apache);
	//	enemy->SetScale(0.1f, 0.1f, 0.1f);
	//	enemy->SetPosition(xPos, yPos, zPos);
	//	//enemy->SetRandomMAterial();

	//	objects[SHADER_TYPE::LIGHT_SHADER].emplace_back(enemy);
	//}

	//terrain = std::make_shared<CTerrain>(device, cmdList, L"HeightMap.raw", 513, 513, 513, 513,
	//	XMFLOAT3(10.0f, 8.f, 10.0f), XMFLOAT4(0.0f, 0.7f, 0.0f, 1.f));

}

void CPlayScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh.reset();
}

std::shared_ptr<CGameObject> CPlayScene::PickObjectPointedByCursor(int xClient, int yClient)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)camera->viewport.Width) - 1) / camera->perspectiveProjectMat._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)camera->viewport.Height) - 1) / camera->perspectiveProjectMat._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&camera->viewMat);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	std::shared_ptr<CGameObject> pNearestObject;

	for (auto& [shader, objectList] : objects) {
		for (auto& object : objectList) {
			float fHitDistance = FLT_MAX;
			nIntersected = object->PickObjectByRayIntersection(xmf3PickPosition, xmmtxView, &fHitDistance);
			if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
			{
				fNearestHitDistance = fHitDistance;
				pNearestObject = object;
			}
		}
	}

	return pNearestObject;
}



void CPlayScene::CheckObjectByObjectCollisions()
{
	for (auto& [shader, objectList] : objects) {
		for (auto& object : objectList)
		{
			object->collidedObject = nullptr;
		}
	}

	for (auto& [shader, objectList] : objects) {
		for (auto& object1 : objectList)
		{
			if (object1->collidedObject) continue;

			for (auto& [shader, objectList] : objects) {
				for (auto& object2 : objectList) {

				}
			}
		}
	}

	/*for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->collidedObject) continue;
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			float distance = Vector3::Distance(m_ppObjects[j]->GetPosition(), m_ppObjects[i]->GetPosition());
			if (!m_ppObjects[j]->collidedObject && distance <= 40.f)
			{
				XMFLOAT3 dir = Vector3::Subtract(m_ppObjects[i]->GetPosition(), m_ppObjects[j]->GetPosition());
				m_ppObjects[i]->movingDirection = Vector3::Normalize(dir);
			}
		}
	}

	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->collidedObject) continue;
		float distance = Vector3::Distance(m_pPlayer->GetPosition(), m_ppObjects[i]->GetPosition());
		if ( distance <= 40.f)
		{
			XMFLOAT3 dir = Vector3::Subtract(m_ppObjects[i]->GetPosition(), m_pPlayer->GetPosition());
			m_ppObjects[i]->movingDirection = Vector3::Normalize(dir);
		}
	}*/
}

void CPlayScene::CheckObjectByWallCollisions()
{
	/*for (int i = 0; i < m_nObjects; i++)
	{
		ContainmentType containType = m_pWallsObject->oobb.Contains(m_ppObjects[i]->oobb);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->oobb.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->movingDirection), xmvNormal);
				XMStoreFloat3(&m_ppObjects[i]->movingDirection, xmvReflect);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->oobb.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->movingDirection), xmvNormal);
				XMStoreFloat3(&m_ppObjects[i]->movingDirection, xmvReflect);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}*/
}

void CPlayScene::CheckPlayerByWallCollision()
{
	/*BoundingOrientedBox xmOOBBPlayerMoveCheck;
	m_pWallsObject->oobbPlayerMoveCheck.Transform(xmOOBBPlayerMoveCheck, XMLoadFloat4x4(&m_pWallsObject->worldMat));
	XMStoreFloat4(&xmOOBBPlayerMoveCheck.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBBPlayerMoveCheck.Orientation)));

	if (!xmOOBBPlayerMoveCheck.Intersects(m_pPlayer->oobb))
		m_pWallsObject->SetPosition(m_pPlayer->GetPosition());*/
}


void CPlayScene::CheckObjectByBulletCollisions()
{
	CBulletObject** ppBullets = ((CAirplanePlayer*)m_pPlayer.get())->m_ppBullets;

	for (auto& [shader, objectList] : objects) {
		for (auto& object : objectList) {
			for (int j = 0; j < BULLETS; j++)
			{
				if (!object->active) continue;
				if (ppBullets[j]->active && object->oobb.Intersects(ppBullets[j]->oobb))
				{
					CExplosiveObject* pExplosiveObject = (CExplosiveObject*)object.get();
					pExplosiveObject->blowingUp = true;
					pExplosiveObject->active = false;
					lockedObject = nullptr;
					ppBullets[j]->Reset();
				}
			}
		}
	}
}

void CPlayScene::CheckPlayerByBulletCollision()
{
	if (((CAirplanePlayer*)m_pPlayer.get())->IsShieldOn()) {

		for (auto& [shader, objectList] : objects) {
			for (auto& object : objectList) 
			{
				CBulletObject** ppBullets = ((CEnemy*)object.get())->m_ppBullets;

				for (int j = 0; j < ENEMY_BULLETS; j++)
				{
					if (ppBullets[j]->active && ((CAirplanePlayer*)m_pPlayer.get())->shieldBS.Intersects(ppBullets[j]->oobb)) {
						ppBullets[j]->Reset();
					}
				}
			}
		}
	}
	else 
	{
		if (m_pPlayer->isInvincible) return;

		for (auto& [shader, objectList] : objects) {
			for (auto& object : objectList) {
				CBulletObject** ppBullets = ((CEnemy*)object.get())->m_ppBullets;

				for (int j = 0; j < ENEMY_BULLETS; j++)
				{
					if (ppBullets[j]->active && m_pPlayer->oobb.Intersects(ppBullets[j]->oobb))
					{
						ppBullets[j]->Reset();
						m_pPlayer->BeInvincible();
						return;
					}
				}
			}
		}
	}
}

void CPlayScene::ProcessInput(HWND hWnd)
{
	float deltaTime = INSTANCE(CGameTimer).GetDeltaTime();
	static UCHAR pKeyBuffer[256];

	if (GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_SHIFT] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_CONTROL] & 0xF0) dwDirection |= DIR_DOWN;

		if (dwDirection) m_pPlayer->Move(dwDirection, deltaTime*15.f);

	}

	if (GetCapture() == hWnd) {
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		float cxMouseDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else
				m_pPlayer->Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}
}

void CPlayScene::Update()
{
	float deltaTime = INSTANCE(CGameTimer).GetDeltaTime();

	m_pPlayer->Update(deltaTime);

	for (auto& [shader, objectList] : objects) {
		for (auto& object : objectList) {
			object->Update(deltaTime);
		}
	}

	CheckPlayerByWallCollision();
	
	CheckObjectByWallCollisions();

	CheckObjectByObjectCollisions();

	CheckObjectByBulletCollisions();

	CheckPlayerByBulletCollision();
}


void CPlayScene::Render()
{

	
}

bool CPlayScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		GetCursorPos(&m_ptOldCursorPos);
		lockedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam));                                      
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return false;
}

bool CPlayScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			break;
		case VK_SPACE:
			((CAirplanePlayer*)m_pPlayer.get())->FireBullet(lockedObject);
			//lockedObject = nullptr;
			break;
		case 'E':
			((CAirplanePlayer*)m_pPlayer.get())->ShieldOn();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return false;
}

void CPlayScene::ChangeViewport(int width, int height)
{
	camera->SetViewport(0, 0, width, height);
	camera->SetScissorRect(0, 0, width, height);
	camera->GeneratePerspectiveProjectionMatrix(1.01f, 1000.0f, 60.0f);
	camera->SetFOVAngle(60.0f);

	camera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, width, height);
}