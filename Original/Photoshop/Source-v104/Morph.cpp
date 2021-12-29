// #####################################################################
// Changed 24.03.97
//		Changes In Brush Handling (uughh), store oldbrush for w95 friendliness

static ISpinnerControl	*SpinMorphNumber				=NULL;
static ISpinnerControl	*SpinMorphSizeVariation			=NULL;
static ISpinnerControl	*SpinMorphLightVariation		=NULL;
static ISpinnerControl	*SpinMorphDistanceVariation		=NULL;
static ISpinnerControl	*SpinMorphAngleOffsetVariation	=NULL;
static ISpinnerControl	*SpinMorphXPositionVariation	=NULL;
static ISpinnerControl	*SpinMorphYPositionVariation	=NULL;
static ISpinnerControl	*SpinMorphColouriseVariation	=NULL;
static ISpinnerControl	*SpinMorphInterplay				=NULL;
static ISpinnerControl	*SpinMorphAngleVariation		=NULL;
static ISpinnerControl	*SpinMorphSeed					=NULL;
static ISpinnerControl	*SpinMorphDepthVariation		=NULL;
static ISpinnerControl	*SpinMorphOcclusionVariation	=NULL;
static ISpinnerControl	*SpinMorphTime					=NULL;
static ICustButton		*SIButton						=NULL;
static ICustButton		*MorphFwd						=NULL;
static ICustButton		*MorphBck						=NULL;
static ICustButton		*MorphOK						=NULL;
static ICustButton		*MorphCancel					=NULL;
static HWND				hwndComboFrom					=NULL;
static HWND				hwndComboTo						=NULL;

// Andrew Mega ScrollBar
HWND			hwndMorphScrollBar;

// Stuff For Preview Window
char			*hwPreviewMorphMem;
BITMAPINFO		hwPreviewMorphInfo;
HWND			hwPreview_Morph;
int				MorphRealTimeDrawn;
float			MorphEditX=-1.0f,MorphEditY=-1.0f;

// Memory and Bitmap Info For Preview Backup
HWND			hwFromPreview_Morph;
char			*hwFromPreviewMorphMem;
BITMAPINFO		hwFromPreviewMorphInfo;

HWND			hwToPreview_Morph;
char			*hwToPreviewMorphMem;
BITMAPINFO		hwToPreviewMorphInfo;

// Blackout Preview
int				MorphBlackOutFlag;

#define Morph_DrawAll Master_DrawAll
#define Morph_DrawSelectedLo Master_DrawSelectedLo
#define Morph_DrawSelectedHi Master_DrawSelectedHi

//****************************************************************************************
// MEGA SCROLLBAR
void ImageFilter_LensF::MorphMegaScrollBar(void)
{				HDC		hdc = GetDC(hwndMorphScrollBar);
				RECT	rect; 
				GetClientRect(hwndMorphScrollBar,&rect);

				int lxres=rect.right-rect.left;
				int lyres=rect.bottom-rect.top;

				HRGN	ClipRegion=CreateRectRgn(rect.left,rect.top,rect.left+lxres,rect.top+lyres);
				SelectClipRgn(hdc,ClipRegion);

				// Draw Background
				HBRUSH brush=CreateSolidBrush(RGB(0,0,100));
				HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);						// 24.03.97
				Rectangle(hdc,rect.left,rect.top,rect.left+lxres,rect.bottom);
				SelectObject(hdc,oldbrush);												// 24.03.97
				DeleteObject(brush);

				// So Markers Fit nicer
				lxres-=6;
				rect.right-=3;
				rect.left+=2;

				// Draw Cursor Position
				float tPos=(float)lxres/(float)data.AnimationMax;
				
				// Draw Position Keys
				HBRUSH PosSel		=CreateSolidBrush(RGB(128,255,128));
				HBRUSH PosUnSel		=CreateSolidBrush(RGB(0,100,50));
				HBRUSH ShpSel		=CreateSolidBrush(RGB(255,128,128));
				HBRUSH ShpUnSel		=CreateSolidBrush(RGB(100,0,50));
				oldbrush=(HBRUSH)SelectObject(hdc,PosUnSel);								// 24.03.97

				for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
				if ((i!=data.MorphSelectFrom)&&(i!=data.MorphSelectTo))
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,PosUnSel);

				            for(int j=0;j!=tmp->NoPosKeys;j++)
									{ float Posn=tPos*(float)tmp->PosKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}
				
				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				if ((i!=data.MorphSelectFrom)&&(i!=data.MorphSelectTo))
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,ShpUnSel);

				            for(int j=0;j!=tmp->NoShpKeys;j++)
									{ float Posn=tPos*(float)tmp->ShpKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}
				
				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				if ((i==data.MorphSelectFrom)||(i==data.MorphSelectTo))
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,PosSel);

				            for(int j=0;j!=tmp->NoPosKeys;j++)
									{ float Posn=tPos*(float)tmp->PosKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}

				for(i=0;i!=data.LensFlare.GetNoFlares();i++)
				if ((i==data.MorphSelectFrom)||(i==data.MorphSelectTo))
						{	Flare *tmp=data.LensFlare.GetFlare(i); 
				            SelectObject(hdc,ShpSel);

				            for(int j=0;j!=tmp->NoShpKeys;j++)
									{ float Posn=tPos*(float)tmp->ShpKeys[j].Time+rect.left;
							          Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
									}
						}
				
				SelectObject(hdc,oldbrush);
				DeleteObject(PosSel);
				DeleteObject(PosUnSel);
				DeleteObject(ShpSel);
				DeleteObject(ShpUnSel);

				// Draw Cursor Position
				brush=CreateSolidBrush(RGB(192,192,255));
				oldbrush=(HBRUSH)SelectObject(hdc,brush);					// 24.03.97
				float Posn=tPos*(float)data.AnimationPosition+rect.left;
				Rectangle(hdc,(int)Posn-3,rect.top,(int)Posn+3,rect.bottom);
				SelectObject(hdc,oldbrush);									// 24.03.97
				DeleteObject(brush);

				DeleteObject(ClipRegion);
				ReleaseDC(hwndMorphScrollBar,hdc);
}

