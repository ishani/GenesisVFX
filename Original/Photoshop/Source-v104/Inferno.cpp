// #####################################################################
// Changed 24.03.97
//		Changes In Brush Handling (uughh), store oldbrush for w95 friendliness

static ICustButton		*FType1Button		= NULL;
static ICustButton		*FType2Button		= NULL;

static ICustButton		*InfernoOK			= NULL;
static ICustButton		*InfernoCancel		= NULL;

static ISpinnerControl	*SizeX1Spin			= NULL;
static ISpinnerControl	*SizeY1Spin			= NULL;
static ISpinnerControl	*SizeX2Spin			= NULL;
static ISpinnerControl	*SizeY2Spin			= NULL;
static ICustButton		*LockSize1Button	= NULL;
static ICustButton		*LockSize2Button	= NULL;

static ISpinnerControl	*DeformX1Spin		= NULL;
static ISpinnerControl	*DeformY1Spin		= NULL;
static ISpinnerControl	*DeformX2Spin		= NULL;
static ISpinnerControl	*DeformY2Spin		= NULL;
static ICustButton		*LockDeform1Button	= NULL;
static ICustButton		*LockDeform2Button	= NULL;

static ISpinnerControl	*AnimationSpd1		= NULL;
static ISpinnerControl	*AnimationSpd2		= NULL;

static ISpinnerControl	*LevelsSpin			= NULL;
static ISpinnerControl	*FrequencySpin		= NULL;
static ISpinnerControl	*Seed2Spin			= NULL;

//*********************************************************
// Editing Dialog Flag
int FlareInfernoDialogBoxExit;

// Preview Stuff
char			*FlareInfernoTempBitmap;
BITMAPINFO		FlareInfernoTempBitmapInfo;
static Flare	Flare_InfernoPreview(0.5f,0.0f);
HWND			hwInfernoPreview;				// Preview Windows;

//****************************************************************
// Preview Stuff
void ImageFilter_LensF::InfernoPreview(int CheckerBoard)
{	SetCursor(LoadCursor(NULL,IDC_WAIT));
    HDC		hdc = GetDC(hwInfernoPreview);
	RECT	rect; GetClientRect(hwInfernoPreview,&rect);

	int lxres=(rect.right-rect.left)&0xfffffff0;
	int lyres=(rect.bottom-rect.top); 

	if (FlareInfernoTempBitmap==NULL)
			{ FlareInfernoTempBitmapInfo.bmiHeader.biSize		=sizeof(BITMAPINFOHEADER);
			  FlareInfernoTempBitmapInfo.bmiHeader.biPlanes		=1;
			  FlareInfernoTempBitmapInfo.bmiHeader.biBitCount	=24;
			  FlareInfernoTempBitmapInfo.bmiHeader.biCompression=BI_RGB;
			  FlareInfernoTempBitmapInfo.bmiHeader.biClrUsed	=0;
			  FlareInfernoTempBitmapInfo.bmiHeader.biClrImportant=0;
			  FlareInfernoTempBitmapInfo.bmiHeader.biWidth		=lxres;
			  FlareInfernoTempBitmapInfo.bmiHeader.biHeight		=lyres;
			  FlareInfernoTempBitmapInfo.bmiHeader.biSizeImage	=lxres*lyres*3;
			  FlareInfernoTempBitmap=(char *)malloc(lxres*lyres*3);
			}

	// Render It
	Flare_InfernoPreview.RenderPreview(FlareInfernoTempBitmap,lxres,lyres,CheckerBoard,0.5f,(float)data.AnimationPosition,1.0f);

	// Display It
	SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareInfernoTempBitmap,&FlareInfernoTempBitmapInfo,DIB_RGB_COLORS);
	
	ReleaseDC(hwInfernoPreview,hdc);
	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

