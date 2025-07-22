#include"stdafx.h"
#include "GameObject.h"
#include"Transform.h"
#include"MeshRenderer.h"
#include"Camera.h"
#include"ResourceManager.h"
#include"Terrain.h"
#include"Collider.h"
#include"UploadBuffer.h"
#include"SceneManager.h"
#include"Scene.h"
#include"Animation.h"
#include"SkinnedMesh.h"
#include"Renderer.h"
#include"SkinnedMeshRenderer.h"
#include"Light.h"
#include"ContinuousRotation.h"
#include"UIRenderer.h"
#include"RenderManager.h"
#include"ParticleAttach.h"
#include"ParticleManager.h"
#include"ParticleEmitter.h"
#include"Button.h"

CGameObject::CGameObject(bool makeTransform)
{
	if (makeTransform) {
		mTransform = AddComponent<CTransform>();
	}
	mActive = false;
}

CGameObject::~CGameObject()
{
}

void CGameObject::Awake()
{
	if (misAwake || !mActive) return;
	for (auto& component : mComponents) {
		if (component->mIsAwake) continue;
		component->Awake();
		component->mIsAwake = true;
	}

	for (int i = 0; i < mChildren.size();i++) {
		mChildren[i]->Awake();
	}
	if (!mCollider) {
		mCollider = GetComponent<CCollider>();
	}
	misAwake = true;
}

void CGameObject::Start()
{
	if (mIsStart || !mActive) return;
	for (auto& component : mComponents) {
		if (component->mIsStart) continue;
		component->Start();
		component->mIsStart = true;
	}

	for (int i = 0; i < mChildren.size(); i++) {
		mChildren[i]->Start();
	}
	mRenderer = GetComponent<CRenderer>();
	mIsStart = true;
}

void CGameObject::Update()
{
	if (!mActive) {
		return;
	}

	for (auto& component : mComponents) {
		component->Update();
	}

	for (auto& child : mChildren) {
		child->Update();
	}
}

void CGameObject::LateUpdate()
{
	if (!mActive) {
		return;
	}

	for (auto& component : mComponents) {
		component->LateUpdate();
	}

	for (auto& child : mChildren) {
		child->LateUpdate();
	}

	if (!mIsStatic) mTransform->UpdateWorldMatrix();
}

void CGameObject::SetActive(bool active)
{
	mActive = active;
	if (mActive) {
		for (auto& component : mComponents) {
			component->EnqueueAwake();
		}
	}
	for (auto& child : mChildren) {
		child->SetActive(active);
	}
	mRenderer = GetComponent<CRenderer>();
}

void CGameObject::SetStatic(bool isStatic)
{
	mIsStatic = isStatic;

	for (auto& child : mChildren) {
		child->SetStatic(isStatic);
	}
}

void CGameObject::SetInstancing(bool isInstancing)
{
	if(mRenderer) {
		if (isInstancing) {
			mRenderer->ReturnCBVIndex();
		}
		else {
			mRenderer->SetCBVIndex();
		}
	}

	mIsInstancing = isInstancing;
}

void CGameObject::SetParent(const std::shared_ptr<CGameObject>& parent)
{
	mTransform->SetParent(parent->mTransform);
}

void CGameObject::ReturnCBVIndex()
{
	if (!mRenderer) return;

	mRenderer->ReturnCBVIndex();
	for (auto& child : mChildren) {
		child->ReturnCBVIndex();
	}
}

