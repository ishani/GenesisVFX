#ifndef __VFX_HSV__
#define __VFX_HSV__

void HSV_to_RGB(int &ri,int &gi,int &bi,int hi,int si,int vi);
void RGB_to_HSV(int ri,int gi,int bi,int &hi,int &si,int &vi);
int HSV(int h,int s,int v);

#endif