BOOL CALLBACK InfernoPreviewProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  switch (msg)    { case WM_RBUTTONDOWN:{ ImageFilterData->InfernoPreview(ImageFilterData->data.DrawChecker);
											  break;
											}
						case WM_PAINT:		{ HDC		hdc = GetDC(hwInfernoPreview);
											  RECT		rect;
											  GetClientRect(hwInfernoPreview,&rect);

											  int lxres=(rect.right-rect.left)&0xfffffff0;
											  int lyres=rect.bottom-rect.top;

											  if (FlareInfernoTempBitmap!=0) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,FlareInfernoTempBitmap,
											                                                &FlareInfernoTempBitmapInfo,DIB_RGB_COLORS);
											  else		{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
											              HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);			// 24.03.97
											              Rectangle(hdc,rect.left,rect.top,rect.left+(rect.right-rect.left)&0xfffffff0,rect.bottom);
														  SelectObject(hdc,oldbrush);						// 24.03.97
											              DeleteObject(brush);
											            }

											  ReleaseDC(hwInfernoPreview,hdc);
											  break;
											}
    	}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	} 

//*********************************************************
// Get Current State Of All Variables
void ImageFilter_LensF::Inferno_GetControls(HWND hWnd)
{		Flare_InfernoPreview.Inferno.UseSimpleNoise=FType1Button->IsChecked();
		Flare_InfernoPreview.Inferno.UseFracNoise  =FType2Button->IsChecked();

		Flare_InfernoPreview.Inferno.SimpleSizeLock=LockSize2Button->IsChecked();
		Flare_InfernoPreview.Inferno.FracSizeLock=LockSize1Button->IsChecked();
		Flare_InfernoPreview.Inferno.SimpleDeformLock=LockDeform2Button->IsChecked();
		Flare_InfernoPreview.Inferno.FracDeformLock=LockDeform1Button->IsChecked();

		Flare_InfernoPreview.Inferno.FracXScale=SizeX1Spin->GetFVal();
		Flare_InfernoPreview.Inferno.FracYScale=SizeY1Spin->GetFVal();

		Flare_InfernoPreview.Inferno.SimpleXScale=SizeX2Spin->GetFVal();
		Flare_InfernoPreview.Inferno.SimpleYScale=SizeY2Spin->GetFVal();

		Flare_InfernoPreview.Inferno.FracXSize=DeformX1Spin->GetFVal();
		Flare_InfernoPreview.Inferno.FracYSize=DeformY1Spin->GetFVal();

		Flare_InfernoPreview.Inferno.SimpleXSize=DeformX2Spin->GetFVal();
		Flare_InfernoPreview.Inferno.SimpleYSize=DeformY2Spin->GetFVal();

		Flare_InfernoPreview.Inferno.FracPhaseSpeed=AnimationSpd1->GetFVal();
		Flare_InfernoPreview.Inferno.SimplePhaseSpeed=AnimationSpd2->GetFVal();

		Flare_InfernoPreview.Inferno.SimpleLevels=LevelsSpin->GetIVal();
		Flare_InfernoPreview.Inferno.FracFreq=FrequencySpin->GetFVal();

		Flare_InfernoPreview.Inferno.Seed=Seed2Spin->GetIVal();
}