BOOL CALLBACK MorphMScrollBarProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);
	  if (((msg==WM_LBUTTONDOWN)&&(wParam&MK_LBUTTON))||
		  ((msg==WM_MOUSEMOVE  )&&(wParam&MK_LBUTTON)))
								{
								  HDC		hdc = GetDC(hwndMorphScrollBar);
								  RECT		rect; 
								  GetClientRect(hwndMorphScrollBar,&rect);

								  int lxres=rect.right-rect.left;

				                  lxres-=6;
								  rect.right-=3;
								  rect.left+=2;

								  int xps=LOWORD(lParam);
								  if (xps>4096) xps=0;  //Wider Than Any Sceen I know !

								  float NewPosn=(float)(xps-rect.left)/lxres;
								  NewPosn*=ImageFilterData->data.AnimationMax;
								  if( NewPosn<0 ) NewPosn=0.0f;

								  ImageFilterData->data.AnimationPosition=(int)(NewPosn+0.5f);

								  ReleaseDC(hwndMorphScrollBar,hdc);

								  if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);

								  // Get New Time Value
								  SpinMorphTime->SetValue(ImageFilterData->data.AnimationPosition,NULL);

								  // Get New Positions
								  ImageFilterData->data.LensFlare.GetFrame(ImageFilterData->data.AnimationPosition);

								  // Update All Display
								  ImageFilterData->MorphMegaScrollBar();
								  ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								  ImageFilterData->FromMorphPreview(ImageFilterData->data.DrawChecker);
								  ImageFilterData->ToMorphPreview(ImageFilterData->data.DrawChecker);

								  if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
								}
			else if (msg==WM_LBUTTONUP)	{ ReleaseCapture();
			                              if (ImageFilterData->data.AnimationPosition>ImageFilterData->data.AnimationMax) 
													ImageFilterData->data.AnimationMax=ImageFilterData->data.AnimationPosition;
										  else	{	int MPos=ImageFilterData->data.LensFlare.MaxKeyValue();
													if (MPos>ImageFilterData->data.AnimationPosition) ImageFilterData->data.AnimationMax=MPos;
													else ImageFilterData->data.AnimationMax=ImageFilterData->data.AnimationPosition;

													if (ImageFilterData->data.AnimationMax<100) ImageFilterData->data.AnimationMax=100;
												}

										  ImageFilterData->MorphMegaScrollBar();
										}
			else if (msg==WM_RBUTTONDOWN)
								{ // Find Closest Key Frame 
				                  HDC		hdc = GetDC(hwndMorphScrollBar);
								  RECT		rect; 
								  GetClientRect(hwndMorphScrollBar,&rect);

								  int lxres=rect.right-rect.left;

				                  lxres-=6;
								  rect.right-=3;
								  rect.left+=2;

								  float NewPosn=(float)((int)LOWORD(lParam)-rect.left)/lxres;
								  NewPosn*=ImageFilterData->data.AnimationMax;
								  int GetPosn=(int)(NewPosn+0.5f);
								  ReleaseDC(hwndMorphScrollBar,hdc);

								  int NearestPosn=999999;
								  int Distance=999999;

								  for(int i=0;i!=ImageFilterData->data.LensFlare.GetNoFlares();i++)
								  if ((i==ImageFilterData->data.MorphSelectFrom)||(i==ImageFilterData->data.MorphSelectTo))
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

								  if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);

								  // Get New Time Value
								  SpinMorphTime->SetValue(ImageFilterData->data.AnimationPosition,NULL);

								  // Get New Positions
								  ImageFilterData->data.LensFlare.GetFrame(ImageFilterData->data.AnimationPosition);

								  // Update All Display						
								  ImageFilterData->MorphMegaScrollBar();
								  ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								  ImageFilterData->FromMorphPreview(ImageFilterData->data.DrawChecker);
								  ImageFilterData->ToMorphPreview(ImageFilterData->data.DrawChecker);
								}
			else if (msg==WM_PAINT)	ImageFilterData->MorphMegaScrollBar();

      return DefWindowProc(hwnd,msg,wParam,lParam);
	}
//****************************************************************************************


//*****************************************************************************************************
// Get Values From Dialog Box
void ImageFilter_LensF::MorphGetValues(HWND hWnd)
{				data.MorphSizeVariation=SpinMorphSizeVariation->GetFVal();
				data.MorphLightVariation=SpinMorphLightVariation->GetFVal();
				data.MorphDistanceVariation=SpinMorphDistanceVariation->GetFVal();
				data.MorphAngleVariation=SpinMorphAngleVariation->GetFVal();
				data.MorphAngleOffsetVariation=SpinMorphAngleOffsetVariation->GetFVal();
				data.MorphXPositionVariation=SpinMorphXPositionVariation->GetFVal();
				data.MorphYPositionVariation=SpinMorphYPositionVariation->GetFVal();				
				data.MorphColouriseVariation=SpinMorphColouriseVariation->GetFVal();
				data.MorphNumber=SpinMorphNumber->GetIVal();
				data.MorphSeed=SpinMorphSeed->GetIVal();
				data.MorphInterplay=SIButton->IsChecked();
				GetDlgItemText(hWnd,IDC_NAMEEDIT,data.MorphName,240);
				data.MorphSelectFrom=SendMessage(hwndComboFrom,CB_GETCURSEL,0,0);
				data.MorphSelectTo  =SendMessage(hwndComboTo,CB_GETCURSEL,0,0);
				data.MorphDepthVariation=SpinMorphDepthVariation->GetFVal();
				data.MorphOcclusionVariation=SpinMorphOcclusionVariation->GetFVal();
}

//*****************************************************************************************************
// Create The Actual Morph
void ImageFilter_LensF::MorphDelete(void)
{				for(int i=0;i!=data.MorphNumber;i++)
						data.LensFlare.Delete(data.LensFlare.GetNoFlares()-1);
}

