#include "pch.h"
#include "Game.h"
#include <fstream>

// this function loads a file into an Array^
Array<byte>^ LoadShaderFile(std::string File)
{
	Array<byte>^ FileData = nullptr;

	// open the file
	std::ifstream VertexFile(File, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
	if (VertexFile.is_open())
	{
		// find the length of the file
		int Length = (int)VertexFile.tellg();

		// collect the file data
		FileData = ref new Array<byte>(Length);
		VertexFile.seekg(0, std::ios::beg);
		VertexFile.read(reinterpret_cast<char*>(FileData->Data), Length);
		VertexFile.close();
	}

	return FileData;
}




// this function initializes and prepares Direct3D for use
void CGame::Initialize()
{
	m_supportInfo = ref new MultisamplingSupportInfo();
	m_qualityFlags = D3D11_STANDARD_MULTISAMPLE_PATTERN;
	m_sampleSize = 8;
	m_qualityFlags = 16;

	// Define temporary pointers to a device and a device context
	ComPtr<ID3D11Device> dev11;
	ComPtr<ID3D11DeviceContext> devcon11;
	D3D_FEATURE_LEVEL featureLevel;
	// Create the device and device context objects
	D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&dev11,
		&featureLevel,
		&devcon11);

	// Convert the pointers from the DirectX 11 versions to the DirectX 11.1 versions
	dev11.As(&dev);
	devcon11.As(&devcon);


	// obtain the DXGI factory
	ComPtr<IDXGIDevice1> dxgiDevice;
	dev.As(&dxgiDevice);
	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);
	ComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

	UINT m4xMsaaQuality;
	dev->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM,8, &m4xMsaaQuality);

	// Determine the format support for multisampling.
	for (UINT i = 1; i < DXGI_FORMAT_MAX; i++)
	{
		DXGI_FORMAT inFormat = safe_cast<DXGI_FORMAT>(i);
		UINT formatSupport = 0;
		HRESULT hr = dev->CheckFormatSupport(inFormat, &formatSupport);		

		if ((formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) &&
			(formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET)
			)
		{
			m_supportInfo->SetFormatSupport(i, true);
		}
		else
		{
			m_supportInfo->SetFormatSupport(i, false);
		}
	}
	// Find available sample sizes for each supported format.
	for (unsigned int i = 0; i < DXGI_FORMAT_MAX; i++)
	{
		for (unsigned int j = 1; j < MAX_SAMPLES_CHECK; j++)
		{
			UINT numQualityFlags;

			HRESULT test = dev->CheckMultisampleQualityLevels(
				(DXGI_FORMAT)i,
				j,
				&numQualityFlags
			);

			if (SUCCEEDED(test) && (numQualityFlags > 0))
			{
				m_supportInfo->SetSampleSize(i, j, 1);
				m_supportInfo->SetQualityFlagsAt(i, j, numQualityFlags);
			}
		}
	}



	// set up the swap chain description
	DXGI_SWAP_CHAIN_DESC1 scd = { 0 };
	
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // how the swap chain should be used
	scd.BufferCount =2;      
	
	scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // the most common swap chain format
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // the recommended flip mode
	scd.SampleDesc.Count = 1;                             // disable anti-aliasing
	scd.SampleDesc.Quality = 0;

	CoreWindow^ Window = CoreWindow::GetForCurrentThread();    // get the window pointer

	swapchain = nullptr;										   // create the swap chain
	dxgiFactory->CreateSwapChainForCoreWindow(
		dev.Get(),                                  // address of the device
		reinterpret_cast<IUnknown*>(Window),        // address of the window
		&scd,                                       // address of the swap chain description
		nullptr,                                    // advanced
		&swapchain);                                // address of the new swap chain pointer

	
	
		// get a pointer directly to the back buffer
		ComPtr<ID3D11Texture2D> backbuffer;
		swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (&backbuffer));

		
		

		

		CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS);		
		
		
		
	
	// create a render target pointing to the back buffer
	dev->CreateRenderTargetView(backbuffer.Get(), &renderTargetViewDesc, &rendertarget);	
	

	// set the viewport
	D3D11_VIEWPORT viewport = { 0 };

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Window->Bounds.Width;
	viewport.Height = Window->Bounds.Height;

	devcon->RSSetViewports(1, &viewport);


	// initialize graphics and the pipeline
	InitGraphics();
	InitPipeline();
	
}

