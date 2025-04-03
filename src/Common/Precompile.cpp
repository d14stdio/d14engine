#include "Common/Precompile.h"

// Refer to https://devblogs.microsoft.com/directx/gettingstarted-dx12agility/#2.-set-agility-sdk-parameters
// We need to set Agility SDK parameters to help locate the custom D3D12Core.dll during building.
#if _D14_AGILITY_SDK
const UINT D3D12SDKVersion = _D14_AGILITY_VER;
const char* D3D12SDKPath = _D14_AGILITY_PATH;
#endif
