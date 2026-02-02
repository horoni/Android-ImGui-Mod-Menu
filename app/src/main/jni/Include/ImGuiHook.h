#pragma once

#if !defined(INPUT_UNIVERSAL) && !defined(INPUT_UNITY) && !defined(INPUT_SDL)
#  warn "Input method is not selected defaulting to INPUT_UNIVERSAL"
#  define INPUT_UNIVERSAL
#endif

void initMenuHooks();
