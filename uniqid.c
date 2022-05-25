
#include "usprotocol.h"

static LCG_GLOBAL lcg_global;

#define LCG(v)	(lcg_global.v)

#define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m

static void lcg_seed(void);

double combined_lcg(void)
{
	unsigned int q;
	unsigned int z;
	q=LCG(seeded);
	if (q==0)
	{
		lcg_seed();
	}
	
	MODMULT(53668, 40014, 12211, 2147483563L,LCG(s1));
	MODMULT(52774, 40692, 3791, 2147483399L,LCG(s2));

	z = LCG(s1) - LCG(s2);
	if (z < 1) {
		z += 2147483562;
	}

	return z * 4.656613e-10;
}
static void lcg_seed(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == 0) {
		LCG(s1) = tv.tv_sec ^ (~tv.tv_usec);
	} else {
		LCG(s1) = 1;
	}
	LCG(s2) = (long) getpid();
	LCG(seeded) = 1;
}
void myuniqid(char* myuqid)
{
	char *prefix = "";
	int sec, usec;
	struct timeval tv;


	usleep(1);
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 0x100000);
	snprintf(myuqid,24, "%s%08x%05x%.8f", prefix, sec, usec,combined_lcg() * 10);

}
