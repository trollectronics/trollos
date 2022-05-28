#ifndef ASSERT_H_
#define ASSERT_H_

#define assert(x) do {extern void panic(const char *); if (!(x)) {panic("Assert failed");}} while (0)

#endif

