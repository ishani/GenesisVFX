// #####################################################################
// Changed 24.03.97
//		Changes In Brush Handling (uughh), store oldbrush for w95 friendliness

static ICustButton		*RenderParameters		= NULL;
static ICustButton		*ElementNewButton		= NULL;
static ICustButton		*ElementDeleteButton	= NULL;
static ICustButton		*ElementMoveUpButton	= NULL;
static ICustButton		*ElementMoveDownButton	= NULL;
static ICustButton		*ElementCopyButton		= NULL;
static ICustButton		*FlareEditButton		= NULL;
static ICustButton		*FlareBuildButton		= NULL;
static ICustButton		*FlareMorphButton		= NULL;
static ICustButton		*ElementLinkButton		= NULL;
static ICustButton		*OpenButton				= NULL;
static ICustButton		*SaveButton				= NULL;
static ICustButton		*OpenElementsButton		= NULL;
static ICustButton		*SaveElementsButton		= NULL;
static ICustButton		*NewSystemButton		= NULL;
static ICustButton		*ShapeKeyButton			= NULL;
static ICustButton		*PositionKeyButton		= NULL;
static ICustButton		*ForwardButton			= NULL;
static ICustButton		*BackwardButton			= NULL;
static ICustButton		*InfernoButton			= NULL;

static ICustButton		*MasterOK				= NULL;
static ICustButton		*MasterCancel			= NULL;
static ICustButton		*MasterHelp				= NULL;

static ISpinnerControl	*DistanceSpin			= NULL;
static ISpinnerControl	*AngleSpin				= NULL;
static ISpinnerControl	*SizeSpin				= NULL;
static ISpinnerControl	*AngleOffSetSpin		= NULL;
static ISpinnerControl	*XPositionSpin			= NULL;
static ISpinnerControl	*YPositionSpin			= NULL;
static ISpinnerControl	*ColouriseSpin			= NULL;
static ISpinnerControl	*IntensSpin				= NULL;
static ISpinnerControl	*DepthSpin				= NULL;
static ISpinnerControl	*OcclusionSpin			= NULL;
static ISpinnerControl	*AnimationSpin			= NULL;

static HWND hwndType			= NULL;
static HWND hwndListBox			= NULL;

// Real Time Preview
int			MasterRealTimeDrawn;
float		FlaresEditX=-1.0f,FlaresEditY=-1.0f;

// Andrew Mega ScrollBar
HWND			hwndScrollBar;

// Memory and Bitmap Info For Preview Backup
HWND			hwPreview_Master;
char			*hwPreviewMasterMem;
BITMAPINFO		hwPreviewMasterInfo;

// Memory and Bitmap Info For Preview Backup
HWND			hwSmallPreview_Master;
char			*hwSmallPreviewMasterMem;
BITMAPINFO		hwSmallPreviewMasterInfo;

// Blackout Flag
int				MasterBlackOutFlag;

// Allow Mouse Evens In Callbacks
int				MasterCallBack;

// detect changes to flare
int				ChangedFlare=FALSE;

// Defines For Real Time Preview
#define Master_DrawSelectedHi		1
#define Master_DrawAll				-1
#define Master_DrawSelectedLo		2

// disable all messages
#define			ID_TIMER	1
int				Disabled=0;
VOID CALLBACK	TimerProc(HWND,UINT,UINT,DWORD);

//******************************************************************************************************************
// Stuff For Loading/Saving
#include "FlaresEdit_File.cpp"

//******************************************************************************************************************
// Small Window Preview
void ImageFilter_LensF::SmallMasterPreview(int DrawChecker)
{				int i=0;
				while(data.SelectedElement[i]==0) i++;

				Flare *tmp=data.LensFlare.GetFlare(i);
				
				HDC		hdc = GetDC(hwSmallPreview_Master);
				RECT	rect;

				GetClientRect(hwSmallPreview_Master,&rect);
				int lxres=(rect.right-rect.left)&0xfffffff0;
				int lyres=rect.bottom-rect.top;

				if (hwSmallPreviewMasterMem==NULL) 
						{ hwSmallPreviewMasterInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
						  hwSmallPreviewMasterInfo.bmiHeader.biPlanes		=1;
						  hwSmallPreviewMasterInfo.bmiHeader.biBitCount		=24;
						  hwSmallPreviewMasterInfo.bmiHeader.biCompression	=BI_RGB;
						  hwSmallPreviewMasterInfo.bmiHeader.biClrUsed		=0;
						  hwSmallPreviewMasterInfo.bmiHeader.biClrImportant	=0;
						  hwSmallPreviewMasterInfo.bmiHeader.biWidth		=lxres;
						  hwSmallPreviewMasterInfo.bmiHeader.biHeight		=lyres;
						  hwSmallPreviewMasterInfo.bmiHeader.biSizeImage	=lxres*lyres*3;
						  hwSmallPreviewMasterMem=(char *)malloc(lxres*lyres*3);
						}

				tmp->RenderPreview(hwSmallPreviewMasterMem,lxres,lyres,DrawChecker,0.6f,(float)data.AnimationPosition,1.0f);

				SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwSmallPreviewMasterMem,
			                      &hwSmallPreviewMasterInfo,DIB_RGB_COLORS);

				ReleaseDC(hwSmallPreview_Master,hdc);
}

//******************************************************************************************************************
// Big Window Preview
void ImageFilter_LensF::MasterPreview(int DrawChecker)
{				SetCursor(LoadCursor(NULL,IDC_WAIT));
				HDC		hdc = GetDC(hwPreview_Master);
				RECT	rect;

				GetClientRect(hwPreview_Master,&rect);
				int lxres=(rect.right-rect.left)&0xfffffff0;
				int lyres=rect.bottom-rect.top;

				int x,y;

				RenderNFO rinfo; rinfo.Init();

				rinfo.xres=lxres; rinfo.yres=lyres;
				rinfo.CameraDepth=data.CameraDepth;
				rinfo.Time=(float)data.AnimationPosition;

				Alloc2D(rinfo.Img_R,uchar,lyres,lxres);
				Alloc2D(rinfo.Img_G,uchar,lyres,lxres);
				Alloc2D(rinfo.Img_B,uchar,lyres,lxres);
				Alloc2D(rinfo.Img_A,uchar,lyres,lxres);

				for(y=0;y!=lyres;y++)
				 for(x=0;x!=lxres;x++)
					{ if (DrawChecker)  { rinfo.Img_R[y][x]=rinfo.Img_G[y][x]=rinfo.Img_B[y][x]=(uchar)(Grid(x,y)*255); 
										  rinfo.Img_A[y][x]=0; 
									    }
					  else rinfo.Img_R[y][x]=rinfo.Img_G[y][x]=rinfo.Img_B[y][x]=rinfo.Img_A[y][x]=0; 
					}

				if ((FlaresEditX==-1.0f)&&(FlaresEditY==-1.0f)) { FlaresEditX=lxres/8.0f; FlaresEditY=lyres/2.0f; }

				rinfo.UseSingle=1;
				rinfo.xpos=FlaresEditX; 
				rinfo.ypos=FlaresEditY;

				StatusText("Preview");
				data.LensFlare.RenderHQArea(rinfo);

				if (hwPreviewMasterMem==NULL) 
						{ hwPreviewMasterInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
						  hwPreviewMasterInfo.bmiHeader.biPlanes		=1;
						  hwPreviewMasterInfo.bmiHeader.biBitCount		=24;
						  hwPreviewMasterInfo.bmiHeader.biCompression	=BI_RGB;
						  hwPreviewMasterInfo.bmiHeader.biClrUsed		=0;
						  hwPreviewMasterInfo.bmiHeader.biClrImportant	=0;
						  hwPreviewMasterInfo.bmiHeader.biWidth			=lxres;
						  hwPreviewMasterInfo.bmiHeader.biHeight		=lyres;
						  hwPreviewMasterInfo.bmiHeader.biSizeImage		=lxres*lyres*3;
						  hwPreviewMasterMem=(char *)malloc(lxres*lyres*3);
						}

				uchar *tmp=(uchar *)hwPreviewMasterMem;
				for(y=1;y<=lyres;y++)
				 for(x=0;x!=lxres;x++)
					{ *(tmp++)=rinfo.Img_B[lyres-y][x];
				      *(tmp++)=rinfo.Img_G[lyres-y][x];
					  *(tmp++)=rinfo.Img_R[lyres-y][x];
					}

				Free2D(rinfo.Img_R,lyres);
				Free2D(rinfo.Img_G,lyres);
				Free2D(rinfo.Img_B,lyres);
				Free2D(rinfo.Img_A,lyres);

				SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewMasterMem,
			                      &hwPreviewMasterInfo,DIB_RGB_COLORS);

				ReleaseDC(hwPreview_Master,hdc);
				MasterRealTimeDrawn=0;
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				MasterBlackOutFlag=1;
}

//****************************************************************************************
// Callback Procedure for BIG preview Window
BOOL CALLBACK MasterPreviewProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
	{ 
	  if( Disabled && msg!=WM_PAINT ) 
		  return DefWindowProc(hwnd,msg,wParam,lParam);

	  int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);

	  if (((msg==WM_LBUTTONDOWN)&&(wParam==MK_LBUTTON+MK_SHIFT))||	// Light Placement
		  ((msg==WM_MOUSEMOVE  )&&(wParam==MK_LBUTTON+MK_SHIFT)))
							{ HDC		hdc = GetDC(hwPreview_Master);
				              RECT		rect; GetClientRect(hwPreview_Master,&rect);

							  int lxres=(rect.right-rect.left)&0xfffffff0;
							  int lyres=rect.bottom-rect.top;
							  ReleaseDC(hwPreview_Master,hdc);

							  if (MasterRealTimeDrawn) ImageFilterData->RealTimePreview(Master_DrawAll);

			                  FlaresEditX = (short)LOWORD(lParam); 
							  FlaresEditY = (short)HIWORD(lParam); 

							  ImageFilterData->RealTimePreview(Master_DrawAll);
							  if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
							  
							  SetCursor(LoadCursor(NULL,IDC_SIZEALL));
							}
	  if (((msg==WM_LBUTTONDOWN)&&(wParam==MK_LBUTTON))||			// Manual Placement
		  ((msg==WM_MOUSEMOVE  )&&(wParam==MK_LBUTTON)))
							{ HDC		hdc = GetDC(hwPreview_Master);
				              RECT		rect; GetClientRect(hwPreview_Master,&rect);

							  int lxres=(rect.right-rect.left)&0xfffffff0;
							  int lyres=rect.bottom-rect.top;
							  if (MasterRealTimeDrawn) ImageFilterData->RealTimePreview(Master_DrawSelectedHi);

			                  float PosnX = (short)LOWORD(lParam); 
							  float PosnY = (short)HIWORD(lParam); 

							  ReleaseDC(hwPreview_Master,hdc);

							  // Convert This To Position
							  if ((FlaresEditX==-1.0f)&&(FlaresEditY==-1.0f)) { FlaresEditX=lxres/8.0f; FlaresEditY=lyres/2.0f; }
							  ImageFilterData->SetPosn(lxres,lyres,FlaresEditX,FlaresEditY,PosnX,PosnY);
							  ChangedFlare=TRUE;

							  ImageFilterData->RealTimePreview(Master_DrawSelectedHi);

							  if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
							}
	  else if (msg==WM_LBUTTONUP) { ReleaseCapture(); SetCursor(LoadCursor(NULL,IDC_ARROW)); }
	  else if (msg==WM_RBUTTONDOWN) ImageFilterData->MasterPreview(ImageFilterData->data.DrawChecker);
	  else if (msg==WM_PAINT)
							{ HDC		hdc = GetDC(hwPreview_Master);
				              RECT		rect;
							  GetClientRect(hwPreview_Master,&rect);

							  int lxres=(rect.right-rect.left)&0xfffffff0;
							  int lyres=rect.bottom-rect.top;

							  if (hwPreviewMasterMem!=NULL) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewMasterMem,&hwPreviewMasterInfo,DIB_RGB_COLORS);
							  else			{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
							                  HBRUSH oldbrush;
							                  oldbrush=(HBRUSH)SelectObject(hdc,brush);									// 24.03.97
											  Rectangle(hdc,rect.left,rect.top,rect.left+lxres,rect.bottom);
											  SelectObject(hdc,oldbrush);											// 24.03.97
							                  DeleteObject(brush);
											}

							  if (MasterRealTimeDrawn) ImageFilterData->RealTimePreview(Master_DrawAll);
							  ReleaseDC(hwPreview_Master,hdc);
							}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 

