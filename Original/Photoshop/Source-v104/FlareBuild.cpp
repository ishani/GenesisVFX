// #####################################################################
// Changed 24.03.97
//		Changes In Brush Handling (uughh), store oldbrush for w95 friendliness

//-----------------------------------------------------------------------------
static ISpinnerControl	*BuildWidthSpin			= NULL;
static ISpinnerControl	*BuildNoStreaksSpin		= NULL;
static ISpinnerControl	*BuildTransparencySpin	= NULL;
static ISpinnerControl	*BuildNoSidesSpin		= NULL;
static ISpinnerControl	*BuildAspectSpin		= NULL;
static ISpinnerControl	*BuildFactorSpin		= NULL;
static ISpinnerControl	*BuildSeedSpin			= NULL;

static ICustButton		*BuildT1		 = NULL;
static ICustButton		*BuildT2		 = NULL;
static ICustButton		*BuildT3		 = NULL;
static ICustButton		*BuildT4		 = NULL;
static ICustButton		*BuildT5		 = NULL;
static ICustButton		*BuildT6		 = NULL;
static ICustButton		*BuildT7		 = NULL;
static ICustButton		*BuildT8		 = NULL;

static ICustButton		*BuildS1		 = NULL;
static ICustButton		*BuildS2		 = NULL;
static ICustButton		*BuildS3		 = NULL;
static ICustButton		*BuildS4		 = NULL;

static ICustButton		*BuildOK		 = NULL;
static ICustButton		*BuildCancel	 = NULL;

static IColorSwatch		*BuildColour	 = NULL;

COLORREF buildcolorref;

//-----------------------------------------------------------------------------
char			*FlareBuildTempBitmap;
BITMAPINFO		FlareBuildTempBitmapInfo;
static Flare	Flare_BuildPreview(0.5f,0.0f);
HWND			hwBuildPreview;				// Preview Windows;

#define BuildInterpolateColours(r1,g1,b1,a1,r2,g2,b2,a2)\
	 { int i; float c=0.0f,dc=1.0f/(float)(PositionTo-PositionFrom);\
	   for(i=PositionFrom;i<=PositionTo;i++,c+=dc)\
					{ fColour tmp; tmp.r=(1.0f-c)*r1+c*r2; tmp.g=(1.0f-c)*g1+c*g2; tmp.b=(1.0f-c)*b1+c*b2;\
					  tmp.a=(1.0f-c)*a1+c*a2; Flare_BuildPreview.PutRadialColour(tmp,i);\
					  tmp.r=tmp.g=tmp.b=tmp.a=1.0f; Flare_BuildPreview.PutAngularColour(tmp,i);\
					}\
	}

#define BuildLine(ax,ay,bx,by)\
	{ for(int ii=i1;ii!=i2;ii++)\
			{ float alpha=6.28318530718f*(float)ii/(float)FLARERESOLUTION;\
              float t=((float)sin(alpha)*ax-(float)cos(alpha)*ay)/\
                      ((float)sin(alpha)*ax-(float)sin(alpha)*bx+\
					   (float)cos(alpha)*by-(float)cos(alpha)*ay);\
	          float cx=ax+t*(bx-ax),cy=ay+t*(by-ay);\
			  float Dst=(float)sqrt(cx*cx+cy*cy);\
			  Flare_BuildPreview.PutShape(Dst,ii);\
			}\
	}



