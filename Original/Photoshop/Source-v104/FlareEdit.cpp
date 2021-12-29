// #####################################################################
// Changed 24.03.97
//		Changes In Brush Handling (uughh), store oldbrush for w95 friendliness
//		Many Changes To Real Time Preview

//*********************************************************************************
//*********************************************************************************
// CODE FOR FLARE EDITOR
//*********************************************************************************
//*********************************************************************************

// Customn Control Variables
static ISpinnerControl	*IntensitySpin		= NULL;
static ISpinnerControl	*NIntensitySpin		= NULL;
static ISpinnerControl	*LocationSpin		= NULL;
static ISpinnerControl	*NLocationSpin		= NULL;
static ISpinnerControl	*FalloffSpin		= NULL;
static ISpinnerControl	*NFalloffSpin		= NULL;
static ISpinnerControl	*FalloffSpin2		= NULL;
static ISpinnerControl	*NFalloffSpin2		= NULL;
static ISpinnerControl	*MinLimitsSpin		= NULL;
static ISpinnerControl	*MaxLimitsSpin		= NULL;
static ISpinnerControl	*SeedSpin			= NULL;
static ISpinnerControl	*RepetitionsSpin	= NULL;
static ICustButton		*Button1			= NULL;
static ICustButton		*Button2			= NULL;
static ICustButton		*Button3			= NULL;
static ICustButton		*Button4			= NULL;
static ICustButton		*Button5			= NULL;
static ICustButton		*Button6			= NULL;
static ICustButton		*ResetButton		= NULL;
static ICustButton		*SwapButton			= NULL;
static ICustButton		*NormaliseButton	= NULL;
static ICustButton		*FalloffLockButton	= NULL;
static ICustButton		*BuildButton		= NULL;
static ICustButton		*UndoButton			= NULL;
static ICustButton		*ResetCurveButton	= NULL;
static ICustButton		*EditOK				= NULL;
static ICustButton		*EditCancel			= NULL;

static IColorSwatch		*ColorSwatch1		= NULL;
static IColorSwatch		*ColorSwatch2		= NULL;
static ISpinnerControl	*Value1Spin			= NULL;
static ISpinnerControl	*Value2Spin			= NULL;
COLORREF colorref;

char			*FlareRealTempBitmap;
BITMAPINFO		FlareRealTempBitmapInfo;

char			*FlareEditTempBitmap;
BITMAPINFO		FlareEditTempBitmapInfo;

char			*FlareCurveTempBitmap;
BITMAPINFO		FlareCurveTempBitmapInfo;

//*********************************************************************************
// Flare used for local preivew
#define UNDOLEVELS		16
Flare					*Flare_Undo;

//*********************************************************************************
// Pointers TO Draw Windows
HWND			hwPreview;
HWND			hwRealPreview;
HWND			hwPreviewCurve;

//*********************************************************************************
// Real Time Preview Flag
int				EditRealTimePreviewFlag;

//*********************************************************************************
// BlackOut Flag
int				EditBlackOutFlag;

//*********************************************************************************
// Two Radial/Angular Previews
HWND			hwPreview_RA1;
HWND			hwPreview_RA2;
HWND			hwPreview_Lock;

char			*hwPreviewRA1Mem;
char			*hwPreviewRA2Mem;

BITMAPINFO		hwPreviewRA1Info;
BITMAPINFO		hwPreviewRA2Info;

//*********************************************************************************
// Lotsa Elements Preview Stuff
void ImageFilter_LensF::MultiplePreview(HWND &Window,char **Mem,BITMAPINFO &MemINFO,Flare &Fl,int Boxs)
{		HDC		hdc = GetDC(Window);
		RECT	rect; GetClientRect(Window,&rect);
		int lxres=(rect.right-rect.left)&0xfffffff0;
		int lyres=(rect.bottom-rect.top); 

		if (*Mem==NULL)	{ MemINFO.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
						  MemINFO.bmiHeader.biPlanes		=1;
						  MemINFO.bmiHeader.biBitCount		=24;
						  MemINFO.bmiHeader.biCompression	=BI_RGB;
						  MemINFO.bmiHeader.biClrUsed		=0;
						  MemINFO.bmiHeader.biClrImportant	=0;
						  MemINFO.bmiHeader.biWidth			=lxres;
						  MemINFO.bmiHeader.biHeight		=lyres;
						  MemINFO.bmiHeader.biSizeImage		=lxres*lyres*3;
						  *Mem=(char *)malloc(lxres*lyres*3);
						}

		// Get Six Little Previews
		int llxres=lxres/6;

		char *Preview1=(char *)malloc(3*llxres*lyres);
		char *Preview2=(char *)malloc(3*llxres*lyres);
		char *Preview3=(char *)malloc(3*llxres*lyres);
		char *Preview4=(char *)malloc(3*llxres*lyres);
		char *Preview5=(char *)malloc(3*llxres*lyres);
		char *Preview6=(char *)malloc(3*llxres*lyres);

		Flare Preview;

		Preview.GetFlare(&Fl,1); Preview.Light=1;
		Preview.DefaultRadialRGB();
		Preview.RenderPreview(Preview1,llxres,lyres,0,1.0f,1.0f);

		Preview.GetFlare(&Fl,1); Preview.Light=1;
		Preview.DefaultRadialA();
		Preview.RenderPreview(Preview2,llxres,lyres,0,1.0f,1.0f);

		Preview.GetFlare(&Fl,1); Preview.Light=1;
		Preview.DefaultAngularRGB();
		Preview.RenderPreview(Preview3,llxres,lyres,0,1.0f,1.0f);
		
		Preview.GetFlare(&Fl,1); Preview.Light=1;
		Preview.DefaultAngularA();
		Preview.RenderPreview(Preview4,llxres,lyres,0,1.0f,1.0f);
		
		Preview.GetFlare(&Fl,1); Preview.Light=1;
		Preview.DefaultShape2();
		Preview.RenderPreview(Preview5,llxres,lyres,0,1.0f,1.0f);

		Preview.GetFlare(&Fl,1); Preview.Light=1;
		Preview.DefaultShape();
		Preview.RenderPreview(Preview6,llxres,lyres,0,1.0f,1.0f);

		if (Boxs)
			{	//****************
				// Lock Buttons
				HDC		hdc2=GetDC(hwPreview_Lock);
				RECT	rect2; 
				GetClientRect(hwPreview_Lock,&rect2);
				int lckres=(rect2.bottom-rect2.top); 

				HBRUSH brush=CreateSolidBrush(RGB(0,0,(data.RadialLock?255:100)));
		        HBRUSH oldbrush=(HBRUSH)SelectObject(hdc2,brush);
                Rectangle(hdc2,0,0,llxres*2,lckres);
				SelectObject(hdc2,oldbrush);
		        DeleteObject(brush);

				brush=CreateSolidBrush(RGB(0,0,(data.AngularLock?255:100)));
		        oldbrush=(HBRUSH)SelectObject(hdc2,brush);
                Rectangle(hdc2,llxres*2,0,llxres*4,lckres);
				SelectObject(hdc2,oldbrush);
		        DeleteObject(brush);
				ReleaseDC(hwPreview_Lock,hdc2);
			
				//*****************
				// Draw Box
				int P1C=(data.EditPage==0)?255:100;
				int P2C=(data.EditPage==1)?255:100;
				int P3C=(data.EditPage==2)?255:100;
				int P4C=(data.EditPage==3)?255:100;
				int P5C=(data.EditPage==5)?255:100;
				int P6C=(data.EditPage==4)?255:100;

				for(int i=0;i!=3;i++) 
				for(int j=0;j!=llxres;j++)
					{ int PosT=(i*llxres+j)*3,PosB=((lyres-1-i)*llxres+j)*3;
					  Preview1[PosT]=P1C;  Preview1[PosB]=P1C; 
					  Preview2[PosT]=P2C;  Preview2[PosB]=P2C; 
					  Preview3[PosT]=P3C;  Preview3[PosB]=P3C; 
					  Preview4[PosT]=P4C;  Preview4[PosB]=P4C; 
					  Preview5[PosT]=P5C;  Preview5[PosB]=P5C; 
					  Preview6[PosT]=P6C;  Preview6[PosB]=P6C; 
					}

				for(i=3;i!=lyres-3;i++)
				for(int j=0;j!=3;j++)
						{ int PosT=(i*llxres+j)*3;
						  int PosB=(i*llxres+(llxres-1-j))*3;

						  Preview1[PosT]=P1C;  Preview1[PosB]=P1C; 
						  Preview2[PosT]=P2C;  Preview2[PosB]=P2C; 
						  Preview3[PosT]=P3C;  Preview3[PosB]=P3C; 
						  Preview4[PosT]=P4C;  Preview4[PosB]=P4C; 
						  Preview5[PosT]=P5C;  Preview5[PosB]=P5C; 
						  Preview6[PosT]=P6C;  Preview6[PosB]=P6C; 
						}

				//*****************
			}

		for(int y=0;y!=lyres;y++)
				{ char *mem =&(*Mem)[y*lxres*3]; //yikes !
				  char *mem2=&Preview1[y*llxres*3];

		          for(int x=0;x!=llxres;x++)
						{ *(mem++)=*(mem2++); *(mem++)=*(mem2++); *(mem++)=*(mem2++); }

				  mem2=&Preview2[y*llxres*3];

		          for(x=0;x!=llxres;x++)
						{ *(mem++)=*(mem2++); *(mem++)=*(mem2++); *(mem++)=*(mem2++); }

				  mem2=&Preview3[y*llxres*3];

		          for(x=0;x!=llxres;x++)
						{ *(mem++)=*(mem2++); *(mem++)=*(mem2++); *(mem++)=*(mem2++); }

				  mem2=&Preview4[y*llxres*3];

		          for(x=0;x!=llxres;x++)
						{ *(mem++)=*(mem2++); *(mem++)=*(mem2++); *(mem++)=*(mem2++); }

				  mem2=&Preview5[y*llxres*3];

		          for(x=0;x!=llxres;x++)
						{ *(mem++)=*(mem2++); *(mem++)=*(mem2++); *(mem++)=*(mem2++); }

				  mem2=&Preview6[y*llxres*3];

		          for(x=0;x!=llxres;x++)
						{ *(mem++)=*(mem2++); *(mem++)=*(mem2++); *(mem++)=*(mem2++); }

				  for(x=llxres*6;x!=lxres;x++) { *(mem++)=0; *(mem++)=0; *(mem++)=0; }
				}

		SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,*Mem,&MemINFO,DIB_RGB_COLORS);

		// Free 6 little Previews
		free(Preview1);
		free(Preview2);
		free(Preview3);
		free(Preview4);
		free(Preview5);
		free(Preview6);

		ReleaseDC(Window,hdc);
}