std::shared_ptr<CGameObject> CGameObject::Instantiate(const std::shared_ptr<CGameObject>& original, 
	const std::shared_ptr<CTransform>& parentTransform)
{
	std::shared_ptr<CGameObject> instance = std::make_shared<CGameObject>(false);

	for (const auto& component : original->mComponents) {
		auto newComponent = component->Clone();
		instance->mComponents.push_back(newComponent);
		newComponent->SetOwner(instance.get());
		newComponent->mIsAwake = false; 
		newComponent->mIsStart = false; 
	}

	instance->mTransform = instance->GetComponent<CTransform>();
	instance->mTag = original->mTag;
	instance->mObjectType = original->mObjectType;
	instance->mRootLocalBS = original->mRootLocalBS;
	instance->mName = original->mName;
	instance->mIsInstancing = original->mIsInstancing;
	instance->misAwake = false;
	instance->mIsStart = false;
	instance->mLocalAABB = original->mLocalAABB;
	instance->mWorldAABB = original->mWorldAABB;
	instance->mCastShadow = original->mCastShadow;
	instance->mRenderLayer = original->mRenderLayer;
	if (instance->mName == "SM_Env_Rock_Cliff_02")
		int a = 0;

	instance->mRenderer = instance->GetComponent<CMeshRenderer>();
	if (!instance->mRenderer) {
		instance->mRenderer = instance->GetComponent<CSkinnedMeshRenderer>();
	}
	instance->mCollider = instance->GetComponent<CCollider>();
	instance->mAnimationController = instance->GetComponent<CAnimationController>();

	if (parentTransform) {
		instance->mTransform->SetParent(parentTransform);
	}

	for (const auto& originChild : original->mChildren) {
		auto childInstance = Instantiate(originChild, instance->mTransform);
	}

	return instance;
}

std::shared_ptr<CGameObject> CGameObject::Instantiate(const std::unique_ptr<CGameObject>& original,
	const std::shared_ptr<CTransform>& parentTransform)
{
	std::shared_ptr<CGameObject> instance = std::make_shared<CGameObject>(false);

	for (const auto& component : original->mComponents) {
		auto newComponent = component->Clone();
		instance->mComponents.push_back(newComponent);
		newComponent->SetOwner(instance.get());
		newComponent->mIsAwake = false;
		newComponent->mIsStart = false;
	}

	instance->mTransform = instance->GetComponent<CTransform>();
	instance->mTag = original->mTag;
	instance->mObjectType = original->mObjectType;
	instance->mRootLocalBS = original->mRootLocalBS;
	instance->mName = original->mName;
	instance->mIsInstancing = original->mIsInstancing;
	instance->misAwake = false;
	instance->mIsStart = false;
	instance->mLocalAABB = original->mLocalAABB;
	instance->mWorldAABB = original->mWorldAABB;
	instance->mCastShadow = original->mCastShadow;
	instance->mRenderLayer = original->mRenderLayer;

	instance->mRenderer = instance->GetComponent<CMeshRenderer>();
	if (!instance->mRenderer) {
		instance->mRenderer = instance->GetComponent<CSkinnedMeshRenderer>();
	}
	instance->mCollider = instance->GetComponent<CCollider>();
	instance->mAnimationController = instance->GetComponent<CAnimationController>();

	if (parentTransform) {
		instance->mTransform->SetParent(parentTransform);
	}

	for (const auto& originChild : original->mChildren) {
		auto childInstance = Instantiate(originChild, instance->mTransform);
	}

	return instance;
}

std::shared_ptr<CGameObject> CGameObject::CreateCameraObject(const std::string& tag, Vec2 rtSize, 
	float nearPlane, float farPlane, float fovAngle)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->SetTag(tag);

	auto camera = std::make_shared<CCamera>();
	object->AddComponent(camera);
	camera->mCameraName = tag;
	camera->SetViewport(0, 0, rtSize.x, rtSize.y);
	camera->SetScissorRect(0, 0, rtSize.x, rtSize.y);

#ifdef REVERSE_Z
	camera->GenerateReverseZPerspectiveProjectionMatrix(nearPlane, farPlane, fovAngle);
#elif // REVERSE_Z
	camera->GeneratePerspectiveProjectionMatrix(nearPlane, farPlane, fovAngle);
