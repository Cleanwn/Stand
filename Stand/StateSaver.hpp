#pragma once

#include "dx_common.hpp"

namespace Stand
{
	class StateSaver
	{
	public:
		StateSaver();
		~StateSaver();

		void saveCurrentState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		void restoreSavedState();
		void releaseSavedState();

	private:
		bool						m_savedState;
		D3D_FEATURE_LEVEL			m_featureLevel;
		ID3D11DeviceContext* m_pContext;
		D3D11_PRIMITIVE_TOPOLOGY	m_primitiveTopology;
		ID3D11InputLayout* m_pInputLayout;
		ID3D11BlendState* m_pBlendState;
		FLOAT						m_blendFactor[4];
		UINT						m_sampleMask;
		ID3D11DepthStencilState* m_pDepthStencilState;
		UINT						m_stencilRef;
		ID3D11RasterizerState* m_pRasterizerState;
		ID3D11ShaderResourceView* m_pPSSRV;
		ID3D11SamplerState* m_pSamplerState;
		ID3D11VertexShader* m_pVS;
		ID3D11ClassInstance* m_pVSClassInstances[256];
		UINT						m_numVSClassInstances;
		ID3D11Buffer* m_pVSConstantBuffer;
		ID3D11GeometryShader* m_pGS;
		ID3D11ClassInstance* m_pGSClassInstances[256];
		UINT						m_numGSClassInstances;
		ID3D11Buffer* m_pGSConstantBuffer;
		ID3D11ShaderResourceView* m_pGSSRV;
		ID3D11PixelShader* m_pPS;
		ID3D11ClassInstance* m_pPSClassInstances[256];
		UINT						m_numPSClassInstances;
		ID3D11HullShader* m_pHS;
		ID3D11ClassInstance* m_pHSClassInstances[256];
		UINT						m_numHSClassInstances;
		ID3D11DomainShader* m_pDS;
		ID3D11ClassInstance* m_pDSClassInstances[256];
		UINT						m_numDSClassInstances;
		ID3D11Buffer* m_pVB;
		UINT						m_vertexStride;
		UINT						m_vertexOffset;
		ID3D11Buffer* m_pIndexBuffer;
		DXGI_FORMAT					m_indexFormat;
		UINT						m_indexOffset;

		StateSaver(const StateSaver&);
	};
}
