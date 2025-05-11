#pragma once
#include"stdafx.h"
#include"CResource.h"

enum PASS_TYPE : UINT8
{
	FORWARD,
	G_PASS,
	SHADOW,
	STENCIL,
	DIRECTIONAL,
	LIGHTING,
	POST_PROCESSING,
	FINAL,
	UI,
	PASS_TYPE_COUNT
};

enum class INPUT_LAYOUT_TYPE : UINT8
{
	BILLBOARD,
	PARTICLE,
	DEFAULT,
	TEXTURE,
	ANIMATION,
	TERRAIN,
	NONE
};

enum class RASTERIZER_TYPE : UINT8
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,
	WIREFRAME,

	RASTERIZER_TYPE_COUNT
};

enum class DEPTH_STENCIL_TYPE : UINT8
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	NO_DEPTH_TEST, // ±Ì¿Ã ≈◊Ω∫∆Æ(X) + ±Ì¿Ã ±‚∑œ(O)
	NO_DEPTH_TEST_NO_WRITE, // ±Ì¿Ã ≈◊Ω∫∆Æ(X) + ±Ì¿Ã ±‚∑œ(X)
	LESS_NO_WRITE, // ±Ì¿Ã ≈◊Ω∫∆Æ(O) + ±Ì¿Ã ±‚∑œ(X)

	DEPTH_STENCIL_TYPE_COUNT
};

enum class BLEND_TYPE : UINT8
{
	DEFAULT,
	ALPHA_BLEND,
	ONE_TO_ONE_BLEND,
	ADD_BLEND,
	SUBTRACT_BLEND,
	MULTIPLE_BLEND,

	BLEND_TYPE_COUNT
};

struct ShaderInfo
{
	PASS_TYPE shaderType = PASS_TYPE::FORWARD;
	INPUT_LAYOUT_TYPE inputLayoutYype = INPUT_LAYOUT_TYPE::DEFAULT;
	RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

class CShader : public CResource
{
public:
	static std::array<std::string, PASS_TYPE_COUNT> passName;

protected:
	ShaderInfo mInfo;
	ComPtr<ID3D12PipelineState> d3dPiplineState = ComPtr<ID3D12PipelineState>();

private:

public:
	CShader() = default;
	virtual ~CShader() {};
	virtual void ReleaseUploadBuffer() {}

protected:
	virtual D3D12_INPUT_LAYOUT_DESC InitInputLayout();
	virtual D3D12_RASTERIZER_DESC InitRasterizerState();
	virtual D3D12_BLEND_DESC InitBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC InitDepthStencilState();

	D3D12_SHADER_BYTECODE CreateShader(ComPtr<ID3DBlob>& blob, 
		const std::string& name, const std::string& fname, const std::string& version);

public:
	virtual bool Initialize(const ShaderInfo& info, const std::string& name, bool getPassName = true);
	
public:
	void SetPipelineState(ID3D12GraphicsCommandList* cmdList) { cmdList->SetPipelineState(d3dPiplineState.Get()); }
	PASS_TYPE GetShaderType() const { return this->mInfo.shaderType; }
};