#endif // REVERSE_Z

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateCameraObject(const std::string& tag, Vec2 rtSize,
	float nearPlane, float farPlane, Vec2 size)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->SetTag(tag);

	auto camera = std::make_shared<CCamera>();
	object->AddComponent(camera);
	camera->mCameraName = tag;
	camera->SetViewport(0, 0, rtSize.x, rtSize.y);
	camera->SetScissorRect(0, 0, rtSize.x, rtSize.y);
	camera->GenerateOrthographicProjectionMatrix(nearPlane, farPlane, size.x, size.y);

	INSTANCE(CSceneManager).GetCurScene()->AddObjectImmediately(object);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateUIObject(const std::string& shader, const std::string& texture, Vec2 pos, Vec2 size, float depth)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = "UI";
	auto uiRenderer = object->AddComponent<CUIRenderer>();
	object->mRenderer = uiRenderer;
	uiRenderer->SetShader(RESOURCE.Get<CShader>(shader));
	uiRenderer->SetTexture(texture);
	uiRenderer->SetSize(size);
	uiRenderer->SetUVOffset({ 0.f, 0.f });
	uiRenderer->SetUVScale({ 1.f, 1.f });
	uiRenderer->SetType(0);
	uiRenderer->SetPosition(pos);

	object->SetRenderLayer(RENDER_LAYER::UI);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateTerrainObject(std::ifstream& ifs)
{
	using namespace BinaryReader;
	std::string name{};
	ReadDateFromFile(ifs, name);
		
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();

	UINT resolution{};
	Vec3 size{};
	ReadDateFromFile(ifs, resolution);

	auto material = std::static_pointer_cast<CTerrainMaterial>(RESOURCE.Get<CMaterial>(name + "Material"));
	if (!material) {
		material = std::make_shared<CTerrainMaterial>();
		material->LoadTerrainData(ifs);
		material->SetName(name + "Material");
		RESOURCE.Add(material);
	}
	size = material->GetSize();

	auto mesh = std::static_pointer_cast<CHeightMapGridMesh>(RESOURCE.Get<CMesh>(name + "Mesh"));
	if (!mesh) { 
		mesh = std::make_shared<CHeightMapGridMesh>();
		mesh->Initialize(name + "Heightmap", resolution, size);
		mesh->SetName(name + "Mesh");
		RESOURCE.Add(mesh);
	}
	auto terrain = object->AddComponent<CTerrain>();
	terrain->SetHeightMapGridMesh(mesh);
	terrain->SetMaterial(material);

