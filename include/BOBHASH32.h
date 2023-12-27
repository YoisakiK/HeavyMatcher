#ifndef _BOBHASH32_H
#define _BOBHASH32_H
#include <stdio.h>
using namespace std;

typedef unsigned int uint;
typedef unsigned long long int uint64;


#define MAX_PRIME32 1229
#define MAX_BIG_PRIME32 50

class BOBHash32
{
public:
	BOBHash32();
	~BOBHash32();
	BOBHash32(uint prime32Num);
	void initialize(uint prime32Num);
	uint run(const char * str, uint len);
private:
	uint prime32Num;
};

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

#endif //_BOBHASH32_H
