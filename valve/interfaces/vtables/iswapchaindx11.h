#pragma once

// forward declarations
struct IDXGISwapChain;

class ISwapChainDx11
{
public:
	char pad_0[0x170];
	IDXGISwapChain* pDXGISwapChain;
};