//****************************************************************************************
// Callback Procedure for SMALL preview Window
BOOL CALLBACK MasterSmallPreviewProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)    {
		case WM_RBUTTONDOWN:{ SetCursor(LoadCursor(NULL,IDC_WAIT));
			                  ImageFilterData->SmallMasterPreview(ImageFilterData->data.DrawChecker);
							  SetCursor(LoadCursor(NULL,IDC_ARROW));
							  break;
							}

		case WM_PAINT:		{ if (hwSmallPreviewMasterMem!=NULL)
									{	HDC hdc = GetDC(hwSmallPreview_Master);
										RECT rect; 
										GetClientRect(hwSmallPreview_Master,&rect);

										int lxres=(rect.right-rect.left)&0xfffffff0;
										int lyres=rect.bottom-rect.top;
										SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwSmallPreviewMasterMem,&hwSmallPreviewMasterInfo,DIB_RGB_COLORS);
										ReleaseDC(hwSmallPreview_Master,hdc);
									}

							  break;
							}
    	}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	}
 
//****************************************************************************************
// MEGA SCROLLBAR
void ImageFilter_LensF::MegaScrollBar(void)
{				HDC		hdc = GetDC(hwndScrollBar);
				RECT	rect; 
				GetClientRect(hwndScrollBar,&rect);

				int lxres=rect.right-rect.left;
				int lyres=rect.bottom-rect.top;

				HRGN	ClipRegion=CreateRectRgn(rect.left,rect.top,rect.left+lxres,rect.top+lyres);
				SelectClipRgn(hdc,ClipRegion);

				// Draw Background
				HBRUSH oldbrush;															// 24.03.97
				HBRUSH brush=CreateSolidBrush(RGB(0,0,100));
				oldbrush=(HBRUSH)SelectObject(hdc,brush);									// 24.03.97
				Rectangle(hdc,rect.left,rect.top,rect.left+lxres,rect.bottom);
				SelectObject(hdc,oldbrush);													// 24.03.97
				DeleteObject(brush);

				// So Markers Fit nicer
				lxres-=6;
				rect.right-=3;
				rect.left+=2;

				// Draw Cursor Position
				float tPos=(float)lxres/(float)data.AnimationMax;

				// Draw Any Selected Region
				if (data.AnimationSelect1!=-1)
						{ brush=CreateSolidBrush(RGB(0,0,200));
				          oldbrush=(HBRUSH)SelectObject(hdc,brush);							// 24.03.97
				          float PosnFrom=tPos*data.AnimationSelect1+rect.left;
				          float PosnTo  =tPos*data.AnimationSelect2+rect.left;
					      Rectangle(hdc,(int)PosnFrom,rect.top,(int)PosnTo,rect.bottom);
						  SelectObject(hdc,oldbrush);										// 24.03.97
						  DeleteObject(brush);
						}
				
				// Draw Position Keys
				HBRUSH PosSel		=CreateSolidBrush(RGB(128,255,128));
				HBRUSH PosUnSel		=CreateSolidBrush(RGB(0,100,50));
				HBRUSH ShpSel		=CreateSolidBrush(RGB(255,128,128));
				HBRUSH ShpUnSel		=CreateSolidBrush(RGB(100,0,50));

				oldbrush=(HBRUSH)SelectObject(hdc,PosUnSel);								// 24.03.97

				for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
				if (!data.SelectedElement[i])
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,PosUnSel);

				            for(int j=0;j!=tmp->NoPosKeys;j++)
									{ float Posn=tPos*(float)tmp->PosKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}
				
				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				if (!data.SelectedElement[i])
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,ShpUnSel);

				            for(int j=0;j!=tmp->NoShpKeys;j++)
									{ float Posn=tPos*(float)tmp->ShpKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}
				
				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				if (data.SelectedElement[i])
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,PosSel);

				            for(int j=0;j!=tmp->NoPosKeys;j++)
									{ float Posn=tPos*(float)tmp->PosKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}

				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				if (data.SelectedElement[i])
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,ShpSel);

				            for(int j=0;j!=tmp->NoShpKeys;j++)
									{ float Posn=tPos*(float)tmp->ShpKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}
				
				SelectObject(hdc,oldbrush);													// 24.03.97
				DeleteObject(PosSel);
				DeleteObject(PosUnSel);
				DeleteObject(ShpSel);
				DeleteObject(ShpUnSel);

				// Draw Cursor Position
				brush=CreateSolidBrush(RGB(192,192,255));
				oldbrush=(HBRUSH)SelectObject(hdc,brush);									// 24.03.97
				float Posn=tPos*(float)data.AnimationPosition+rect.left;
				Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
				SelectObject(hdc,oldbrush);													// 24.03.97
				DeleteObject(brush);

				DeleteObject(ClipRegion);
				ReleaseDC(hwndScrollBar,hdc);
}