//********************** 12.4.97 *****************************
int MorphXRes,MorphYRes;
void ImageFilter_LensF::MorphCreate(void)
{				SetCursor(LoadCursor(NULL,IDC_WAIT));

				if ((MorphEditX==-1)&&(MorphEditY==-1)) { MorphEditX=MorphXRes/8.0f; MorphEditY=MorphYRes/2.0f; }

				int i;
				Flare *tmpFrom=data.LensFlare.GetFlare(data.MorphSelectFrom);
				Flare *tmpTo  =data.LensFlare.GetFlare(data.MorphSelectTo);

				float dT=1.0f/(data.MorphNumber+1.0f);
				float T =dT;

				srand48(data.MorphSeed);

				for(i=0;i!=data.MorphNumber;i++,T+=dT)
						{ // Copy Animation Frames
					      data.LensFlare.Copy(data.MorphSelectFrom);
						  sprintf(data.LensFlare.GetName(data.LensFlare.GetNoFlares()-1),"%s [%d]\0",data.MorphName,i+1);
						  Flare *tmpFlare=data.LensFlare.GetFlare(data.LensFlare.GetNoFlares()-1);

						  // Add From Positional Keys
						  for(int ii=0;ii!=tmpFlare->NoPosKeys;ii++)
								{ tmpFrom->GetPosValues(ii); 
						          tmpTo  ->GetPosition(tmpFlare->PosKeys[ii].Time);

								  if (tmpFrom->Polar!=tmpTo->Polar)
											{ if (tmpFrom->Polar) tmpTo->ToPolar(0);
								              else tmpTo->FromPolar(0);
											}

								  if ((tmpFrom->OffScreenRender&2)!=(tmpTo->OffScreenRender&2))
											{ int tmp=tmpTo->OffScreenRender;
								              tmpTo->ToAbsolute(MorphXRes,MorphYRes,MorphEditX,MorphEditY,data.CameraDepth,(float)tmpFlare->PosKeys[ii].Time,0,tmp,tmpFrom->OffScreenRender);
                                              tmpTo->OffScreenRender=tmp;
											}

							      tmpFlare->PositionX  =T*tmpFrom->PositionX+(1.0f-T)*tmpTo->PositionX+Distrib(data.MorphXPositionVariation);
							      tmpFlare->PositionY  =T*tmpFrom->PositionY+(1.0f-T)*tmpTo->PositionY+Distrib(data.MorphYPositionVariation);
							      tmpFlare->Position   =T*tmpFrom->Position +(1.0f-T)*tmpTo->Position+Distrib(data.MorphDistanceVariation);
						          tmpFlare->Angle      =T*tmpFrom->Angle    +(1.0f-T)*tmpTo->Angle+Distrib(data.MorphAngleVariation);

						          tmpFlare->Depth =T*tmpFrom->Depth +(1.0f-T)*tmpTo->Depth+Distrib(data.MorphDepthVariation);
								  
								  tmpFlare->Occlusion  =T*tmpFrom->Occlusion+(1.0f-T)*tmpTo->Occlusion+Distrib(data.MorphOcclusionVariation);
						          tmpFlare->Intensity  =T*tmpFrom->Intensity+(1.0f-T)*tmpTo->Intensity;

						          do { tmpFlare->FlareSize=T*tmpFrom->FlareSize+(1.0f-T)*tmpTo->FlareSize+Distrib(data.MorphSizeVariation);
						             }
						          while((tmpFlare->FlareSize<0.0f)||(tmpFlare->FlareSize>1.0f));
						  
						          do { tmpFlare->Intensity=T*tmpFrom->Intensity+(1.0f-T)*tmpTo->Intensity+Distrib(data.MorphLightVariation);
						             }
						          while(tmpFlare->Intensity<0.0f);

						          if (data.MorphInterplay) tmpFlare->Intensity=tmpFrom->Intensity*tmpFrom->FlareSize*tmpFrom->FlareSize/
									                                           (tmpFlare->FlareSize*tmpFlare->FlareSize);

						          tmpFlare->AngleOffSet=T*tmpFrom->AngleOffSet+(1.0f-T)*tmpTo->AngleOffSet+Distrib(data.MorphAngleOffsetVariation);
						          tmpFlare->Colourise  =T*tmpFrom->Colourise+(1.0f-T)*tmpTo->Colourise+Distrib(data.MorphColouriseVariation);

						          tmpFlare->IR2=tmpFlare->Intensity*tmpFlare->Intensity*tmpFlare->FlareSize;

								  tmpFlare->PutPosValues(ii);
						        }

						  // Add To Positional Keys
						  for(ii=0;ii!=tmpTo->NoPosKeys;ii++)
						  // Already A KeyFrame ?
						  if (tmpFlare->IsPosKey(tmpTo->PosKeys[ii].Time)==-1)
									{ tmpTo->GetPosValues(ii); 
									  tmpFrom->GetPosition(tmpTo->PosKeys[ii].Time);

									  if (tmpFrom->Polar!=tmpTo->Polar)
											{ if (tmpFrom->Polar) tmpTo->ToPolar(0);
								              else tmpTo->FromPolar(0);
											}

									  if ((tmpFrom->OffScreenRender&2)!=(tmpTo->OffScreenRender&2))
											{ int tmp=tmpTo->OffScreenRender;
								              tmpTo->ToAbsolute(MorphXRes,MorphYRes,MorphEditX,MorphEditY,data.CameraDepth,(float)tmpTo->PosKeys[ii].Time,0,tmp,tmpFrom->OffScreenRender);
                                              tmpTo->OffScreenRender=tmp;
											}

									  tmpFlare->PositionX  =T*tmpFrom->PositionX+(1.0f-T)*tmpTo->PositionX+Distrib(data.MorphXPositionVariation);
									  tmpFlare->PositionY  =T*tmpFrom->PositionY+(1.0f-T)*tmpTo->PositionY+Distrib(data.MorphYPositionVariation);
									  tmpFlare->Position   =T*tmpFrom->Position +(1.0f-T)*tmpTo->Position+Distrib(data.MorphDistanceVariation);
									  tmpFlare->Angle      =T*tmpFrom->Angle    +(1.0f-T)*tmpTo->Angle+Distrib(data.MorphAngleVariation);

									  tmpFlare->Depth =T*tmpFrom->Depth +(1.0f-T)*tmpTo->Depth+Distrib(data.MorphDepthVariation);
									  
									  tmpFlare->Occlusion  =T*tmpFrom->Occlusion+(1.0f-T)*tmpTo->Occlusion+Distrib(data.MorphOcclusionVariation);
									  tmpFlare->Intensity  =T*tmpFrom->Intensity+(1.0f-T)*tmpTo->Intensity;
									  tmpFlare->FlareSize  =T*tmpFrom->FlareSize+(1.0f-T)*tmpTo->FlareSize;
									  do { tmpFlare->FlareSize=T*tmpFrom->FlareSize+(1.0f-T)*tmpTo->FlareSize+Distrib(data.MorphSizeVariation);
										 }
									  while((tmpFlare->FlareSize<0.0f)||(tmpFlare->FlareSize>1.0f));
						  
									  do { tmpFlare->Intensity=T*tmpFrom->Intensity+(1.0f-T)*tmpTo->Intensity+Distrib(data.MorphLightVariation);
										 }
									  while(tmpFlare->Intensity<0.0f);

									  if (data.MorphInterplay) tmpFlare->Intensity=tmpFrom->Intensity*tmpFrom->FlareSize*tmpFrom->FlareSize/
									                                               (tmpFlare->FlareSize*tmpFlare->FlareSize);

									  tmpFlare->AngleOffSet=T*tmpFrom->AngleOffSet+(1.0f-T)*tmpTo->AngleOffSet+Distrib(data.MorphAngleOffsetVariation);
									  tmpFlare->Colourise  =T*tmpFrom->Colourise+(1.0f-T)*tmpTo->Colourise+Distrib(data.MorphColouriseVariation);

									  tmpFlare->IR2=tmpFlare->Intensity*tmpFlare->Intensity*tmpFlare->FlareSize;

									  tmpFlare->AddPosKey(tmpTo->PosKeys[ii].Time);
									}

						  // Shape Keys
						  for(ii=0;ii!=tmpFlare->NoShpKeys;ii++)
								{ tmpFrom->GetShpValues(ii); 
						          tmpTo  ->GetShape(tmpFlare->ShpKeys[ii].Time);

								  for(int j=0;j!=FLARERESOLUTION;j++)
										{ fColour ftmpFrom,ftmpTo,ftmp;
										  float FloatFrom,FloatTo;

										  tmpFrom->GetRadialColour(ftmpFrom,j);
										  tmpTo  ->GetRadialColour(ftmpTo,j);
											  ftmp.r=T*ftmpFrom.r+(1.0f-T)*ftmpTo.r;
											  ftmp.g=T*ftmpFrom.g+(1.0f-T)*ftmpTo.g;
											  ftmp.b=T*ftmpFrom.b+(1.0f-T)*ftmpTo.b;
											  ftmp.a=T*ftmpFrom.a+(1.0f-T)*ftmpTo.a;
										  tmpFlare->PutRadialColour(ftmp,j);

										  tmpFrom->GetAngularColour(ftmpFrom,j);
										  tmpTo  ->GetAngularColour(ftmpTo,j);
											  ftmp.r=T*ftmpFrom.r+(1.0f-T)*ftmpTo.r;
											  ftmp.g=T*ftmpFrom.g+(1.0f-T)*ftmpTo.g;
											  ftmp.b=T*ftmpFrom.b+(1.0f-T)*ftmpTo.b;
											  ftmp.a=T*ftmpFrom.a+(1.0f-T)*ftmpTo.a;
										  tmpFlare->PutAngularColour(ftmp,j);

										  tmpFrom->GetShape (FloatFrom,j);
										  tmpTo  ->GetShape (FloatTo  ,j);
										  tmpFlare->PutShape(FloatFrom*T+(1.0f-T)*FloatTo,j);

										  tmpFrom->GetShape2 (FloatFrom,j);
										  tmpTo  ->GetShape2 (FloatTo  ,j);
										  tmpFlare->PutShape2(FloatFrom*T+(1.0f-T)*FloatTo,j);
										}
								  tmpFlare->PutShpValues(ii);

								}

						  // Add To Shape Keys
						  for(ii=0;ii!=tmpTo->NoShpKeys;ii++)
						  // Already A KeyFrame ?
						  if (tmpFlare->IsShpKey(tmpTo->ShpKeys[ii].Time)==-1)
									{ tmpTo->GetShpValues(ii); 
									  tmpFrom->GetShape(tmpTo->ShpKeys[ii].Time);

									  for(int j=0;j!=FLARERESOLUTION;j++)
										{ fColour ftmpFrom,ftmpTo,ftmp;
										  float FloatFrom,FloatTo;

										  tmpFrom->GetRadialColour(ftmpFrom,j);
										  tmpTo  ->GetRadialColour(ftmpTo,j);
											  ftmp.r=T*ftmpFrom.r+(1.0f-T)*ftmpTo.r;
											  ftmp.g=T*ftmpFrom.g+(1.0f-T)*ftmpTo.g;
											  ftmp.b=T*ftmpFrom.b+(1.0f-T)*ftmpTo.b;
											  ftmp.a=T*ftmpFrom.a+(1.0f-T)*ftmpTo.a;
										  tmpFlare->PutRadialColour(ftmp,j);

										  tmpFrom->GetAngularColour(ftmpFrom,j);
										  tmpTo  ->GetAngularColour(ftmpTo,j);
											  ftmp.r=T*ftmpFrom.r+(1.0f-T)*ftmpTo.r;
											  ftmp.g=T*ftmpFrom.g+(1.0f-T)*ftmpTo.g;
											  ftmp.b=T*ftmpFrom.b+(1.0f-T)*ftmpTo.b;
											  ftmp.a=T*ftmpFrom.a+(1.0f-T)*ftmpTo.a;
										  tmpFlare->PutAngularColour(ftmp,j);

										  tmpFrom->GetShape (FloatFrom,j);
										  tmpTo  ->GetShape (FloatTo  ,j);
										  tmpFlare->PutShape(FloatFrom*T+(1.0f-T)*FloatTo,j);

										  tmpFrom->GetShape2 (FloatFrom,j);
										  tmpTo  ->GetShape2 (FloatTo  ,j);
										  tmpFlare->PutShape2(FloatFrom*T+(1.0f-T)*FloatTo,j);
										}
									  tmpFlare->AddShpKey(tmpTo->ShpKeys[ii].Time);

									}

						  tmpFlare->GetPosition	(data.AnimationPosition);
						  tmpFlare->GetShape	(data.AnimationPosition);
						}

				tmpFrom->GetPosition	(data.AnimationPosition); 
				tmpTo->GetPosition		(data.AnimationPosition);
				tmpFrom->GetShape		(data.AnimationPosition); 
				tmpTo->GetShape			(data.AnimationPosition);

				SetCursor(LoadCursor(NULL,IDC_ARROW));
}
//**************************************************************