//*********************************************************************************
// CALLBACK FOR REAL PREVIEW WINDOW
BOOL CALLBACK EditLockProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)  { case WM_PAINT:		{	//****************
												// Lock Buttons
												RECT	rect; 
												GetClientRect(hwPreview_RA2,&rect);
												int lxres=(rect.right-rect.left)&0xfffffff0;
												int llxres=lxres/6;

												HDC		hdc2=GetDC(hwPreview_Lock);
												RECT	rect2; 
												GetClientRect(hwPreview_Lock,&rect2);
												int lckres=(rect2.bottom-rect2.top); 

												HBRUSH brush=CreateSolidBrush(RGB(0,0,(ImageFilterData->data.RadialLock?255:100)));
												HBRUSH oldbrush=(HBRUSH)SelectObject(hdc2,brush);	// 24.03.97
												Rectangle(hdc2,0,0,llxres*2,lckres);
												SelectObject(hdc2,oldbrush);						// 24.03.97
												DeleteObject(brush);

												brush=CreateSolidBrush(RGB(0,0,(ImageFilterData->data.AngularLock?255:100)));
												oldbrush=(HBRUSH)SelectObject(hdc2,brush);
												Rectangle(hdc2,llxres*2,0,llxres*4,lckres);
												SelectObject(hdc2,oldbrush);						// 24.03.97
												DeleteObject(brush);
												ReleaseDC(hwPreview_Lock,hdc2);
												break;
											}
					  case WM_LBUTTONDOWN:	{	//****************
												// Lock Buttons
												RECT	rect; 
												GetClientRect(hwPreview_RA2,&rect);
												int lxres=(rect.right-rect.left)&0xfffffff0;
												int llxres=lxres/6;

												int PosX = (short)LOWORD(lParam); 
												int PosY = (short)HIWORD(lParam); 

												if (PosX<llxres*2) { ImageFilterData->data.RadialLock=(ImageFilterData->data.RadialLock==0);
												                     if      (ImageFilterData->data.EditPage==0) ImageFilterData->data.Link=(ImageFilterData->data.RadialLock)?&ImageFilterData->data.RadialAlpha:NULL;
																	 else if (ImageFilterData->data.EditPage==1) ImageFilterData->data.Link=(ImageFilterData->data.RadialLock)?&ImageFilterData->data.RadialRGB:NULL;
												                   }
												else			   { ImageFilterData->data.AngularLock=(ImageFilterData->data.AngularLock==0);
												                     if      (ImageFilterData->data.EditPage==2) ImageFilterData->data.Link=(ImageFilterData->data.AngularLock)?&ImageFilterData->data.AngularAlpha:NULL;
																	 else if (ImageFilterData->data.EditPage==3) ImageFilterData->data.Link=(ImageFilterData->data.AngularLock)?&ImageFilterData->data.AngularRGB:NULL;
												                   }
												
												HDC		hdc2=GetDC(hwPreview_Lock);
												RECT	rect2; 
												GetClientRect(hwPreview_Lock,&rect2);
												int lckres=(rect2.bottom-rect2.top); 

												HBRUSH brush=CreateSolidBrush(RGB(0,0,(ImageFilterData->data.RadialLock?255:100)));
												HBRUSH oldbrush=(HBRUSH)SelectObject(hdc2,brush);	// 24.03.97
												Rectangle(hdc2,0,0,llxres*2,lckres);
												SelectObject(hdc2,oldbrush);				// 24.03.97
												DeleteObject(brush);

												brush=CreateSolidBrush(RGB(0,0,(ImageFilterData->data.AngularLock?255:100)));
												oldbrush=(HBRUSH)SelectObject(hdc2,brush);	// 24.03.97
												Rectangle(hdc2,llxres*2,0,llxres*4,lckres);
												SelectObject(hdc2,oldbrush);				// 24.03.97
												DeleteObject(brush);
												ReleaseDC(hwPreview_Lock,hdc2);
												break;
											}
					}
	  return DefWindowProc(hwnd,msg,wParam,lParam);
	}

BOOL CALLBACK RA1PreviewProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)    { case WM_RBUTTONDOWN:{ ImageFilterData->RealPreview(ImageFilterData->data.DrawChecker);
	                                          break;
											}
						case WM_PAINT:		{ HDC		hdc = GetDC(hwPreview_RA1);
											  RECT		rect;
											  GetClientRect(hwPreview_RA1,&rect);

											  int lxres=(rect.right-rect.left)&0xfffffff0;
											  int lyres=rect.bottom-rect.top;

											  if (hwPreviewRA1Mem!=NULL) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewRA1Mem,
											                                                &hwPreviewRA1Info,DIB_RGB_COLORS);
											  else		{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
											              HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);		// 24.03.97
											              Rectangle(hdc,rect.left,rect.top,rect.left+(rect.right-rect.left)&0xfffffff0,rect.bottom);
														  SelectObject(hdc,oldbrush);
											              DeleteObject(brush);
											            }

											  ReleaseDC(hwPreview_RA1,hdc);
											  break;
											}
    	}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 

BOOL CALLBACK RA2PreviewProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)    { case WM_LBUTTONDOWN:{	
												RECT	rect; GetClientRect(hwPreview_RA2,&rect);
												int lxres=(rect.right-rect.left)&0xfffffff0;
												int lyres=(rect.bottom-rect.top); 

												// Get Six Little Previews
												int llxres=lxres/6;

												int PosX = (short)LOWORD(lParam); 
												int PosY = (short)HIWORD(lParam); 
												
												PosX/=llxres; if (PosX>5) llxres=5;
												ImageFilterData->SetEditPage(hwnd,PosX);
												break;
	                                        }
					    case WM_RBUTTONDOWN:{	ImageFilterData->Preview(ImageFilterData->data.DrawChecker);
												break;
											}
						case WM_PAINT:		{	HDC		hdc = GetDC(hwPreview_RA2);
												RECT		rect;
												GetClientRect(hwPreview_RA2,&rect);

												int lxres=(rect.right-rect.left)&0xfffffff0;
												int lyres=rect.bottom-rect.top;

												if (hwPreviewRA2Mem!=NULL) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewRA2Mem,
																						     &hwPreviewRA2Info,DIB_RGB_COLORS);
												else		{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
												              HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);  // 24.03.97
															  Rectangle(hdc,rect.left,rect.top,rect.left+(rect.right-rect.left)&0xfffffff0,rect.bottom);
															  SelectObject(hdc,oldbrush);						// 24.03.97
															  DeleteObject(brush);
															}

												ReleaseDC(hwPreview_RA2,hdc);
												break;
											}
    	}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 


//*********************************************************************************
// Renders Current Curve Into Flare
void ImageFilter_LensF::RenderIntoFlare(Flare &In)
{		if ((data.EditPage==0)||(data.EditPage==1))
				{ data.Active->ComputeCurve(&In,0);
                  if (data.Link!=NULL) data.Link->ComputeCurve(&In,0);
				}
		if ((data.EditPage==2)||(data.EditPage==3))
				{ data.Active->ComputeCurve(&In,1);
                  if (data.Link!=NULL) data.Link->ComputeCurve(&In,1);
				}
		if (data.EditPage==4)
				{ data.Active->ComputeCurve(&In,2);
                  if (data.Link!=NULL) data.Link->ComputeCurve(&In,2);
				}
		if (data.EditPage==5)
				{ data.Active->ComputeCurve(&In,3);
                  if (data.Link!=NULL) data.Link->ComputeCurve(&In,3);
				}
}

