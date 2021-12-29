#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include <math.h>
#include <windows.h>
#include "HSV.h"

#define Inv255	3.92156862745E-3f;

void HSV_to_RGB(int &ri,int &gi,int &bi,int hi,int si,int vi)
{	int i;
	
	float h=(float)hi*Inv255;
	float s=(float)si*Inv255;
	float v=(float)vi*Inv255;

	float aa,bb,cc,f,r,g,b;

	if (s==0) 
			{	ri=gi=bi=vi;
				return;
			}
	else	{	if (h==1.0f) h=0.0f;
				h*=6.0f;
				i=(float)floor(h);
				f=h-i;
				aa=v*(1-s);
				bb=v*(1-(s*f));
				cc=v*(1-(s*(1-f)));

				switch(i)	{	case 0: r=v; g=cc; b=aa; break;
								case 1: r=bb; g=v; b=aa; break;
								case 2: r=aa; g=v; b=cc; break;
								case 3: r=aa; g=bb; b=v; break;
								case 4: r=cc; g=aa; b=v; break;
								case 5: r=v; g=aa; b=bb; break;
							}

				ri=(r*255.0f+0.5f);
				gi=(g*255.0f+0.5f);
				bi=(b*255.0f+0.5f);
				return;
			}
}

#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)

void RGB_to_HSV(int ri,int gi,int bi,int &hi,int &si,int &vi)
{	
	float r=(float)ri*Inv255;
	float g=(float)gi*Inv255;
	float b=(float)bi*Inv255;

	float h,s,v;

	float max=MAX(r,MAX(g,b)),min=MIN(r,MIN(g,b));
	float delta=max-min;

	v=max;

	if (max!=0.0f) s=delta/max;
	else s=0.0f;

	if (s==0.0f) h=0.0f;
	else	{			if (r==max) h=(g-b)/delta;
				else	if (g==max) h=2+(b-r)/delta;
				else	if (b==max) h=4+(r-g)/delta;

				h*=60.0f;
				if (h<0) h+=360.0f;
				h/=360.0;
			}

	hi=(h*255.0f+0.5f);
	si=(s*255.0f+0.5f);
	vi=(v*255.0f+0.5f);
}

int HSV(int h,int s,int v)
{	int r,g,b;

	HSV_to_RGB(r,g,b,h,s,v);

	return RGB(r,g,b);
}