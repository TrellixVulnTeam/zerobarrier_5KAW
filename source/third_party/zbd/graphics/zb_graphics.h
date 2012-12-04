#pragma once

//
// See copyright notice in zbd.h.
//

#if defined(ZB_PLATFORM_WINDOWS)
#include <d3d9.h>
#include <d3dx9.h>
#endif

#include "../zbd.h"
#include "../zb_stl.h"

#include "../math/zb_math.h"
#include "../time/zb_time.h"
#include "../thread/zb_thread.h"

#include "internal/camera.h"
#include "internal/color.h"
#include "internal/word_aligned_buffer.h"
#include "internal/graphics.h"