BOOL CALLBACK MScrollBarProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
	{
			if( Disabled && msg!=WM_PAINT ) 
				return DefWindowProc(hwnd,msg,wParam,lParam);

			int id = GetWindowLong(hwnd,GWL_ID);
			HWND hwpar = GetParent(hwnd);

			if ((msg==WM_LBUTTONDOWN)||
				((msg==WM_MOUSEMOVE)&&(wParam&MK_LBUTTON)))
								{ // Goto Selected
								  HDC		hdc = GetDC(hwndScrollBar);
								  RECT		rect; 
								  GetClientRect(hwndScrollBar,&rect);

								  int lxres=rect.right-rect.left;

				                  lxres-=6;
								  rect.right-=3;
								  rect.left+=2;

								  int xps=LOWORD(lParam);
								  if (xps>4096) xps=0;  //Wider Than Any Sceen I know !

								  float NewPosn=(float)(xps-rect.left)/lxres;
								  NewPosn*=ImageFilterData->data.AnimationMax;
								  if( NewPosn<0 ) NewPosn=0.0f; 

								  // Erase Previous Preview
								  if (MasterRealTimeDrawn) ImageFilterData->RealTimePreview(Master_DrawAll);

								  // Get New Position
								  ImageFilterData->data.AnimationPosition=(int)(NewPosn+0.5f);

								  ReleaseDC(hwndScrollBar,hdc);

								  // Get New Time Value
								  AnimationSpin->SetValue(ImageFilterData->data.AnimationPosition,NULL);

								  // Get New Positions
								  ImageFilterData->data.LensFlare.GetFrame(ImageFilterData->data.AnimationPosition);

								  // Handle Selection Of Frames
								  if (msg==WM_LBUTTONDOWN) ImageFilterData->data.AnimationSelect1=-1;
								  if ((wParam&MK_SHIFT)&&(ImageFilterData->data.AnimationSelect1==-1))  ImageFilterData->data.AnimationSelect1=ImageFilterData->data.AnimationPosition;
								  if (ImageFilterData->data.AnimationSelect1!=-1) ImageFilterData->data.AnimationSelect2=ImageFilterData->data.AnimationPosition;

								  // Update All Display
								  ImageFilterData->MegaScrollBar();
								  ImageFilterData->SetAnimation();
								  ImageFilterData->MasterSetValues(hwnd);
								  ImageFilterData->RealTimePreview(Master_DrawAll);
								  if (ImageFilterData->data.RealTimePreview) 
										ImageFilterData->SmallMasterPreview(ImageFilterData->data.DrawChecker);
								  if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
								}
			else if (msg==WM_LBUTTONUP)	{ ReleaseCapture();
			                              ImageFilterData->RealTimePreview(Master_DrawAll);
										  
										  //*****************************************
										  // Sort Out Selection Regions
			                              if (ImageFilterData->data.AnimationSelect1!=-1)
													{ if (ImageFilterData->data.AnimationSelect2<ImageFilterData->data.AnimationSelect1) 
																	{ int tmp=ImageFilterData->data.AnimationSelect2;
										                              ImageFilterData->data.AnimationSelect2=ImageFilterData->data.AnimationSelect1;
																	  ImageFilterData->data.AnimationSelect1=tmp;
																	}
													}

										  if (ImageFilterData->data.AnimationSelect1!=-1)
													{ // Find First KeyFrame In Selected Region
											          int i,j;
													  int Min=999999;

													  for(i=0;i!=ImageFilterData->data.LensFlare.GetNoFlares();i++)
													  if (ImageFilterData->data.SelectedElement[i])
																{ Flare *tmp=ImageFilterData->data.LensFlare.GetFlare(i);
																  
																  for(j=0;j!=tmp->NoPosKeys;j++)
																		{ if ((tmp->PosKeys[j].Time>=ImageFilterData->data.AnimationSelect1)&&
																		      (tmp->PosKeys[j].Time<=ImageFilterData->data.AnimationSelect2)&&
																              (tmp->PosKeys[j].Time<Min)) Min=tmp->PosKeys[j].Time;
																		}

																  for(j=0;j!=tmp->NoShpKeys;j++)
																		{ if ((tmp->ShpKeys[j].Time>=ImageFilterData->data.AnimationSelect1)&&
																		      (tmp->ShpKeys[j].Time<=ImageFilterData->data.AnimationSelect2)&&
																              (tmp->ShpKeys[j].Time<Min)) Min=tmp->ShpKeys[j].Time;
																		}
																}
													  
													  if (Min!=999999) ImageFilterData->data.AnimationPosition=Min;
													}

										  //*****************************************

			                              if (ImageFilterData->data.AnimationPosition>ImageFilterData->data.AnimationMax) 
													ImageFilterData->data.AnimationMax=ImageFilterData->data.AnimationPosition;
										  else	{	int MPos=ImageFilterData->data.LensFlare.MaxKeyValue();
													if (MPos>ImageFilterData->data.AnimationPosition) ImageFilterData->data.AnimationMax=MPos;
													else ImageFilterData->data.AnimationMax=ImageFilterData->data.AnimationPosition;

													if (ImageFilterData->data.AnimationMax<100) ImageFilterData->data.AnimationMax=100;
												}

										  //*****************************************
										  // Get New Time Value
										  AnimationSpin->SetValue(ImageFilterData->data.AnimationPosition,NULL);

										  // Get New Positions
										  ImageFilterData->data.LensFlare.GetFrame(ImageFilterData->data.AnimationPosition);

										  // Update All That Screen Stuff !
										  ImageFilterData->MegaScrollBar();
										  ImageFilterData->SetAnimation();
										  ImageFilterData->MasterSetValues(hwnd);
										  ImageFilterData->RealTimePreview(Master_DrawAll);
										  if (ImageFilterData->data.RealTimePreview) 
												ImageFilterData->SmallMasterPreview(ImageFilterData->data.DrawChecker);
										}
			else if (msg==WM_RBUTTONDOWN)
								{ // Find Closest Key Frame 
				                  HDC		hdc = GetDC(hwndScrollBar);
								  RECT		rect; 
								  GetClientRect(hwndScrollBar,&rect);

								  // Erase Selction
								  ImageFilterData->data.AnimationSelect1=-1;

								  int lxres=rect.right-rect.left;

				                  lxres-=6;
								  rect.right-=3;
								  rect.left+=2;

								  float NewPosn=(float)((int)LOWORD(lParam)-rect.left)/lxres;
								  NewPosn*=ImageFilterData->data.AnimationMax;
								  int GetPosn=(int)(NewPosn+0.5f);
								  ReleaseDC(hwndScrollBar,hdc);

								  int NearestPosn=999999;
								  int Distance=999999;

								  for(int i=0;i!=ImageFilterData->data.LensFlare.GetNoFlares();i++)
								  if (ImageFilterData->data.SelectedElement[i])
										{ Flare *Tmp=ImageFilterData->data.LensFlare.GetFlare(i); 
								          
								          for(int j=0;j!=Tmp->NoPosKeys;j++)
												{ int k=Tmp->PosKeys[j].Time-GetPosn;
										          if (k<0) k=-k;

												  if (k<Distance) { Distance=k; 
												                    NearestPosn=Tmp->PosKeys[j].Time;
																  }
												}

										  for(j=0;j!=Tmp->NoShpKeys;j++)
												{ int k=Tmp->ShpKeys[j].Time-GetPosn;
										          if (k<0) k=-k;

												  if (k<Distance) { Distance=k; 
												                    NearestPosn=Tmp->ShpKeys[j].Time;
																  }
												}
										}

								  // Set New Position
								  ImageFilterData->data.AnimationPosition=NearestPosn;

								  if (ImageFilterData->data.AnimationPosition>ImageFilterData->data.AnimationMax) 
											ImageFilterData->data.AnimationMax=ImageFilterData->data.AnimationPosition;
								  else {	int MPos=ImageFilterData->data.LensFlare.MaxKeyValue();
											if (MPos>ImageFilterData->data.AnimationPosition) ImageFilterData->data.AnimationMax=MPos;
											else ImageFilterData->data.AnimationMax=ImageFilterData->data.AnimationPosition;

											if (ImageFilterData->data.AnimationMax<100) ImageFilterData->data.AnimationMax=100;
									   }

								  if (MasterRealTimeDrawn) ImageFilterData->RealTimePreview(Master_DrawAll);

								  // Get New Time Value
								  AnimationSpin->SetValue(ImageFilterData->data.AnimationPosition,NULL);

								  // Get New Positions
								  ImageFilterData->data.LensFlare.GetFrame(ImageFilterData->data.AnimationPosition);

								  // Update All Display
								  ImageFilterData->MegaScrollBar();
								  ImageFilterData->SetAnimation();
								  ImageFilterData->MasterSetValues(hwnd);
								  ImageFilterData->RealTimePreview(Master_DrawAll);
								  if (ImageFilterData->data.RealTimePreview) 
										ImageFilterData->SmallMasterPreview(ImageFilterData->data.DrawChecker);

								  if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
								}
			else if (msg==WM_LBUTTONUP)	ReleaseCapture();
			else if (msg==WM_PAINT)	ImageFilterData->MegaScrollBar();

			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
//****************************************************************************************

#define RealTimePreview_Add			16
#define RealTimePreview_AngleAdd	(FLARERESOLUTION/RealTimePreview_Add)

//**********************************************************************************************************
// REAL TIME PREVIEW STUFF
void ImageFilter_LensF::RealTimePreview(int WhatToDo)
{				HDC		hdc = GetDC(hwPreview_Master);
				RECT	rect; GetClientRect(hwPreview_Master,&rect);

				// Get Resolution
				int lxres=(rect.right-rect.left)&0xfffffff0;
				int lyres=rect.bottom-rect.top;

				// Black Out Bitmap
				if ((!MasterRealTimeDrawn)&&(MasterBlackOutFlag)&&(hwPreviewMasterMem!=NULL))
						{ // Black Out Bitmap
						  unsigned char *tmp=(unsigned char *)hwPreviewMasterMem;
						  for(int y=0;y!=lyres;y++)
						  for(int x=0;x!=lxres;x++)
								{ *(tmp++)=(*(tmp))/4; *(tmp++)=(*(tmp))/4; *(tmp++)=(*(tmp))/4;
								}

						  SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewMasterMem,&hwPreviewMasterInfo,DIB_RGB_COLORS);

						  MasterBlackOutFlag=0;
						}

				// Clip Region
				HRGN	ClipRegion=CreateRectRgn(rect.left,rect.top,rect.left+lxres,rect.top+lyres);
				SelectClipRgn(hdc,ClipRegion);

				// Lotsa Nice Pens
				HPEN	oldpen;
				HPEN	CirclePen =CreatePen(PS_SOLID,0,RGB(255,128,128));
				HPEN	CirclePen2=CreatePen(PS_SOLID,0,RGB(128,64,64));
				HPEN	CentrePen =CreatePen(PS_SOLID,0,RGB(128,128,255));
				
				oldpen=(HPEN)SelectObject(hdc,CentrePen);												// 24.03.97
				
				// Compute Centre
				if ((FlaresEditX==-1)&&(FlaresEditY==-1)) { FlaresEditX=lxres/8.0f; FlaresEditY=lyres/2.0f; }
				SetROP2(hdc,R2_XORPEN);

				// Draw All Elements
				for(int j=0;j!=data.LensFlare.GetNoFlares();j++)
				{  Flare *Local=data.LensFlare.GetFlare(j);
				
				  int Draw=(WhatToDo==Master_DrawAll);
				  if  ((MasterRealTimeDrawn==0)||(data.SelectedElement[j])) Draw=TRUE;
				   
				  if (Draw) {	if (data.SelectedElement[j]) 
											{ // Draw Centre Marker
					                          SelectObject(hdc,CentrePen);
											  float xps=(0.5f+Local->PositionX*0.5f)*lxres;
											  float yps=(0.5f+Local->PositionY*0.5f)*lyres;
											  
											  if (WhatToDo!=Master_DrawSelectedLo)
													{ MoveToEx(hdc,(int)xps-5,(int)yps-5,NULL); LineTo(hdc,(int)xps+6,(int)yps+6);
													  MoveToEx(hdc,(int)xps-5,(int)yps+5,NULL); LineTo(hdc,(int)xps+6,(int)yps-6);
													}

											  // Draw Selected Element ?
											  SelectObject(hdc,CirclePen);
											  if (WhatToDo==Master_DrawSelectedLo) SelectObject(hdc,CirclePen2);
											}
								else SelectObject(hdc,CirclePen2);

								// Draw Element
								if (Local->IsVisible(data.CameraDepth))
										{	Local->PreProcess(lxres,lyres,FlaresEditX,FlaresEditY,data.CameraDepth,(float)data.AnimationPosition,0.0f);

											float xpos=Local->RenderCX;
											float ypos=Local->RenderCY;

											float AngleAdd=Local->AngleOffSet*3.14159f;
											if (Local->ReAlign)	{ float ScreenCX=(0.5f+Local->PositionX*0.5f)*lxres;
																  float ScreenCY=(0.5f+Local->PositionY*0.5f)*lyres;
																  AngleAdd+=(float)atan2(ypos-ScreenCY,xpos-ScreenCX);
																}

											int		i=0;
											float	Angle=AngleAdd,dAngle=6.283185f/RealTimePreview_AngleAdd;
											float	Amult=lxres*Local->RenderSize;
											  
											float	tmp; Local->GetShape(tmp,i);
											float	x=xpos+tmp*Amult*(float)cos(Angle);
											float	y=ypos+tmp*Amult*(float)sin(Angle);
											MoveToEx(hdc,(int)x,(int)y,NULL);

											for(;i<FLARERESOLUTION;i+=RealTimePreview_Add,Angle+=dAngle)
												  { Local->GetShape(tmp,i);
													x=xpos+tmp*Amult*(float)cos(Angle);
													y=ypos+tmp*Amult*(float)sin(Angle);

													LineTo(hdc,(int)x,(int)y);
												  }

											Local->GetShape(tmp,0); 
											x=xpos+tmp*Amult*(float)cos(Angle);
											y=ypos+tmp*Amult*(float)sin(Angle);
											LineTo(hdc,(int)x,(int)y);
										}
				          }
				}

				MasterRealTimeDrawn=1;
				SelectObject(hdc,oldpen);
				DeleteObject(ClipRegion);
				DeleteObject(CirclePen);
				DeleteObject(CirclePen2);
				DeleteObject(CentrePen);				

				ReleaseDC(hwPreview_Master,hdc);
}

#undef RealTimePreview_Add
#undef RealTimePreview_AngleAdd

void ImageFilter_LensF::SetPosn(int xres,int yres,float xpos,float ypos,float X,float Y)
{				int i=0; while(data.SelectedElement[i]==0) i++;
				Flare *tmp=data.LensFlare.GetFlare(i);

				// Store Old Values
				float Position=tmp->Position;
				float Angle=tmp->Angle;

				// Get new Position
				tmp->RealPosn(xres,yres,xpos,ypos,X,Y,data.CameraDepth);
				DistanceSpin->SetValue(tmp->Position,NULL);
				AngleSpin->SetValue(tmp->Angle,NULL);

				// Compute Changes
				float Add1=tmp->Position-Position;
				float Add2=tmp->Angle   -Angle;

				// Restore Old Values
				tmp->Position=Position;
				tmp->Angle=Angle;

				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 

				          for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time))
								{ tmp->PosKeys[j].Position+=Add1; 
								  tmp->PosKeys[j].Angle+=Add2; 
								}

						  tmp->Position+=Add1; 
						  tmp->Angle+=Add2; 
						}

				PutPosKeyFrame();
}

void ImageFilter_LensF::SendTextToListBox(void)
{				SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0); 		

				int i;
				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
					SendMessage(hwndListBox,LB_ADDSTRING,0,(LPARAM)data.LensFlare.GetName(i));

				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
					SendMessage(hwndListBox,LB_SETSEL,data.SelectedElement[i],i);
}

int ImageFilter_LensF::GetSelected(void)
{				int i;

				int n=0;
				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
					n+=SendMessage(hwndListBox,LB_GETSEL,i,0);

				if (n!=0) for(i=0;i!=data.LensFlare.GetNoFlares();i++)
							{ data.SelectedElement[i]=(char)SendMessage(hwndListBox,LB_GETSEL,i,0);
							}

				return n;
}

