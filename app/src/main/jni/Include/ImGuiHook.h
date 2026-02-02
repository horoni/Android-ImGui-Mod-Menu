#ifndef HORONI_IMGUIHOOK_H
#define HORONI_IMGUIHOOK_H

#if !defined(INPUT_UNIVERSAL) && !defined(INPUT_UNITY) && !defined(INPUT_SDL)
#  warn "Input method is not selected defaulting to INPUT_UNIVERSAL"
#  define INPUT_UNIVERSAL
#endif

void initMenuHooks();

#endif // HORONI_IMGUIHOOK_H