void ImageFilter_LensF::Inferno_SetControls(HWND hWnd)
{		FType2Button->SetCheck(Flare_InfernoPreview.Inferno.UseFracNoise);
		FType1Button->SetCheck(Flare_InfernoPreview.Inferno.UseSimpleNoise);

		LockSize2Button->SetCheck(Flare_InfernoPreview.Inferno.SimpleSizeLock);
		LockSize1Button->SetCheck(Flare_InfernoPreview.Inferno.FracSizeLock);

		LockDeform2Button->SetCheck(Flare_InfernoPreview.Inferno.SimpleDeformLock);
		LockDeform1Button->SetCheck(Flare_InfernoPreview.Inferno.FracDeformLock);

		SizeX1Spin->SetValue(Flare_InfernoPreview.Inferno.FracXScale,NULL);
		SizeY1Spin->SetValue(Flare_InfernoPreview.Inferno.FracYScale,NULL);

		SizeX2Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleXScale,NULL);
		SizeY2Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleYScale,NULL);

		DeformX1Spin->SetValue(Flare_InfernoPreview.Inferno.FracXSize,NULL);
		DeformY1Spin->SetValue(Flare_InfernoPreview.Inferno.FracYSize,NULL);

		DeformX2Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleXSize,NULL);
		DeformY2Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleYSize,NULL);

		AnimationSpd1->SetValue(Flare_InfernoPreview.Inferno.FracPhaseSpeed,NULL);
		AnimationSpd2->SetValue(Flare_InfernoPreview.Inferno.SimplePhaseSpeed,NULL);

		LevelsSpin->SetValue(Flare_InfernoPreview.Inferno.SimpleLevels,NULL);
		FrequencySpin->SetValue(Flare_InfernoPreview.Inferno.FracFreq,NULL);
		Seed2Spin->SetValue(Flare_InfernoPreview.Inferno.Seed,NULL);

		if( Flare_InfernoPreview.Inferno.UseFracNoise ) {
			SizeX1Spin->Enable();
			SizeY1Spin->Enable();
			DeformX1Spin->Enable();
			DeformY1Spin->Enable();
			AnimationSpd1->Enable();
			FrequencySpin->Enable();
			LockSize1Button->Enable();
			LockDeform1Button->Enable();
		} else {
			SizeX1Spin->Disable();
			SizeY1Spin->Disable();
			DeformX1Spin->Disable();
			DeformY1Spin->Disable();
			AnimationSpd1->Disable();
			FrequencySpin->Disable();
			LockSize1Button->Disable();
			LockDeform1Button->Disable();
		}

		if( Flare_InfernoPreview.Inferno.UseSimpleNoise ) {
			SizeX2Spin->Enable();
			SizeY2Spin->Enable();
			DeformX2Spin->Enable();
			DeformY2Spin->Enable();
			AnimationSpd2->Enable();
			LevelsSpin->Enable();
			LockSize2Button->Enable();
			LockDeform2Button->Enable();
		} else {
			SizeX2Spin->Disable();
			SizeY2Spin->Disable();
			DeformX2Spin->Disable();
			DeformY2Spin->Disable();
			AnimationSpd2->Disable();
			LevelsSpin->Disable();
			LockSize2Button->Disable();
			LockDeform2Button->Disable();
		}

		CheckDlgButton(hWnd,IDC_NOISECHECKER,data.DrawChecker);
		CheckDlgButton(hWnd,IDC_APV,data.InfernoActivePreview);
}


HIMAGELIST		HIL_Inferno_OK;
HIMAGELIST		HIL_Inferno_Cancel;
HIMAGELIST		HIL_Inferno_Turb;
HIMAGELIST		HIL_Inferno_Regular;
HIMAGELIST		HIL_Lock1;
HIMAGELIST		HIL_Lock2;
HIMAGELIST		HIL_Lock3;
HIMAGELIST		HIL_Lock4;

void ImageFilter_LensF::InfernoBuildButtons(void)
{		BuildButton(InfernoOK,HIL_Inferno_OK,IDB_MASTEROK,IDB_MASTEROK,100,50);
		BuildButton(InfernoCancel,HIL_Inferno_Cancel,IDB_MASTERCANCEL,IDB_MASTERCANCEL,100,50);

		BuildButton(FType1Button,HIL_Inferno_Turb,IDB_INFERNOUP1,IDB_INFERNODOWN2,100,50);
		BuildButton(FType2Button,HIL_Inferno_Regular,IDB_INFERNOUP2,IDB_INFERNODOWN1,100,50);

		BuildButton(LockSize2Button,HIL_Lock1,IDB_REGLOCKUP,IDB_REGLOCKDOWN,48,50);
		BuildButton(LockSize1Button,HIL_Lock2,IDB_TURLOCKUP,IDB_TURLOCKDOWN,48,50);
		BuildButton(LockDeform2Button,HIL_Lock3,IDB_REGLOCKUP,IDB_REGLOCKDOWN,48,50);
		BuildButton(LockDeform1Button,HIL_Lock4,IDB_TURLOCKUP,IDB_TURLOCKDOWN,48,50);
}