void ImageFilter_LensF::MasterSetValues(HWND hWnd)
{				int i=0;
				while(data.SelectedElement[i]==0) i++;

				Flare *tmp=data.LensFlare.GetFlare(i);
			
				DistanceSpin->SetValue(tmp->Position,NULL);
				AngleSpin->SetValue(tmp->Angle,NULL);
				SizeSpin->SetValue(tmp->FlareSize,NULL);
				AngleOffSetSpin->SetValue(tmp->AngleOffSet,NULL);
				XPositionSpin->SetValue(tmp->PositionX,NULL);
				YPositionSpin->SetValue(tmp->PositionY,NULL);
				ColouriseSpin->SetValue(tmp->Colourise,NULL);
				CheckDlgButton(hWnd,IDC_ElementActive,tmp->Active);
				CheckDlgButton(hWnd,IDC_ReAlign,tmp->ReAlign);
				CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
				CheckDlgButton(hWnd,IDC_OffScreen,(tmp->OffScreenRender)&1);

				CheckDlgButton(hWnd,IDC_ABSOLUTE,!((tmp->OffScreenRender)&2));
				//EnableIt(IDC_XPosition_SPIN,!((tmp->OffScreenRender)&2));
				//EnableIt(IDC_YPosition_SPIN,!((tmp->OffScreenRender)&2));

				CheckDlgButton(hWnd,IDC_POLAR,tmp->Polar);
				CheckDlgButton(hWnd,IDC_ZSORT,data.LensFlare.ZSort);
				IntensSpin->SetValue(tmp->Intensity,NULL);
				DepthSpin->SetValue(tmp->Depth,NULL);
				OcclusionSpin->SetValue(tmp->Occlusion,NULL);
				ElementLinkButton->SetCheck(tmp->Linked);
				SendMessage(hwndType,CB_SETCURSEL,tmp->Light,0);
				SetDlgItemText(hWnd,IDC_LISTEDIT,tmp->GetName());

				SetDlgItemText(hWnd,IDC_LENSNAME,data.LensFlare.GetName());
}

void ImageFilter_LensF::SetAnimation(void)
{				int PKey=0;
				int SKey=0;

	            for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ Flare *tmp=data.LensFlare.GetFlare(i);

						  for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time)) PKey=1;

						  for(j=0;j!=tmp->NoShpKeys;j++)
						  if (Valid(tmp->ShpKeys[j].Time)) SKey=1;
						}
						
				PositionKeyButton->SetCheck(PKey);
				ShapeKeyButton->SetCheck(SKey);

				if ((SKey)||(PKey)) data.AnimationSelectedFlag=1;
				else data.AnimationSelectedFlag=0;
}

void ImageFilter_LensF::PutPosKeyFrame(void)
{				for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
				if (data.SelectedElement[i])
						{	Flare *tmp=data.LensFlare.GetFlare(i); 

							if (data.SelectedElement[i])
								{ Flare *tmp=data.LensFlare.GetFlare(i); 
														  
								  // Is It Already A Key Frame ?
								  int Pos=tmp->IsPosKey(data.AnimationPosition);

								  if (Pos==-1)	{ // No : Add New KeyFrame
												  tmp->AddPosKey(data.AnimationPosition);
												}
								  else			{ // Yes : Overright Old KeyFrane
												  tmp->PutPosValues(Pos);
												}
								}
						}

				PositionKeyButton->SetCheck(TRUE);
				MegaScrollBar();
}

void ImageFilter_LensF::PutShpKeyFrame(void)
{				for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
				if (data.SelectedElement[i])
						{	Flare *tmp=data.LensFlare.GetFlare(i); 

							if (data.SelectedElement[i])
								{ Flare *tmp=data.LensFlare.GetFlare(i); 
														  
								  // Is It Already A Key Frame ?
								  int Pos=tmp->IsShpKey(data.AnimationPosition);

								  if (Pos==-1)	{ // No : Add New KeyFrame
												  tmp->AddShpKey(data.AnimationPosition);
												}
								  else			{ // Yes : Overright Old KeyFrane
												  tmp->PutShpValues(Pos);
												}
								}
						}

				ShapeKeyButton->SetCheck(TRUE);
				MegaScrollBar();
}

// build button images
HIMAGELIST		HIL_ElementNew;
HIMAGELIST		HIL_ElementCopy;
HIMAGELIST		HIL_ElementDelete;
HIMAGELIST		HIL_SystemNew;
HIMAGELIST		HIL_SystemLoad;
HIMAGELIST		HIL_SystemSave;
HIMAGELIST		HIL_SystemConfig;
HIMAGELIST		HIL_MasterLock;
HIMAGELIST		HIL_AnimShp;
HIMAGELIST		HIL_AnimPos;
HIMAGELIST		HIL_ShapeEdit;
HIMAGELIST		HIL_ShapeMake;
HIMAGELIST		HIL_ShapeMorph;
HIMAGELIST		HIL_ShapeNoise;
HIMAGELIST		HIL_ShapeLoad;
HIMAGELIST		HIL_ShapeSave;
HIMAGELIST		HIL_UpArrow;
HIMAGELIST		HIL_DownArrow;
HIMAGELIST		HIL_MasterOK;
HIMAGELIST		HIL_MasterCancel;
HIMAGELIST		HIL_MasterHelp;

void ImageFilter_LensF::MasterBuildButtons(void)
{		BuildButton(ElementNewButton,HIL_ElementNew,IDB_ELEMENTNEW,IDB_ELEMENTNEW,100,50);	// This stuff is a mess, a lot of IDB_'s are wrong
		BuildButton(ElementCopyButton,HIL_ElementCopy,IDB_ELEMENTCOPY,IDB_ELEMENTCOPY,100,50);
		BuildButton(ElementDeleteButton,HIL_ElementDelete,IDB_ELEMENTDELETE,IDB_ELEMENTDELETE,100,50);
		BuildButton(NewSystemButton,HIL_SystemNew,IDB_SYSTEMNEW,IDB_SYSTEMNEW,100,50);
		BuildButton(OpenButton,HIL_SystemLoad,IDB_SYSTEMLOAD,IDB_SYSTEMLOAD,100,50);
		BuildButton(SaveButton,HIL_SystemSave,IDB_SYSTEMSAVE,IDB_SYSTEMSAVE,100,50);
		BuildButton(RenderParameters,HIL_SystemConfig,IDB_SYSTEMCONFIG,IDB_SYSTEMCONFIG,100,50);
		BuildButton(ElementLinkButton,HIL_MasterLock,IDB_MASTERLOCK_DOWN,IDB_MASTERLOCK_UP,48,50);
		BuildButton(PositionKeyButton,HIL_AnimPos,IDB_ANIMATIONPOSUP,IDB_ANIMATIONPOSDOWN,100,50);
		BuildButton(ShapeKeyButton,HIL_AnimShp,IDB_ANIMATIONSHPUP,IDB_ANIMATIONSHPDOWN,100,50);
		BuildButton(InfernoButton,HIL_ShapeEdit,IDB_SHAPEEDIT,IDB_SHAPEEDIT,100,50);
		BuildButton(FlareMorphButton,HIL_ShapeMake,IDB_SHAPEMAKE,IDB_SHAPEMAKE,100,50);
		BuildButton(FlareBuildButton,HIL_ShapeMorph,IDB_SHAPEMORPH,IDB_SHAPEMORPH,100,50);
		BuildButton(FlareEditButton,HIL_ShapeNoise,IDB_SHAPENOISE,IDB_SHAPENOISE,100,50);
		BuildButton(OpenElementsButton,HIL_ShapeLoad,IDB_SHAPESAVE,IDB_SHAPESAVE,100,50);
		BuildButton(SaveElementsButton,HIL_ShapeSave,IDB_SHAPELOAD,IDB_SHAPELOAD,100,50);
		BuildButton(ElementMoveUpButton,HIL_UpArrow,IDB_UPARROW,IDB_UPARROW,50,50);
		BuildButton(ElementMoveDownButton,HIL_DownArrow,IDB_ARROWDOWN,IDB_ARROWDOWN,50,50);
		BuildButton(MasterOK,HIL_MasterOK,IDB_MASTEROK,IDB_MASTEROK,100,50);
		BuildButton(MasterCancel,HIL_MasterCancel,IDB_MASTERCANCEL,IDB_MASTERCANCEL,100,50);
		BuildButton(MasterHelp,HIL_MasterHelp,IDB_MASTERHELP,IDB_MASTERHELP,100,50);
}

void ImageFilter_LensF::MasterFreeButtons(void)
{		ReleaseButton(HIL_ElementNew);
		ReleaseButton(HIL_ElementCopy);
		ReleaseButton(HIL_ElementDelete);
		ReleaseButton(HIL_SystemNew);
		ReleaseButton(HIL_SystemLoad);
		ReleaseButton(HIL_SystemSave);
		ReleaseButton(HIL_SystemConfig);
		ReleaseButton(HIL_MasterLock);
		ReleaseButton(HIL_AnimPos);
		ReleaseButton(HIL_AnimShp);
		ReleaseButton(HIL_ShapeEdit);
		ReleaseButton(HIL_ShapeMake);
		ReleaseButton(HIL_ShapeMorph);
		ReleaseButton(HIL_ShapeNoise);
		ReleaseButton(HIL_ShapeLoad);
		ReleaseButton(HIL_ShapeSave);
		ReleaseButton(HIL_UpArrow);
		ReleaseButton(HIL_DownArrow);
		ReleaseButton(HIL_MasterOK);
		ReleaseButton(HIL_MasterCancel);
		ReleaseButton(HIL_MasterHelp);
}


///////////////////////////////////////////////////////////////////
//
// Resample background bitmap
//

void MakeBitmapInfo(BITMAPINFO& info,int x,int y)
{
	info.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biPlanes			=1;
	info.bmiHeader.biBitCount		=24;
	info.bmiHeader.biCompression	=BI_RGB;
	info.bmiHeader.biClrUsed		=0;
	info.bmiHeader.biClrImportant	=0;
	info.bmiHeader.biWidth			=x;
	info.bmiHeader.biHeight			=-y;
	info.bmiHeader.biXPelsPerMeter=1000;
	info.bmiHeader.biYPelsPerMeter=1000;
	info.bmiHeader.biSizeImage	=0;
}