//******************************************************************************************************************
// Small Window Preview From
void ImageFilter_LensF::FromMorphPreview(int DrawChecker)
{				HDC		hdc = GetDC(hwFromPreview_Morph);
				RECT	rect;
				Flare	*tmpF=data.LensFlare.GetFlare(data.MorphSelectFrom);

				GetClientRect(hwFromPreview_Morph,&rect);
				int lxres=(rect.right-rect.left)&0xfffffff0;
				int lyres=rect.bottom-rect.top;

				if (hwFromPreviewMorphMem==NULL) 
						{ hwFromPreviewMorphInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
						  hwFromPreviewMorphInfo.bmiHeader.biPlanes			=1;
						  hwFromPreviewMorphInfo.bmiHeader.biBitCount		=24;
						  hwFromPreviewMorphInfo.bmiHeader.biCompression	=BI_RGB;
						  hwFromPreviewMorphInfo.bmiHeader.biClrUsed		=0;
						  hwFromPreviewMorphInfo.bmiHeader.biClrImportant	=0;
						  hwFromPreviewMorphInfo.bmiHeader.biWidth			=lxres;
						  hwFromPreviewMorphInfo.bmiHeader.biHeight			=lyres;
						  hwFromPreviewMorphInfo.bmiHeader.biSizeImage		=lxres*lyres*3;
						  hwFromPreviewMorphMem=(char *)malloc(lxres*lyres*3);
						}

				
				tmpF->RenderPreview(hwFromPreviewMorphMem,lxres,lyres,DrawChecker,0.9f,1.0f);
				SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwFromPreviewMorphMem,
			                      &hwFromPreviewMorphInfo,DIB_RGB_COLORS);

				ReleaseDC(hwFromPreview_Morph,hdc);
}

//******************************************************************************************************************
// Small Window Preview From
void ImageFilter_LensF::ToMorphPreview(int DrawChecker)
{				HDC		hdc = GetDC(hwToPreview_Morph);
				RECT	rect;
				Flare	*tmpF=data.LensFlare.GetFlare(data.MorphSelectTo);

				GetClientRect(hwToPreview_Morph,&rect);
				int lxres=(rect.right-rect.left)&0xfffffff0;
				int lyres=rect.bottom-rect.top;

				if (hwToPreviewMorphMem==NULL) 
						{ hwToPreviewMorphInfo.bmiHeader.biSize				=sizeof(BITMAPINFOHEADER);
						  hwToPreviewMorphInfo.bmiHeader.biPlanes			=1;
						  hwToPreviewMorphInfo.bmiHeader.biBitCount			=24;
						  hwToPreviewMorphInfo.bmiHeader.biCompression		=BI_RGB;
						  hwToPreviewMorphInfo.bmiHeader.biClrUsed			=0;
						  hwToPreviewMorphInfo.bmiHeader.biClrImportant		=0;
						  hwToPreviewMorphInfo.bmiHeader.biWidth			=lxres;
						  hwToPreviewMorphInfo.bmiHeader.biHeight			=lyres;
						  hwToPreviewMorphInfo.bmiHeader.biSizeImage		=lxres*lyres*3;
						  hwToPreviewMorphMem=(char *)malloc(lxres*lyres*3);
						}

				tmpF->RenderPreview(hwToPreviewMorphMem,lxres,lyres,DrawChecker,0.9f,1.0f);
				SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwToPreviewMorphMem,
			                      &hwToPreviewMorphInfo,DIB_RGB_COLORS);

				ReleaseDC(hwToPreview_Morph,hdc);
}