void ImageFilter_LensF::InfernoFreeButtons(void)
{		ReleaseButton(HIL_Inferno_OK);
		ReleaseButton(HIL_Inferno_Cancel);
		ReleaseButton(HIL_Inferno_Turb);
		ReleaseButton(HIL_Inferno_Regular);
		ReleaseButton(HIL_Lock1);
		ReleaseButton(HIL_Lock2);
		ReleaseButton(HIL_Lock3);
		ReleaseButton(HIL_Lock4);
}

// release controls
void ImageFilter_LensF::InfernoReleaseButtons()
{
					ReleaseICustButton(FType1Button); 
					ReleaseICustButton(FType2Button); 

					ReleaseSpinner(SizeX1Spin);
					ReleaseSpinner(SizeY1Spin);

					ReleaseSpinner(SizeX2Spin);
					ReleaseSpinner(SizeY2Spin);

					ReleaseSpinner(DeformX1Spin);
					ReleaseSpinner(DeformY1Spin);

					ReleaseSpinner(DeformX2Spin);
					ReleaseSpinner(DeformY2Spin);

					ReleaseSpinner(AnimationSpd1);
					ReleaseSpinner(AnimationSpd2);

					ReleaseSpinner(LevelsSpin);
					ReleaseSpinner(FrequencySpin);
					ReleaseSpinner(Seed2Spin);

					ReleaseICustButton(LockSize1Button); 
					ReleaseICustButton(LockSize2Button); 
					ReleaseICustButton(LockDeform1Button); 
					ReleaseICustButton(LockDeform2Button); 

					ReleaseICustButton(InfernoOK); 
					ReleaseICustButton(InfernoCancel); 
}

//*********************************************************
// Main Control

HBITMAP InfernoLoadBitmap;
HBITMAP InfernoBackBitmap;

