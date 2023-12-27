#pragma once
#include <stdio.h>
using namespace std;

typedef unsigned int uint;
typedef unsigned long long int uint64;

#define MAX_PRIME64 1229
#define MAX_BIG_PRIME64 50

class BOBHash64
{
public:
	BOBHash64();
	~BOBHash64();
	BOBHash64(uint prime64Num);
	void initialize(uint prime64Num);
	uint64 run(const char * str, uint len);
private:
	uint prime64Num;
};
#define mix64(a,b,c) \
{  \
  a -= b; a -= c; a ^= (c>>43); \
  b -= c; b -= a; b ^= (a<<9); \
  c -= a; c -= b; c ^= (b>>8); \
  a -= b; a -= c; a ^= (c>>38); \
  b -= c; b -= a; b ^= (a<<23); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>35); \
  b -= c; b -= a; b ^= (a<<49); \
  c -= a; c -= b; c ^= (b>>11); \
  a -= b; a -= c; a ^= (c>>12); \
  b -= c; b -= a; b ^= (a<<18); \
  c -= a; c -= b; c ^= (b>>22); \
}