//*****************************************************************************************************
// Draw The Preview
void ImageFilter_LensF::MorphPreview(int CheckerBoard)
{				SetCursor(LoadCursor(NULL,IDC_WAIT));

				HDC		hdc = GetDC(hwPreview_Morph);
				RECT	rect;

				GetClientRect(hwPreview_Morph,&rect);
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
					{ if (CheckerBoard)  { rinfo.Img_R[y][x]=rinfo.Img_G[y][x]=rinfo.Img_B[y][x]=(uchar)(Grid(x,y)*255); 
										   rinfo.Img_A[y][x]=0; 
									     }
					  else rinfo.Img_R[y][x]=rinfo.Img_G[y][x]=rinfo.Img_B[y][x]=rinfo.Img_A[y][x]=0; 
					}

				if ((MorphEditX==-1)&&(MorphEditX==-1)) { MorphEditX=lxres/8.0f; MorphEditY=lyres/2.0f; }

				rinfo.UseSingle=1;
				rinfo.xpos=MorphEditX; 
				rinfo.ypos=MorphEditY;

				StatusText("Preview");
				data.LensFlare.RenderHQArea(rinfo);

				if (hwPreviewMorphMem==NULL) 
						{ hwPreviewMorphInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
						  hwPreviewMorphInfo.bmiHeader.biPlanes			=1;
						  hwPreviewMorphInfo.bmiHeader.biBitCount		=24;
						  hwPreviewMorphInfo.bmiHeader.biCompression	=BI_RGB;
						  hwPreviewMorphInfo.bmiHeader.biClrUsed		=0;
						  hwPreviewMorphInfo.bmiHeader.biClrImportant	=0;
						  hwPreviewMorphInfo.bmiHeader.biWidth			=lxres;
						  hwPreviewMorphInfo.bmiHeader.biHeight			=lyres;
						  hwPreviewMorphInfo.bmiHeader.biSizeImage		=lxres*lyres*3;
						  hwPreviewMorphMem=(char *)malloc(lxres*lyres*3);
						}

				uchar *tmp=(uchar *)hwPreviewMorphMem;
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

				SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewMorphMem,
			                      &hwPreviewMorphInfo,DIB_RGB_COLORS);

				ReleaseDC(hwPreview_Morph,hdc);

				MorphBlackOutFlag=1;
				SetCursor(LoadCursor(NULL,IDC_ARROW));
}

#define RealTimePreview_Add			32
#define RealTimePreview_AngleAdd	(FLARERESOLUTION/RealTimePreview_Add)

//**********************************************************************************************************
// REAL TIME PREVIEW STUFF

void ImageFilter_LensF::MorphRealTimePreview(int WhatToDo)
{				HDC		hdc = GetDC(hwPreview_Morph);
				RECT	rect; GetClientRect(hwPreview_Morph,&rect);

				int lxres=(rect.right-rect.left)&0xfffffff0;
				int lyres=rect.bottom-rect.top;

				if ((!MorphRealTimeDrawn)&&(MorphBlackOutFlag)&&(hwPreviewMorphMem!=NULL))
						{ // Black Out Bitmap
						  unsigned char *tmp=(unsigned char *)hwPreviewMorphMem;
						  for(int y=0;y!=lyres;y++)
						  for(int x=0;x!=lxres;x++)
								{ *(tmp++)=(*(tmp))/4; *(tmp++)=(*(tmp))/4; *(tmp++)=(*(tmp))/4;
								}

						  SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewMorphMem,&hwPreviewMorphInfo,DIB_RGB_COLORS);

						  MorphBlackOutFlag=0;
						}

				HRGN	ClipRegion=CreateRectRgn(rect.left,rect.top,rect.left+lxres,rect.top+lyres);
				SelectClipRgn(hdc,ClipRegion);

				HPEN	CirclePen =CreatePen(PS_SOLID,0,RGB(255,128,128));
				HPEN	CirclePen2=CreatePen(PS_SOLID,0,RGB(128,64,64));
				HPEN	CentrePen =CreatePen(PS_SOLID,0,RGB(128,128,255));
				HPEN	oldpen=(HPEN)SelectObject(hdc,CentrePen);						// 24.03.97
				
				// Compute Centre
				if ((MorphEditX==-1)&&(MorphEditY==-1)) { MorphEditX=lxres/8.0f; MorphEditY=lyres/2.0f; }
				SetROP2(hdc,R2_XORPEN);

				// Draw All Elements
				for(int j=0;j!=data.LensFlare.GetNoFlares();j++)
				{  Flare *Local=data.LensFlare.GetFlare(j);
				
				  int Draw=(WhatToDo==Morph_DrawAll);
				  if  ((MorphRealTimeDrawn==0)||(data.SelectedElement[j])) Draw=TRUE;
				   
				  if (Draw) {	if ((j==data.MorphSelectFrom)||(j==data.MorphSelectTo))
											{ SelectObject(hdc,CentrePen);
											  float xps=(0.5f+Local->PositionX*0.5f)*lxres;
											  float yps=(0.5f+Local->PositionY*0.5f)*lyres;
											  
											  if (WhatToDo!=Morph_DrawSelectedLo)
													{ MoveToEx(hdc,(int)xps-5,(int)yps-5,NULL); LineTo(hdc,(int)xps+6,(int)yps+6);
													  MoveToEx(hdc,(int)xps-5,(int)yps+5,NULL); LineTo(hdc,(int)xps+6,(int)yps-6);
													}

											  SelectObject(hdc,CirclePen);
											  if (WhatToDo==Morph_DrawSelectedLo) SelectObject(hdc,CirclePen2);
											}
								else SelectObject(hdc,CirclePen2);

								if (Local->Depth > -data.CameraDepth+1.0E-6f) 
										{	Local->PreProcess(lxres,lyres,MorphEditX,MorphEditY,data.CameraDepth,(float)data.AnimationPosition,0.0f);

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

				MorphRealTimeDrawn=1;
				SelectObject(hdc,oldpen);				// 23.04.97
				DeleteObject(ClipRegion);
				DeleteObject(CirclePen);
				DeleteObject(CirclePen2);
				DeleteObject(CentrePen);				

				ReleaseDC(hwPreview_Morph,hdc);
}

#undef RealTimePreview_Add
#undef RealTimePreview_AngleAdd

BOOL CALLBACK MorphBSPreviewProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) 
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);

	  if (msg==WM_PAINT)
							{ HDC		hdc;
				              RECT		rect;
							  
							  if (hwFromPreviewMorphMem!=NULL)
									{	hdc = GetDC(hwFromPreview_Morph);
										GetClientRect(hwFromPreview_Morph,&rect);
										int lxres=(rect.right-rect.left)&0xfffffff0;
										int lyres=rect.bottom-rect.top;
										SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwFromPreviewMorphMem,&hwFromPreviewMorphInfo,DIB_RGB_COLORS);
										ReleaseDC(hwFromPreview_Morph,hdc);
									}

							  if (hwToPreviewMorphMem!=NULL)
									{	hdc = GetDC(hwToPreview_Morph);
										GetClientRect(hwToPreview_Morph,&rect);
										int lxres=(rect.right-rect.left)&0xfffffff0;
										int lyres=rect.bottom-rect.top;
										SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwToPreviewMorphMem,&hwToPreviewMorphInfo,DIB_RGB_COLORS);
										ReleaseDC(hwToPreview_Morph,hdc);
									}
							}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	}

HIMAGELIST		HIL_Morph_OK;
HIMAGELIST		HIL_Morph_Cancel;
HIMAGELIST		HIL_Morph_Lock;

void ImageFilter_LensF::MorphBuildButtons(void)
{		BuildButton(MorphOK,HIL_Morph_OK,IDB_MASTEROK,IDB_MASTEROK,100,50);
		BuildButton(MorphCancel,HIL_Morph_Cancel,IDB_MASTERCANCEL,IDB_MASTERCANCEL,100,50);
		BuildButton(SIButton,HIL_Morph_Lock,IDB_MASTERLOCK_DOWN,IDB_MASTERLOCK_UP,50,50);
}

void ImageFilter_LensF::MorphFreeButtons(void)
{		ReleaseButton(HIL_Morph_OK);
		ReleaseButton(HIL_Morph_Cancel);
		ReleaseButton(HIL_Morph_Lock);
}