BOOL ImageFilter_LensF::Inferno_Control(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
  if( message==WM_COMMAND || message==CC_SPINNER_CHANGE )
	switch(LOWORD(wParam))
			{	case IDC_SIZEX1_SPIN:
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.FracXScale=SizeX1Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.FracSizeLock) 
												{ Flare_InfernoPreview.Inferno.FracYScale=Flare_InfernoPreview.Inferno.FracXScale;
										          		SizeY1Spin->SetValue(Flare_InfernoPreview.Inferno.FracYScale,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_SIZEX2_SPIN:
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.SimpleXScale=SizeX2Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.SimpleSizeLock) 
												{ Flare_InfernoPreview.Inferno.SimpleYScale=Flare_InfernoPreview.Inferno.SimpleXScale;
										          SizeY2Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleYScale,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_SIZEY1_SPIN:
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.FracYScale=SizeY1Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.FracSizeLock) 
												{ Flare_InfernoPreview.Inferno.FracXScale=Flare_InfernoPreview.Inferno.FracYScale;
										          SizeX1Spin->SetValue(Flare_InfernoPreview.Inferno.FracXScale,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_SIZEY2_SPIN:
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.SimpleYScale=SizeY2Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.SimpleSizeLock) 
												{ Flare_InfernoPreview.Inferno.SimpleXScale=Flare_InfernoPreview.Inferno.SimpleYScale;
										          SizeX1Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleXScale,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_DEFORMX1_SPIN:
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.FracXSize=DeformX1Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.FracDeformLock) 
												{ Flare_InfernoPreview.Inferno.FracYSize=Flare_InfernoPreview.Inferno.FracXSize;
										          DeformY1Spin->SetValue(Flare_InfernoPreview.Inferno.FracYSize,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_DEFORMX2_SPIN:
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.SimpleXSize=DeformX2Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.SimpleDeformLock) 
												{ Flare_InfernoPreview.Inferno.SimpleYSize=Flare_InfernoPreview.Inferno.SimpleXSize;
										          DeformY2Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleYSize,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_DEFORMY1_SPIN:	
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.FracYSize=DeformY1Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.FracDeformLock) 
												{ Flare_InfernoPreview.Inferno.FracXSize=Flare_InfernoPreview.Inferno.FracYSize;
										          DeformX1Spin->SetValue(Flare_InfernoPreview.Inferno.FracXSize,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_DEFORMY2_SPIN:	
									if( (message==CC_SPINNER_CHANGE) )
										{ Flare_InfernoPreview.Inferno.SimpleYSize=DeformY2Spin->GetFVal();
                                          if (Flare_InfernoPreview.Inferno.SimpleDeformLock) 
												{ Flare_InfernoPreview.Inferno.SimpleXSize=Flare_InfernoPreview.Inferno.SimpleYSize;
										          DeformX2Spin->SetValue(Flare_InfernoPreview.Inferno.SimpleXSize,NULL);
												}

										  if ((data.InfernoActivePreview)&&((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE)))
													InfernoPreview(data.DrawChecker);
										}
	                                      break;

				case IDC_ANIM1_SPIN:
				case IDC_ANIM2_SPIN:
				case IDC_FREQ_SPIN:
				case IDC_LEVELS_SPIN:
				case IDC_Seed2_SPIN:
					if ((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{ Inferno_GetControls(hWnd);
						          if (data.InfernoActivePreview) InfernoPreview(data.DrawChecker);
								}
								  break;
			}
	
	switch (message) 
		   { case WM_INITDIALOG: 
			   { CenterWindow(hWnd,GetParent(hWnd));
				 SetCursor(LoadCursor(NULL,IDC_ARROW));

			 // Setup background bitmap rcw 18/4/97
				// load bitmap and get window dc
				InfernoLoadBitmap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_NOISEDLG));
				HDC hdc=GetDC(hWnd);
				RECT	rect;
				BITMAP	bm;

				// get sizes of bitmap and window
				GetClientRect(hWnd,&rect);
				GetObject(InfernoLoadBitmap,sizeof(BITMAP),&bm);

				if( abs(rect.right-bm.bmWidth)<=2 && abs(rect.bottom-bm.bmHeight)<=2 )
					// keep original
					InfernoBackBitmap=InfernoLoadBitmap;
				else {
					// get the final background bitmap
					InfernoBackBitmap=CreateCompatibleBitmap(hdc,(rect.right+3)&0xfffffffc,rect.bottom);
					// interpolate new size
					// note this deletes old bitmap internally
					LinearResample(hdc,InfernoLoadBitmap,bm.bmWidth,bm.bmHeight,hdc,InfernoBackBitmap,rect.right,rect.bottom);
				}

				// clean up DC
				ReleaseDC(hWnd,hdc);

				//*************************
				 // Preview Window
					hwInfernoPreview = GetDlgItem(hWnd, IDC_INFERNOPREVIEW);
					SetWindowLong(hwInfernoPreview,GWL_WNDPROC,(LONG)InfernoPreviewProc);

				 //*************************
			     // Fractal Type Buttons
					FType1Button=GetICustButton(GetDlgItem(hWnd,IDC_FTYPE1)); 
					FType1Button->SetType(CBT_CHECK);
					
					FType2Button=GetICustButton(GetDlgItem(hWnd,IDC_FTYPE2)); 
					FType2Button->SetType(CBT_CHECK);

					SizeX1Spin=SetupFloatSpinner(hWnd,IDC_SIZEX1_SPIN,IDC_SIZEX1_EDIT,-20.0f,20.0f,0.0f,0.005f,0.0f);
					SizeY1Spin=SetupFloatSpinner(hWnd,IDC_SIZEY1_SPIN,IDC_SIZEY1_EDIT,-20.0f,20.0f,0.0f,0.005f,0.0f);

					SizeX2Spin=SetupFloatSpinner(hWnd,IDC_SIZEX2_SPIN,IDC_SIZEX2_EDIT,-20.0f,20.0f,0.0f,0.005f,0.0f);
					SizeY2Spin=SetupFloatSpinner(hWnd,IDC_SIZEY2_SPIN,IDC_SIZEY2_EDIT,-20.0f,20.0f,0.0f,0.005f,0.0f);

					DeformX1Spin=SetupFloatSpinner(hWnd,IDC_DEFORMX1_SPIN,IDC_DEFORMX1_EDIT,-20.0f,20.0f,0.0f,0.005f,0.0f);
					DeformY1Spin=SetupFloatSpinner(hWnd,IDC_DEFORMY1_SPIN,IDC_DEFORMY1_EDIT,-20.0f,20.0f,0.0f,0.005f,0.0f);

					DeformX2Spin=SetupFloatSpinner(hWnd,IDC_DEFORMX2_SPIN,IDC_DEFORMX2_EDIT,-20.0f,20.0f,0.0f,0.01f,0.0f);
					DeformY2Spin=SetupFloatSpinner(hWnd,IDC_DEFORMY2_SPIN,IDC_DEFORMY2_EDIT,-20.0f,20.0f,0.0f,0.01f,0.0f);

					AnimationSpd1=SetupFloatSpinner(hWnd,IDC_ANIM1_SPIN,IDC_ANIM1_EDIT,-20.0f,20.0f,0.0f,0.01f,0.0f);
					AnimationSpd2=SetupFloatSpinner(hWnd,IDC_ANIM2_SPIN,IDC_ANIM2_EDIT,-20.0f,20.0f,0.0f,0.01f,0.0f);

					LevelsSpin=SetupIntSpinner(hWnd,IDC_LEVELS_SPIN,IDC_LEVELS_EDIT,0,10,0,1,0);
					FrequencySpin=SetupFloatSpinner(hWnd,IDC_FREQ_SPIN,IDC_FREQ_EDIT,1.0f,10.0f,1.0f,0.01f,1.0f);
					Seed2Spin=SetupIntSpinner(hWnd,IDC_Seed2_SPIN,IDC_Seed2_EDIT,0,9999,0,1,0);

					LockSize1Button=GetICustButton(GetDlgItem(hWnd,IDC_SZLINK1)); 
					LockSize1Button->SetType(CBT_CHECK);
					
					LockSize2Button=GetICustButton(GetDlgItem(hWnd,IDC_SZLINK2)); 
					LockSize2Button->SetType(CBT_CHECK);

					LockDeform1Button=GetICustButton(GetDlgItem(hWnd,IDC_DFLINK1)); 
					LockDeform1Button->SetType(CBT_CHECK);
					
					LockDeform2Button=GetICustButton(GetDlgItem(hWnd,IDC_DFLINK2)); 
					LockDeform2Button->SetType(CBT_CHECK);

					InfernoOK=GetICustButton(GetDlgItem(hWnd,IDC_INFERNO_OK)); 
					InfernoCancel=GetICustButton(GetDlgItem(hWnd,IDC_INFERNO_CANCEL)); 

					if (data.InfernoActivePreview) InfernoPreview(data.DrawChecker);

					InfernoBuildButtons();

					Inferno_SetControls(hWnd);
				 break;
			   }

			 case WM_COMMAND:
				 switch (LOWORD(wParam)) 
					{ case IDC_FTYPE1:
				      case IDC_FTYPE2:	{ 
										  if( HIWORD(wParam)==BN_CLICKED ) 
													{ Inferno_GetControls(hWnd);
						                              Inferno_SetControls(hWnd);
													  if (data.InfernoActivePreview) InfernoPreview(data.DrawChecker);
													}
									      break;
										}

					  case IDC_SZLINK1:
					  case IDC_SZLINK2:
					  case IDC_DFLINK1:
					  case IDC_DFLINK2:	{ if (HIWORD(wParam)==BN_CLICKED) Inferno_GetControls(hWnd);
						                  break;
										}

					  case IDC_NOISECHECKER:	{ int tmp=data.DrawChecker;

										  data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_NOISECHECKER); 

										  if ((tmp!=data.DrawChecker)&&(data.InfernoActivePreview)) 
													InfernoPreview(data.DrawChecker);
										  break; 
										}

					  case IDC_APV:		{ int tmp=data.InfernoActivePreview;

										  data.InfernoActivePreview=IsDlgButtonChecked(hWnd,IDC_APV); 

										  if ((tmp!=data.InfernoActivePreview)&&(data.InfernoActivePreview)) 
													InfernoPreview(data.DrawChecker);
										  break; 
										}

				 
				      case IDOK:
					  case IDC_INFERNO_OK:
									{ 
									  if( HIWORD(wParam)==BN_BUTTONUP ) {
										FlareInfernoDialogBoxExit=1;
									   InfernoFreeButtons();
									   InfernoReleaseButtons();
									   EndDialog(hWnd,1); 
									  }
									   break; 
									 }
					  case IDCANCEL:
					  case IDC_INFERNO_CANCEL:
									{ 
									  if( HIWORD(wParam)==BN_BUTTONUP ) {
										FlareInfernoDialogBoxExit=0;
									   InfernoFreeButtons();
									   InfernoReleaseButtons();
						               EndDialog(hWnd,0);
									  }
									   break; 
									 }
					}
				return 1;
		   }

		return 0;
}

int initing;
BOOL CALLBACK InfernoMasterCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)	
{	 static ImageFilter_LensF *flt = NULL;

	 if( message == WM_PAINT ) {
		 if( initing ) {
			 if( flt ) flt->Inferno_SetControls(hWnd);
			 initing=FALSE;
		 }
		 PAINTSTRUCT ps;
		 BeginPaint(hWnd,&ps);
		 EndPaint(hWnd,&ps);
	 }

	 if( message == WM_ERASEBKGND ) {
		 HDC hdc=(HDC)wParam;
		 RECT	rect;
		 GetClientRect(hWnd,&rect);
		 HDC hdcBitmap=CreateCompatibleDC(hdc);
		 SelectObject(hdcBitmap,InfernoBackBitmap);
		 BitBlt(hdc,0,0,rect.right,rect.bottom,hdcBitmap,0,0,SRCCOPY);
		 DeleteDC(hdcBitmap);
		 return TRUE;
	}
	 if (message == WM_INITDIALOG) initing=TRUE;
     if (message == WM_INITDIALOG) flt = (ImageFilter_LensF *)lParam;
     if (flt) return (flt->Inferno_Control(hWnd,message,wParam,lParam));
     else return(FALSE);
}

BOOL ImageFilter_LensF::ShowInfernoControl(HWND hWnd, Flare *Edit) 
{	 float PositionX  =Edit->PositionX;
	 float PositionY  =Edit->PositionY;
	 float Position   =Edit->Position ;
	 float Angle      =Edit->Angle;
	 float FlareSize  =Edit->FlareSize;
	 float AngleOffSet=Edit->AngleOffSet;
	 float Colourise  =Edit->Colourise;
	 float Depth	  =Edit->Depth;
	 //float Intensity  =Edit->Intensity;

	 Flare_InfernoPreview.GetFlare(Edit,1); 

	 Flare_InfernoPreview.FlareSize  =0.5f;
	 Flare_InfernoPreview.PositionX  =0.0f;
	 Flare_InfernoPreview.PositionY  =0.0f;
	 Flare_InfernoPreview.Position   =0.0f;
	 Flare_InfernoPreview.Angle      =0.0f;
	 Flare_InfernoPreview.AngleOffSet=0.0f;
	 Flare_InfernoPreview.Colourise  =0.0f;
	 //Flare_InfernoPreview.Light      =1;
	 Flare_InfernoPreview.Active	   =1;
	 Flare_InfernoPreview.Depth	   =0.0f;
	 //Flare_InfernoPreview.Intensity  =1.0f;

	 FlareInfernoTempBitmap=NULL;

	 DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_FNOISE),hWnd,(DLGPROC)InfernoMasterCtrlDlgProc,(LPARAM)this);
	 DeleteObject(InfernoBackBitmap);

	 if (FlareInfernoDialogBoxExit) { Edit->GetFlare(&Flare_InfernoPreview,1); 
								      Edit->PositionX=PositionX;
								      Edit->PositionY=PositionY;
								      Edit->Position=Position;
								      Edit->Angle=Angle;
								      Edit->FlareSize=FlareSize;
								      Edit->AngleOffSet=AngleOffSet;
								      Edit->Colourise=Colourise;
									  Edit->Depth=Depth;
									  Edit->IR2=Edit->Intensity*Edit->FlareSize*Edit->FlareSize;
								  }

	 if (FlareInfernoTempBitmap) free(FlareInfernoTempBitmap);

	 return FlareInfernoDialogBoxExit;
}