//*********************************************************************************
// Deal With Preview
void ImageFilter_LensF::Preview(int CheckerBoard)
{	//*********************************************************************************
	// Get Backup of previous flare values
	//
		SetCursor(LoadCursor(NULL,IDC_WAIT));

		Flare_Undo[0].GetFlare(&Flare_Undo[1],1); 
		RenderIntoFlare(Flare_Undo[0]);

		ImageFilterData->MultiplePreview(hwPreview_RA2,&hwPreviewRA2Mem,hwPreviewRA2Info,Flare_Undo[0],1);
	//*********************************************************************************
	// Get Image Info
	//
		HDC		hdc = GetDC(hwPreview);
		RECT	rect; GetClientRect( hwPreview, &rect );
		int lxres=(rect.right-rect.left)&0xfffffff0;
		int lyres=(rect.bottom-rect.top); 

		if (FlareEditTempBitmap==NULL)
				{ FlareEditTempBitmapInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
				  FlareEditTempBitmapInfo.bmiHeader.biPlanes		=1;
				  FlareEditTempBitmapInfo.bmiHeader.biBitCount		=24;
				  FlareEditTempBitmapInfo.bmiHeader.biCompression	=BI_RGB;
				  FlareEditTempBitmapInfo.bmiHeader.biClrUsed		=0;
				  FlareEditTempBitmapInfo.bmiHeader.biClrImportant	=0;
				  FlareEditTempBitmapInfo.bmiHeader.biWidth			=lxres;
				  FlareEditTempBitmapInfo.bmiHeader.biHeight		=lyres;
				  FlareEditTempBitmapInfo.bmiHeader.biSizeImage		=lxres*lyres*3;
				  FlareEditTempBitmap=(char *)malloc(lxres*lyres*3);
				}
	
		
		Flare_Undo[0].RenderPreview(FlareEditTempBitmap,lxres,lyres,CheckerBoard,1.0f,1.0f);

		SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareEditTempBitmap,
			              &FlareEditTempBitmapInfo,DIB_RGB_COLORS);
		
		ReleaseDC(hwPreview,hdc);

		EditRealTimePreviewFlag=0;
		EditBlackOutFlag=1;

		SetCursor(LoadCursor(NULL,IDC_ARROW));
}

//*********************************************************************************
// REAL PREVIEW : WHAT IS CURRENT STATE OF THE ART !!!
void ImageFilter_LensF::RealPreview(int CheckerBoard)
{	//*********************************************************************************
	// Get Image Info
	//
		ImageFilterData->MultiplePreview(hwPreview_RA1,&hwPreviewRA1Mem,hwPreviewRA1Info,Flare_Undo[1],0);

		SetCursor(LoadCursor(NULL,IDC_WAIT));

		HDC		hdc = GetDC(hwRealPreview);
		RECT	rect; GetClientRect(hwRealPreview, &rect);
		int lxres=(rect.right-rect.left)&0xfffffff0;
		int lyres=(rect.bottom-rect.top); 

		if (FlareRealTempBitmap==NULL)
				{ FlareRealTempBitmapInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
				  FlareRealTempBitmapInfo.bmiHeader.biPlanes		=1;
				  FlareRealTempBitmapInfo.bmiHeader.biBitCount		=24;
				  FlareRealTempBitmapInfo.bmiHeader.biCompression	=BI_RGB;
				  FlareRealTempBitmapInfo.bmiHeader.biClrUsed		=0;
				  FlareRealTempBitmapInfo.bmiHeader.biClrImportant	=0;
				  FlareRealTempBitmapInfo.bmiHeader.biWidth			=lxres;
				  FlareRealTempBitmapInfo.bmiHeader.biHeight		=lyres;
				  FlareRealTempBitmapInfo.bmiHeader.biSizeImage		=lxres*lyres*3;
				  FlareRealTempBitmap=(char *)malloc(lxres*lyres*3);
				}
	
		Flare_Undo[1].RenderPreview(FlareRealTempBitmap,lxres,lyres,CheckerBoard,1.0f,1.0f);

		SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareRealTempBitmap,&FlareRealTempBitmapInfo,DIB_RGB_COLORS);
		
		ReleaseDC(hwRealPreview,hdc);

		SetCursor(LoadCursor(NULL,IDC_ARROW));
}

//*********************************************************************************
// REAL TIME PREVIEW
#define NoCirclePoints	32
float CircleX[NoCirclePoints];
float CircleY[NoCirclePoints];

void ImageFilter_LensF::EditRealTimePreview(void)
{		HDC		hdc = GetDC(hwPreview);
		RECT	rect; GetClientRect(hwPreview,&rect);
		int		lxres=(rect.right-rect.left)&0xfffffff0,cx=lxres/2;
		int		lyres=(rect.bottom-rect.top),cy=lyres/2; 

		if ((!EditRealTimePreviewFlag)&&(EditBlackOutFlag)&&(FlareEditTempBitmap!=NULL))
						{ // Black Out Bitmap
						  unsigned char *tmp=(unsigned char *)FlareEditTempBitmap;
						  for(int y=0;y!=lyres;y++)
						  for(int x=0;x!=lxres;x++)
								{ *(tmp++)=(*(tmp))/4; *(tmp++)=(*(tmp))/4; *(tmp++)=(*(tmp))/4;
								}

						  SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareEditTempBitmap,&FlareEditTempBitmapInfo,DIB_RGB_COLORS);

						  EditBlackOutFlag=0;
						}
		
		float	radius=0.5f*(float)sqrt(lxres*lxres+lyres*lyres);

		HRGN	ClipRegion=CreateRectRgn(rect.left,rect.top,rect.left+lxres,rect.top+lyres);
		SelectClipRgn(hdc,ClipRegion);

		HPEN		CentrePen=CreatePen(PS_SOLID,0,RGB(255,255,128));
		HPEN		CentrePen2=CreatePen(PS_SOLID,0,RGB(128,128,64));
		HPEN		oldpen=(HPEN)SelectObject(hdc,CentrePen);							// 24.03.97
		SetROP2		(hdc,R2_XORPEN);

		if ((data.EditPage==0)||(data.EditPage==1)||(data.EditPage==5))
				{ float Radius=0.0f,dR=0.45f*(float)lxres/(float)FLARERESOLUTION;

				  // Draw it
			      for(int i=1;i!=FLARERESOLUTION-1;i++,Radius+=dR)
						{ int im=i-1; 
						  int ip=i+1;

						  if ((data.Active->Curve[i]>data.Active->Curve[im])&&(data.Active->Curve[i]>data.Active->Curve[ip]))
										{ SelectObject	(hdc,CentrePen);

										  MoveToEx(hdc,(int)((float)(cx+Radius*CircleX[NoCirclePoints-1])),
											           (int)((float)(cy+Radius*CircleY[NoCirclePoints-1])),NULL);

										  for(int j=0;j!=NoCirclePoints;j++)
											LineTo(hdc,(int)((float)(cx+Radius*CircleX[j])),(int)((float)(cy+Radius*CircleY[j])));
										}
						  else if ((data.Active->Curve[i]<data.Active->Curve[im])&&(data.Active->Curve[i]<data.Active->Curve[ip]))
										{ SelectObject	(hdc,CentrePen2);
						                  
										  MoveToEx(hdc,(int)((float)(cx+Radius*CircleX[NoCirclePoints-1])),
											           (int)((float)(cy+Radius*CircleY[NoCirclePoints-1])),NULL);

										  for(int j=0;j!=NoCirclePoints;j++)
											LineTo(hdc,(int)((float)(cx+Radius*CircleX[j])),(int)((float)(cy+Radius*CircleY[j])));
										}
						}
				}
		else	{ for(int i=0;i!=FLARERESOLUTION;i++)
						{ int im=i-1; if (im<0)					im+=FLARERESOLUTION;
						  int ip=i+1; if (ip>=FLARERESOLUTION)	ip-=FLARERESOLUTION;

						  if ((data.Active->Curve[i]>=data.Active->Curve[im])&&(data.Active->Curve[i]>=data.Active->Curve[ip]))
										{ SelectObject	(hdc,CentrePen);
										  MoveToEx(hdc,(int)cx,(int)cy,NULL);
										  
										  float tmp=6.28318530718f*(float)i/(float)FLARERESOLUTION;
										  LineTo(hdc,cx+(int)((float)(cos(tmp)*radius)),
													 cy+(int)((float)(sin(tmp)*radius)));
										}
						  else if ((data.Active->Curve[i]<=data.Active->Curve[im])&&(data.Active->Curve[i]<=data.Active->Curve[ip]))
										{ SelectObject	(hdc,CentrePen2);
										  MoveToEx(hdc,(int)cx,(int)cy,NULL);
										  
										  float tmp=6.28318530718f*(float)i/(float)FLARERESOLUTION;
										  LineTo(hdc,cx+(int)((float)(cos(tmp)*radius)),
													 cy+(int)((float)(sin(tmp)*radius)));
										}
						}
				}

		EditRealTimePreviewFlag=1;

		SelectObject(hdc,oldpen);
		DeleteObject(CentrePen);
		DeleteObject(CentrePen2);
		ReleaseDC(hwPreview,hdc);
}