	object->CreateTransformFromFile(ifs);


	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateObjectFromFile(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs)
{
	std::shared_ptr<CGameObject> root = InitFromFile(ifs, prefabs);
	if (root) {
		Vec3 boundingCenter;
		float radius;

		BinaryReader::ReadDateFromFile(ifs, boundingCenter);
		BinaryReader::ReadDateFromFile(ifs, radius);

		root->mRootLocalBS = BoundingSphere(boundingCenter, radius);

		float isStatic{};
		BinaryReader::ReadDateFromFile(ifs, isStatic);

		if (isStatic) {
			root->SetStatic(true);
		}
		else {
			root->SetStatic(false);
		}
	}

	if (root->mTag == "Obstacle" || root->mTag == "Environment") {
		root->mObjectType = OBJECT_TYPE::OBSTACLE;
	}
	else if (root->mTag == "Enemy") {
		root->mObjectType = OBJECT_TYPE::ENEMY;
	}
	else root->mObjectType = OBJECT_TYPE::NONE;
	return root;
}

std::shared_ptr<CGameObject> CGameObject::CreateObjectFromFile(const std::string& name)
{
	std::ifstream ifs{ OBJECT_PATH(name), std::ios::binary };
	if (!ifs) {
		return nullptr;
	}
	std::unordered_map<std::string, std::shared_ptr<CGameObject>> prefabs{};
	return CGameObject::CreateObjectFromFile(ifs, prefabs);
}

std::shared_ptr<CGameObject> CGameObject::GetSptrFromThis()
{
	return shared_from_this();
}

std::shared_ptr<CGameObject> CGameObject::InitFromFile(std::ifstream& inFile, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs)
{
	using namespace BinaryReader;

	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();

	std::string token{};

	while (true) {
		ReadDateFromFile(inFile, token);
		if (token == "<Prefab>:") {
			std::string prefabName{};
			ReadDateFromFile(inFile, prefabName);
			obj = CGameObject::Instantiate(prefabs[prefabName]);
			obj->CreateTransformFromFile(inFile);

			std::string tag{};
			ReadDateFromFile(inFile, tag);
			obj->SetTag(tag);
			
			return obj;
		}
		if (token == "<Frame>:") {
			ReadDateFromFile(inFile, obj->mName);
			obj->InitByObjectName();
		}
		else if (token == "<Tag>:") {
			ReadDateFromFile(inFile, obj->mTag);
		}
		else if (token == "<Transform>:") {
			obj->CreateTransformFromFile(inFile);
		}
		else if (token == "<Renderer>:") {
			obj->CreateRendererFromFile(inFile);
		}
		else if (token == "<Terrain>:") {
			obj->CreateTerrainFromFile(inFile);
		}
		else if (token == "<Collider>:") {
			Vec3 center{};
			Vec3 size{};
			ReadDateFromFile(inFile, center);
			ReadDateFromFile(inFile, size);

			auto collider = obj->AddComponent<CCollider>();
			collider->Initialize(center, size);
		}
		else if (token == "<Children>:") {
			int childrenCnt{};
			ReadDateFromFile(inFile, childrenCnt);

			for (int i = 0; i < childrenCnt; i++) {
				std::shared_ptr<CGameObject> child = InitFromFile(inFile, prefabs);
				child->SetParent(obj);
			}
		}
		else if (token == "<Animation>:") {
			obj->mAnimationController = obj->AddComponent<CAnimationController>();
			std::string animName{};
			ReadDateFromFile(inFile, animName);
			obj->CreateAnimationFromFile(ANIMATION_PATH(obj->mName));
		}
		else if (token == "<Light>:") {
			obj->CreateLightFromFile(inFile);

		}
		else if (token == "<Image>:") {
			obj->CreateUIrendererFromFile(inFile);

		}
		else if (token == "<Button>:") {
			obj->CreateButtonFromFile(inFile);

		}
		else if (token == "<ParticleSystem>:") {
			obj->CreateParticleAttachmentFromFile(inFile);
		}

		else if (token == "</Frame>") {
			break;
		}
	}

	

	return obj;
}

void CGameObject::InitByObjectName()
{
	if (mName == "SM_Bld_Windmill_01_Blades_01") {
		auto rotator = AddComponent<CContinuousRotation>();
		//z������ ȸ��
		rotator->SetRotationSpeed({ 0.f, 0.f, 10.f });
		rotator->SetRotationAxis({ 0.f, 0.f, 1.f });
	}
	else if (mName.contains("Bush") || mName.contains("_Tree")|| mName.contains("_Grass")|| mName.contains("Env_Ground") || mName.contains("Env_Backrgound")
		|| mName.contains("SM_Env_Rock") || mName.contains("Env_Lillies") || mName.contains("Env_Wildflowers") || mName.contains("Env_Fern")
		|| mName.contains("Env_Undergrowth_Fern") || mName.contains("Env_Dirt") || mName.contains("Env_Moss") || mName.contains("Env_Succulent") || mName.contains("SM_Env_Crater")
		|| mName.contains("SM_Env_Cactus") || mName.contains("SM_Env_DirtRoad") ) {
		SetInstancing(true);
	}
}

void CGameObject::CreateAnimationFromFile(const std::string& fileName)
{
	using namespace BinaryReader;

	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return;
	}

	std::string token{};
	int setsNum{};