void ImageFilter_LensF::MorphReleaseButtons()
{
				ReleaseSpinner(SpinMorphSizeVariation);
				ReleaseSpinner(SpinMorphLightVariation);
				ReleaseSpinner(SpinMorphDistanceVariation);
				ReleaseSpinner(SpinMorphAngleVariation);
				ReleaseSpinner(SpinMorphAngleOffsetVariation);
				ReleaseSpinner(SpinMorphXPositionVariation);
				ReleaseSpinner(SpinMorphYPositionVariation);
				ReleaseSpinner(SpinMorphColouriseVariation);
				ReleaseSpinner(SpinMorphDepthVariation);
				ReleaseSpinner(SpinMorphOcclusionVariation);
				ReleaseSpinner(SpinMorphTime);

				ReleaseICustButton(SIButton); 

				ReleaseICustButton(MorphFwd);
				ReleaseICustButton(MorphBck);

				ReleaseICustButton(MorphOK);
				ReleaseICustButton(MorphCancel);
								
				ReleaseSpinner(SpinMorphNumber);
				ReleaseSpinner(SpinMorphSeed);
}

BOOL CALLBACK MorphPreviewProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) 
	{ int id = GetWindowLong(hwnd,GWL_ID);
	  HWND hwpar = GetParent(hwnd);

	  if (((msg==WM_LBUTTONDOWN)&&(wParam==MK_LBUTTON+MK_SHIFT))||	// Light Placement
		  ((msg==WM_MOUSEMOVE  )&&(wParam==MK_LBUTTON+MK_SHIFT)))
							{ HDC		hdc = GetDC(hwPreview_Morph);
				              RECT		rect;
							  GetClientRect(hwPreview_Morph,&rect);

							  int lxres=(rect.right-rect.left)&0xfffffff0;
							  int lyres=rect.bottom-rect.top;
						      if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);

			                  MorphEditX = (float)LOWORD(lParam); if (MorphEditX>4096.0f) MorphEditX=0.0f; if (MorphEditX>lxres) MorphEditX=(float)lxres;
							  MorphEditY = (float)HIWORD(lParam); if (MorphEditY>4096.0f) MorphEditY=0.0f; if (MorphEditY>lyres) MorphEditY=(float)lyres;

							  ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
							  if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
							  ReleaseDC(hwPreview_Morph,hdc);
							}
	  else if (msg==WM_LBUTTONUP) ReleaseCapture();
	  else if (msg==WM_RBUTTONDOWN)
							{ ImageFilterData->MorphPreview(ImageFilterData->data.DrawChecker);
							  MorphRealTimeDrawn=0;
							}
	  else if (msg==WM_PAINT)
							{ HDC		hdc = GetDC(hwPreview_Morph);
				              RECT		rect;
							  GetClientRect(hwPreview_Morph,&rect);

							  int lxres=(rect.right-rect.left)&0xfffffff0;
							  int lyres=rect.bottom-rect.top;

							  if (hwPreviewMorphMem!=0) SetDIBitsToDevice(hdc,0,0,lxres,lyres,0,0,0,lyres,hwPreviewMorphMem,&hwPreviewMorphInfo,DIB_RGB_COLORS);
							  else			{ HBRUSH brush=CreateSolidBrush(RGB(0,0,40));
							                  HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,brush);				//24.03.97
											  Rectangle(hdc,rect.left,rect.top,rect.left+lxres,rect.bottom);
											  SelectObject(hdc,oldbrush);									//24.03.97
							                  DeleteObject(brush);
											}

							  if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
							  ReleaseDC(hwPreview_Morph,hdc);
							}

	  return DefWindowProc(hwnd,msg,wParam,lParam);
	}


HBITMAP MorphLoadBitmap;
HBITMAP MorphBackBitmap;

