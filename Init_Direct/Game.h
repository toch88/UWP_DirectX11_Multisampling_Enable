#pragma once

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace DirectX;


#define DXGI_FORMAT_MAX 116
#define MAX_SAMPLES_CHECK 128
// a struct to represent a single vertex
struct VERTEX
{
	float X, Y, Z;    // vertex position
};

private ref class MultisamplingSupportInfo sealed
{
internal:
	MultisamplingSupportInfo()
	{
		ZeroMemory(&sampleSizeData, sizeof(sampleSizeData));
		ZeroMemory(&qualityFlagData, sizeof(qualityFlagData));
	};

public:
	unsigned int GetSampleSize(int i, int j) { return sampleSizeData[i][j]; };
	void         SetSampleSize(int i, int j, unsigned int value) { sampleSizeData[i][j] = value; };

	unsigned int GetQualityFlagsAt(int i, int j) { return qualityFlagData[i][j]; };
	void         SetQualityFlagsAt(int i, int j, unsigned int value) { qualityFlagData[i][j] = value; };

	bool         GetFormatSupport(int format) { return formatSupport[format]; };
	void         SetFormatSupport(int format, bool value) { formatSupport[format] = value; };

	unsigned int GetFormat() { return m_format; };
	void         SetFormat(unsigned int i) { m_format = (DXGI_FORMAT)i; };

	unsigned int GetLargestSampleSize() { return m_largestSampleSize; };
	unsigned int GetSmallestSampleSize() { return m_smallestSampleSize; };

	void SetLargestSampleSize(unsigned int value) { m_largestSampleSize = value; };
	void SetSmallestSampleSize(unsigned int value) { m_smallestSampleSize = value; };

private:
	unsigned int sampleSizeData[DXGI_FORMAT_MAX][MAX_SAMPLES_CHECK];
	unsigned int qualityFlagData[DXGI_FORMAT_MAX][MAX_SAMPLES_CHECK];

	bool formatSupport[DXGI_FORMAT_MAX];

	unsigned int m_largestSampleSize;
	unsigned int m_smallestSampleSize;

	DXGI_FORMAT m_format;
};


class CGame
{
public:
	ComPtr<ID3D11Device1> dev;                      // the device interface
	ComPtr<ID3D11DeviceContext1> devcon;            // the device context interface
	ComPtr<IDXGISwapChain1> swapchain;              // the swap chain interface
	ComPtr<ID3D11RenderTargetView> rendertarget;    // the render target interface
	ComPtr<ID3D11Buffer> vertexbuffer;              // the vertex buffer interface
	ComPtr<ID3D11VertexShader> vertexshader;        // the vertex shader interface
	ComPtr<ID3D11PixelShader> pixelshader;          // the pixel shader interface
	ComPtr<ID3D11InputLayout> inputlayout;          // the input layout interface


	
	UINT m4xMsaaQuality;
	unsigned int            GetSampleSize() { return m_sampleSize; }
	unsigned int m_sampleSize;
	MultisamplingSupportInfo^ m_supportInfo;
	unsigned int m_qualityFlags;
	ComPtr<ID3D11Texture2D> m_offScreenSurface;
	ComPtr<ID3D11RenderTargetView>	m_d3dRenderTargetView;
	ComPtr<ID3D11Texture2D> backbuffer;
	void Initialize();
	void Resize();
	void Update();
	void Render();
	void InitGraphics();
	void InitPipeline();
};