	while (true) {
		ReadDateFromFile(ifs, token);

		if (token == "<AnimationSets>:") {
			ReadDateFromFile(ifs, setsNum);
			mAnimationController->mAnimationSets = std::make_shared<CAnimationSets>(setsNum);
		}
		else if (token == "<FrameNames>:") {
			int cacheNum{};
			ReadDateFromFile(ifs, cacheNum);

			auto& sets = mAnimationController->mAnimationSets;
			sets->mBoneFrameCaches.resize(cacheNum);
			sets->mBoneNames.resize(cacheNum);

			for (int i = 0; i < cacheNum; i++) {
				ReadDateFromFile(ifs, sets->mBoneNames[i]);
			}
		}
		else if (token == "<AnimationSet>:") {
			int setNum{}, framesPerSecondNum{}, keyFrameNum{};
			float length{};
			std::string setName;

			ReadDateFromFile(ifs, setNum);
			ReadDateFromFile(ifs, setName);
			ReadDateFromFile(ifs, length);
			ReadDateFromFile(ifs, framesPerSecondNum);
			ReadDateFromFile(ifs, keyFrameNum);

			auto& animSets = mAnimationController->mAnimationSets;
			auto animSet = std::make_shared<CAnimationSet>(length, framesPerSecondNum, keyFrameNum, animSets->mBoneNames.size(), setName);

			ReadDateFromFile(ifs, token);
			if (token == "<Loop>:") {
				int animationType{};
				ReadDateFromFile(ifs, animationType);
				animSet->mType = (ANIMATION_TYPE)animationType;
			}

			ReadDateFromFile(ifs, token);
			if (token == "<Events>:") {
				int eventCount{};
				ReadDateFromFile(ifs, eventCount);
				animSet->mEventKeys.resize(eventCount);

				for (auto& key : animSet->mEventKeys) {
					float eventTime{};
					float floatParam{};

					std::string eventStr;

					ReadDateFromFile(ifs, eventTime);
					ReadDateFromFile(ifs, floatParam);
					ReadDateFromFile(ifs, eventStr);

					key = std::make_shared<EventKey>(eventTime, floatParam, eventStr);
				}
			}

			animSets->mAnimationSet[setNum] = animSet;

			for (int i = 0; i < keyFrameNum; i++) {
				ReadDateFromFile(ifs, token);
				if (token == "<Transforms>:") {
					int keyNum{};
					float keyTime{};

					ReadDateFromFile(ifs, keyNum);
					ReadDateFromFile(ifs, keyTime);

					animSet->mKeyFrameTimes[i] = keyTime;

					for (int j = 0; j < mAnimationController->mAnimationSets->mBoneNames.size(); ++j) {
						ReadDateFromFile(ifs, animSet->mKeyFrameTransforms[i][j]);
					}
				}
			}
		}
		else if (token == "</AnimationSets>")
		{
			break;
		}
	}
}

void CGameObject::CreateUIrendererFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	std::string textureName{};
	ReadDateFromFile(inFile, textureName);
	Color color{};
	ReadDateFromFile(inFile, color);
	Vec2 size{};
	Vec2 ratio = INSTANCE(CDX12Manager).GetRenderTargetSize() / Vec2(1920.f, 1080.f);
	ReadDateFromFile(inFile, size);
	size *= ratio;
	Vec2 pos{};
	ReadDateFromFile(inFile, pos);

	auto uiRenderer = AddComponent<CUIRenderer>();
	mRenderer = uiRenderer;
	std::string path = TEXTURE_PATH(textureName);
	auto mainTex = std::make_shared<CTexture>(textureName, path);
	RESOURCE.Add(mainTex);
	uiRenderer->SetTexture(mainTex);
	uiRenderer->SetSize(size);
	uiRenderer->SetType(0);
	uiRenderer->SetColor(color);
	uiRenderer->SetPosition(pos);
	uiRenderer->SetShader("Sprite");
	SetRenderLayer(RENDER_LAYER::UI);
	mCastShadow = false;
}

void CGameObject::CreateButtonFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	auto button = AddComponent<CButton>();
	Color color{};
	ReadDateFromFile(inFile, button->mTargetSize);
	ReadDateFromFile(inFile, button->mTargetPos);
	ReadDateFromFile(inFile, color);


}

void CGameObject::CreateParticleAttachmentFromFile(std::ifstream& inFile)
{
	auto particle = AddComponent<CParticleAttach>();
	using namespace BinaryReader;
	bool canEmit{};
	ReadDateFromFile(inFile, canEmit);
	particle->mCanEmit = canEmit;
	if (!canEmit) {
		return;
	}
	std::string str{};
	ReadDateFromFile(inFile, str);
	bool isLoop{};
	ReadDateFromFile(inFile, isLoop);
	particle->SetLoop(isLoop);
	ParticleProperties particleProperties;
	ParticleProperties::ReadParticlePropertiesFromFile(inFile, particleProperties);
	INSTANCE(CParticleManager).AddParticleProperties(mName, particleProperties);

	particle->SetParticleEmitterName(mName);
}