//*********************************************************************************
// CALLBACK FOR MAIN PREVIEW WINDOW
BOOL CALLBACK EditPreviewProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)    { case WM_RBUTTONDOWN:{ ImageFilterData->Preview(ImageFilterData->data.DrawChecker);
											  break;
											}
						case WM_PAINT:		{ HDC		hdc = GetDC(hwPreview);
											  RECT		rect;
											  GetClientRect(hwPreview,&rect);

											  int lxres=(rect.right-rect.left)&0xfffffff0;
											  int lyres=rect.bottom-rect.top;

											  if (FlareEditTempBitmap!=NULL) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareEditTempBitmap,
											                                                &FlareEditTempBitmapInfo,DIB_RGB_COLORS);
											  else		{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
											              HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);		// 24.03.97
											              Rectangle(hdc,rect.left,rect.top,rect.left+(rect.right-rect.left)&0xfffffff0,rect.bottom);
														  SelectObject(hdc,brush);								// 24.03.97
											              DeleteObject(brush);
											            }

											  ReleaseDC(hwPreview,hdc);

											  int tmp=EditRealTimePreviewFlag;
											  if (EditRealTimePreviewFlag) ImageFilterData->EditRealTimePreview();
											  EditRealTimePreviewFlag=tmp;

											  break;
											}
    	}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 

//*********************************************************************************
// CALLBACK FOR REAL PREVIEW WINDOW
BOOL CALLBACK RealPreviewProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)    { case WM_RBUTTONDOWN:{ ImageFilterData->RealPreview(ImageFilterData->data.DrawChecker);
											  break;
											}
						case WM_PAINT:		{ HDC		hdc = GetDC(hwRealPreview);
											  RECT		rect;
											  GetClientRect(hwRealPreview,&rect);

											  int lxres=(rect.right-rect.left)&0xfffffff0;
											  int lyres=rect.bottom-rect.top;

											  if (FlareEditTempBitmap!=NULL) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareRealTempBitmap,
											                                                &FlareRealTempBitmapInfo,DIB_RGB_COLORS);
											  else		{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
											              HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);	//24.03.97
											              Rectangle(hdc,rect.left,rect.top,rect.left+(rect.right-rect.left)&0xfffffff0,rect.bottom);
														  SelectObject(hdc,oldbrush);
											              DeleteObject(brush);
											            }

											  ReleaseDC(hwRealPreview,hdc);

											  break;
											}
    	}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 

//*********************************************************************************
// Preview In Curve Window
void ImageFilter_LensF::PreviewCurve(void)
{		HDC		hdc = GetDC(hwPreviewCurve);
		RECT	rect; GetClientRect( hwPreviewCurve, &rect );
		int		lxres=(rect.right-rect.left)&0xfffffff0;
		int		lyres=(rect.bottom-rect.top); 

		if (FlareCurveTempBitmap==NULL)
				{ FlareCurveTempBitmapInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
				  FlareCurveTempBitmapInfo.bmiHeader.biPlanes		=1;
				  FlareCurveTempBitmapInfo.bmiHeader.biBitCount		=24;
				  FlareCurveTempBitmapInfo.bmiHeader.biCompression	=BI_RGB;
				  FlareCurveTempBitmapInfo.bmiHeader.biClrUsed		=0;
				  FlareCurveTempBitmapInfo.bmiHeader.biClrImportant	=0;
				  FlareCurveTempBitmapInfo.bmiHeader.biWidth		=lxres;
				  FlareCurveTempBitmapInfo.bmiHeader.biHeight		=lyres;
				  FlareCurveTempBitmapInfo.bmiHeader.biSizeImage	=lxres*lyres*3;
				  FlareCurveTempBitmap=(char *)malloc(lxres*lyres*3);
				}

		float	XPos,dXPos=(float)((float)FLARERESOLUTION-1.0f)/(float)lxres;
		float	YPos,dYPos=1.0f/(float)lyres;
		int		x,y;
		char	*Bitmap=FlareCurveTempBitmap;

		data.Active->ComputeCurve();

		for(y=lyres-1,YPos=0.0f;y>=0;y--,YPos+=dYPos)
		for(x=lxres-1,XPos=0.0f;x>=0;x--,XPos+=dXPos)
			{ float G=Grid(x,y);
		      int	iPos=(int)XPos;

			  float tmp=data.Active->Curve[iPos];

			  if (tmp>YPos) 
					{ float tmp2=1.0f-tmp;
			          *(Bitmap++)=(char)((data.Active->fCurve[iPos].b*tmp+G*tmp2)*255.0);
			          *(Bitmap++)=(char)((data.Active->fCurve[iPos].g*tmp+G*tmp2)*255.0);
					  *(Bitmap++)=(char)((data.Active->fCurve[iPos].r*tmp+G*tmp2)*255.0);
					}
			  else	{ *(Bitmap++)=(char)(G*255.0f);
			          *(Bitmap++)=(char)(G*255.0f);
					  *(Bitmap++)=(char)(G*255.0f);
					}
			}

		SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareCurveTempBitmap,&FlareCurveTempBitmapInfo,DIB_RGB_COLORS);
		
		ReleaseDC(hwPreviewCurve,hdc);
}

//*********************************************************************************
// CALLBACK FOR CURVE PREVIEW WINDOW
#define SetVariable(A,Val) { ImageFilterData->data.Active->A=Val; if (ImageFilterData->data.Link!=NULL) ImageFilterData->data.Link->A=ImageFilterData->data.Active->A; }

BOOL CALLBACK EditCurvePreviewProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);

	  if (msg==WM_RBUTTONDOWN)				ImageFilterData->Preview(ImageFilterData->data.DrawChecker);
	  else if ((msg==WM_LBUTTONDOWN)||
	          ((msg==WM_MOUSEMOVE)&&(wParam&MK_LBUTTON)))
										    { 
											  RECT		rect;
											  GetClientRect(hwPreviewCurve,&rect);
											  int lxres=rect.right-rect.left;
											  int lyres=rect.bottom-rect.top;

											  if (EditRealTimePreviewFlag) ImageFilterData->EditRealTimePreview();

											  int xps=(short)LOWORD(lParam);
											  int yps=(short)HIWORD(lParam);

											  float xpos = (float)xps/(float)(rect.right-rect.left);
											  float ypos = ImageFilterData->data.Active->MinY+
												           (ImageFilterData->data.Active->MaxY-ImageFilterData->data.Active->MinY)*
												           (1.0f-((float)yps/(float)(rect.bottom-rect.top)));

											  SetVariable(Intensity,ypos); 
											  IntensitySpin->SetValue(ImageFilterData->data.Active->Intensity,NULL);

											  SetVariable(Location,xpos); 
											  LocationSpin->SetValue(ImageFilterData->data.Active->Location,NULL);

											  ImageFilterData->PreviewCurve();
											  ImageFilterData->EditRealTimePreview();

											  if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
											}
	  else if (msg==WM_LBUTTONUP)			{ ReleaseCapture();
	                                          if (ImageFilterData->data.EditActivePreview) ImageFilterData->Preview(ImageFilterData->data.DrawChecker);
											}
	  else if (msg==WM_PAINT)				ImageFilterData->PreviewCurve();

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 

#undef SetVariable

HIMAGELIST		HIL_Exp2;
HIMAGELIST		HIL_Exp;
HIMAGELIST		HIL_Hyp;
HIMAGELIST		HIL_Lin;
HIMAGELIST		HIL_Sin;
HIMAGELIST		HIL_Noise;
HIMAGELIST		HIL_EditOK;
HIMAGELIST		HIL_EditCancel;
HIMAGELIST		HIL_Build;
HIMAGELIST		HIL_Normalise;
HIMAGELIST		HIL_Reset;
HIMAGELIST		HIL_ResetCurve;
HIMAGELIST		HIL_Swap;
HIMAGELIST		HIL_Undo;
HIMAGELIST		HIL_Lock;