BOOL ImageFilter_LensF::MorphControl(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
  if( message==WM_COMMAND || message==CC_SPINNER_CHANGE )
	switch (LOWORD(wParam)) 
		{	case IDC_MTIME_SPIN:
				if ((message==CC_SPINNER_CHANGE) && (HIWORD(wParam)==FALSE))
							  {	  // Erase Display
								  if (MorphRealTimeDrawn) MorphRealTimePreview(Morph_DrawAll);

								  // Get New Time Value
								  data.AnimationPosition=SpinMorphTime->GetIVal();

								  // Get New Positions
								  data.LensFlare.GetFrame(data.AnimationPosition);

								  if (data.AnimationPosition>data.AnimationMax) data.AnimationMax=data.AnimationPosition;
								  else {	int MPos=data.LensFlare.MaxKeyValue();
											if (MPos>data.AnimationPosition) data.AnimationMax=MPos;
											else data.AnimationMax=data.AnimationPosition;

											if (data.AnimationMax<100) data.AnimationMax=100;
									   }

								  // Update All Display						
								  MorphMegaScrollBar();
								  MorphRealTimePreview(Morph_DrawAll);
								  FromMorphPreview(data.DrawChecker);
								  ToMorphPreview(data.DrawChecker);
								  break;
							}
		  case IDC_Depth_SPIN:	if ((message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
                            		{ if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
									  MorphDelete();
                                      data.MorphDepthVariation=SpinMorphDepthVariation->GetFVal();
									  MorphCreate();
									  ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
									  break;
									}

		  case IDC_Occlusion_SPIN:	{ data.MorphOcclusionVariation=SpinMorphOcclusionVariation->GetFVal();
									  break;
									}

          case IDC_CHECKER:		{ data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_CHECKER);
								  FromMorphPreview(data.DrawChecker);
								  ToMorphPreview(data.DrawChecker);
								  break; 
								}

          case IDC_SZ_SPIN:	 if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
									MorphDelete();
									data.MorphSizeVariation=SpinMorphSizeVariation->GetFVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
								break;

		  case IDC_Int_SPIN:  if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
									MorphDelete();
									data.MorphLightVariation=SpinMorphLightVariation->GetFVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
								break;
								
		  case IDC_Dist_SPIN: if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
			                        MorphDelete();
									data.MorphDistanceVariation=SpinMorphDistanceVariation->GetFVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
							    }
							  break;

		  case IDC_ANG_SPIN: if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
			                        MorphDelete();
									data.MorphAngleVariation=SpinMorphAngleVariation->GetFVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
							 break;

		  case IDC_ANG2_SPIN: if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
			                        MorphDelete();
									data.MorphAngleOffsetVariation=SpinMorphAngleOffsetVariation->GetFVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
								break;

		  case IDC_XPOS_SPIN: if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
			                        MorphDelete();
									data.MorphXPositionVariation=SpinMorphXPositionVariation->GetFVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
								break;

		  case IDC_YPOS_SPIN: if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
			                        MorphDelete();
									data.MorphYPositionVariation=SpinMorphYPositionVariation->GetFVal();				
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
								break;

		  case IDC_COL_SPIN:	data.MorphColouriseVariation=SpinMorphColouriseVariation->GetFVal();
								break;

		  case IDC_NUM_SPIN: if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
			                        MorphDelete();
									data.MorphNumber=SpinMorphNumber->GetIVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
								break;

		  case IDC_Seed2_SPIN: if ( (message==CC_SPINNER_CHANGE)&&(HIWORD(wParam)==FALSE))
								{	if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
			                        MorphDelete();
									data.MorphSeed=SpinMorphSeed->GetIVal();
									MorphCreate();
									ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
								}
								break;

		  case IDC_SI_LOCK:	if( HIWORD(wParam)==BN_BUTTONDOWN || HIWORD(wParam)==BN_BUTTONUP )
							{	
								MorphDelete();
								data.MorphInterplay=SIButton->IsChecked();

								int Flag=(!data.MorphInterplay);
								EnableIt(IDC_Int_EDIT,Flag); 
								EnableIt(IDC_Int_SPIN,Flag);
								MorphCreate();
								ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
							}
								break;

		  case IDC_NAMEEDIT:	GetDlgItemText(hWnd,IDC_NAMEEDIT,data.MorphName,240);
								break;

		  case IDC_COMBOFROM: if (HIWORD(wParam)==CBN_SELCHANGE)
								{		if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
										MorphDelete();
										data.MorphSelectFrom=SendMessage(hwndComboFrom,CB_GETCURSEL,0,0);
										FromMorphPreview(data.DrawChecker);
										MorphCreate();
										ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
										MorphMegaScrollBar();
								}
								break;

		  case IDC_COMBOTO:	if (HIWORD(wParam)==CBN_SELCHANGE)
								{		if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
									    MorphDelete();
										data.MorphSelectTo  =SendMessage(hwndComboTo,CB_GETCURSEL,0,0);
										ToMorphPreview(data.DrawChecker);
										MorphCreate();
										ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
										MorphMegaScrollBar();
								}
								break;

		  case IDC_MFORWARD:
				{ switch (HIWORD(wParam))
					{	case BN_BUTTONUP:	
							{ int TimePos=999999;

							  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
							  if ((i==data.MorphSelectFrom)||(i==data.MorphSelectTo))
									{ Flare *tmp=data.LensFlare.GetFlare(i); 
									  
								      for(int j=0;j!=tmp->NoPosKeys;j++)
											if ((tmp->PosKeys[j].Time>data.AnimationPosition)&&
											    (tmp->PosKeys[j].Time<TimePos)) TimePos=tmp->PosKeys[j].Time;

									  for(j=0;j!=tmp->NoShpKeys;j++)
											if ((tmp->ShpKeys[j].Time>data.AnimationPosition)&&
											    (tmp->ShpKeys[j].Time<TimePos)) TimePos=tmp->ShpKeys[j].Time;
									}

							  if (TimePos!=999999) { data.AnimationPosition=TimePos;
							                         if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
							                         data.LensFlare.GetFrame(data.AnimationPosition);

													 SpinMorphTime->SetValue(ImageFilterData->data.AnimationPosition,NULL);
													 
													 ImageFilterData->MorphMegaScrollBar();
													 ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
													 ImageFilterData->FromMorphPreview(ImageFilterData->data.DrawChecker);
													 ImageFilterData->ToMorphPreview(ImageFilterData->data.DrawChecker);
							                       }
							  break;
							}
					}
			      break;
				}

		 case IDC_MBACK:
				{ switch (HIWORD(wParam))
					{	case BN_BUTTONUP:	
							{ int TimePos=-1;

							  for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
							  if ((i==data.MorphSelectFrom)||(i==data.MorphSelectTo))
									{ Flare *tmp=data.LensFlare.GetFlare(i); 
									  
								      for(int j=0;j!=tmp->NoPosKeys;j++)
											if ((tmp->PosKeys[j].Time<data.AnimationPosition)&&
											    (tmp->PosKeys[j].Time>TimePos)) TimePos=tmp->PosKeys[j].Time;

									  for(j=0;j!=tmp->NoShpKeys;j++)
											if ((tmp->ShpKeys[j].Time<data.AnimationPosition)&&
											    (tmp->ShpKeys[j].Time>TimePos)) TimePos=tmp->ShpKeys[j].Time;
									}

							  if (TimePos!=-1) { data.AnimationPosition=TimePos;
							                         if (MorphRealTimeDrawn) ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
							                         data.LensFlare.GetFrame(data.AnimationPosition);
													 SpinMorphTime->SetValue(ImageFilterData->data.AnimationPosition,NULL);

													 ImageFilterData->MorphMegaScrollBar();
													 ImageFilterData->MorphRealTimePreview(Morph_DrawAll);
													 ImageFilterData->FromMorphPreview(ImageFilterData->data.DrawChecker);
													 ImageFilterData->ToMorphPreview(ImageFilterData->data.DrawChecker);
							                       }
							  break;
							}
					}
			      break;
				}
		}
	
	switch (message) 
	   { case WM_INITDIALOG: 
		   { CenterWindow(hWnd,GetParent(hWnd));
		     SetCursor(LoadCursor(NULL,IDC_WAIT));

			 // Setup background bitmap rcw 18/4/97
				// load bitmap and get window dc
				MorphLoadBitmap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_MORPH));
				HDC hdc=GetDC(hWnd);
				RECT	rect;
				BITMAP	bm;

				// get x and y res
				GetClientRect(hWnd,&rect);

				// get size of bitmap
				GetObject(MorphLoadBitmap,sizeof(BITMAP),&bm);

				if( abs(rect.right-bm.bmWidth)<=2 && abs(rect.bottom-bm.bmHeight)<=2 )
					// keep original
					MorphBackBitmap=MorphLoadBitmap;
				else {
					// get the final background bitmap
					MorphBackBitmap=CreateCompatibleBitmap(hdc,(rect.right+3)&0xfffffffc,rect.bottom);
					// interpolate new size
					// note this deletes old bitmap internally
					LinearResample(hdc,MorphLoadBitmap,bm.bmWidth,bm.bmHeight,hdc,MorphBackBitmap,rect.right,rect.bottom);
				}

				// clean up DC
				ReleaseDC(hWnd,hdc);

		     //-----------------------------------------------------
			 // Send Text To Combo Boxes
				 hwndComboFrom	=GetDlgItem(hWnd,IDC_COMBOFROM);
				 hwndComboTo	=GetDlgItem(hWnd,IDC_COMBOTO);

				 SendMessage(hwndComboFrom,CB_RESETCONTENT, 0, 0); 
				 SendMessage(hwndComboTo  ,CB_RESETCONTENT, 0, 0); 

				 for(int i=0;i!=data.LensFlare.GetNoFlares();i++)
					{ SendMessage(hwndComboFrom,CB_ADDSTRING,0,(LPARAM)data.LensFlare.GetName(i));
					  SendMessage(hwndComboTo,CB_ADDSTRING,0,(LPARAM)data.LensFlare.GetName(i));
					}

				 SendMessage(hwndComboFrom,CB_SETCURSEL,data.MorphSelectFrom,0);
				 SendMessage(hwndComboTo  ,CB_SETCURSEL,data.MorphSelectTo,0);

				 SetDlgItemText(hWnd,IDC_NAMEEDIT,data.LensFlare.GetName(data.MorphSelectFrom));

			//-----------------------------------------------------
			// Mega Scroll Bar
				 hwndMorphScrollBar = GetDlgItem(hWnd, IDC_MMEGASCROLL);					  
				 SetWindowLong(hwndMorphScrollBar,GWL_WNDPROC,(LONG)MorphMScrollBarProc);

			//-----------------------------------------------------

				SpinMorphSizeVariation			= SetupFloatSpinner(hWnd,IDC_SZ_SPIN,IDC_SZ_EDIT,0.0f,1.0f,data.MorphSizeVariation,0.01f,data.MorphSizeVariation);
				SpinMorphLightVariation			= SetupFloatSpinner(hWnd,IDC_Int_SPIN,IDC_Int_EDIT,0.0f,2.0f,data.MorphLightVariation,0.01f,data.MorphLightVariation);
				SpinMorphDistanceVariation		= SetupFloatSpinner(hWnd,IDC_Dist_SPIN,IDC_Dist_EDIT,0.0f,10.0f,data.MorphDistanceVariation,0.01f,data.MorphDistanceVariation);
				SpinMorphAngleVariation			= SetupFloatSpinner(hWnd,IDC_ANG_SPIN,IDC_ANG_EDIT,0.0f,5.0f,data.MorphAngleVariation,0.01f,data.MorphAngleVariation);
				SpinMorphAngleOffsetVariation	= SetupFloatSpinner(hWnd,IDC_ANG2_SPIN,IDC_ANG2_EDIT,0.0f,1.0f,data.MorphAngleOffsetVariation,0.01f,data.MorphAngleOffsetVariation);
				SpinMorphXPositionVariation		= SetupFloatSpinner(hWnd,IDC_XPOS_SPIN,IDC_XPOS_EDIT,0.0f,1.0f,data.MorphXPositionVariation,0.01f,data.MorphXPositionVariation);
				SpinMorphYPositionVariation		= SetupFloatSpinner(hWnd,IDC_YPOS_SPIN,IDC_YPOS_EDIT,0.0f,1.0f,data.MorphYPositionVariation,0.01f,data.MorphYPositionVariation);
				SpinMorphColouriseVariation		= SetupFloatSpinner(hWnd,IDC_COL_SPIN,IDC_COL_EDIT,0.0f,1.0f,data.MorphColouriseVariation,0.01f,data.MorphColouriseVariation);
				SpinMorphDepthVariation			= SetupFloatSpinner(hWnd,IDC_Depth_SPIN,IDC_Depth_EDIT,0.0f,10.0f,data.MorphDepthVariation,0.01f,data.MorphDepthVariation);
				SpinMorphOcclusionVariation		= SetupFloatSpinner(hWnd,IDC_Occlusion_SPIN,IDC_Occlusion_EDIT,0.0f,1.0f,data.MorphOcclusionVariation,0.01f,data.MorphOcclusionVariation);
				SpinMorphTime					= SetupIntSpinner(hWnd,IDC_MTIME_SPIN,IDC_MTIME_EDIT,0,999999,data.AnimationPosition,1,data.AnimationPosition);

				SIButton=GetICustButton(GetDlgItem(hWnd,IDC_SI_LOCK)); 
				SIButton->SetType(CBT_CHECK);

				MorphFwd=GetICustButton(GetDlgItem(hWnd,IDC_MFORWARD));
				MorphBck=GetICustButton(GetDlgItem(hWnd,IDC_MBACK));

				MorphOK=GetICustButton(GetDlgItem(hWnd,IDC_MORPH_OK));
				MorphCancel=GetICustButton(GetDlgItem(hWnd,IDC_MORPH_CANCEL));

				SIButton->SetCheck((data.MorphInterplay)); 
								
				SpinMorphNumber					= SetupIntSpinner(hWnd,IDC_NUM_SPIN,IDC_NUM_EDIT,1,100,data.MorphNumber);
				SpinMorphSeed					= SetupIntSpinner(hWnd,IDC_Seed2_SPIN,IDC_Seed2_EDIT,0,9999,data.MorphSeed);

				CheckDlgButton(hWnd,IDC_CHECKER,data.DrawChecker);

				hwPreview_Morph			= GetDlgItem(hWnd,IDC_MORPH_PREVIEW_WINDOW);
				hwFromPreview_Morph		= GetDlgItem(hWnd,IDC_FROMPREVIEW);
				hwToPreview_Morph		= GetDlgItem(hWnd,IDC_TOPREVIEW);

				// get x and y of preview window
				GetClientRect(hwPreview_Morph,&rect);
				MorphXRes=(rect.right-rect.left)&0xfffffff0;
				MorphYRes=rect.bottom-rect.top;

				
				FromMorphPreview(data.DrawChecker);
				ToMorphPreview(data.DrawChecker);

				SetWindowLong(hwPreview_Morph,GWL_WNDPROC,(LONG)MorphPreviewProc);
				SetWindowLong(hwFromPreview_Morph,GWL_WNDPROC,(LONG)MorphBSPreviewProc);
				SetWindowLong(hwToPreview_Morph,GWL_WNDPROC,(LONG)MorphBSPreviewProc);

				MorphBuildButtons();

				int Flag=(!data.MorphInterplay);
				EnableIt(IDC_Int_EDIT,Flag); 
				EnableIt(IDC_Int_SPIN,Flag);

				MorphCreate();
				MorphRealTimePreview(Morph_DrawAll);
				MorphMegaScrollBar();
				SetCursor(LoadCursor(NULL,IDC_ARROW));
			//-----------------------------------------------------
			 return 1;
           }

         case WM_COMMAND:
			 switch (LOWORD(wParam)) 
				{ case IDOK:
				  case IDC_MORPH_OK:
					  if( HIWORD(wParam)!=BN_CLICKED )
								{ SetCursor(LoadCursor(NULL,IDC_WAIT));
				                   MorphGetValues(hWnd);
								   MorphDelete();
				                   MorphCreate();
								   SetCursor(LoadCursor(NULL,IDC_ARROW));
								   MorphFreeButtons();
								   MorphReleaseButtons();
								   ChangedFlare=TRUE;
				                   EndDialog(hWnd,1); 
							     }
								   break; 
				  case IDCANCEL:
				  case IDC_MORPH_CANCEL:
					  if( HIWORD(wParam)!=BN_CLICKED )
								{ FlareEditDialogBoxExit=0;
					               MorphDelete();
								   MorphFreeButtons();
								   MorphReleaseButtons();
								   EndDialog(hWnd,0); 
								 }
								   break; 
				  case IDC_CHECKER:			
								{ data.DrawChecker=IsDlgButtonChecked(hWnd,IDC_CHECKER); 
								  break; 
								}

			 }
             return 1;
       }     
     return 0;
}