void ImageFilter_LensF::LinearResample(HDC hdc1,HBITMAP hbm1,int x1,int y1,HDC hdc2,HBITMAP hbm2,int x2,int y2)
{
	float x,y,xscale,yscale;
	int xpos,ypos;
	int oldx1=x1;
	int oldx2=x2;
	int width;

	BITMAPINFO info1,info2;
	
	// find x scale
	xscale=(float)x1/x2;

	x2=(x2+3)&0xfffffffc;
	width=(x1*3+3)&0xfffffffc;

	// make DIB headers
	MakeBitmapInfo(info1,x1,y1);
	MakeBitmapInfo(info2,x2,y2);

	// space for original bitmap and X-stretched
	uchar* bits1=(uchar*)malloc(width*(y1+1));
	uchar* bitsI=(uchar*)malloc(x2*(y1+1)*3);

	// get pixels for old bitmap
	int i=GetDIBits(hdc1,hbm1,0,y1,bits1,&info1,DIB_RGB_COLORS);
	if( i==0 ) {
		// doesn't work; use StretchBlt
		free(bits1);
		free(bitsI);
		HDC dc1=CreateCompatibleDC(hdc1);
		HDC dc2=CreateCompatibleDC(hdc1);
		SelectObject(dc1,hbm1);
		SelectObject(dc2,hbm2);
		SetStretchBltMode(dc2,BLACKONWHITE);
		StretchBlt(dc2,0,0,oldx2,y2,dc1,0,0,oldx1,y1,SRCCOPY);
		DeleteDC(dc1);
		DeleteDC(dc2);
		DeleteObject(hbm1);
		return;
	}
	DeleteObject(hbm1);

	// generate x lookup table
	uchar* fraction1=new uchar[x2];
	uchar* fraction2=new uchar[x2];
	int* pos=new int[x2];
	for(xpos=0;xpos<x2;xpos++) {
		x=xpos*xscale;
		fraction2[xpos]=(uchar)((x-(int)x)*128.0f);
		fraction1[xpos]=128-fraction2[xpos];
		pos[xpos]=(int)x;
	}

	// interpolate x
	uchar* ToBits=bitsI;
	uchar* FromBits;
	for(ypos=0;ypos<y1;ypos++) {
		for(xpos=0;xpos<x2;xpos++) {
			FromBits	=bits1+ypos*width+pos[xpos]*3;
			*ToBits++=(uchar)((*FromBits*fraction1[xpos]+*(FromBits+3)*fraction2[xpos])>>7);FromBits++;
			*ToBits++=(uchar)((*FromBits*fraction1[xpos]+*(FromBits+3)*fraction2[xpos])>>7);FromBits++;
			*ToBits++=(uchar)((*FromBits*fraction1[xpos]+*(FromBits+3)*fraction2[xpos])>>7);
		}
	}

	// clean up
	delete fraction1;
	delete fraction2;
	delete pos;
	free(bits1);

	// get mem for final bitmap
	uchar* bits2=(uchar*)malloc(x2*(y2+1)*3);

	// generate y lookup
	fraction1=new uchar[y2];
	fraction2=new uchar[y2];
	pos=new int[y2];
	yscale=(float)y1/y2;
	for(ypos=0;ypos<y2;ypos++) {
		y=ypos*yscale;
		fraction2[ypos]=(uchar)((y-(int)y)*128.0f);
		fraction1[ypos]=128-fraction2[ypos];
		pos[ypos]=(int)y;
	}

	// interpolate y
	ToBits=bits2;
	for(ypos=0;ypos<y2;ypos++) {
		for(xpos=0;xpos<x2;xpos++) {
			FromBits	=bitsI+(pos[ypos]*x2+xpos)*3;
			*ToBits++=(uchar)((*FromBits*fraction1[ypos]+*(FromBits+x2*3)*fraction2[ypos])>>7);FromBits++;
			*ToBits++=(uchar)((*FromBits*fraction1[ypos]+*(FromBits+x2*3)*fraction2[ypos])>>7);FromBits++;
			*ToBits++=(uchar)((*FromBits*fraction1[ypos]+*(FromBits+x2*3)*fraction2[ypos])>>7);
		}
	}

	i=SetDIBits(hdc2,hbm2,0,y2,bits2,&info2,DIB_RGB_COLORS);
	// cleanup
	delete fraction1;
	delete fraction2;
	delete pos;
	free(bitsI);
	free(bits2);
}


//******************************************************************
// MAIN WINDOW CALLBACK
int FlaresEditFlag;
HBITMAP MasterLoadBitmap;
HBITMAP MasterBackBitmap;

BOOL ImageFilter_LensF::Master_Control(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
  if( message==WM_COMMAND || message==CC_SPINNER_CHANGE )
	switch (LOWORD(wParam)) 
		{ case IDC_Dist_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

				  ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Add=DistanceSpin->GetFVal()-tmp->Position;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 
				          
				          for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time)) tmp->PosKeys[j].Position+=Add;
						  
						  tmp->Position+=Add; 
						}

				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  RealTimePreview(Master_DrawSelectedHi);
				  break;
				}
		  case IDC_Angle_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

					ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Add=AngleSpin->GetFVal()-tmp->Angle;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 

				          for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time)) tmp->PosKeys[j].Angle+=Add;
						  
						  tmp->Angle+=Add;; 
						}
				  
				  RealTimePreview(Master_DrawSelectedHi);
				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  break;
				}
		  case IDC_LinkButton:
				{ if (HIWORD(wParam)==BN_CLICKED)
						{ ChangedFlare=TRUE;
						  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
						  if (data.SelectedElement[i])
								{ Flare *tmp=data.LensFlare.GetFlare(i);
								  tmp->Linked=ElementLinkButton->IsChecked(); 
								}
						}
				  break;
				}
		  case IDC_Size_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);
				  int NotUsed=1;
				  ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Mult=SizeSpin->GetFVal()/tmp->FlareSize;
				  int Linked=tmp->Linked;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 
				  
						  for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time))
								  { tmp->PosKeys[j].FlareSize*=Mult; 
									if (tmp->Linked) tmp->PosKeys[j].Intensity=tmp->IR2/(tmp->PosKeys[j].FlareSize*tmp->PosKeys[j].FlareSize);
								  }

						  tmp->FlareSize*=Mult; 
						  if (tmp->Linked) 
								{	tmp->Intensity=tmp->IR2/(tmp->FlareSize*tmp->FlareSize);
									if (NotUsed) { IntensSpin->SetValue(tmp->Intensity,NULL); NotUsed=0; }
								}
						  else tmp->IR2=tmp->FlareSize*tmp->FlareSize*tmp->Intensity;
						}

				  RealTimePreview(Master_DrawSelectedHi);
				  if ((Linked)&&(data.RealTimePreview)) SmallMasterPreview(data.DrawChecker);
				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  break;
				}
		  case IDC_Intensity_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ int i=0,NotUsed=1,AnyLinked=0;
				  Flare *tmp;

				  ChangedFlare=TRUE;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
						{ tmp=data.LensFlare.GetFlare(i); 
				          if ((data.SelectedElement[i])&&(tmp->Linked)) { AnyLinked=1; break; }
						}

				  i=0; while(data.SelectedElement[i]==0) i++;
				  tmp=data.LensFlare.GetFlare(i);
				  float Mult=IntensSpin->GetFVal()/tmp->Intensity;

				  if ((AnyLinked)&&(MasterRealTimeDrawn)) RealTimePreview(Master_DrawSelectedHi);
					
				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 
				          
						  for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time))
									{	tmp->PosKeys[j].Intensity*=Mult; 
										if (tmp->Linked) tmp->PosKeys[j].FlareSize=(float)sqrt(tmp->IR2/tmp->PosKeys[j].Intensity);
									}
							
						  tmp->Intensity*=Mult; 
						  if (tmp->Linked) 
									{	tmp->FlareSize=(float)sqrt(tmp->IR2/tmp->Intensity);
										if (NotUsed) { SizeSpin->SetValue(tmp->FlareSize,NULL); NotUsed=0; }
									}
						  else tmp->IR2=tmp->FlareSize*tmp->FlareSize*tmp->Intensity;
						}

				  if (AnyLinked) RealTimePreview(Master_DrawSelectedHi);
				  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  break;
				}
		  case IDC_AngleOffSet_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

				  ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Add=AngleOffSetSpin->GetFVal()-tmp->AngleOffSet;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 

				          for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time))
									{ tmp->PosKeys[j].AngleOffSet+=Add; 
									}

				          tmp->AngleOffSet+=Add; 
						}

				  RealTimePreview(Master_DrawSelectedHi);
				  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  break;
				}
		  case IDC_XPosition_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

				  ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Add=XPositionSpin->GetFVal()-tmp->PositionX;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 

						  for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time))
									{ tmp->PosKeys[j].PositionX+=Add; 
									}

						  tmp->PositionX+=Add; 
						}
				  
				  RealTimePreview(Master_DrawSelectedHi);
				  PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  break;
				}
		  case IDC_YPosition_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

				  ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Add=YPositionSpin->GetFVal()-tmp->PositionY;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 
						  for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time))
									{ tmp->PosKeys[j].PositionY+=Add; 
									}

				          tmp->PositionY+=Add; 
						}
				  
				  if (!data.AnimationSelectedFlag) RealTimePreview(Master_DrawSelectedHi);
				  break;
				}
		  case IDC_Colourise_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ 
				  ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Mult=ColouriseSpin->GetFVal()/tmp->Colourise;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 

				          for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time))
									{ tmp->PosKeys[j].Colourise*=Mult; 
									  if (tmp->PosKeys[j].Colourise<0.002f) tmp->PosKeys[j].Colourise=0.002f;
									  if (tmp->PosKeys[j].Colourise>1.0f) tmp->PosKeys[j].Colourise=1.0f;
									}

				          tmp->Colourise*=Mult; 
						  if (tmp->Colourise<0.002f) tmp->Colourise=0.002f;
						  if (tmp->Colourise>1.0f) tmp->Colourise=1.0f;
						}

				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  break;
				}
		  case IDC_Dpth_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);
				  
				  ChangedFlare=TRUE;
		          
		          int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Mult=DepthSpin->GetFVal()-tmp->Depth;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 

						  for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time)) tmp->PosKeys[j].Depth+=Mult; 

				          tmp->Depth+=Mult; 
						}

				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  RealTimePreview(Master_DrawSelectedHi);
				  break;
				}
		  case IDC_Occl_SPIN:
		  if (message==CC_SPINNER_CHANGE)
				{ 
				  ChangedFlare=TRUE;

				  int i=0; while(data.SelectedElement[i]==0) i++;
				  Flare *tmp=data.LensFlare.GetFlare(i);
				  float Mult=OcclusionSpin->GetFVal()-tmp->Occlusion;

				  for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				  if (data.SelectedElement[i])
						{ tmp=data.LensFlare.GetFlare(i); 

						  for(int j=0;j!=tmp->NoPosKeys;j++)
						  if (Valid(tmp->PosKeys[j].Time)) 
								{ tmp->PosKeys[j].Occlusion+=Mult; 
								  if (tmp->PosKeys[j].Occlusion<0.002f) tmp->PosKeys[j].Occlusion=0.002f;
								  if (tmp->PosKeys[j].Occlusion>1.0f) tmp->PosKeys[j].Occlusion=1.0f;
								}

				          tmp->Occlusion+=Mult; 
						  if (tmp->Occlusion<0.002f) tmp->Occlusion=0.002f;
						  if (tmp->Occlusion>1.0f) tmp->Occlusion=1.0f;
						}

				  if (!data.AnimationSelectedFlag) PutPosKeyFrame(); // Put An Animation Key Frame If Need Be
				  break;
				}
		  //******************************************************************************************
		  // ANIMATION
		  case IDC_FORWARD:
				{ switch (HIWORD(wParam))
					{	case BN_BUTTONUP:	
							{ int TimePos=999999;
							  data.AnimationSelect1=-1;

							  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
							  if (data.SelectedElement[i])
									{ Flare *tmp=data.LensFlare.GetFlare(i); 
									  
								      for(int j=0;j!=tmp->NoPosKeys;j++)
											if ((tmp->PosKeys[j].Time>data.AnimationPosition)&&
											    (tmp->PosKeys[j].Time<TimePos)) TimePos=tmp->PosKeys[j].Time;

									  for(j=0;j!=tmp->NoShpKeys;j++)
											if ((tmp->ShpKeys[j].Time>data.AnimationPosition)&&
											    (tmp->ShpKeys[j].Time<TimePos)) TimePos=tmp->ShpKeys[j].Time;
									}

							  if (TimePos!=999999) { data.LensFlare.GetFrame(data.AnimationPosition);
							                         data.AnimationPosition=TimePos;
							                         if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
													 AnimationSpin->SetValue(ImageFilterData->data.AnimationPosition,NULL);
													 MegaScrollBar();
													 SetAnimation();
													 MasterSetValues(hWnd);
													 RealTimePreview(Master_DrawAll);
													 if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
							                       }
							  break;
							}
					}
			      break;
				}

		 case IDC_BACK:
				{ switch (HIWORD(wParam))
					{	case BN_BUTTONUP:	
							{ int TimePos=-1;
							  data.AnimationSelect1=-1;

							  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
							  if (data.SelectedElement[i])
									{ Flare *tmp=data.LensFlare.GetFlare(i); 
									  
								      for(int j=0;j!=tmp->NoPosKeys;j++)
											if ((tmp->PosKeys[j].Time<data.AnimationPosition)&&
											    (tmp->PosKeys[j].Time>TimePos)) TimePos=tmp->PosKeys[j].Time;

									  for(j=0;j!=tmp->NoShpKeys;j++)
											if ((tmp->ShpKeys[j].Time<data.AnimationPosition)&&
											    (tmp->ShpKeys[j].Time>TimePos)) TimePos=tmp->ShpKeys[j].Time;
									}

									if (TimePos!=-1) { if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
									                   data.AnimationPosition=TimePos;
							                           data.LensFlare.GetFrame(data.AnimationPosition);
													   AnimationSpin->SetValue(ImageFilterData->data.AnimationPosition,NULL);
													   MegaScrollBar();
													   SetAnimation();
													   MasterSetValues(hWnd);
													   RealTimePreview(Master_DrawAll);
													   if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
							                       }
							  break;
							}
					}
			      break;
				}
		  case IDC_Time_SPIN:
				if (message==CC_SPINNER_CHANGE)
							  {	  // Erase Display
								  if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);

								  // Get New Time Value
								  data.AnimationPosition=AnimationSpin->GetIVal();
								  data.AnimationSelect1=-1;

								  // Get New Positions
								  data.LensFlare.GetFrame(data.AnimationPosition);

								  if (data.AnimationPosition>data.AnimationMax) data.AnimationMax=data.AnimationPosition;
								  else {	int MPos=data.LensFlare.MaxKeyValue();
											if (MPos>data.AnimationPosition) data.AnimationMax=MPos;
											else data.AnimationMax=data.AnimationPosition;

											if (data.AnimationMax<100) data.AnimationMax=100;
									   }

								  // Update All Display
								  MegaScrollBar();
								  SetAnimation();
								  MasterSetValues(hWnd);
								  RealTimePreview(Master_DrawAll);
								  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
							}
				break;

		  case IDC_SHAPE_KEYFRAME:
				{ switch (HIWORD(wParam))
					{	case BN_CLICKED:	
							{ if (data.AnimationPosition!=0)
									{ int AddNew=ShapeKeyButton->IsChecked(); 
									  
									  if (AddNew)	{ if (!data.AnimationSelectedFlag) PutShpKeyFrame();
													}
									  else			{ for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
										              if (data.SelectedElement[i])
															{ Flare *tmp=data.LensFlare.GetFlare(i); 
									                          
									                          for(int j=0;j<tmp->NoShpKeys;j++)
															  if (Valid(tmp->ShpKeys[j].Time)) { tmp->DeleteShpKey(j); j--; }
															}
													}
									}

							  SetAnimation();
							  MegaScrollBar();
							  ChangedFlare=TRUE;
							  break;
							}
					}
				 break;
				}

		    case IDC_POSITION_KEYFRAME:
				{ switch (HIWORD(wParam))
					{	case BN_CLICKED:	
							{ if (data.AnimationPosition!=0)
									{ int AddNew=PositionKeyButton->IsChecked(); 
									  
									  if (AddNew)	{ if (!data.AnimationSelectedFlag) PutPosKeyFrame();
													}
									  else			{ for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
										              if (data.SelectedElement[i])
															{ Flare *tmp=data.LensFlare.GetFlare(i); 

															  for(int j=0;j<tmp->NoPosKeys;j++)
															  if (Valid(tmp->PosKeys[j].Time)) { tmp->DeletePosKey(j); j--; }
															}
													}
									}

							  SetAnimation();
							  MegaScrollBar();
							  ChangedFlare=TRUE;
							  break;
							}
					}
				 break;
				}
		  //******************************************************************************************
		}
				
	switch (message) 
	   { case WM_INITDIALOG: 
		   { CenterWindow(hWnd,GetParent(hWnd));
             SetCursor(LoadCursor(NULL,IDC_ARROW));

			 // Setup background bitmap rcw 18/4/97
				// load bitmap and get window dc
				MasterLoadBitmap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_MASTER));