void ImageFilter_LensF::EditBuildButtons(void)
{		BuildButton(Button1,HIL_Exp2,IDB_EXP2,IDB_EXP2_PUSHED,50,50);
		BuildButton(Button2,HIL_Exp,IDB_EXP,IDB_EXP_PUSHED,50,50);
		BuildButton(Button3,HIL_Hyp,IDB_HYP,IDB_HYP_PUSHED,50,50);
		BuildButton(Button4,HIL_Lin,IDB_LIN,IDB_LIN_PUSHED,50,50);
		BuildButton(Button5,HIL_Sin,IDB_SIN,IDB_SIN_PUSHED,50,50);
		BuildButton(Button6,HIL_Noise,IDB_NOISE,IDB_NOISE_PUSHED,50,50);
		BuildButton(EditOK,HIL_EditOK,IDB_MASTEROK,IDB_MASTEROK,100,50);
		BuildButton(EditCancel,HIL_EditCancel,IDB_MASTERCANCEL,IDB_MASTERCANCEL,100,50);
		BuildButton(BuildButton,HIL_Build,IDB_BUILD,IDB_BUILD,100,50);
		BuildButton(NormaliseButton,HIL_Normalise,IDB_NORMALISE,IDB_NORMALISE,100,50);
		BuildButton(ResetButton,HIL_Reset,IDB_RESET,IDB_RESET,100,50);
		BuildButton(ResetCurveButton,HIL_ResetCurve,IDB_RESETCURVE,IDB_RESETCURVE,100,50);
		BuildButton(SwapButton,HIL_Swap,IDB_SWAP,IDB_SWAP,100,50);
		BuildButton(UndoButton,HIL_Undo,IDB_UNDO,IDB_UNDO,100,50);
		// cheating on width to centre image
		BuildButton(FalloffLockButton,HIL_Lock,IDB_EDIT_LOCKDOWN,IDB_EDIT_LOCKUP,48,50);
}

void ImageFilter_LensF::EditFreeButtons(void)
{		ReleaseButton(HIL_Exp2);
		ReleaseButton(HIL_Exp);
		ReleaseButton(HIL_Hyp);
		ReleaseButton(HIL_Lin);
		ReleaseButton(HIL_Sin);
		ReleaseButton(HIL_Noise);
		ReleaseButton(HIL_EditOK);
		ReleaseButton(HIL_EditCancel);
		ReleaseButton(HIL_Build);
		ReleaseButton(HIL_Normalise);
		ReleaseButton(HIL_Reset);
		ReleaseButton(HIL_ResetCurve);
		ReleaseButton(HIL_Swap);
		ReleaseButton(HIL_Undo);
		ReleaseButton(HIL_Lock);
}

// release buttons
void ImageFilter_LensF::EditReleaseButtons()
{
			  ReleaseSpinner(IntensitySpin);
			 ReleaseSpinner(NIntensitySpin);
			  ReleaseSpinner(LocationSpin);
			 ReleaseSpinner(NLocationSpin);
			  ReleaseSpinner(FalloffSpin);
			 ReleaseSpinner(NFalloffSpin);
			  ReleaseSpinner(FalloffSpin2);
			 ReleaseSpinner(NFalloffSpin2);
			  ReleaseSpinner(MaxLimitsSpin);
			  ReleaseSpinner(MinLimitsSpin);

			  ReleaseSpinner(Value1Spin);
			  ReleaseSpinner(Value2Spin);

			 ReleaseSpinner(RepetitionsSpin);
			 ReleaseSpinner(SeedSpin);

			 ReleaseICustButton(NormaliseButton);
			 ReleaseICustButton(ResetCurveButton);

			 ReleaseICustButton(Button1);
			 ReleaseICustButton(Button2);
			 ReleaseICustButton(Button3);
			 ReleaseICustButton(Button4);
			 ReleaseICustButton(Button5);
			 ReleaseICustButton(Button6);

			 ReleaseICustButton(EditOK);
			 ReleaseICustButton(EditCancel);

			 ReleaseIColorSwatch(ColorSwatch1);
		     ReleaseIColorSwatch(ColorSwatch2);

			 ReleaseICustButton(SwapButton); 
			 ReleaseICustButton(ResetButton);
			 ReleaseICustButton(FalloffLockButton);
			 ReleaseICustButton(BuildButton); 
			 ReleaseICustButton(UndoButton); 
}

//*********************************************************************************
// Stuff For Controls
#define Controls_Buttons		1
#define Controls_Spinners		2
#define Controls_Modes			4
#define Controls_EditModes		8
#define Controls_All			0xffff

void ImageFilter_LensF::SetControls(HWND hWnd,int Update)
{ if (Update&Controls_Spinners)
        { IntensitySpin->SetValue(data.Active->Intensity,NULL);
		  NIntensitySpin->SetValue(data.Active->NIntensity,NULL);
		  LocationSpin->SetValue(data.Active->Location,NULL);
		  NLocationSpin->SetValue(data.Active->NLocation,NULL);
		  FalloffSpin->SetValue(data.Active->Falloff,NULL);
		  NFalloffSpin->SetValue(data.Active->NFalloff,NULL);
		  FalloffSpin2->SetValue(data.Active->Falloff2,NULL);
		  NFalloffSpin2->SetValue(data.Active->NFalloff2,NULL);
		  MaxLimitsSpin	->SetValue(data.Active->MaxY,NULL);
		  MinLimitsSpin	->SetValue(data.Active->MinY,NULL);
		  RepetitionsSpin->SetValue(data.Active->Repetitions,NULL);

		  SeedSpin->SetValue(data.Active->Seed,NULL);

		  FalloffLockButton->SetCheck((data.Active->FalloffLock)); 
		  
		  CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
		  CheckDlgButton(hWnd,IDC_RTP,data.EditActivePreview);

		  if (data.Active->EditColour)
				{ ColorSwatch1->SetColor(RGB(data.Active->Colour1.r>>8,data.Active->Colour1.g>>8,data.Active->Colour1.b>>8), FALSE);
				  ColorSwatch2->SetColor(RGB(data.Active->Colour2.r>>8,data.Active->Colour2.g>>8,data.Active->Colour2.b>>8), FALSE);
				  ColorSwatch1->Enable();
				  ColorSwatch2->Enable();

				  Value1Spin->SetValue(0,NULL);
				  Value2Spin->SetValue(0,NULL);
				  Value1Spin->Disable();
				  Value2Spin->Disable();
				}
		  else	{ ColorSwatch1->SetColor(0, FALSE);
				  ColorSwatch2->SetColor(0, FALSE);
				  ColorSwatch1->Disable();
				  ColorSwatch2->Disable();

				  Value1Spin->SetValue(data.Active->Value1,NULL);
				  Value2Spin->SetValue(data.Active->Value2,NULL);
				  Value1Spin->Enable();
				  Value2Spin->Enable();
				}
		}

  if (Update&Controls_Buttons)
		{ Button1->SetCheck((data.Active->FunctionCurve==1)); 
          Button2->SetCheck((data.Active->FunctionCurve==2)); 
		  Button3->SetCheck((data.Active->FunctionCurve==3)); 
	      Button4->SetCheck((data.Active->FunctionCurve==4)); 
		  Button5->SetCheck((data.Active->FunctionCurve==5)); 
		  Button6->SetCheck((data.Active->FunctionCurve==6)); 
        }
}

static int FlareEditDialogBoxExit;

#define SetVariable(A,Val) { data.Active->A=Val; if (data.Link!=NULL) data.Link->A=data.Active->A; }

void ImageFilter_LensF::SetEditPage(HWND hWnd,int i)
{		int Update=0;
		int CurvePreview=0;

		switch (i) 
		  { case 0:				{ if (EditRealTimePreviewFlag) { ImageFilterData->EditRealTimePreview(); 
																 EditRealTimePreviewFlag=0; 
															   }
								  data.EditPage=0; data.Active=&data.RadialRGB; 
								  data.Link=(data.RadialLock)?&data.RadialAlpha:NULL;
								  break;
								}

			case 1:				{ if (EditRealTimePreviewFlag) { ImageFilterData->EditRealTimePreview(); 
																 EditRealTimePreviewFlag=0; 
															   }
								  data.EditPage=1; data.Active=&data.RadialAlpha; 
								  data.Link=(data.RadialLock)?&data.RadialRGB:NULL;
								  break;
								}	

			case 2:				{ if (EditRealTimePreviewFlag) { ImageFilterData->EditRealTimePreview(); 
																 EditRealTimePreviewFlag=0; 
															   }
								  data.EditPage=2; data.Active=&data.AngularRGB; 
								  data.Link=(data.AngularLock)?&data.AngularAlpha:NULL;
								  break;
								}

			case 3:				{ if (EditRealTimePreviewFlag) { ImageFilterData->EditRealTimePreview(); 
																 EditRealTimePreviewFlag=0; 
															   }
								  data.EditPage=3; data.Active=&data.AngularAlpha; 
								  data.Link=(data.AngularLock)?&data.AngularRGB:NULL;
								  break;
								}

			case 5:				{ if (EditRealTimePreviewFlag) { ImageFilterData->EditRealTimePreview(); 
																 EditRealTimePreviewFlag=0; 
															   }
								  data.EditPage=4; data.Active=&data.Shape; 
								  data.Link=NULL;
								  break;
								}

			case 4:				{ if (EditRealTimePreviewFlag) { ImageFilterData->EditRealTimePreview(); 
																 EditRealTimePreviewFlag=0; 
															   }
								  data.EditPage=5; data.Active=&data.Shape2; 
								  data.Link=NULL;
								  break;
								}
		 }

		if ((!data.EditActivePreview)&&(EditRealTimePreviewFlag)) ImageFilterData->EditRealTimePreview();

		if (!data.EditActivePreview)	{	Flare_Undo[0].GetFlare(&Flare_Undo[1],1); 
										RenderIntoFlare(Flare_Undo[0]);
										MultiplePreview(hwPreview_RA2,&hwPreviewRA2Mem,hwPreviewRA2Info,Flare_Undo[0],1);
									}
		else	{ Preview(data.DrawChecker); }

		SetControls(hWnd,Controls_All);
		PreviewCurve();
	    if (!data.EditActivePreview) ImageFilterData->EditRealTimePreview();
}


