#include <windows.h>
#include "VertexBuffer.h"
#include <DirectXMath.h>
#include "InputLayout.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "FSQuad.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
namespace wrl = Microsoft::WRL;
using namespace DirectX;

//Structs
struct vertex
{
    XMFLOAT3 pos;
};

//Variables
static bool FXAAEnabled = true;
constexpr LPCTSTR WndClassName = "fxaaWindow";
HWND hWnd = nullptr;
constexpr int width = 1024;
constexpr int height = 768;
wrl::ComPtr<ID3D11Device> pDevice;
wrl::ComPtr<ID3D11DeviceContext> pContext;
wrl::ComPtr<IDXGISwapChain> pChain = nullptr;
wrl::ComPtr<ID3D11RenderTargetView> pRtv = nullptr;
wrl::ComPtr<ID3D11SamplerState> gSplr;
std::vector<vertex> gVertices;
std::unique_ptr<VertexBuffer<vertex>> gVertexBuffer;
VertexShader gVS;
PixelShader gPS;
std::unique_ptr<InputLayout> gLayout;
std::unique_ptr<RenderTarget> gRt;
std::unique_ptr<DepthStencil> gDs;
std::unique_ptr<FSQuad> quad;

//Functions
bool InitializeWindow(HINSTANCE hInstance,
    const int ShowWnd,
    const int width, int height,
    const bool windowed) noexcept;

bool InitializeGraphics(HINSTANCE hInstance) noexcept;

int MessageLoop() noexcept;

bool InitScene() noexcept;

void DrawScene() noexcept;

LRESULT CALLBACK WndProc(HWND hWnd,
    const UINT msg,
    const WPARAM wParam,
    const LPARAM lParam);

//Windows Main
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
	//Initialize window
    if (!InitializeWindow(hInstance, nShowCmd, width, height, true))
    {
        Error::Log("Failed to initialize window.");
        return 0;
    }

	//Initialize graphics
	if(!InitializeGraphics(hInstance))
	{
        Error::Log("Failed to initialize graphics.");
        return 0;
	}

	//Initialize scene
    if (!InitScene())
    {
        Error::Log("Failed to initialize scene.");
        return 0;
    }

	//Run message loop
    MessageLoop();

    return 0;
}

bool InitializeWindow(HINSTANCE hInstance,
    const int ShowWnd,
    const int width, const int height,
    bool windowed) noexcept
{
    //Create window
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = WndClassName;
    wc.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

	//Register class properties
    if (!RegisterClassEx(&wc))
    {
        Error::Log("Failed to registering class.");
;        return false;
    }

	//Create window ex for handle
    hWnd = CreateWindowEx(
        NULL,
        WndClassName,
        "FXAA App <DX11>",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width,
        height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hWnd)
    {
        Error::Log("Failed to creating window.");
        return false;
    }

	//Show window
    ShowWindow(hWnd, ShowWnd);
    UpdateWindow(hWnd);

    return true;
}

bool InitializeGraphics(HINSTANCE hInstance) noexcept
{
    //Describe our Buffer
    DXGI_MODE_DESC bufferDesc;

    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = width;
    bufferDesc.Height = height;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //Describe our SwapChain
    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    //Create our SwapChain
    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, NULL, nullptr, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, &pChain, &pDevice, nullptr, &pContext);

    //Create our BackBuffer
    wrl::ComPtr<ID3D11Texture2D> BackBuffer;
    pChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(BackBuffer.GetAddressOf()));

    //Create our Render Target
    pDevice->CreateRenderTargetView(BackBuffer.Get(), nullptr, &pRtv);
    BackBuffer->Release();

    //Set our Render Target
    pContext->OMSetRenderTargets(1, pRtv.GetAddressOf(), nullptr);

    //create viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pContext->RSSetViewports(1u, &vp);

    return true;
}

bool InitScene() noexcept
{
    //Vertex Buffer Initialize
    gVertices.push_back(vertex(XMFLOAT3(-0.5, -0.5, 0.0)));
    gVertices.push_back(vertex(XMFLOAT3(0.0, 0.5, 0.0)));
    gVertices.push_back(vertex(XMFLOAT3(0.5, -0.5, 0.0)));
    gVertexBuffer = std::make_unique<VertexBuffer<vertex>>();
    gVertexBuffer->Init(pDevice.Get(), gVertices.data(), gVertices.size());

    //Shaders initialize
    gVS.Init(L"VSColor.cso", pDevice.Get());
    gPS.Init(L"PSColor.cso", pDevice.Get());

    //Create Input Layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> fxaa_ied =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    gLayout = std::make_unique<InputLayout>(pDevice.Get(), fxaa_ied, &gVS);

    //Depth Stencil initialize
    gDs = std::make_unique<DepthStencil>(pDevice.Get(),
        pContext.Get(), width, height, 1,
        false, DepthStencil::Usage::DepthStencil);

    //Render Target initialize
    gRt = std::make_unique<RenderTarget>(pDevice.Get(),
        pContext.Get(), width, height, 1,
        false, RenderTarget::Usage::Default);

    quad = std::make_unique<FSQuad>(pDevice.Get(), pContext.Get(), width, height);

    //create sampler state
    CD3D11_SAMPLER_DESC sampler_desc(D3D11_DEFAULT);
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = pDevice->CreateSamplerState(&sampler_desc, &gSplr);
    if (FAILED(hr)) { Error::Log(hr, "Failed to create sampler state"); }
	
    return true;
}

void DrawScene() noexcept
{
    float bgColor[4] = {0.0f, 0.0f, 0.3f, 1.0f};
    if (!FXAAEnabled)
    pContext->ClearRenderTargetView(pRtv.Get(),
        bgColor);
    pContext->PSSetSamplers(0, 1, gSplr.GetAddressOf());

    if (FXAAEnabled)
    {
        gRt->BindAsTarget(pContext.Get(), gDs->pDepthStencilView.Get());
        gRt->Clear(pContext.Get(), bgColor);
        gDs->Clear(pContext.Get());
    }

	//Vertex Buffer bind
	gVertexBuffer->Bind(pContext.Get());
	
	//Shades bind
    gLayout->Bind(pContext.Get());
    gVS.Bind(pContext.Get());
    gPS.Bind(pContext.Get());

    pContext->Draw(gVertices.size(), 0);

    if (FXAAEnabled)
    {
        pContext->OMSetRenderTargets(1u, pRtv.GetAddressOf(), nullptr);
        gRt->BindAsTexture(pContext.Get(), 0);
        quad->draw(pContext.Get());
    }
	
    pChain->Present(0, 0);
}

int MessageLoop() noexcept {

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DrawScene();
        }
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,
    const UINT msg,
    const WPARAM wParam,
    const LPARAM lParam)
{
    switch (msg)
    {

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            DestroyWindow(hwnd);
        }
        if (wParam == 'F')
        {
            if (!FXAAEnabled)
                FXAAEnabled = true;
            else
                FXAAEnabled = false;
        }

        return 0;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd,
            msg,
            wParam,
            lParam);
    }
}