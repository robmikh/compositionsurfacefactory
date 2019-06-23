//
// pch.h
// Header for platform projection include files
//

#pragma once

#include <unknwn.h>
#include <inspectable.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

#include <winrt/Microsoft.Graphics.Canvas.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.Composition.h>
#include <winrt/Microsoft.Graphics.Canvas.Text.h>

#include <atomic>
#include <memory>
#include <algorithm>
#include <vector>
#include <iterator>

#include <d3d11_4.h>
#include <dxgi1_6.h>

#include "direct3d11.interop.h"

#include <winrt/Robmikh.CompositionSurfaceFactory.h>
namespace API = winrt::Robmikh::CompositionSurfaceFactory;