HBITMAP EditLoadBitmap;
HBITMAP EditBackBitmap;

BOOL ImageFilter_LensF::Control(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	int Update=0;
	int CurvePreview=0;
	int RTPreview=1;

	int Finishing=((data.EditActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)));
	int Spinning=((message==CC_SPINNER_CHANGE)||(Finishing));

	if( message==CC_SPINNER_CHANGE || message==WM_COMMAND || message==CC_COLOR_CHANGE )
	switch (LOWORD(wParam)) 
	  { case IDC_Mode1: { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
							{ SetVariable(FunctionCurve,1); Update|=Controls_Buttons; CurvePreview=1; 
	                          if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
							} 
	                      break; 
	                    }

		case IDC_Mode2: { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
							{ SetVariable(FunctionCurve,2); Update|=Controls_Buttons; CurvePreview=1; 
			                  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
							} 
						  break; 
						}

		case IDC_Mode3: { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
							{ SetVariable(FunctionCurve,3); Update|=Controls_Buttons; CurvePreview=1; 
			                  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
							} 
						  break; 
						}

		case IDC_Mode4: { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
							{ SetVariable(FunctionCurve,4); Update|=Controls_Buttons; CurvePreview=1; 
			                  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
							} 
						  break; 
						}

		case IDC_Mode5: { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
							{ SetVariable(FunctionCurve,5); Update|=Controls_Buttons; CurvePreview=1; 
			                  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
							} 
						  break; 
						}

		case IDC_Mode6: { if ( (HIWORD(wParam)==BN_BUTTONDOWN) || (HIWORD(wParam)==BN_BUTTONUP) ) 
							{ SetVariable(FunctionCurve,6); Update|=Controls_Buttons; CurvePreview=1; 
			                  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
							} 
						  break; 
						}

		case IDC_BUILD:	{ if (HIWORD(wParam)==BN_BUTTONUP) 
								{ Flare_Undo[0].GetFlare(&Flare_Undo[1],1); 
			                      RenderIntoFlare(Flare_Undo[0]);

								  for(int i=UNDOLEVELS-1;i>=1;i--)
										Flare_Undo[i].GetFlare(&Flare_Undo[i-1],1); 

								  if (data.EditActivePreview) {	Preview(data.DrawChecker); 
																RealPreview(data.DrawChecker);
																RTPreview=0; 
															}
								}
			              break; 
						}

		case IDC_RESETC:	
			            { if (HIWORD(wParam)==BN_BUTTONUP) 
								{ if ((data.EditPage==0)||((data.EditPage==1)&&(data.Link!=NULL))) Flare_Undo[0].ResetRadialRGB();
								  if ((data.EditPage==1)||((data.EditPage==0)&&(data.Link!=NULL))) Flare_Undo[0].ResetRadialA();

								  if ((data.EditPage==2)||((data.EditPage==3)&&(data.Link!=NULL))) Flare_Undo[0].ResetAngularRGB();
								  if ((data.EditPage==3)||((data.EditPage==2)&&(data.Link!=NULL))) Flare_Undo[0].ResetAngularA();

								  if (data.EditPage==4) Flare_Undo[0].ResetShape();
								  if (data.EditPage==5) Flare_Undo[0].ResetShape2();

								  for(int i=UNDOLEVELS-1;i>=1;i--)
											Flare_Undo[i].GetFlare(&Flare_Undo[i-1],1); 

								  if (data.EditActivePreview) { Preview(data.DrawChecker); 
															  RealPreview(data.DrawChecker);
															  RTPreview=0; 
															}
								}
			              break; 
						}

		case IDC_UNDO:	{ if (HIWORD(wParam)==BN_BUTTONUP) 
								{ for(int i=1;i!=UNDOLEVELS;i++)
										Flare_Undo[i-1].GetFlare(&Flare_Undo[i],1);

								  if (data.EditActivePreview) {	Preview(data.DrawChecker); 
																RealPreview(data.DrawChecker);
																RTPreview=0; 
															}
								}

			              break; 
						}

		case IDC_Repetitions_SPIN:	
		if (Spinning)
									{ SetVariable(Repetitions,RepetitionsSpin->GetIVal()); CurvePreview=1; 
									  if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}
		case IDC_Intensity_SPIN:	
		if (Spinning)
									{ SetVariable(Intensity,IntensitySpin->GetFVal()); CurvePreview=1; 
		                              if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}

		
		case IDC_NIntensity_SPIN:	
		if (Spinning)
									{ SetVariable(NIntensity,NIntensitySpin->GetFVal()); CurvePreview=1; 
		                              if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
									  break; 
									}

		
		case IDC_Location_SPIN:		
		if (Spinning)
									{ SetVariable(Location,LocationSpin->GetFVal()); CurvePreview=1; 
		                              if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}

		case IDC_NLocation_SPIN:	
		if (Spinning)
									{ SetVariable(NLocation,NLocationSpin->GetFVal()); CurvePreview=1; 
		                              if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}

		case IDC_Value1_SPIN:	
		if (Spinning)
									{ SetVariable(Value1,Value1Spin->GetFVal()); CurvePreview=1; 
		                              if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}

		case IDC_Value2_SPIN:	
		if (Spinning)
									{ SetVariable(Value2,Value2Spin->GetFVal()); CurvePreview=1; 
		                              if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}

		case IDC_Falloff_SPIN:		
		if (Spinning)
									{ SetVariable(Falloff,FalloffSpin->GetFVal()); 
			                          if (data.Active->FalloffLock) 
											{ SetVariable(Falloff2,data.Active->Falloff); 
									          FalloffSpin2->SetValue(data.Active->Falloff2,NULL);
											}

			                          CurvePreview=1; 
									  if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
			                          break; 
									}

		case IDC_Falloff_SPIN2:		
		if (Spinning)
									{ SetVariable(Falloff2,FalloffSpin2->GetFVal()); 
			                          if (data.Active->FalloffLock) 
											{ SetVariable(Falloff,data.Active->Falloff2); 
									          FalloffSpin->SetValue(data.Active->Falloff,NULL);
											}

			                          CurvePreview=1; 
									  if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
			                          break; 
									}

		case IDC_NFalloff_SPIN:		
		if (Spinning)
									{ SetVariable(NFalloff,NFalloffSpin->GetFVal()); 
			                          if (data.Active->FalloffLock) 
											{ SetVariable(NFalloff2,data.Active->NFalloff); 
									          NFalloffSpin2->SetValue(data.Active->NFalloff2,NULL);
											}

			                          CurvePreview=1; 
									  if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
			                          break; 
									}

		case IDC_NFalloff_SPIN2:	
		if (Spinning)
									{ SetVariable(NFalloff2,NFalloffSpin->GetFVal()); 
			                          if (data.Active->FalloffLock) 
											{ SetVariable(NFalloff,data.Active->NFalloff2); 
									          NFalloffSpin->SetValue(data.Active->NFalloff,NULL);
											}

			                          CurvePreview=1; 
									  if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
			                          break; 
									}

		case IDC_LMax_SPIN:			
		if (Spinning)
									{ SetVariable(MaxY,MaxLimitsSpin->GetFVal()); CurvePreview=1; 
									  if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}
		case IDC_LMin_SPIN:			
		if (Spinning)
									{ SetVariable(MinY,MinLimitsSpin->GetFVal()); CurvePreview=1; 
									  if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}
		case IDC_Seed_SPIN:			
		if (Spinning)
									{ SetVariable(Seed,SeedSpin->GetIVal()); CurvePreview=1; 
		                              if (Finishing) { Preview(data.DrawChecker); RTPreview=0; }
		                              break; 
									}
	
		case IDC_CHECKER:			{ int tmp=data.DrawChecker;
									  data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_CHECKER); 
			                          
									  if ((data.EditActivePreview)&&(tmp!=data.DrawChecker)) 
																{ Preview(data.DrawChecker); 
																  RealPreview(data.DrawChecker);
																}
			                          break; 
									}

		case IDC_RTP:				{ int tmp=data.EditActivePreview;
			                          data.EditActivePreview=IsDlgButtonChecked(hWnd,IDC_RTP);

			                          if ((data.EditActivePreview)&&(tmp!=data.EditActivePreview)) 
																{ Preview(data.DrawChecker); 
																  RealPreview(data.DrawChecker);
																}
			                          break; 
									}

		case IDC_Colour1:			{ colorref = ColorSwatch1->GetColor();
			                          if (!data.Active->EditColour) { ColorSwatch1->SetColor(0, FALSE); break; }
									  SetVariable(Colour1.r,GetRValue(colorref)<<8|0xFF); 
									  SetVariable(Colour1.g,GetGValue(colorref)<<8|0xFF); 
									  SetVariable(Colour1.b,GetBValue(colorref)<<8|0xFF);
									  CurvePreview=1; 
									  RTPreview=0;

									  if ((data.EditActivePreview)&&(message==CC_COLOR_CHANGE)&&(HIWORD(wParam)==1)) 
												{ Preview(data.DrawChecker); RTPreview=0; }

									  break;
									}

	    case IDC_Colour2: {         { colorref = ColorSwatch2->GetColor();
									  if (!data.Active->EditColour) { ColorSwatch2->SetColor(0, FALSE); break; }
									  SetVariable(Colour2.r,GetRValue(colorref)<<8|0xFF); 
									  SetVariable(Colour2.g,GetGValue(colorref)<<8|0xFF); 
									  SetVariable(Colour2.b,GetBValue(colorref)<<8|0xFF);	
									  CurvePreview=1; 
									  RTPreview=0;

									  if ((data.EditActivePreview)&&(message==CC_COLOR_CHANGE)&&(HIWORD(wParam)==1)) 
												{ Preview(data.DrawChecker); RTPreview=0; }
									}
							break;
						  }

		case IDC_FallOffLock: { if (HIWORD(wParam)==BN_CLICKED)
									{ SetVariable(FalloffLock,FalloffLockButton->IsChecked()); }
								break;
							  }
									
		case IDC_Normalise:	{ if (HIWORD(wParam)==BN_BUTTONUP)
								    { if (EditRealTimePreviewFlag) { ImageFilterData->EditRealTimePreview(); 
						                                             EditRealTimePreviewFlag=0; 
						                                           }
									  int i; 
			                          data.Active->MaxY=1.0f;
									  data.Active->MinY=0.0f;

									  data.Active->ComputeCurve();
			
			                          float Max=data.Active->Curve[0];
									  float Min=data.Active->Curve[0];

			                          for(i=1;i!=FLARERESOLUTION;i++) 
											     if (data.Active->Curve[i]>Max) Max=data.Active->Curve[i];
											else if (data.Active->Curve[i]<Min) Min=data.Active->Curve[i];
									  
									  SetVariable(MaxY,Max);
									  SetVariable(MinY,Min);
							
									  Update|=Controls_Spinners;
			                          CurvePreview=1;

									  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
									}
							  break;
							}

		case IDC_RESET:	{ if (HIWORD(wParam)==BN_BUTTONUP)
									{ data.Active->Reset();
									  if (data.Link!=NULL)	data.Link->Reset();

									  Update=Controls_All; CurvePreview=1;
									  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
									}
			              break;
						}

		case IDC_SWAP:	{ if (HIWORD(wParam)==BN_BUTTONUP)
									{ float tmp=data.Active->MinY; data.Active->MinY=data.Active->MaxY; 
			                          data.Active->MaxY=tmp;

			                          if (data.Link!=NULL)	
											{ tmp=data.Link->MinY; data.Link->MinY=data.Link->MaxY; 
											  data.Link->MaxY=tmp;
											}

									  Update=Controls_All; CurvePreview=1;
									  if (data.EditActivePreview) { Preview(data.DrawChecker); RTPreview=0; }
									}
						  break;
						}
	  };

	SetControls(hWnd,Update);
	if (CurvePreview) 
			{ if ((RTPreview)&&(EditRealTimePreviewFlag)) ImageFilterData->EditRealTimePreview();
			  PreviewCurve();
	          if (RTPreview) ImageFilterData->EditRealTimePreview();
			}
	
	switch (message) 
	   { case WM_INITDIALOG: 
		   { CenterWindow(hWnd,GetParent(hWnd));
             SetCursor(LoadCursor(NULL,IDC_ARROW));

			 // Setup background bitmap rcw 18/4/97
				// load bitmap and get window dc
				EditLoadBitmap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_EDIT));
				HDC hdc=GetDC(hWnd);
				RECT	rect;
				BITMAP	bm;

				// get sizes of bitmap and window
				GetClientRect(hWnd,&rect);
				GetObject(EditLoadBitmap,sizeof(BITMAP),&bm);

				if( abs(rect.right-bm.bmWidth)<=2 && abs(rect.bottom-bm.bmHeight)<=2 )
					// keep original
					EditBackBitmap=EditLoadBitmap;
				else {
					// get the final background bitmap
					EditBackBitmap=CreateCompatibleBitmap(hdc,(rect.right+3)&0xfffffffc,rect.bottom);
					// interpolate new size
					// note this deletes old bitmap internally
					LinearResample(hdc,EditLoadBitmap,bm.bmWidth,bm.bmHeight,hdc,EditBackBitmap,rect.right,rect.bottom);
				}

				// clean up DC
				ReleaseDC(hWnd,hdc);

			 switch(data.EditPage)
					 { case 0: { data.Active=&data.RadialRGB;    
								 data.Link=(data.RadialLock)?&data.RadialAlpha:NULL;
								 break; 
							   }
					   case 1: { data.Active=&data.RadialAlpha;  
								 data.Link=(data.RadialLock)?&data.RadialRGB:NULL;
								 break; 
							   }
					   case 2: { data.Active=&data.AngularRGB;   
								 data.Link=(data.AngularLock)?&data.AngularAlpha:NULL;
								 break; 
							   }
					   case 3: { data.Active=&data.AngularAlpha; 
								 data.Link=(data.AngularLock)?&data.AngularRGB:NULL;
								 break; 
							   }
					   case 4: { data.Active=&data.Shape; 
								 data.Link=NULL;
								 break; 
							   }
					 }

			 hwPreview         = GetDlgItem(hWnd, IDC_PREVIEW);
			 hwRealPreview     = GetDlgItem(hWnd, IDC_REALPREVIEW);
			 hwPreviewCurve    = GetDlgItem(hWnd, IDC_PREVIEW_CURVE);
			 hwPreview_RA1	   = GetDlgItem(hWnd, IDC_PREVIEWRA1);
			 hwPreview_RA2	   = GetDlgItem(hWnd, IDC_PREVIEWRA2);
			 hwPreview_Lock	   = GetDlgItem(hWnd, IDC_PLOCK);

			 SetWindowLong(hwPreview,GWL_WNDPROC,(LONG)EditPreviewProc);
			 SetWindowLong(hwRealPreview,GWL_WNDPROC,(LONG)RealPreviewProc);
			 SetWindowLong(hwPreviewCurve,GWL_WNDPROC,(LONG)EditCurvePreviewProc);
			 SetWindowLong(hwPreview_Lock,GWL_WNDPROC,(LONG)EditLockProc);

		     SetWindowLong(hwPreview_RA1,GWL_WNDPROC,(LONG)RA1PreviewProc);
			 SetWindowLong(hwPreview_RA2,GWL_WNDPROC,(LONG)RA2PreviewProc);

			 CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
			 CheckDlgButton(hWnd,IDC_RTP,data.EditActivePreview);

			  IntensitySpin	=SetupFloatSpinner(hWnd,IDC_Intensity_SPIN,IDC_Intensity_EDIT,-10.0f,10.0f,data.Active->Intensity,0.01f,data.Active->Intensity);
			 NIntensitySpin	=SetupFloatSpinner(hWnd,IDC_NIntensity_SPIN,IDC_NIntensity_EDIT,-10.0f,10.0f,data.Active->NIntensity,0.01f,data.Active->NIntensity);
			  LocationSpin	=SetupFloatSpinner(hWnd,IDC_Location_SPIN,IDC_Location_EDIT,-10.0f,10.0f,data.Active->Location,0.01f,data.Active->Location);
			 NLocationSpin	=SetupFloatSpinner(hWnd,IDC_NLocation_SPIN,IDC_NLocation_EDIT,-10.0f,10.0f,data.Active->NLocation,0.01f,data.Active->NLocation);
			  FalloffSpin	=SetupFloatSpinner(hWnd,IDC_Falloff_SPIN,IDC_Falloff_EDIT,-100.0f,100.0f,data.Active->Falloff,0.01f,data.Active->Falloff);
			 NFalloffSpin	=SetupFloatSpinner(hWnd,IDC_NFalloff_SPIN,IDC_NFalloff_EDIT,-100.0f,100.0f,data.Active->NFalloff,0.01f,data.Active->NFalloff);
			  FalloffSpin2	=SetupFloatSpinner(hWnd,IDC_Falloff_SPIN2,IDC_Falloff_EDIT2,-100.0f,100.0f,data.Active->Falloff2,0.01f,data.Active->Falloff2);
			 NFalloffSpin2	=SetupFloatSpinner(hWnd,IDC_NFalloff_SPIN2,IDC_NFalloff_EDIT2,-100.0f,100.0f,data.Active->NFalloff2,0.01f,data.Active->NFalloff2);
			  MaxLimitsSpin	=SetupFloatSpinner(hWnd,IDC_LMax_SPIN,IDC_LMax_EDIT,-10.0f,10.0f,data.Active->MaxY,0.01f,data.Active->MaxY);
			  MinLimitsSpin	=SetupFloatSpinner(hWnd,IDC_LMin_SPIN,IDC_LMin_EDIT,-10.0f,10.0f,data.Active->MinY,0.01f,data.Active->MinY);

			  Value1Spin	=SetupFloatSpinner(hWnd,IDC_Value1_SPIN,IDC_Value1_EDIT,0.0f,1.0f,data.Active->Value1,0.01f,data.Active->Value1);
			  Value2Spin	=SetupFloatSpinner(hWnd,IDC_Value2_SPIN,IDC_Value2_EDIT,0.0f,1.0f,data.Active->Value2,0.01f,data.Active->Value2);

			 RepetitionsSpin=SetupIntSpinner(hWnd,IDC_Repetitions_SPIN,IDC_Repetitions_EDIT,1,100,data.Active->Repetitions,1,data.Active->Repetitions);
			 SeedSpin		=SetupIntSpinner(hWnd,IDC_Seed_SPIN,IDC_Seed_EDIT,0,9999,data.Active->Seed,1,data.Active->Seed);

			 NormaliseButton=GetICustButton(GetDlgItem(hWnd,IDC_Normalise));
			 ResetCurveButton=GetICustButton(GetDlgItem(hWnd,IDC_RESETC));

			 Button1=GetICustButton(GetDlgItem(hWnd, IDC_Mode1)); Button1->SetType(CBT_CHECK);
			 Button2=GetICustButton(GetDlgItem(hWnd, IDC_Mode2)); Button2->SetType(CBT_CHECK);
			 Button3=GetICustButton(GetDlgItem(hWnd, IDC_Mode3)); Button3->SetType(CBT_CHECK);
			 Button4=GetICustButton(GetDlgItem(hWnd, IDC_Mode4)); Button4->SetType(CBT_CHECK);
			 Button5=GetICustButton(GetDlgItem(hWnd, IDC_Mode5)); Button5->SetType(CBT_CHECK);
			 Button6=GetICustButton(GetDlgItem(hWnd, IDC_Mode6)); Button6->SetType(CBT_CHECK);

			 EditOK=GetICustButton(GetDlgItem(hWnd, IDC_EDIT_OK)); EditOK->SetType(CBT_CHECK);
			 EditCancel=GetICustButton(GetDlgItem(hWnd, IDC_EDIT_CANCEL)); EditCancel->SetType(CBT_CHECK);

			 ColorSwatch1 = GetIColorSwatch(GetDlgItem(hWnd, IDC_Colour1),RGB(data.Active->Colour1.r>>8,data.Active->Colour1.g>>8,
											data.Active->Colour1.b>>8),_T("Colour 1"));

		     ColorSwatch2 = GetIColorSwatch(GetDlgItem(hWnd, IDC_Colour2),RGB(data.Active->Colour2.r>>8,data.Active->Colour2.g>>8,
											data.Active->Colour2.b>>8),_T("Colour 2"));

			 SwapButton	=GetICustButton(GetDlgItem(hWnd,IDC_SWAP)); 
			 ResetButton=GetICustButton(GetDlgItem(hWnd, IDC_RESET));
			 FalloffLockButton=GetICustButton(GetDlgItem(hWnd, IDC_FallOffLock)); FalloffLockButton->SetType(CBT_CHECK);
			 BuildButton=GetICustButton(GetDlgItem(hWnd, IDC_BUILD)); 
			 UndoButton=GetICustButton(GetDlgItem(hWnd, IDC_UNDO)); 

			 SetControls(hWnd,Controls_All);

			 if (!data.EditActivePreview)	{	Flare_Undo[0].GetFlare(&Flare_Undo[1],1); 
											RenderIntoFlare(Flare_Undo[0]);
											MultiplePreview(hwPreview_RA2,&hwPreviewRA2Mem,hwPreviewRA2Info,Flare_Undo[0],1);
									}
			 else	{ Preview(data.DrawChecker); 
			          RealPreview(data.DrawChecker); 
			        }

			 EditBuildButtons();

			 return 1;
           }

         case WM_COMMAND:
			 switch (LOWORD(wParam)) 
				{ case IDOK:
				  case IDC_EDIT_OK:
					  if( HIWORD(wParam)!=BN_CLICKED )
								{ FlareEditDialogBoxExit=1;
								   EditFreeButtons();
								   EditReleaseButtons();
				                   EndDialog(hWnd,1); 
							     }
								   break; 
				  case IDCANCEL:
				  case IDC_EDIT_CANCEL:
					  if( HIWORD(wParam)!=BN_CLICKED )
								{ FlareEditDialogBoxExit=0;
								   EditFreeButtons();
								   EditReleaseButtons();
					               EndDialog(hWnd,0); 
								 }
								   break; 
				}
             return 1;
       }     
     return 0;
}

