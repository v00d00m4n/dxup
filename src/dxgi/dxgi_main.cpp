#include "dxgi_swapchain.h"
#include "dxgi_factory.h"

typedef HRESULT(__stdcall *CreateDXGIFactory2Func)(UINT Flags, REFIID riid, void **ppFactory);

using namespace dxup;

class DXGIWrapper
{
public:
	DXGIWrapper()
	{
		HMODULE dxgiModule = LoadLibraryA("dxgi_original.dll");
		if (dxgiModule)
			CreateDXGIFactory2Function = (CreateDXGIFactory2Func)GetProcAddress(dxgiModule, "CreateDXGIFactory2");
	}

	CreateDXGIFactory2Func CreateDXGIFactory2Function;

	static DXGIWrapper& Get()
	{
		if (!s_dxgiWrapper)
			s_dxgiWrapper = new DXGIWrapper();

		return *s_dxgiWrapper;
	}

private:
	static DXGIWrapper* s_dxgiWrapper;
};

DXGIWrapper* DXGIWrapper::s_dxgiWrapper = nullptr;

extern "C"
{
	void __stdcall DXUPWrapSwapChain(IDXGISwapChain** ppSwapChain)
	{
		auto* dxupSwapchain = new DXGISwapChain(*ppSwapChain);

		*ppSwapChain = dxupSwapchain;
	}

	HRESULT __stdcall CreateDXGIFactory2(UINT Flags, REFIID riid, void **ppFactory)
	{
		IDXGIFactory2* pWrappedFactory = nullptr;
		HRESULT result = DXGIWrapper::Get().CreateDXGIFactory2Function(Flags, __uuidof(IDXGIFactory2), (void**)&pWrappedFactory);

		if (pWrappedFactory)
		{
			DXGIFactory* dxupFactory = new DXGIFactory(pWrappedFactory);
			if (ppFactory)
				*ppFactory = dxupFactory;
		}

		return result;
	}

	HRESULT __stdcall CreateDXGIFactory1(REFIID riid, void **ppFactory)
	{
		return CreateDXGIFactory2(0, riid, ppFactory);
	}

	HRESULT __stdcall CreateDXGIFactory(REFIID riid, void **ppFactory)
	{
		return CreateDXGIFactory2(0, riid, ppFactory);
	}

	HRESULT __stdcall DXGID3D10RegisterLayers(const struct dxgi_device_layer *layers, UINT layer_count)
	{
		return E_NOTIMPL;
	}
	HRESULT __stdcall  DXGID3D10GetLayeredDeviceSize(const struct dxgi_device_layer *layers, UINT layer_count)
	{
		return E_NOTIMPL;
	}
	struct Byte20 { BYTE p[20]; };
	HRESULT __stdcall  DXGID3D10CreateLayeredDevice(Byte20 z)
	{
		return E_NOTIMPL;
	}
	HRESULT __stdcall  DXGID3D10CreateDevice(HMODULE d3d10core, IDXGIFactory *factory, IDXGIAdapter *adapter, UINT flags, DWORD arg5, void **device)
	{
		return E_NOTIMPL;
	}
}