BOOL CALLBACK EditMorphCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)	
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
		 SelectObject(hdcBitmap,MorphBackBitmap);
		 BitBlt(hdc,0,0,rect.right,rect.bottom,hdcBitmap,0,0,SRCCOPY);
		 DeleteDC(hdcBitmap);
		 return TRUE;
	}
     
	 if (message == WM_INITDIALOG) flt = (ImageFilter_LensF *)lParam;
     if (flt) return (flt->MorphControl(hWnd,message,wParam,lParam));
     else return(FALSE);
}

BOOL ImageFilter_LensF::MorphShowControl(HWND hWnd) 
{	 	 
	 
	 MorphRealTimeDrawn		=0;

     int i=0; while(data.SelectedElement[i]==0) i++;

	 data.MorphSelectFrom	=i;
	 data.MorphSelectTo		=i;

	 for(i++;i<data.LensFlare.GetNoFlares();i++)
	 if (data.SelectedElement[i]) { data.MorphSelectTo=i; break; }

	 hwPreviewMorphMem		=NULL;
	 hwFromPreviewMorphMem	=NULL;
	 hwToPreviewMorphMem	=NULL;

	 DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_MORPH),hWnd,(DLGPROC)EditMorphCtrlDlgProc,(LPARAM)this);
	 DeleteObject(MorphBackBitmap);

	 if (hwPreviewMorphMem)		free(hwPreviewMorphMem);
	 if (hwFromPreviewMorphMem) free(hwFromPreviewMorphMem);
	 if (hwToPreviewMorphMem)	free(hwToPreviewMorphMem);

	 return TRUE;
}