void ImageFilter_LensF::BuildRenderIntoFlare(void)
{	float r,g,b,a;

	data.BuildWidth=BuildWidthSpin->GetFVal();
	data.BuildNoStreaks=BuildNoStreaksSpin->GetIVal();
	data.BuildTransparency=BuildTransparencySpin->GetFVal();
	data.BuildNoSides=BuildNoSidesSpin->GetIVal();
	data.BuildAspect=BuildAspectSpin->GetFVal();
	data.BuildFactor=BuildFactorSpin->GetFVal();
	data.BuildSeed=BuildSeedSpin->GetIVal();

	buildcolorref = BuildColour->GetColor();
	data.BuildCol.r=GetRValue(buildcolorref)<<8|0xFF; r=(float)data.BuildCol.r/65535.0f;
	data.BuildCol.g=GetGValue(buildcolorref)<<8|0xFF; g=(float)data.BuildCol.g/65535.0f;
	data.BuildCol.b=GetBValue(buildcolorref)<<8|0xFF; b=(float)data.BuildCol.b/65535.0f;
	a=1.0f;

	srand48(data.BuildSeed);

	Flare_BuildPreview.Intensity=data.BuildTransparency;

	float ftmp=1.0f;
	float ftmp2=0.0f;

	for(int i=0;i!=FLARERESOLUTION;i++)
				{ Flare_BuildPreview.PutShape(ftmp,i);
	              Flare_BuildPreview.PutShape2(ftmp2,i);
				}

	if ((data.BuildColour==1)||(data.BuildColour==5)||(data.BuildColour==6))
			{ float x=0.0f,dx=1.0f/(float)FLARERESOLUTION;
	          
			  for(i=0;i!=FLARERESOLUTION;i++,x+=dx)
					{ fColour tmp;
			          tmp.r=r*(float)exp(-5.454*x*x);
					  tmp.g=g*(float)exp(-5.454*x*x);
					  tmp.b=b*(float)exp(-5.454*x*x);
					  tmp.a=a*(float)exp(-5.454*x*x);
					  Flare_BuildPreview.PutRadialColour(tmp,i);

					  tmp.r=tmp.b=tmp.g=tmp.a=1.0f;
					  Flare_BuildPreview.PutAngularColour(tmp,i);
					}
			}
	else if (data.BuildColour==2)
	        { float x=0.0f,dx=1.0f/(float)FLARERESOLUTION;
			  float a=1.0f-0.5f*data.BuildWidth;
	          float k=5.541f/data.BuildWidth;
			  
	          for(i=0;i!=FLARERESOLUTION;i++,x+=dx)
					{ fColour tmp;
			          tmp.r=r*(0.5f*((float)tanh(-k*(x-a))+1.0f));
					  tmp.g=g*(0.5f*((float)tanh(-k*(x-a))+1.0f));
					  tmp.b=b*(0.5f*((float)tanh(-k*(x-a))+1.0f));
					  tmp.a=a*(0.5f*((float)tanh(-k*(x-a))+1.0f));
					  Flare_BuildPreview.PutRadialColour(tmp,i);

					  tmp.r=tmp.b=tmp.g=tmp.a=1.0f;
					  Flare_BuildPreview.PutAngularColour(tmp,i);
					}
			}
	else if (data.BuildColour==3)
	        { float x=data.BuildWidth-1.0f,dx=1.0f/(float)FLARERESOLUTION;
	          float k=5.545f/(data.BuildWidth*data.BuildWidth);
	          
			  for(i=0;i!=FLARERESOLUTION;i++,x+=dx)
					{ fColour tmp;
			          tmp.r=r*(float)exp(-k*x*x);
					  tmp.g=g*(float)exp(-k*x*x);
					  tmp.b=b*(float)exp(-k*x*x);
					  tmp.a=a*(float)exp(-k*x*x);
					  Flare_BuildPreview.PutRadialColour(tmp,i);

					  tmp.r=tmp.b=tmp.g=tmp.a=1.0f;
					  Flare_BuildPreview.PutAngularColour(tmp,i);
					}
			}
	else if (data.BuildColour==4)
	        { float x=0.0f,dx=1.0f/(float)FLARERESOLUTION;
			  float a=1.0f-0.5f*(1.0f-data.BuildWidth);
	          float k=5.541f/(1.0f-data.BuildWidth);
			  
	          for(i=0;i!=FLARERESOLUTION;i++,x+=dx)
					{ fColour tmp;
			          float ftmp=(0.5f*((float)tanh(-k*(x-a))+1.0f));

					  if (ftmp>=0.996)
							{ tmp.r=0.0f;
							  tmp.g=0.0f;
							  tmp.b=0.0f;
							  tmp.a=1.0f;
							}
					  else	{ tmp.r=r*ftmp;
							  tmp.g=g*ftmp;
							  tmp.b=b*ftmp;
							  tmp.a=a*ftmp;
					        }

					  Flare_BuildPreview.PutRadialColour(tmp,i);

					  tmp.r=tmp.b=tmp.g=tmp.a=1.0f;
					  Flare_BuildPreview.PutAngularColour(tmp,i);
					}
			}
	if ((data.BuildColour==5)||(data.BuildColour==6))
			{ float k=9.212f/(data.BuildWidth*data.BuildWidth);
			  fColour tmp;
	          
			  tmp.r=tmp.g=tmp.b=tmp.a=0.0f;
			  for(i=0;i!=FLARERESOLUTION;i++)
					Flare_BuildPreview.PutAngularColour(tmp,i);

	          for(i=0;i!=data.BuildNoStreaks;i++)
					{ float Angle;
			          if (data.BuildColour==5) Angle=(float)i/(float)data.BuildNoStreaks;
					  else Angle=drand48();

					  int j=(int)((float)Angle*(float)FLARERESOLUTION);
					  float x=-0.5f,dx=1.0f/(float)FLARERESOLUTION;

					  for(;x<=0.5;x+=dx)	
							{ j++; if (j>=FLARERESOLUTION) j=0;
						      Flare_BuildPreview.GetAngularColour(tmp,j);
					          tmp.r+=(float)exp(-k*x*x); 
							  tmp.g+=(float)exp(-k*x*x); 
							  tmp.b+=(float)exp(-k*x*x); 
							  tmp.a+=(float)exp(-k*x*x); 
							  Flare_BuildPreview.PutAngularColour(tmp,j);
							}
					}
			}
	 else if (data.BuildColour==7)
			{ int Width=(int)((float)FLARERESOLUTION*(float)data.BuildWidth/5.0f);

		      int PositionFrom=0;
              int PositionTo  =FLARERESOLUTION-5*Width;

			  BuildInterpolateColours(0.0f  ,0.0f  ,0.0f  ,0.0f,
				                     1.0f*r,0.0f*g,0.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(1.0f*r,0.0f*g,0.0f*b,a,
				                     1.0f*r,1.0f*g,0.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(1.0f*r,1.0f*g,0.0f*b,a,
				                     0.0f*r,1.0f*g,0.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(0.0f*r,1.0f*g,0.0f*b,a,
				                     0.0f*r,1.0f*g,1.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(0.0f*r,1.0f*g,1.0f*b,a,
				                     0.0f*r,0.0f*g,1.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  =FLARERESOLUTION-1;

			  BuildInterpolateColours(0.0f*r,0.0f*g,1.0f*b,a,
				                     0.0f*r,0.0f*g,0.0f*b,0.0f);
			}
	else if (data.BuildColour==8)
			{ int Width=(int)((float)FLARERESOLUTION*(float)data.BuildWidth/5.0f);

		      int PositionFrom=0;
              int PositionTo  =FLARERESOLUTION-5*Width;

			  BuildInterpolateColours(0.0f  ,0.0f  ,0.0f  ,0.0f,
				                     0.0f*r,0.0f*g,1.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(0.0f*r,0.0f*g,1.0f*b,a,
				                     0.0f*r,1.0f*g,1.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(0.0f*r,1.0f*g,1.0f*b,a,
				                     0.0f*r,1.0f*g,0.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(0.0f*r,1.0f*g,0.0f*b,a,
				                     1.0f*r,1.0f*g,0.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  +=Width;

			  BuildInterpolateColours(1.0f*r,1.0f*g,0.0f*b,a,
				                     1.0f*r,0.0f*g,0.0f*b,a);

			  PositionFrom=PositionTo;
              PositionTo  =FLARERESOLUTION-1;

			  BuildInterpolateColours(1.0f*r,0.0f*g,0.0f*b,a,
				                     0.0f*r,0.0f*g,0.0f*b,0.0f);
			}

	if ((data.BuildShape==1)||(data.BuildShape==2))
			{ float r1=1.0f;
			  float r2=(data.BuildShape==2)?data.BuildAspect:1.0f;
			  
			  float x=0.0f,dx=6.283185f/(float)FLARERESOLUTION;
			  
			  for(int i=0;i!=FLARERESOLUTION;i++,x+=dx)
					{ float Alpha=(float)atan(r2*(float)tan(x));
				      float _y=r1*(float)sin(Alpha);
			          float _x=r2*(float)cos(Alpha);

					  float ftmp=(float)sqrt(_x*_x+_y*_y);

					  Flare_BuildPreview.PutShape(ftmp,i);
					}
			}
	else if (data.BuildShape==3)
			{ float dA=6.28318530718f/(float)data.BuildNoSides;
	          for(int i=0;i!=data.BuildNoSides;i++)
					{ float ax=(float)cos((float)i*dA);
			          float ay=(float)sin((float)i*dA);
					  float bx=(float)cos(((float)i+1.0f)*dA);
			          float by=(float)sin(((float)i+1.0f)*dA);
					  int i1=(int)((float)FLARERESOLUTION*(float)i/(float)data.BuildNoSides);
					  int i2=(int)((float)FLARERESOLUTION*(float)(i+1.0f)/(float)data.BuildNoSides);
					  BuildLine(ax,ay,bx,by);
					}
			}
	else if (data.BuildShape==4)
			{ float dA=6.28318530718f/(float)data.BuildNoSides;
	          for(int i=0;i!=data.BuildNoSides;i++)
					{ int i1=(int)((float)FLARERESOLUTION*(float)i/(float)data.BuildNoSides);
				      float ax=(float)cos((float)i*dA);
			          float ay=(float)sin((float)i*dA);

					  int i2=(int)((float)FLARERESOLUTION*(float)(i+0.5f)/(float)data.BuildNoSides);
					  float bx=data.BuildFactor*(float)cos(((float)i+0.5f)*dA);
			          float by=data.BuildFactor*(float)sin(((float)i+0.5f)*dA);
					  
					  BuildLine(ax,ay,bx,by);

					  ax=(float)cos(((float)i+1.0f)*dA);
			          ay=(float)sin(((float)i+1.0f)*dA);
					  i1=i2; i2=(int)((float)FLARERESOLUTION*(float)(i+1.0f)/(float)data.BuildNoSides);

					  BuildLine(ax,ay,bx,by);
					}
			}

	Flare_BuildPreview.CheckColours();
}


//****************************************************************
// Preview Stuff
void ImageFilter_LensF::BuildPreview(int CheckerBoard)
{	SetCursor(LoadCursor(NULL,IDC_WAIT));
    HDC		hdc = GetDC(hwBuildPreview);
	RECT	rect; GetClientRect(hwBuildPreview,&rect);
	int lxres=(rect.right-rect.left)&0xfffffff0;
	int lyres=(rect.bottom-rect.top); 

	BuildRenderIntoFlare();

	if (FlareBuildTempBitmap==NULL)
			{ FlareBuildTempBitmapInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
			  FlareBuildTempBitmapInfo.bmiHeader.biPlanes		=1;
			  FlareBuildTempBitmapInfo.bmiHeader.biBitCount		=24;
			  FlareBuildTempBitmapInfo.bmiHeader.biCompression	=BI_RGB;
			  FlareBuildTempBitmapInfo.bmiHeader.biClrUsed		=0;
			  FlareBuildTempBitmapInfo.bmiHeader.biClrImportant	=0;
			  FlareBuildTempBitmapInfo.bmiHeader.biWidth		=lxres;
			  FlareBuildTempBitmapInfo.bmiHeader.biHeight		=lyres;
			  FlareBuildTempBitmapInfo.bmiHeader.biSizeImage	=lxres*lyres*3;
			  FlareBuildTempBitmap=(char *)malloc(lxres*lyres*3);
			}

	// Render It
	Flare_BuildPreview.RenderPreview(FlareBuildTempBitmap,lxres,lyres,CheckerBoard,1.0f,1.0f);

	// Display It
	SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareBuildTempBitmap,&FlareBuildTempBitmapInfo,DIB_RGB_COLORS);
	
	ReleaseDC(hwBuildPreview,hdc);
	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

BOOL CALLBACK BuildPreviewProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)    { case WM_RBUTTONDOWN:{ ImageFilterData->BuildPreview(ImageFilterData->data.DrawChecker);
											  break;
											}
						case WM_PAINT:		{ HDC		hdc = GetDC(hwBuildPreview);
											  RECT		rect;
											  GetClientRect(hwBuildPreview,&rect);

											  int lxres=(rect.right-rect.left)&0xfffffff0;
											  int lyres=rect.bottom-rect.top;

											  if (FlareBuildTempBitmap!=0) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareBuildTempBitmap,
											                                                &FlareBuildTempBitmapInfo,DIB_RGB_COLORS);
											  else		{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
											              HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);			// 24.03.97
											              Rectangle(hdc,rect.left,rect.top,rect.left+(rect.right-rect.left)&0xfffffff0,rect.bottom);
														  SelectObject(hdc,oldbrush);								// 24.03.97
											              DeleteObject(brush);
											            }

											  ReleaseDC(hwBuildPreview,hdc);
											  break;
											}
    	}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 

void ImageFilter_LensF::BuildUpdateControls(HWND hWnd)
{	int Flag;

	BuildT1->SetCheck((data.BuildColour==1)); 
	BuildT2->SetCheck((data.BuildColour==2)); 
	BuildT3->SetCheck((data.BuildColour==3)); 
	BuildT4->SetCheck((data.BuildColour==4)); 
	BuildT5->SetCheck((data.BuildColour==5)); 
	BuildT6->SetCheck((data.BuildColour==6)); 
	BuildT7->SetCheck((data.BuildColour==7)); 
	BuildT8->SetCheck((data.BuildColour==8)); 

	BuildS1->SetCheck((data.BuildShape==1)); 
	BuildS2->SetCheck((data.BuildShape==2)); 
	BuildS3->SetCheck((data.BuildShape==3)); 
	BuildS4->SetCheck((data.BuildShape==4)); 

	Flag=(data.BuildColour!=1)?1:0;;
	EnableIt(IDC_SPIN1,Flag); EnableIt(IDC_EDIT1,Flag);	// Width

	Flag=((data.BuildColour==5)||(data.BuildColour==6))?1:0;
    EnableIt(IDC_SPIN2,Flag); EnableIt(IDC_EDIT2,Flag);	// No Streaks

	Flag=1;
    EnableIt(IDC_SPIN3,Flag); EnableIt(IDC_EDIT3,Flag);	// Transparancy

	Flag=((data.BuildShape==3)||(data.BuildShape==4))?1:0;;
	EnableIt(IDC_SPIN4,Flag); EnableIt(IDC_EDIT4,Flag);	// No Sides

	Flag=(data.BuildShape==2)?1:0;;
	EnableIt(IDC_SPIN5,Flag); EnableIt(IDC_EDIT5,Flag);	// Aspect

	Flag=(data.BuildShape==4)?1:0;;
	EnableIt(IDC_SPIN6,Flag); EnableIt(IDC_EDIT6,Flag);	// Factor
}

// make button images 
HIMAGELIST		HIL_Button1;
HIMAGELIST		HIL_Button2;
HIMAGELIST		HIL_Button3;
HIMAGELIST		HIL_Button4;
HIMAGELIST		HIL_Button5;
HIMAGELIST		HIL_Button6;
HIMAGELIST		HIL_Button7;
HIMAGELIST		HIL_Button8;

HIMAGELIST		HIL_Button2_1;
HIMAGELIST		HIL_Button2_2;
HIMAGELIST		HIL_Button2_3;
HIMAGELIST		HIL_Button2_4;

HIMAGELIST		HIL_Build_OK;
HIMAGELIST		HIL_Build_Cancel;

void ImageFilter_LensF::BuildBuildButtons(void)
{		BuildButton(BuildT1,HIL_Button1,IDB_MAKE_BUTTON1B,IDB_MAKE_BUTTON1,50,50);
		BuildButton(BuildT2,HIL_Button2,IDB_MAKE_BUTTON2B,IDB_MAKE_BUTTON2,50,50);
		BuildButton(BuildT3,HIL_Button3,IDB_MAKE_BUTTON3B,IDB_MAKE_BUTTON3,50,50);
		BuildButton(BuildT4,HIL_Button4,IDB_MAKE_BUTTON4B,IDB_MAKE_BUTTON4,50,50);
		BuildButton(BuildT5,HIL_Button5,IDB_MAKE_BUTTON5B,IDB_MAKE_BUTTON5,50,50);
		BuildButton(BuildT6,HIL_Button6,IDB_MAKE_BUTTON6B,IDB_MAKE_BUTTON6,50,50);
		BuildButton(BuildT7,HIL_Button7,IDB_MAKE_BUTTON7B,IDB_MAKE_BUTTON7,50,50);
		BuildButton(BuildT8,HIL_Button8,IDB_MAKE_BUTTON8B,IDB_MAKE_BUTTON8,50,50);
		BuildButton(BuildS1,HIL_Button2_1,IDB_MAKE_BUTTON2_1B,IDB_MAKE_BUTTON2_1,50,50);
		BuildButton(BuildS2,HIL_Button2_2,IDB_MAKE_BUTTON2_2B,IDB_MAKE_BUTTON2_2,50,50);
		BuildButton(BuildS3,HIL_Button2_3,IDB_MAKE_BUTTON2_3B,IDB_MAKE_BUTTON2_3,50,50);
		BuildButton(BuildS4,HIL_Button2_4,IDB_MAKE_BUTTON2_4B,IDB_MAKE_BUTTON2_4,50,50);
		BuildButton(BuildOK,HIL_Build_OK,IDB_MASTEROK,IDB_MASTEROK,100,50);
		BuildButton(BuildCancel,HIL_Build_Cancel,IDB_MASTERCANCEL,IDB_MASTERCANCEL,100,50);
}

void ImageFilter_LensF::BuildFreeButtons(void)
{		ReleaseButton(HIL_Button1);
		ReleaseButton(HIL_Button2);
		ReleaseButton(HIL_Button3);
		ReleaseButton(HIL_Button4);
		ReleaseButton(HIL_Button5);
		ReleaseButton(HIL_Button6);
		ReleaseButton(HIL_Button7);
		ReleaseButton(HIL_Button8);
		ReleaseButton(HIL_Button2_1);
		ReleaseButton(HIL_Button2_2);
		ReleaseButton(HIL_Button2_3);
		ReleaseButton(HIL_Button2_4);
		ReleaseButton(HIL_Build_OK);
		ReleaseButton(HIL_Build_Cancel);
}

void ImageFilter_LensF::BuildReleaseButtons()
{
								   ReleaseICustButton(BuildT1);
								   ReleaseICustButton(BuildT2);
								   ReleaseICustButton(BuildT3);
								   ReleaseICustButton(BuildT4);
								   ReleaseICustButton(BuildT5);
								   ReleaseICustButton(BuildT6);
								   ReleaseICustButton(BuildT7);
								   ReleaseICustButton(BuildT8);
								   ReleaseICustButton(BuildS1);
								   ReleaseICustButton(BuildS2);
								   ReleaseICustButton(BuildS3);
								   ReleaseICustButton(BuildS4);

								   ReleaseICustButton(BuildOK);
								   ReleaseICustButton(BuildCancel);

								   ReleaseSpinner(BuildWidthSpin);
								   ReleaseSpinner(BuildNoStreaksSpin);
								   ReleaseSpinner(BuildTransparencySpin);
								   ReleaseSpinner(BuildNoSidesSpin);
								   ReleaseSpinner(BuildAspectSpin);
								   ReleaseSpinner(BuildFactorSpin);
								   ReleaseSpinner(BuildSeedSpin);

								   ReleaseIColorSwatch(BuildColour);
}


static int FlareBuildDialogBoxExit;
HBITMAP BuildLoadBitmap;
HBITMAP BuildBackBitmap;

BOOL ImageFilter_LensF::BuildControl(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
  if( message==WM_COMMAND || message==CC_SPINNER_CHANGE || message==CC_COLOR_CHANGE)
	switch (LOWORD(wParam)) 
	  { case IDC_T1:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildColour=1;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
				           break;
						 }

		case IDC_T2:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) )
										{ data.BuildColour=2;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_T3:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildColour=3;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_T4:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildColour=4;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_T5:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildColour=5;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_T6:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildColour=6;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }
      
		case IDC_T7:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildColour=7;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_T8:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildColour=8;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_S1:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildShape=1;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_S2:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildShape=2;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_S3:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildShape=3;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_S4:	 { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
										{ data.BuildShape=4;
										  BuildUpdateControls(hWnd);
										  if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
										}
						   break;
						 }

		case IDC_BUILDCHECKER: { int tmp=data.DrawChecker;
								 data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_BUILDCHECKER); 

								 if ((data.DrawChecker!=tmp)&&(data.BuildActivePreview)) BuildPreview(data.DrawChecker);
								 break; 
							   }

		case IDC_APV:		   { int tmp=data.BuildActivePreview;
								 data.BuildActivePreview=IsDlgButtonChecked(hWnd,IDC_APV); 

								 if ((data.BuildActivePreview!=tmp)&&(data.BuildActivePreview)) BuildPreview(data.DrawChecker);
								 break; 
							   }

		case IDC_COL1:		   { if ((data.BuildActivePreview)&&(message==CC_COLOR_CHANGE)&&(HIWORD(wParam)==1)) 
												{ BuildPreview(data.DrawChecker); }
								
			                     break;
							   }

		case IDC_SPIN1:
		case IDC_SPIN2:
		case IDC_SPIN3:
		case IDC_SPIN4:
		case IDC_SPIN5:
		case IDC_SPIN6:
		case IDC_MAKESEED_SPIN:
			if ((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
						{ if (data.BuildActivePreview) BuildPreview(data.DrawChecker);
			              break;
						}
			break;
	  }

	switch (message) 
	   { case WM_INITDIALOG: 
		   { CenterWindow(hWnd,GetParent(hWnd));
             SetCursor(LoadCursor(NULL,IDC_ARROW));

			 // Setup background bitmap rcw 18/4/97
				// load bitmap and get window dc
				BuildLoadBitmap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_MAKE));
				HDC hdc=GetDC(hWnd);
				RECT	rect;
				BITMAP	bm;

				// get sizes of bitmap and window
				GetClientRect(hWnd,&rect);
				GetObject(BuildLoadBitmap,sizeof(BITMAP),&bm);

				if( abs(rect.right-bm.bmWidth)<=2 && abs(rect.bottom-bm.bmHeight)<=2 )
					// keep original
					BuildBackBitmap=BuildLoadBitmap;
				else {
					// get the final background bitmap
					BuildBackBitmap=CreateCompatibleBitmap(hdc,(rect.right+3)&0xfffffffc,rect.bottom);
					// interpolate new size
					// note this deletes old bitmap internally
					LinearResample(hdc,BuildLoadBitmap,bm.bmWidth,bm.bmHeight,hdc,BuildBackBitmap,rect.right,rect.bottom);
				}

				// clean up DC
				ReleaseDC(hWnd,hdc);

			 hwBuildPreview = GetDlgItem(hWnd, IDC_BUILDPREVIEW);
			 SetWindowLong(hwBuildPreview,GWL_WNDPROC,(LONG)BuildPreviewProc);
			 CheckDlgButton(hWnd,IDC_BUILDCHECKER,data.DrawChecker);
			 CheckDlgButton(hWnd,IDC_APV,data.BuildActivePreview);

			 BuildT1=GetICustButton(GetDlgItem(hWnd,IDC_T1)); BuildT1->SetType(CBT_CHECK);
			 BuildT2=GetICustButton(GetDlgItem(hWnd,IDC_T2)); BuildT2->SetType(CBT_CHECK);
			 BuildT3=GetICustButton(GetDlgItem(hWnd,IDC_T3)); BuildT3->SetType(CBT_CHECK);
			 BuildT4=GetICustButton(GetDlgItem(hWnd,IDC_T4)); BuildT4->SetType(CBT_CHECK);
			 BuildT5=GetICustButton(GetDlgItem(hWnd,IDC_T5)); BuildT5->SetType(CBT_CHECK);
			 BuildT6=GetICustButton(GetDlgItem(hWnd,IDC_T6)); BuildT6->SetType(CBT_CHECK);
			 BuildT7=GetICustButton(GetDlgItem(hWnd,IDC_T7)); BuildT7->SetType(CBT_CHECK);
			 BuildT8=GetICustButton(GetDlgItem(hWnd,IDC_T8)); BuildT8->SetType(CBT_CHECK);

			 BuildS1=GetICustButton(GetDlgItem(hWnd,IDC_S1)); BuildS1->SetType(CBT_CHECK);
			 BuildS2=GetICustButton(GetDlgItem(hWnd,IDC_S2)); BuildS2->SetType(CBT_CHECK);
			 BuildS3=GetICustButton(GetDlgItem(hWnd,IDC_S3)); BuildS3->SetType(CBT_CHECK);
			 BuildS4=GetICustButton(GetDlgItem(hWnd,IDC_S4)); BuildS4->SetType(CBT_CHECK);

			 BuildOK=GetICustButton(GetDlgItem(hWnd,IDC_BUILD_OK)); 
			 BuildCancel=GetICustButton(GetDlgItem(hWnd,IDC_BUILD_CANCEL)); 

			 BuildWidthSpin			=SetupFloatSpinner(hWnd,IDC_SPIN1,IDC_EDIT1,0.01f,1.0f,data.BuildWidth,0.005f,data.BuildWidth);
             BuildNoStreaksSpin		=SetupIntSpinner  (hWnd,IDC_SPIN2,IDC_EDIT2,1,100,data.BuildNoStreaks,1,data.BuildNoStreaks);
             BuildTransparencySpin  =SetupFloatSpinner(hWnd,IDC_SPIN3,IDC_EDIT3,0.0f,2.0f,data.BuildTransparency,0.01f,data.BuildTransparency);
             BuildNoSidesSpin       =SetupIntSpinner  (hWnd,IDC_SPIN4,IDC_EDIT4,3,100,data.BuildNoSides,1,data.BuildNoSides);
             BuildAspectSpin        =SetupFloatSpinner(hWnd,IDC_SPIN5,IDC_EDIT5,0.01f,1.0f,data.BuildAspect,0.01f,data.BuildAspect);
             BuildFactorSpin        =SetupFloatSpinner(hWnd,IDC_SPIN6,IDC_EDIT6,0.01f,1.0f,data.BuildFactor,0.01f,data.BuildFactor);
			 BuildSeedSpin			=SetupIntSpinner  (hWnd,IDC_MAKESEED_SPIN,IDC_MAKESEED_EDIT,0,9999,data.BuildSeed,1,data.BuildSeed);

			 BuildColour = GetIColorSwatch (GetDlgItem(hWnd, IDC_COL1),RGB(data.BuildCol.r>>8,data.BuildCol.g>>8,
											data.BuildCol.b>>8),_T("Element Colour"));

			 BuildUpdateControls(hWnd);

			 BuildBuildButtons();

			 if (data.BuildActivePreview) BuildPreview(data.DrawChecker);

			 return 1;
           } 

         case WM_COMMAND:
			 switch (LOWORD(wParam)) 
			   { case IDOK:
				 case IDC_BUILD_OK:
					 if( HIWORD(wParam)!=BN_CLICKED )
								{ SetCursor(LoadCursor(NULL,IDC_WAIT));
			                       data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_BUILDCHECKER);
					               FlareBuildDialogBoxExit=1;
								   BuildRenderIntoFlare();

								   BuildFreeButtons();
								   BuildReleaseButtons();

								   SetCursor(LoadCursor(NULL,IDC_ARROW));
								   
				                   EndDialog(hWnd,1);
								}
								   break; 

				 case IDCANCEL:
				 case IDC_BUILD_CANCEL:
					 if( HIWORD(wParam)!=BN_CLICKED )
							{ data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_BUILDCHECKER);
					               FlareBuildDialogBoxExit=0;
								   BuildRenderIntoFlare();

								   BuildFreeButtons();
								   BuildReleaseButtons();

								   EndDialog(hWnd,0);
							}
				                   break;

               }
             return 1;
       }     
     return 0;
}