void CGameObject::CreateTransformFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	ReadDateFromFile(inFile, mTransform->mLocalPosition);
	ReadDateFromFile(inFile, mTransform->mLocalEulerAngle);
	ReadDateFromFile(inFile, mTransform->mLocalScale);
	ReadDateFromFile(inFile, mTransform->mLocalRotation);

	ReadDateFromFile(inFile, mTransform->mLocalMat);
}

void CGameObject::CreateRendererFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	std::string token{};

	ReadDateFromFile(inFile, token);
	if (token == "<SkinnedMesh>:") {
		auto skinnedMeshRenderer = AddComponent<CSkinnedMeshRenderer>();
		mRenderer = skinnedMeshRenderer;

		std::string meshName{};
		ReadDateFromFile(inFile, meshName);
		skinnedMeshRenderer->SetSkinnedMesh(meshName);

		UINT boneCount{};
		ReadDateFromFile(inFile, boneCount);
		skinnedMeshRenderer->mBoneNames.resize(boneCount);
		for (UINT i = 0; i < boneCount; i++) {
			std::string boneName{};
			ReadDateFromFile(inFile, boneName);
			skinnedMeshRenderer->mBoneNames[i] = boneName;
		}
	}
	else if (token == "<Mesh>:") {
		auto meshRenderer = AddComponent<CMeshRenderer>();
		mRenderer = meshRenderer;

		std::string meshName{};
		ReadDateFromFile(inFile, meshName);
		meshRenderer->SetMesh(meshName);
	}
	if(mRenderer == nullptr) {
		return;
	}
	mRootLocalBS = mRenderer->GetWorldBS();
	BoundingOrientedBox localOBB = mRenderer->GetWorldOOBB();
	mLocalAABB = BoundingBox(localOBB.Center, localOBB.Extents);
	if (mName == "SM_Env_Rock_Cliff_02")
		int a = 0;
	int materialCnt{};
	std::string materialName{};
	ReadDateFromFile(inFile, materialCnt);
	if (mName == "SM_Env_Water_Plane_01")
		materialCnt = 1;
	for (int i = 0; i < materialCnt; i++) {
		ReadDateFromFile(inFile, materialName);
		mRenderer->AddMaterial(materialName);
	}
	auto material = mRenderer->GetMaterial();
	if (material && (material->mShaderName.contains("Common") || material->mShaderName.contains("Lit") || material->mShaderName.contains("Scrolling"))) {
		SetInstancing(false);
	}
	if(mTag == "Water" || mTag == "UI" || mTag == "SkyDome") mCastShadow = false;
	else if (mTag == "DirectionalLight") {
		mCastShadow = false;
	}
	else {
		mCastShadow = true;
	}
	if (mTag == "SkyDome") mRenderLayer = RENDER_LAYER::Transparent;
	if(mTag == "Cloud") {
		mCastShadow = false;
		mRenderLayer = RENDER_LAYER::Opaque;
	}
}

void CGameObject::CreateTerrainFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	std::string name{};
	ReadDateFromFile(inFile, name);
	UINT resolution{};
	Vec3 size{};
	Vec3 offset{};
	ReadDateFromFile(inFile, resolution);

	auto material = std::static_pointer_cast<CTerrainMaterial>(RESOURCE.Get<CMaterial>(name + "Material"));
	if (!material) {
		material = std::make_shared<CTerrainMaterial>();
		material->SetName(name + "Material");
		RESOURCE.Add(material);
	}
	material->LoadTerrainData(inFile);
	size = material->GetSize();

	ReadDateFromFile(inFile, offset);

	auto mesh = std::static_pointer_cast<CHeightMapGridMesh>(RESOURCE.Get<CMesh>(name + "Mesh"));
	if (!mesh) {
		mesh = std::make_shared<CHeightMapGridMesh>();
		mesh->Initialize(name + "Heightmap", resolution, size, offset);
		mesh->SetName(name + "Mesh");
		RESOURCE.Add(mesh);
	}
	auto terrain = AddComponent<CTerrain>();
	terrain->SetHeightMapGridMesh(mesh);
	terrain->SetMaterial(material);
	terrain->MakeNavMap(name + "NavMap", resolution*2);
	material->Initialize();

	mLocalAABB = mWorldAABB = terrain->mWorldAABB;

	mCastShadow = true;
	INSTANCE(CSceneManager).GetCurScene()->SetTerrain(terrain);
}

