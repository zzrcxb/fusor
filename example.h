#ifndef FUSOR_H
#define FUSOR_H

#ifdef VS_PLATFORM
__declspec(noinline) void fusor_symvar(void* v) {
  ((unsigned char volatile *) v)[0] = ((unsigned char *)v)[0];
};

#else
void __attribute__((noinline)) fusor_symvar(void* v) {
  ((unsigned char volatile *) v)[0] = ((unsigned char *)v)[0];
};

#endif // VS_PLATFORM
#endif // Header file