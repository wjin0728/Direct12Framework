#pragma once
#include "stdafx.h"
#include "Renderer.h"

class CUIRenderer : public CRenderer
{
public:
	static std::shared_ptr<class CMesh> mQuad;
private:
	CBUIData mUIData;
	std::shared_ptr<class CShader> mShader{};

	int mUIdataIdx = -1;

	UINT mUIDirtyFrame = FRAME_RESOURCE_COUNT + 1;

public:
	CUIRenderer();
	CUIRenderer(const CUIRenderer& other) : CRenderer(other) {}
	virtual ~CUIRenderer();
public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;
	void Render(std::shared_ptr<CCamera> camera, int pass = 0) override;
	void UpdataObjectDataToShader() override;
	void UpdateUIDataToShader();
	std::shared_ptr<CComponent> Clone() override { return std::make_shared<CUIRenderer>(*this); }

	void SetUICBVIndex();
	void ReturnUICBVIndex();

public:
	void SetShader(const std::shared_ptr<CShader>& shader) { mShader = shader; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetShader(const std::string& name);
	void SetUIData(const CBUIData& data) { mUIData = data; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetColor(const Vec4& color) { mUIData.color = color; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetAlpha(float alpha) { mUIData.color.w = alpha; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetType(int type) { mUIData.type = type; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetTextureIdx(int idx) { mUIData.textureIdx = idx; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetTexture(const std::string& name);
	void SetTexture(const std::shared_ptr<class CTexture>& texture);
	void SetPosition(const Vec2& pos) { mUIData.pos = pos; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetSize(const Vec2& size) { mUIData.size = size; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetUVOffset(const Vec2& uvOffset) { mUIData.uvOffset = uvOffset; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }
	void SetUVScale(const Vec2& uvScale) { mUIData.uvScale = uvScale; mUIDirtyFrame = FRAME_RESOURCE_COUNT; }

	CBUIData GetUIData() const { return mUIData; }
	Vec4 GetColor() const { return mUIData.color; }
	Vec2 GetSize() const { return mUIData.size; }
	Vec2 GetUVOffset() const { return mUIData.uvOffset; }
	Vec2 GetUVScale() const { return mUIData.uvScale; }
	int GetTextureIdx() const { return mUIData.textureIdx; }
	int GetType() const { return mUIData.type; }
};