void CGameObject::CreateLightFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;

	//mTransform->LookTo(Vec3(0.02, -0.9f, 0.02).GetNormalized());
	auto light = AddComponent<CLight>();
	CBLightsData lightData{};
	ReadDateFromFile(inFile, lightData.type);
	Color color{};
	ReadDateFromFile(inFile, color);
	lightData.color = color.ToVector3();
	ReadDateFromFile(inFile, lightData.range);
	ReadDateFromFile(inFile, lightData.strength);
	ReadDateFromFile(inFile, lightData.innerSpotAngle);
	ReadDateFromFile(inFile, lightData.spotAngle);
	light->SetLightData(lightData);

	if (lightData.type == (UINT)LIGHT_TYPE::DIRECTIONAL) {
		mTag = "DirectionalLight";	
		auto camera = AddComponent<CCamera>();
		float shadowMapResolution = INSTANCE(CDX12Manager).GetShadowMapResolution();
		camera->mCameraName = "DirectionalLight";
		camera->SetViewport(0, 0, shadowMapResolution, shadowMapResolution);
		camera->SetScissorRect(0, 0, shadowMapResolution, shadowMapResolution);
	}
	mCastShadow = false;
}

void CGameObject::CacheFrameHierarchies(std::vector<std::shared_ptr<CGameObject>>& boneFrameCaches)
{
	boneFrameCaches.push_back(GetSptrFromThis());

	for (auto& child : mChildren) {
		child->CacheFrameHierarchies(boneFrameCaches);
	}
}

void CGameObject::PrepareSkinning()
{
	mAnimationController->PrepareSkinning();
}

void CGameObject::UpdateWorldMatrices(std::shared_ptr<CTransform> parent)
{
	GetTransform()->UpdateWorldMatrix(parent, false);

	for (auto& child : mChildren) {
		child->UpdateWorldMatrices(mTransform);
	}
}

void CGameObject::PrintSRT()
{
}

void CGameObject::RegisterRenderer()
{
	if (!mActive) return;
	if (mRenderer) {
		mRenderer->RegisterRenderQueue();
	}
	for (const auto& child : mChildren) {
		child->RegisterRenderer();
	}
}

std::shared_ptr<CGameObject> CGameObject::FindChildByName(const std::string& name)
{
	std::shared_ptr<CGameObject> obj{ nullptr };

	for (const auto& child : mChildren) {
		if (child->mName == name) 
			return child;

		if (obj = child->FindChildByName(name))
			return obj;
	}

	return obj;
}

void CGameObject::AddChild(std::shared_ptr<CGameObject> child)
{
	auto itr = findByRawPointer(mChildren, child.get());

	if (itr != mChildren.end()) {
		return;
	}
	mChildren.push_back(child);
}

void CGameObject::RemoveChild(std::shared_ptr<CGameObject> child)
{
	auto itr = findByRawPointer(mChildren, child.get());

	if (itr != mChildren.end()) {
		mChildren.erase(itr);
	}
}

std::shared_ptr<CGameObject> CGameObject::AddBoneSocket(const std::string& boneName, const std::string& socketName)
{
	std::shared_ptr<CGameObject> socket = std::make_shared<CGameObject>();
	socket->mName = socketName;
	socket->mTag = "BoneSocket";
	socket->mObjectType = OBJECT_TYPE::NONE;
	auto bone = FindChildByName(boneName);
	if (!bone) {
		std::cerr << "Bone not found: " << boneName << std::endl;
		return nullptr;
	}
	bone->AddChild(socket);
	return socket;
}

void CGameObject::GetAllChildren(std::vector<std::shared_ptr<CGameObject>>& children)
{
	for (const auto& child : mChildren) {
		children.push_back(child);
		child->GetAllChildren(children);
	}
}