BOOL CALLBACK EditBuildCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)	
{	 static ImageFilter_LensF *flt = NULL;

 	 if( message == WM_PAINT ) {
		 PAINTSTRUCT ps;
		 BeginPaint(hWnd,&ps);
		 EndPaint(hWnd,&ps);
	 }

	 if( message == WM_ERASEBKGND ) {
		 HDC hdc=(HDC)wParam;
		 RECT	rect;
		 GetClientRect(hWnd,&rect);
		 HDC hdcBitmap=CreateCompatibleDC(hdc);
		 SelectObject(hdcBitmap,BuildBackBitmap);
		 BitBlt(hdc,0,0,rect.right,rect.bottom,hdcBitmap,0,0,SRCCOPY);
		 DeleteDC(hdcBitmap);
		 return TRUE;
	}
	 
	 if (message == WM_INITDIALOG) flt = (ImageFilter_LensF *)lParam;
     if (flt) return (flt->BuildControl(hWnd,message,wParam,lParam));
     else return(FALSE);
}

BOOL ImageFilter_LensF::BuildShowControl(HWND hWnd, Flare *Edit) 
{	 float PositionX  =Edit->PositionX;
	 float PositionY  =Edit->PositionY;
	 float Position   =Edit->Position ;
	 float Angle      =Edit->Angle;
	 float FlareSize  =Edit->FlareSize;
	 float AngleOffSet=Edit->AngleOffSet;
	 float Colourise  =Edit->Colourise;
	 float Depth	  =Edit->Depth;
	 //float Intensity  =Edit->Intensity;

	 Flare_BuildPreview.GetFlare(Edit,1); 

	 Flare_BuildPreview.FlareSize  =0.5f;
	 Flare_BuildPreview.PositionX  =0.0f;
	 Flare_BuildPreview.PositionY  =0.0f;
	 Flare_BuildPreview.Position   =0.0f;
	 Flare_BuildPreview.Angle      =0.0f;
	 Flare_BuildPreview.AngleOffSet=0.0f;
	 Flare_BuildPreview.Colourise  =0.0f;
	 //Flare_BuildPreview.Light      =1;
	 Flare_BuildPreview.Active	   =1;
	 Flare_BuildPreview.Depth	   =0.0f;
	 //Flare_BuildPreview.Intensity  =1.0f;

	 data.BuildColour=Edit->BuildColour;
	 data.BuildShape=Edit->BuildShape;
	 data.BuildWidth=Edit->BuildWidth;
	 data.BuildTransparency=Edit->BuildTransparency;
	 data.BuildNoStreaks=Edit->BuildNoStreaks;
	 data.BuildNoSides=Edit->BuildNoSides;
	 data.BuildFactor=Edit->BuildFactor;
	 data.BuildAspect=Edit->BuildAspect;
	 data.BuildSeed=Edit->BuildSeed;
	 data.BuildCol.r=(unsigned short)(Edit->BuildCol.r*65535.0f);
	 data.BuildCol.g=(unsigned short)(Edit->BuildCol.g*65535.0f);
	 data.BuildCol.b=(unsigned short)(Edit->BuildCol.b*65535.0f);
	 
	 FlareBuildTempBitmap=NULL;

	 DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_BUILD),hWnd,(DLGPROC)EditBuildCtrlDlgProc,(LPARAM)this);
	 DeleteObject(BuildBackBitmap);

	 if (FlareBuildDialogBoxExit) { Edit->GetFlare(&Flare_BuildPreview,1); 
								    Edit->PositionX=PositionX;
								    Edit->PositionY=PositionY;
								    Edit->Position=Position;
								    Edit->Angle=Angle;
								    Edit->FlareSize=FlareSize;
								    Edit->AngleOffSet=AngleOffSet;
								    Edit->Colourise=Colourise;
									Edit->Depth=Depth;
									Edit->IR2=Edit->Intensity*Edit->FlareSize*Edit->FlareSize;

									Edit->BuildColour=data.BuildColour;
									Edit->BuildShape=data.BuildShape;
									Edit->BuildWidth=data.BuildWidth;
									Edit->BuildTransparency=data.BuildTransparency;
									Edit->BuildNoStreaks=data.BuildNoStreaks;
									Edit->BuildNoSides=data.BuildNoSides;
									Edit->BuildFactor=data.BuildFactor;
									Edit->BuildAspect=data.BuildAspect;
									Edit->BuildSeed=data.BuildSeed;
									Edit->BuildCol.r=data.BuildCol.r/65535.0f;
									Edit->BuildCol.g=data.BuildCol.g/65535.0f;
									Edit->BuildCol.b=data.BuildCol.b/65535.0f;
								  }


	 if (FlareBuildTempBitmap) free(FlareBuildTempBitmap);

	 return FlareBuildDialogBoxExit;
}