//				MasterLoadBitmap=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_MASTER),IMAGE_BITMAP,16,16,LR_CREATEDIBSECTION);
				HDC hdc=GetDC(hWnd);
				RECT	rect;
				BITMAP	bm;

				// get sizes of bitmap and window
				GetClientRect(hWnd,&rect);
				GetObject(MasterLoadBitmap,sizeof(BITMAP),&bm);

				if( abs(rect.right-bm.bmWidth)<=2 && abs(rect.bottom-bm.bmHeight)<=2 )
					// keep original
					MasterBackBitmap=MasterLoadBitmap;
				else {
					// get the final background bitmap
					MasterBackBitmap=CreateCompatibleBitmap(hdc,(rect.right+3)&0xfffffffc,rect.bottom);
					// interpolate new size
					// note this deletes old bitmap internally
					LinearResample(hdc,MasterLoadBitmap,bm.bmWidth,bm.bmHeight,hdc,MasterBackBitmap,rect.right,rect.bottom);
				}

				// clean up DC
				ReleaseDC(hWnd,hdc);

			 // Get Andrews Mega ScrollBar Control
				hwndScrollBar = GetDlgItem(hWnd, IDC_MEGASCROLL);					  
				SetWindowLong(hwndScrollBar,GWL_WNDPROC,(LONG)MScrollBarProc);
				data.AnimationMax=data.LensFlare.MaxKeyValue();
				if (data.AnimationMax<100) data.AnimationMax=100;

			 // Get Preview Window
			    hwPreview_Master = GetDlgItem(hWnd, IDC_MASTERPREVIEW);					  
			    SetWindowLong(hwPreview_Master,GWL_WNDPROC,(LONG)MasterPreviewProc);

			 // Get Small Preview Window
			    hwSmallPreview_Master = GetDlgItem(hWnd, IDC_SMALLPREVIEW);
			    SetWindowLong(hwSmallPreview_Master,GWL_WNDPROC,(LONG)MasterSmallPreviewProc);

			 // Deal With List Box
				hwndListBox=GetDlgItem(hWnd,IDC_LISTBOX);
				SendTextToListBox();

			 // Deal With Type Box
				hwndType=GetDlgItem(hWnd, IDC_Type);
				SendMessage(hwndType,CB_RESETCONTENT, 0, 0);
				
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Light"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Object"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Tint"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Add"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Subtract"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Negative"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Blur I"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Blur RGB"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Ang. Displace I"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Ang. Displace RGB"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Rad. Displace I"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Rad. Displace RGB"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Image Warp I"));
				SendMessage(hwndType,CB_ADDSTRING,0,(LPARAM)_T("Image Warp RGB"));

			 // Setup Controls
				InfernoButton=GetICustButton(GetDlgItem(hWnd,IDC_INFERNO));
				FlareEditButton=GetICustButton(GetDlgItem(hWnd, IDC_EditFlare));
				FlareBuildButton=GetICustButton(GetDlgItem(hWnd, IDC_BuildFlare));
				FlareMorphButton=GetICustButton(GetDlgItem(hWnd,IDC_MorphElement));
				ElementNewButton=GetICustButton(GetDlgItem(hWnd,IDC_NewElement));
				ElementDeleteButton=GetICustButton(GetDlgItem(hWnd,IDC_DeleteElement));
				ElementMoveUpButton=GetICustButton(GetDlgItem(hWnd,IDC_MoveUP));
				ElementMoveDownButton=GetICustButton(GetDlgItem(hWnd,IDC_MoveDown));
				ElementCopyButton=GetICustButton(GetDlgItem(hWnd,IDC_CopyElement));
				ElementLinkButton=GetICustButton(GetDlgItem(hWnd,IDC_LinkButton));
				ElementLinkButton->SetType(CBT_CHECK);

			 // Ok And Cancel Buttons
				MasterOK=GetICustButton(GetDlgItem(hWnd,IDC_MASTER_OK));
				MasterCancel=GetICustButton(GetDlgItem(hWnd,IDC_MASTER_CANCEL));
				MasterHelp=GetICustButton(GetDlgItem(hWnd,IDC_HELPB));
				
			// Key Frame Buttons
				PositionKeyButton=GetICustButton(GetDlgItem(hWnd,IDC_POSITION_KEYFRAME));
				PositionKeyButton->SetType(CBT_CHECK);

				ShapeKeyButton=GetICustButton(GetDlgItem(hWnd,IDC_SHAPE_KEYFRAME));
				ShapeKeyButton->SetType(CBT_CHECK);

				ForwardButton=GetICustButton(GetDlgItem(hWnd,IDC_FORWARD));
				BackwardButton=GetICustButton(GetDlgItem(hWnd,IDC_BACK));

			// Filing Buttons
				OpenButton=GetICustButton(GetDlgItem(hWnd,IDC_OPEN));
				SaveButton=GetICustButton(GetDlgItem(hWnd,IDC_SAVE));

				NewSystemButton=GetICustButton(GetDlgItem(hWnd,IDC_New));
				OpenElementsButton=GetICustButton(GetDlgItem(hWnd,IDC_ELEMENTLOAD));
				SaveElementsButton=GetICustButton(GetDlgItem(hWnd,IDC_ELEMENTSAVE));

			// Disable some buttons in the eval version
				int a=GetCode();
					if (encr(a)!=encr(encr(Code()))) {
						SaveButton->Disable();
						OpenElementsButton->Disable();
						SaveElementsButton->Disable();
					}

			// Render Parameters
				RenderParameters=GetICustButton(GetDlgItem(hWnd,IDC_PARAMETERS));

			// Other Spinner Controls
				DistanceSpin=SetupFloatSpinner(hWnd,IDC_Dist_SPIN,IDC_Dist_EDIT,-10.0f,10.0f,0.0f,0.01f,0.0f);
				AngleSpin   =SetupFloatSpinner(hWnd,IDC_Angle_SPIN,IDC_Angle_EDIT,-10.0f,10.0f,0.0f,0.01f,0.0f);
				SizeSpin    =SetupFloatSpinner(hWnd,IDC_Size_SPIN,IDC_Size_EDIT,0.002f,1.0f,0.0f,0.002f,0.0f);
				AngleOffSetSpin=SetupFloatSpinner(hWnd,IDC_AngleOffSet_SPIN,IDC_AngleOffSet_EDIT,-10.0f,10.0f,0.0f,0.01f,0.0f);
				XPositionSpin=SetupFloatSpinner(hWnd,IDC_XPosition_SPIN,IDC_XPosition_EDIT,-1.0f,1.0f,0.0f,0.01f,0.0f);
				YPositionSpin=SetupFloatSpinner(hWnd,IDC_YPosition_SPIN,IDC_YPosition_EDIT,-1.0f,1.0f,0.0f,0.01f,0.0f);
				ColouriseSpin=SetupFloatSpinner(hWnd,IDC_Colourise_SPIN,IDC_Colourise_EDIT,0.0f,1.0f,0.0f,0.002f,0.0f);
				IntensSpin=SetupFloatSpinner(hWnd,IDC_Intensity_SPIN,IDC_Intensity_EDIT,0.01f,10.0f,0.0f,0.01f,0.0f);
				DepthSpin=SetupFloatSpinner(hWnd,IDC_Dpth_SPIN,IDC_Dpth_EDIT,-10.0f,10.0f,0.0f,0.01f,0.0f);
				OcclusionSpin=SetupFloatSpinner(hWnd,IDC_Occl_SPIN,IDC_Occl_EDIT,0.0f,1.0f,0.0f,0.002f,0.0f);
				AnimationSpin=SetupIntSpinner(hWnd,IDC_Time_SPIN,IDC_Time_EDIT,0,100000,data.AnimationPosition,1,data.AnimationPosition);


			// protection
				DrawDisplay(Img_R,Img_G,Img_B,Img_A);


				CheckDlgButton(hWnd,IDC_REALTIME,data.RealTimePreview);
				data.LensFlare.GetFrame(data.AnimationPosition);

				MasterBuildButtons();

			// Setup Display
				MasterSetValues(hWnd);
				SetAnimation();
				SmallMasterPreview(data.DrawChecker);
				MegaScrollBar();
			 return 1;
           }
         case WM_COMMAND:			 
			 switch (LOWORD(wParam)) 
				{ 

				  case IDC_MASTER_OK:
					  if( HIWORD(wParam)!=BN_CLICKED )
								{ MasterFreeButtons();
								   MasterReleaseButtons();
								   EndDialog(hWnd,1);	// Todo : Free Temporary Memory Store
								   FlaresEditFlag=1;
							     }
								   break; 
				  case IDC_MASTER_CANCEL:
				  case IDCANCEL:
					  if( HIWORD(wParam)!=BN_CLICKED )
								 { MasterFreeButtons();
								   MasterReleaseButtons();
								   EndDialog(hWnd,0);	// Todo : Get,Free Temporary Memory Store 
					               FlaresEditFlag=0;
								 }
								   break;

				  // help button
				  case IDC_HELPB: 
					if( HIWORD(wParam)==BN_BUTTONUP ) {
						char helpcmd[MAX_PATH+30];
						sprintf(helpcmd,"%s\\GenesisVFX\\genVFXps.chm",InstallLoc+10);
						ShellExecute(NULL,"open",helpcmd,NULL,NULL,SW_SHOW);
					}
					break;


				  case IDC_PARAMETERS:
								 { if (HIWORD(wParam)==BN_BUTTONUP)
										{ 
										if( ShowRenderControl(hWnd)==0 ) {
											MasterFreeButtons();
											MasterReleaseButtons();
											EndDialog(hWnd,1);	// Todo : Free Temporary Memory Store
											FlaresEditFlag=1;
										}
										}
								   break;
								 }

				  case IDC_OPEN: { if (HIWORD(wParam)==BN_BUTTONUP)
										{	if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
											Load(hWnd);

											data.SelectedElement[0]=1;
											for(int i=1;i!=data.LensFlare.GetNoFlares();i++) data.SelectedElement[i]=0;

											SendTextToListBox();

											data.LensFlare.GetFrame(data.AnimationPosition);

											MasterSetValues(hWnd);
											RealTimePreview(Master_DrawAll);
											SmallMasterPreview(data.DrawChecker);
											SetAnimation();
											MegaScrollBar();
										}
								   break;
								 }

				  case IDC_SAVE: { if (HIWORD(wParam)==BN_BUTTONUP) Save(hWnd);
					               break;
								 }

				  case IDC_ELEMENTLOAD: 
								 { if (HIWORD(wParam)==BN_BUTTONUP)
										{	if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
											LoadElements(hWnd);

											SendTextToListBox();

											data.LensFlare.GetFrame(data.AnimationPosition);

											MasterSetValues(hWnd);
											RealTimePreview(Master_DrawAll);
											SmallMasterPreview(data.DrawChecker);
											SetAnimation();
											MegaScrollBar();
										}
								   break;
								 }

				  case IDC_ELEMENTSAVE: 
								 { if (HIWORD(wParam)==BN_BUTTONUP) SaveElements(hWnd);
					               break;
								 }

				  case IDC_New:  { if (HIWORD(wParam)==BN_BUTTONUP)
										{	
											if( ChangedFlare ) {
												char s[256];
												sprintf(s,"Effect \"%s\" has been modified.\nDo you want to save it?",data.LensFlare.GetName());
												int result=MessageBox(hWnd,s,"Warning",MB_YESNOCANCEL|MB_ICONQUESTION);
												if( result==IDCANCEL ) return TRUE;
												if( result==IDYES ) Save(hWnd);
											}

											if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);

											data.LensFlare.DeleteAll();

											data.LensFlare.Reset();

											data.AnimationPosition=0;
											data.AnimationMax=100;

											data.SelectedElement[0]=1;
											for(int i=1;i!=data.LensFlare.GetNoFlares();i++) data.SelectedElement[i]=0;

											SendTextToListBox();

											MasterSetValues(hWnd);
											RealTimePreview(Master_DrawAll);
											SmallMasterPreview(data.DrawChecker);
											SetAnimation();
											MegaScrollBar();
											ChangedFlare=FALSE;
										}
					               break;
								 }

				  case IDC_LENSNAME: 
								{ if (HIWORD(wParam)==EN_UPDATE) {
									GetDlgItemText(hWnd,IDC_LENSNAME,data.LensFlare.GetName(),256);
								  }
								  break;
								}

				  case IDC_LISTEDIT: 
								{ if (HIWORD(wParam)==EN_KILLFOCUS)
											{ char stmp[256];
											  ChangedFlare=TRUE;
											  GetDlgItemText(hWnd,IDC_LISTEDIT,stmp,256);

											  int n=0;
											  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
											  if (data.SelectedElement[i]) n++;

											  if (n==1) { for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
														  if (data.SelectedElement[i])
											                    { Flare *tmp=data.LensFlare.GetFlare(i);
																  tmp->SetName(stmp);
																}
														}
											  else		{ int n=1;
														  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
														  if (data.SelectedElement[i])
																{ Flare *tmp=data.LensFlare.GetFlare(i);
																  char stmp2[256];
																  sprintf(stmp2,"%s [%d]",stmp,n++);
																  tmp->SetName(stmp2);
																}
														}

											  SendTextToListBox();
											  MasterSetValues(hWnd);
											}
								  break;
								}

				  case IDC_EditFlare:
								{ if (HIWORD(wParam)==BN_BUTTONUP)
											{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
											
											  int i=0; while(data.SelectedElement[i]==0) i++;
											  Flare *tmpFrom=data.LensFlare.GetFlare(i);

											  if (EditorShowControl(hWnd,data.LensFlare.GetFlare(i)))
													{ for(i=0;i!=data.LensFlare.GetNoFlares();i++)
													  if (data.SelectedElement[i])
															{ Flare *tmp=data.LensFlare.GetFlare(i);
															  tmp->GetFlare(tmpFrom,0);

															  for(int j=0;j!=tmp->NoShpKeys;j++)
															  if (Valid(tmp->ShpKeys[j].Time)) tmp->PutShpValues(j);
															}
													  if (!data.AnimationSelectedFlag) PutShpKeyFrame(); // Put An Animation Key Frame If Need Be
													  ChangedFlare=TRUE;
													}

											  CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
											  RealTimePreview(Master_DrawAll);
											  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
											}
								  break;
								}

				  case IDC_INFERNO:
								{ if (HIWORD(wParam)==BN_BUTTONUP)
											{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
											  MasterRealTimeDrawn=0;
											
											  int i=0; while(data.SelectedElement[i]==0) i++;
											  Flare *tmpFrom=data.LensFlare.GetFlare(i);

											  if (ShowInfernoControl(hWnd,data.LensFlare.GetFlare(i)))
													{ for(i=0;i!=data.LensFlare.GetNoFlares();i++)
													  if (data.SelectedElement[i])
															{ Flare *tmp=data.LensFlare.GetFlare(i);
															  tmp->Inferno.GetFractal(tmpFrom->Inferno);
															}
													  ChangedFlare=TRUE;
													}

											  CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
											  RealTimePreview(Master_DrawAll);
											  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
											  MasterSetValues(hWnd);
											}

								  break;
								}

				  case IDC_BuildFlare:
								{ if (HIWORD(wParam)==BN_BUTTONUP)
											{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
											  MasterRealTimeDrawn=0;
											
											  int i=0; while(data.SelectedElement[i]==0) i++;
											  Flare *tmpFrom=data.LensFlare.GetFlare(i);

											  if (BuildShowControl(hWnd,data.LensFlare.GetFlare(i)))
													{ for(i=0;i!=data.LensFlare.GetNoFlares();i++)
													  if (data.SelectedElement[i])
															{ Flare *tmp=data.LensFlare.GetFlare(i);
															  tmp->GetFlare(tmpFrom,0);

															  for(int j=0;j!=tmp->NoShpKeys;j++)
															  if (Valid(tmp->ShpKeys[j].Time)) tmp->PutShpValues(j);
															}
													  if (!data.AnimationSelectedFlag) PutShpKeyFrame(); // Put An Animation Key Frame If Need Be
													  ChangedFlare=TRUE;
													}

											  CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
											  RealTimePreview(Master_DrawAll);
											  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
											  MasterSetValues(hWnd);
											}

								  break;
								}

					case IDC_MoveUP:
								{ if (HIWORD(wParam)==BN_BUTTONUP)
											{ if (data.SelectedElement[0]==0)
												{ for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
													if (data.SelectedElement[i]) data.LensFlare.MoveUp(i,data.SelectedElement);

												  SendTextToListBox();
												  ChangedFlare=TRUE;
												}
											}
								  break;
								}

				  case IDC_MoveDown:
								{ if (HIWORD(wParam)==BN_BUTTONUP)
											{ if (data.SelectedElement[data.LensFlare.GetNoFlares()-1]==0)
												{ for(int i=data.LensFlare.GetNoFlares()-1;i>=0;i--)
													if (data.SelectedElement[i]) data.LensFlare.MoveDown(i,data.SelectedElement);

												  SendTextToListBox();
												  ChangedFlare=TRUE;
												}
											}
								  break;
								}

				  case IDC_DeleteElement:
								{ if (HIWORD(wParam)==BN_BUTTONUP)
											{ int Pos=0; while(data.SelectedElement[Pos]==0) Pos++;

											  if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);
											
											  data.LensFlare.Delete(data.SelectedElement);

											  int Reset=0;
											  if (data.LensFlare.GetNoFlares()==0) { data.LensFlare.New(); Reset=1; }
											  if (Pos>data.LensFlare.GetNoFlares()-1) Pos=data.LensFlare.GetNoFlares()-1;
												
											  for(int i=0;i!=MAXFLARES;i++) data.SelectedElement[i]=0;
											  data.SelectedElement[Pos]=1;

											  SendTextToListBox();
											  MasterSetValues(hWnd);

											  if (!Reset) RealTimePreview(Master_DrawSelectedLo);
											  RealTimePreview(Master_DrawSelectedHi);
											  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
											  MegaScrollBar();
											  ChangedFlare=TRUE;
											}

								  break;
								}

				  case IDC_CopyElement:
								{ if ( HIWORD(wParam)==BN_BUTTONUP || HIWORD(wParam)==1 )
											{ if (MasterRealTimeDrawn) { RealTimePreview(Master_DrawSelectedHi);
																		 RealTimePreview(Master_DrawSelectedLo);
																	   }
											  
											  int InitialNo=data.LensFlare.GetNoFlares();

											  for(int i=0;i!=InitialNo;i++)
											  if (data.SelectedElement[i]) 
														{ data.SelectedElement[i]=0;
														  Flare *tmp=data.LensFlare.GetFlare(i);
														  data.LensFlare.Copy(i);
														  data.SelectedElement[data.LensFlare.GetNoFlares()-1]=1;
														}
											 
											  SendTextToListBox();
											  RealTimePreview(Master_DrawSelectedHi);
											  ChangedFlare=TRUE;
											}
								  break;
								}

				  case IDC_NewElement:
								{ if (HIWORD(wParam)==BN_BUTTONUP)
											{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);
											  MasterRealTimeDrawn=0;

											  data.LensFlare.New();
											  for(int i=0;i!=data.LensFlare.GetNoFlares()-1;i++)
													data.SelectedElement[i]=0;

											  data.SelectedElement[data.LensFlare.GetNoFlares()-1]=1;
											  SendTextToListBox();
											  BuildShowControl(hWnd,data.LensFlare.GetFlare(data.LensFlare.GetNoFlares()-1));

											  // Write Shape Values into NODE Keyframe
													Flare *tmp=data.LensFlare.GetFlare(data.LensFlare.GetNoFlares()-1);
													tmp->PutShpValues(0);
													tmp->PutPosValues(0);

											  CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
											  MasterSetValues(hWnd);

											  RealTimePreview(Master_DrawSelectedHi);
											  MegaScrollBar();
											  ChangedFlare=TRUE;
											  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
											}
								  break;
								}

				  case IDC_MorphElement:
								{ if (HIWORD(wParam)==BN_BUTTONUP) 
										{ for(int i=data.LensFlare.GetNoFlares();i<MAXFLARES;i++)
												data.SelectedElement[i]=0;

										  if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
										  MasterRealTimeDrawn=0;

										  MorphShowControl(hWnd);
										  SendTextToListBox();
										  CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);

										  MegaScrollBar();
										  SetAnimation();
										  MasterSetValues(hWnd);
										  RealTimePreview(Master_DrawAll);
										  AnimationSpin->SetValue(ImageFilterData->data.AnimationPosition,NULL);
										}
								  break;
								}

				  case IDC_CHECKER:
								{ data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_CHECKER); 
								  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
								  break; 
								}

				  case IDC_ZSORT:
								{ data.LensFlare.ZSort=IsDlgButtonChecked(hWnd,IDC_ZSORT);
								  ChangedFlare=TRUE;
								  break;
								}
				  
				  case IDC_ReAlign:
								{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

								  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
								  if (data.SelectedElement[i])
										{ Flare *tmp=data.LensFlare.GetFlare(i);
										  tmp->ReAlign=IsDlgButtonChecked(hWnd,IDC_ReAlign); 
										}

								  RealTimePreview(Master_DrawSelectedHi);
								  ChangedFlare=TRUE;
								  break;
								}