#undef SetVariable

BOOL CALLBACK EditRadialCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)	
{
     static ImageFilter_LensF *flt = NULL;

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
		 SelectObject(hdcBitmap,EditBackBitmap);
		 BitBlt(hdc,0,0,rect.right,rect.bottom,hdcBitmap,0,0,SRCCOPY);
		 DeleteDC(hdcBitmap);
		 return TRUE;
	}
     
	 if (message == WM_INITDIALOG) flt = (ImageFilter_LensF *)lParam;
     if (flt) return (flt->Control(hWnd,message,wParam,lParam));
     else return(FALSE);
}

BOOL ImageFilter_LensF::EditorShowControl(HWND hWnd, Flare *Edit) 
{	 Flare_Undo=(Flare *)malloc(sizeof(Flare)*UNDOLEVELS);  // ARG !!!! Can't use New Here !!! BUGS !!!
	
	 // Setup Circle Lookup Tables
	 float A=0.0f,dA=6.28318530718f/(float)NoCirclePoints;
	 for(int i=0;i!=NoCirclePoints;i++,A+=dA)
		{ CircleX[i]=(float)cos(A); CircleY[i]=(float)sin(A); }
	
	 // Set Real Time Preview Flag 
	 EditRealTimePreviewFlag=0;
	 
     // Backup Flare Parameters
	 float PositionX  =Edit->PositionX;
	 float PositionY  =Edit->PositionY;
	 float Position   =Edit->Position ;
	 float Angle      =Edit->Angle;
	 float FlareSize  =Edit->FlareSize;
	 float AngleOffSet=Edit->AngleOffSet;
	 float Colourise  =Edit->Colourise;
	 float Intensity  =Edit->Intensity;
	 float Depth      =Edit->Depth;
	  
	 for(i=0;i!=UNDOLEVELS;i++)
			{ Flare_Undo[i].GetFlare(Edit,1); 
			  Flare_Undo[i].FlareSize  =0.5f;
			  Flare_Undo[i].PositionX  =0.0f;
			  Flare_Undo[i].PositionY  =0.0f;
			  Flare_Undo[i].Position   =0.0f;
			  Flare_Undo[i].Angle      =0.0f;
			  Flare_Undo[i].Depth	   =0.0f;
			  Flare_Undo[i].AngleOffSet=0.0f;
			  Flare_Undo[i].Colourise  =0.0f;
			  Flare_Undo[i].Intensity  =1.0f;
			}

	 FlareEditTempBitmap=NULL;
	 FlareCurveTempBitmap=NULL;
	 FlareRealTempBitmap=NULL;
	 hwPreviewRA1Mem=NULL;
     hwPreviewRA2Mem=NULL;
	 DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_EDIT),hWnd,(DLGPROC)EditRadialCtrlDlgProc,(LPARAM)this);
	 DeleteObject(EditBackBitmap);

	 if (FlareEditDialogBoxExit) 
			 { Edit->GetFlare(&Flare_Undo[1],1); 

			   Edit->PositionX=PositionX;
			   Edit->PositionY=PositionY;
			   Edit->Position=Position;
			   Edit->Angle=Angle;
			   Edit->FlareSize=FlareSize;
			   Edit->AngleOffSet=AngleOffSet;
			   Edit->Colourise=Colourise;
			   Edit->Intensity=Intensity;
			   Edit->Depth=Depth;
			 }

	 if (FlareEditTempBitmap) free(FlareEditTempBitmap);
	 if (FlareCurveTempBitmap) free(FlareCurveTempBitmap);
	 if (FlareRealTempBitmap ) free(FlareRealTempBitmap);
	 if (hwPreviewRA1Mem) free(hwPreviewRA1Mem);
	 if (hwPreviewRA2Mem) free(hwPreviewRA2Mem);
	 free(Flare_Undo);
	 
	 return FlareEditDialogBoxExit;
}