void CGame::Resize()
{
	//rendertarget.Get()->Release();
	
	CoreWindow^ Window = CoreWindow::GetForCurrentThread();
	
	swapchain->ResizeBuffers(2, Window->Bounds.Width, Window->Bounds.Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY);
	ComPtr<ID3D11Texture2D> backbuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (&backbuffer));

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS);

	// create a render target pointing to the back buffer
	dev->CreateRenderTargetView(backbuffer.Get(), &renderTargetViewDesc, &rendertarget);

	// set the viewport
	D3D11_VIEWPORT viewport = { 0 };

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Window->Bounds.Width;
	viewport.Height = Window->Bounds.Height;

	devcon->RSSetViewports(1, &viewport);
	InitGraphics();
}

// this function performs updates to the state of the game
void CGame::Update()
{
}

// this function renders a single frame of 3D graphics
void CGame::Render()
{
	CoreWindow^ Window = CoreWindow::GetForCurrentThread();
	D3D11_TEXTURE2D_DESC offScreenSurfaceDesc;
	ZeroMemory(&offScreenSurfaceDesc, sizeof(D3D11_TEXTURE2D_DESC));
	offScreenSurfaceDesc.Width = Window->Bounds.Width;
	offScreenSurfaceDesc.Height = Window->Bounds.Height;
	offScreenSurfaceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	offScreenSurfaceDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	offScreenSurfaceDesc.MipLevels = 1;
	offScreenSurfaceDesc.ArraySize = 1;
	offScreenSurfaceDesc.SampleDesc.Count = 8;
	offScreenSurfaceDesc.SampleDesc.Quality = m4xMsaaQuality - 1;

	dev->CreateTexture2D(
		&offScreenSurfaceDesc,
		nullptr,
		&m_offScreenSurface);

	unsigned int sub = D3D11CalcSubresource(0, 0, 1);
	devcon->ResolveSubresource(
		backbuffer.Get(),
		sub,
		m_offScreenSurface.Get(),
		sub,
		DXGI_FORMAT_B8G8R8A8_UNORM
	);
	// set our new render target object as the active render target
	devcon->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);

	// clear the back buffer to a deep blue
	float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView(rendertarget.Get(), color);

	// set the vertex buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset);

	// set the primitive topology
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw 3 vertices, starting from vertex 0
	devcon->Draw(3, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(1, 0);
}

// this function loads and initializes all graphics data
void CGame::InitGraphics()
{
	// create a triangle out of vertices
	VERTEX OurVertices[] =
	{
		{ 0.0f, 0.5f, 0.0f },
		{ 0.45f, -0.5f, 0.0f },
		{ -0.45f, -0.5f, 0.0f },
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(OurVertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { OurVertices, 0, 0 };

	dev->CreateBuffer(&bd, &srd, &vertexbuffer);
}

// this function initializes the GPU settings and prepares it for rendering
void CGame::InitPipeline()
{
	// load the shader files
	Array<byte>^ VSFile = LoadShaderFile("VertexShader.cso");
	Array<byte>^ PSFile = LoadShaderFile("PixelShader.cso");

	// create the shader objects
	dev->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &vertexshader);
	dev->CreatePixelShader(PSFile->Data, PSFile->Length, nullptr, &pixelshader);

	// set the shader objects as the active shaders
	devcon->VSSetShader(vertexshader.Get(), nullptr, 0);
	devcon->PSSetShader(pixelshader.Get(), nullptr, 0);

	// initialize input layout
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// create and set the input layout
	dev->CreateInputLayout(ied, ARRAYSIZE(ied), VSFile->Data, VSFile->Length, &inputlayout);
	devcon->IASetInputLayout(inputlayout.Get());
}