//********************** 12.4.97 *****************************
				  case IDC_ABSOLUTE:
								{ if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

								  HDC		hdc = GetDC(hwPreview_Master);
							      RECT	rect;

								  GetClientRect(hwPreview_Master,&rect);
								  int lxres=(rect.right-rect.left)&0xfffffff0;
								  int lyres=rect.bottom-rect.top;

								  if ((FlaresEditX==-1.0f)&&(FlaresEditY==-1.0f)) { FlaresEditX=lxres/8.0f; FlaresEditY=lyres/2.0f; }

								  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
								  if (data.SelectedElement[i])
										{ Flare *tmp=data.LensFlare.GetFlare(i);
								          
								          int Old=tmp->OffScreenRender;
										  SetBit(tmp->OffScreenRender,2,!IsDlgButtonChecked(hWnd,IDC_ABSOLUTE));

										  if (tmp->OffScreenRender!=Old) tmp->ToAbsolute(lxres,lyres,FlaresEditX,FlaresEditY,data.CameraDepth,(float)data.AnimationPosition,1,Old,tmp->OffScreenRender);
										}

								  ReleaseDC(hwPreview_Master,hdc);

								  MasterSetValues(hWnd);

								  RealTimePreview(Master_DrawSelectedHi);
								  ChangedFlare=TRUE;
								  break;
								}
