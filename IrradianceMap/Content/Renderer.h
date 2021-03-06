//--------------------------------------------------------------------------------------
// Copyright (c) XU, Tianchen. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "Core/XUSG.h"

class Renderer
{
public:
	enum RenderMode : uint8_t
	{
		MIP_APPROX,
		SH_APPROX,
		GROUND_TRUTH
	};

	Renderer(const XUSG::Device& device);
	virtual ~Renderer();

	bool Init(XUSG::CommandList* pCommandList, uint32_t width, uint32_t height,
		const XUSG::DescriptorTableCache::sptr& descriptorTableCache,
		std::vector<XUSG::Resource>& uploaders, const char* fileName, XUSG::Format rtFormat,
		const DirectX::XMFLOAT4& posScale = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bool SetLightProbes(const XUSG::Descriptor& irradiance, const XUSG::Descriptor& radiance);
	bool SetLightProbesGT(const XUSG::Descriptor& irradiance, const XUSG::Descriptor& radiance);
	bool SetLightProbesSH(const XUSG::Descriptor& coeffSH);

	void UpdateFrame(uint32_t frameIndex, DirectX::CXMVECTOR eyePt,
		DirectX::CXMMATRIX viewProj, float glossy, bool isPaused);
	void Render(const XUSG::CommandList* pCommandList, uint32_t frameIndex, XUSG::ResourceBarrier* barriers,
		uint32_t numBarriers = 0, RenderMode mode = MIP_APPROX, bool needClear = false);
	void Postprocess(const XUSG::CommandList* pCommandList, const XUSG::Descriptor& rtv,
		uint32_t numBarriers, XUSG::ResourceBarrier* pBarriers);

protected:
	enum PipelineLayoutSlot : uint8_t
	{
		OUTPUT_VIEW,
		SHADER_RESOURCES,
		SAMPLER,
		VS_CONSTANTS,
		CBUFFER,
		PS_CONSTANTS = OUTPUT_VIEW,
	};

	enum PipelineIndex : uint8_t
	{
		BASE_PASS,
		BASE_PASS_SH,
		ENVIRONMENT,
		TEMPORAL_AA,
		POSTPROCESS,

		NUM_PIPELINE
	};

	enum SRVTable : uint8_t
	{
		SRV_TABLE_BASE,
		SRV_TABLE_GT,
		SRV_TABLE_TAA,
		SRV_TABLE_TAA1,
		SRV_TABLE_PP,
		SRV_TABLE_PP1,

		NUM_SRV_TABLE
	};

	enum UAVTable : uint8_t
	{
		UAV_TABLE_TAA,
		UAV_TABLE_TAA1,

		NUM_UAV_TABLE
	};

	enum RenderTargetIndex : uint8_t
	{
		RT_COLOR,
		RT_VELOCITY,

		NUM_RENDER_TARGET
	};

	enum OutputView : uint8_t
	{
		UAV_PP_TAA,
		UAV_PP_TAA1,

		NUM_OUTPUT_VIEW
	};

	struct BasePassConstants
	{
		DirectX::XMFLOAT4X4	WorldViewProj;
		DirectX::XMFLOAT4X4	WorldViewProjPrev;
		DirectX::XMFLOAT4X4	World;
		DirectX::XMFLOAT2	ProjBias;
	};

	struct PerFrameConstants
	{
		DirectX::XMFLOAT4	EyePtGlossy;
		DirectX::XMFLOAT4X4	ScreenToWorld;
	};

	struct SHConstants
	{
		DirectX::XMFLOAT4	CoeffSH[9];
	};

	bool createVB(XUSG::CommandList* pCommandList, uint32_t numVert,
		uint32_t stride, const uint8_t* pData, std::vector<XUSG::Resource>& uploaders);
	bool createIB(XUSG::CommandList* pCommandList, uint32_t numIndices,
		const uint32_t* pData, std::vector<XUSG::Resource>& uploaders);
	bool createInputLayout();
	bool createPipelineLayouts();
	bool createPipelines(XUSG::Format rtFormat);
	bool createDescriptorTables();

	void render(const XUSG::CommandList* pCommandList, RenderMode mode, bool needClear);
	void environment(const XUSG::CommandList* pCommandList);
	void temporalAA(const XUSG::CommandList* pCommandList);

	XUSG::Device m_device;

	uint32_t	m_numIndices;
	uint8_t		m_frameParity;

	DirectX::XMUINT2	m_viewport;
	DirectX::XMFLOAT4	m_posScale;
	BasePassConstants	m_cbBasePass;
	PerFrameConstants	m_cbPerFrame;

	XUSG::InputLayout		m_inputLayout;
	XUSG::PipelineLayout	m_pipelineLayouts[NUM_PIPELINE];
	XUSG::Pipeline			m_pipelines[NUM_PIPELINE];

	XUSG::DescriptorTable	m_cbvTable;
	XUSG::DescriptorTable	m_srvTables[NUM_SRV_TABLE];
	XUSG::DescriptorTable	m_uavTables[NUM_UAV_TABLE];
	XUSG::DescriptorTable	m_samplerTable;
	XUSG::Framebuffer		m_framebuffer;

	XUSG::VertexBuffer::uptr	m_vertexBuffer;
	XUSG::IndexBuffer::uptr		m_indexBuffer;

	XUSG::RenderTarget::uptr	m_renderTargets[NUM_RENDER_TARGET];
	XUSG::Texture2D::uptr		m_outputViews[NUM_OUTPUT_VIEW];
	XUSG::DepthStencil::uptr	m_depth;

	XUSG::ShaderPool::uptr				m_shaderPool;
	XUSG::Graphics::PipelineCache::uptr	m_graphicsPipelineCache;
	XUSG::Compute::PipelineCache::uptr	m_computePipelineCache;
	XUSG::PipelineLayoutCache::uptr		m_pipelineLayoutCache;
	XUSG::DescriptorTableCache::sptr	m_descriptorTableCache;
};