//**************************************************************

				  case IDC_POLAR:
								{ //if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedHi);

								  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
								  if (data.SelectedElement[i])
										{ Flare *tmp=data.LensFlare.GetFlare(i);
								          
								          int OldPolar=tmp->Polar;
										  tmp->Polar=IsDlgButtonChecked(hWnd,IDC_POLAR); 

										  if (tmp->Polar!=OldPolar) { if (tmp->Polar) tmp->ToPolar(1);
										                              else tmp->FromPolar(1);
										                            }
										}

								  MasterSetValues(hWnd);
								  ChangedFlare=TRUE;

								  //RealTimePreview(Master_DrawSelectedHi);
								  break;
								}

				  case IDC_REALTIME:
								{ data.RealTimePreview=IsDlgButtonChecked(hWnd,IDC_REALTIME); 
								  if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
								}
								break;

				  case IDC_ElementActive:			
								{ for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
								  if (data.SelectedElement[i])
										{ Flare *tmp=data.LensFlare.GetFlare(i);
										  tmp->Active=IsDlgButtonChecked(hWnd,IDC_ElementActive); 
										}
								  ChangedFlare=TRUE;

								  break; 
								}

				  case IDC_Type:{ if (HIWORD(wParam)==CBN_SELCHANGE)
										{ for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
										  if (data.SelectedElement[i])
												{ Flare *tmp=data.LensFlare.GetFlare(i);
												  tmp->Light=SendMessage(hwndType,CB_GETCURSEL,0,0);
												}

										  SmallMasterPreview(data.DrawChecker);
										  ChangedFlare=TRUE;
										}

					  			  break;
								}

				  case IDC_OffScreen:
								{ for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
										  if (data.SelectedElement[i])
												{ Flare *tmp=data.LensFlare.GetFlare(i);
												  SetBit(tmp->OffScreenRender,1,IsDlgButtonChecked(hWnd,IDC_OffScreen));
												}
								  ChangedFlare=TRUE;
								  break;
								}

				  case IDC_LISTBOX:
					  switch (HIWORD(wParam)) { case LBN_SELCHANGE: {	if (MasterRealTimeDrawn) 
																			{ RealTimePreview(Master_DrawSelectedHi);
																		      RealTimePreview(Master_DrawSelectedLo);
																			}

																		if (!GetSelected()) SendTextToListBox();

																		if (MasterRealTimeDrawn) RealTimePreview(Master_DrawSelectedLo);
																		RealTimePreview(Master_DrawSelectedHi);
																		if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
																		MasterSetValues(hWnd);
																		MegaScrollBar();
																		break;
																	}
												case LBN_DBLCLK:	{	if (MasterRealTimeDrawn) RealTimePreview(Master_DrawAll);
																		MasterRealTimeDrawn=0;
								
																		int i=0; while(data.SelectedElement[i]==0) i++;
																		Flare *tmpFrom=data.LensFlare.GetFlare(i);

																		if (BuildShowControl(hWnd,data.LensFlare.GetFlare(i)))
																			{ for(i=0;i!=data.LensFlare.GetNoFlares();i++)
																			  if (data.SelectedElement[i])
																					{ Flare *tmp=data.LensFlare.GetFlare(i);
																					  tmp->GetFlare(tmpFrom,0);
																					}
																		      ChangedFlare=TRUE;
																			}

																		PutShpKeyFrame(); // Put An Animation Key Frame If Need Be
																		CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);
																		RealTimePreview(Master_DrawAll);

																		if (data.RealTimePreview) SmallMasterPreview(data.DrawChecker);
																		break;
																	}
											  }
			
                }
             return 1;
       }     
     return 0;
}

// release buttons
void ImageFilter_LensF::MasterReleaseButtons()
{
				 // Setup Controls
				ReleaseICustButton(InfernoButton);
				ReleaseICustButton(FlareEditButton);
				ReleaseICustButton(FlareBuildButton);
				ReleaseICustButton(FlareMorphButton);
				ReleaseICustButton(ElementNewButton);
				ReleaseICustButton(ElementDeleteButton);
				ReleaseICustButton(ElementMoveUpButton);
				ReleaseICustButton(ElementMoveDownButton);
				ReleaseICustButton(ElementCopyButton);
				ReleaseICustButton(ElementLinkButton);

			 // Ok And Cancel Buttons
				ReleaseICustButton(MasterOK);
				ReleaseICustButton(MasterCancel);
				ReleaseICustButton(MasterHelp);
				
			// Key Frame Buttons
				ReleaseICustButton(PositionKeyButton);
				ReleaseICustButton(ShapeKeyButton);

				ReleaseICustButton(ForwardButton);
				ReleaseICustButton(BackwardButton);

			// Filing Buttons
				ReleaseICustButton(OpenButton);
				ReleaseICustButton(SaveButton);
				ReleaseICustButton(NewSystemButton);
				ReleaseICustButton(OpenElementsButton);
				ReleaseICustButton(SaveElementsButton);

			// Render Parameters
				ReleaseICustButton(RenderParameters);

			// Other Spinner Controls
				ReleaseSpinner(DistanceSpin);
				ReleaseSpinner(AngleSpin);
				ReleaseSpinner(SizeSpin);
				ReleaseSpinner(AngleOffSetSpin);
				ReleaseSpinner(XPositionSpin);
				ReleaseSpinner(YPositionSpin);
				ReleaseSpinner(ColouriseSpin);
				ReleaseSpinner(IntensSpin);
				ReleaseSpinner(DepthSpin);
				ReleaseSpinner(OcclusionSpin);
				ReleaseSpinner(AnimationSpin);
}


VOID CALLBACK TimerProc(HWND hwnd,UINT msg,UINT id,DWORD time)
{
	KillTimer(hwnd,id);
	Disabled=FALSE;
}


BOOL CALLBACK EditMasterCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)	
{	 static ImageFilter_LensF *flt = NULL;

	 if( Disabled && (message!=WM_PAINT) && (message!=WM_ERASEBKGND) )
		 return DefWindowProc(hWnd,message,wParam,lParam);

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
		 SelectObject(hdcBitmap,MasterBackBitmap);
		 BitBlt(hdc,0,0,rect.right,rect.bottom,hdcBitmap,0,0,SRCCOPY);
		 DeleteDC(hdcBitmap);
		 return TRUE;
	}

     if (message == WM_INITDIALOG) flt = (ImageFilter_LensF *)lParam;
     if (flt) return (flt->Master_Control(hWnd,message,wParam,lParam));
     else return(FALSE);
}

BOOL ImageFilter_LensF::ShowControl(HWND hWnd) 
{	 
//	 void *Mem=(void *)malloc(EvaluateConfigure());		// Take Temporary Copy
//	 SaveConfigure(Mem);

	 unsigned int code=GetCode();
	 if (code!=encr(Code())) {
		if( Register(hWnd)==1 ) {
			if (GetCode()!=encr(Code())) {
				MessageBox(hWnd,"Registration information incorrect!","Error",MB_OK);
			}
		}
	 }


	 int retval;
	 MasterBlackOutFlag			=0;					// todo : Allocate & Add Temporary memory Store
	 ImageFilterData			=this;

	 MasterRealTimeDrawn		=0;

	 data.SelectedElement[0]=1;
	 for(int i=1;i!=MAXFLARES;i++) data.SelectedElement[i]=0;

	 hwPreviewMasterMem			=NULL;
	 hwSmallPreviewMasterMem	=NULL;

	 retval=DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_MASTER),hWnd,(DLGPROC)EditMasterCtrlDlgProc,(LPARAM)this);
	 DeleteObject(MasterBackBitmap);

	 if (hwPreviewMasterMem)		free(hwPreviewMasterMem);
	 if (hwSmallPreviewMasterMem)	free(hwSmallPreviewMasterMem);

//	 if (!FlaresEditFlag) LoadConfigure(Mem);		// Recover Temporary Copy If Need Be

//	 free(Mem);
	 return retval;
}