void ImageFilter_LensF::Load(HWND hwnd,char* fname)
{				
				// warn about not saved
				if( ChangedFlare ) {
					char s[256];
					sprintf(s,"Effect \"%s\" has been modified.\nDo you want to save it?",data.LensFlare.GetName());
					int result=MessageBox(hwnd,s,"Warning",MB_YESNOCANCEL|MB_ICONQUESTION);
					if( result==IDCANCEL ) return;
					if( result==IDYES ) Save(hwnd);
				}

				OPENFILENAME ofn;
				memset(&ofn, 0, sizeof(OPENFILENAME));
		
				char szDirName[MAX_PATH];
				char szFile[1024]="\0";

			if( fname==NULL ) {

				UINT  i, cbString;
				TCHAR  chReplace;    /* string separator for szFilter */
				TCHAR  szFilter[] = _T("Genesis FX Files (*.gfx)|*.gfx|All Files (*.*)|*.*|") ;

				cbString = lstrlen( szFilter ) ;
				chReplace = szFilter[cbString - 1]; /* retrieve wildcard */
				for (i = 0; szFilter[i] != _T('\0'); i++)
					if (szFilter[i] == chReplace) szFilter[i] = _T('\0');
								
				ofn.lStructSize		  = sizeof(OPENFILENAME); 

				ofn.hwndOwner		  = hwnd; 
				ofn.hInstance		  = hInst; 

				ofn.lpstrFilter		  = szFilter;
				ofn.nFilterIndex	  = 1; 

				ofn.lpstrFile		  = szFile; 
				ofn.nMaxFile		  = sizeof(szFile); 
				
				ofn.lpstrInitialDir   = szDirName; 
				ofn.lpstrTitle		  = "Open Genesis File [system]";
				
				ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_LONGNAMES|OFN_HIDEREADONLY; 

				if (!openGFX(&ofn)) return;
			} else {
				strncpy(szFile,fname,1023);
			}
				SetCursor(LoadCursor(NULL,IDC_WAIT));

				//***********************************************************
				//* Read File
					FILE *FP=fopen(szFile,"rb"); 
					char MemMode[10];
					_FileReadS(FP,MemMode,10);

					if ( (!strncmp(MemMode,"GFX100",6)) || (!strncmp(MemMode,"GFX101",6)))
								{	_FileReadI(FP,&data.AntiAliasing);
									_FileReadI(FP,&data.Single);
									_FileReadI(FP,&data.CSE);
									_FileReadI(FP,&data.MaskInside);
									_FileReadF(FP,&data.CameraDepth);
									_FileReadF(FP,&data.ExternalSmoothness);
									_FileReadF(FP,&data.InternalSmoothness);
									_FileReadI(FP,&data.Colourise);
									_FileReadF(FP,&data.ColouriseSmoothness);

									data.LensFlare.Load(FP);
								}
				//***********************************************************

				if( SetTimer(hwnd,ID_TIMER,1000,TimerProc)!=0 )
					Disabled=TRUE;
				
				SetCursor(LoadCursor(NULL,IDC_ARROW));

				data.AnimationPosition=0;
				data.AnimationMax=data.LensFlare.MaxKeyValue();

				fclose(FP);
				
				ChangedFlare=FALSE;
}

void ImageFilter_LensF::Save(HWND hwnd)
{				OPENFILENAME ofn;
				memset(&ofn, 0, sizeof(OPENFILENAME));
		
				char szDirName[MAX_PATH]="";
				char szFile[1024]="\0";
								
				UINT  i, cbString;
				TCHAR  chReplace;    /* string separator for szFilter */
				TCHAR  szFilter[] = _T("Genesis FX Files (*.gfx)|*.gfx|All Files (*.*)|*.*|") ;

				cbString = lstrlen( szFilter ) ;
				chReplace = szFilter[cbString - 1]; /* retrieve wildcard */
				for (i = 0; szFilter[i] != _T('\0'); i++)
					if (szFilter[i] == chReplace) szFilter[i] = _T('\0');
								
				ofn.lStructSize		  = sizeof(OPENFILENAME); 

				ofn.hwndOwner		  = hwnd; 
				ofn.hInstance		  = hInst; 

				ofn.lpstrFilter		  = szFilter;
				ofn.nFilterIndex	  = 1; 

				ofn.lpstrFile		  = szFile; 
				ofn.nMaxFile		  = sizeof(szFile); 
				
				ofn.lpstrInitialDir   = szDirName; 
				ofn.lpstrTitle		  = "Save Genesis File [system]";
				
				ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST|OFN_LONGNAMES|OFN_HIDEREADONLY; 

				if (!saveGFX(&ofn)) return;

				FILE *FP=fopen(szFile,"wb"); 

				SetCursor(LoadCursor(NULL,IDC_WAIT));

				//***********************************************************
				//* Write File
				char FileMode[10]="GFX101";
                _FileWriteS(FP,FileMode,10);

				_FileWriteI(FP,data.AntiAliasing);
				_FileWriteI(FP,data.Single);
				_FileWriteI(FP,data.CSE);
				_FileWriteI(FP,data.MaskInside);
				_FileWriteF(FP,data.CameraDepth);
				_FileWriteF(FP,data.ExternalSmoothness);
				_FileWriteF(FP,data.InternalSmoothness);
				_FileWriteI(FP,data.Colourise);
				_FileWriteF(FP,data.ColouriseSmoothness);

				data.LensFlare.Save(FP);
				//***********************************************************

				if( SetTimer(hwnd,ID_TIMER,1000,TimerProc)!=0 )
					Disabled=TRUE;

				SetCursor(LoadCursor(NULL,IDC_ARROW));

				fclose(FP);

				ChangedFlare=FALSE;
}

void ImageFilter_LensF::LoadElements(HWND hwnd)
{				OPENFILENAME ofn;
				memset(&ofn, 0, sizeof(OPENFILENAME));
		
				char szDirName[MAX_PATH];
				char szFile[1024]="\0";

				int	   i;
				UINT   cbString;
				TCHAR  chReplace;    /* string separator for szFilter */
				TCHAR  szFilter[] = _T("Genesis FX Files (*.gfx)|*.gfx|All Files (*.*)|*.*|") ;

				cbString = lstrlen( szFilter ) ;
				chReplace = szFilter[cbString - 1]; /* retrieve wildcard */
				for (i = 0; szFilter[i] != _T('\0'); i++)
					if (szFilter[i] == chReplace) szFilter[i] = _T('\0');
								
				ofn.lStructSize		  = sizeof(OPENFILENAME); 

				ofn.hwndOwner		  = hwnd; 
				ofn.hInstance		  = hInst; 

				ofn.lpstrFilter		  = szFilter;
				ofn.nFilterIndex	  = 1; 

				ofn.lpstrFile		  = szFile; 
				ofn.nMaxFile		  = sizeof(szFile); 
				
				ofn.lpstrInitialDir   = szDirName; 
				ofn.lpstrTitle		  = "Open Genesis File [elements]";
				
				ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_LONGNAMES|OFN_HIDEREADONLY; 

				if (!openGFX(&ofn)) return;

				FILE *FP=fopen(szFile,"rb"); 

				SetCursor(LoadCursor(NULL,IDC_WAIT));
				
				//***********************************************************
				//* Read File
					char MemMode[10];
					_FileReadS(FP,MemMode,10);

					if ((!strncmp(MemMode,"GFX100",6)) || (!strncmp(MemMode,"GFX101",6)))
								{	int idum;
					                float fdum;

					                _FileReadI(FP,&idum);
									_FileReadI(FP,&idum);
									_FileReadI(FP,&idum);
									_FileReadI(FP,&idum);
									_FileReadF(FP,&fdum);
									_FileReadF(FP,&fdum);
									_FileReadF(FP,&fdum);
									_FileReadI(FP,&idum);
									_FileReadF(FP,&fdum);

									int n;

									char tmpStr[256];
									_FileReadS(FP,tmpStr,256);
									_FileReadI(FP,&n);
									_FileReadI(FP,&idum);

									for(i=0;i!=data.LensFlare.GetNoFlares();i++)
											data.SelectedElement[i]=0;

									for(i=0;i!=n;i++)
											{ data.LensFlare.New();
											  Flare *tmp=data.LensFlare.GetFlare(data.LensFlare.GetNoFlares()-1);
											  tmp->Load(FP);
											  data.SelectedElement[data.LensFlare.GetNoFlares()-1]=1;
											}
								}
				//***********************************************************

				if( SetTimer(hwnd,ID_TIMER,1000,TimerProc)!=0 )
					Disabled=TRUE;

				SetCursor(LoadCursor(NULL,IDC_ARROW));

				data.AnimationPosition=0;
				data.AnimationMax=data.LensFlare.MaxKeyValue();

				fclose(FP);

				ChangedFlare=TRUE;
}

void ImageFilter_LensF::SaveElements(HWND hwnd)
{				OPENFILENAME ofn;
				memset(&ofn, 0, sizeof(OPENFILENAME));
		
				char szDirName[MAX_PATH]="";
				char szFile[1024]="\0";
								
				int  i, cbString;
				TCHAR  chReplace;    /* string separator for szFilter */
				TCHAR  szFilter[] = _T("Genesis FX Files (*.gfx)|*.gfx|All Files (*.*)|*.*|") ;

				cbString = lstrlen( szFilter ) ;
				chReplace = szFilter[cbString - 1]; /* retrieve wildcard */
				for (i = 0; szFilter[i] != _T('\0'); i++)
					if (szFilter[i] == chReplace) szFilter[i] = _T('\0');
								
				ofn.lStructSize		  = sizeof(OPENFILENAME); 

				ofn.hwndOwner		  = hwnd; 
				ofn.hInstance		  = hInst; 

				ofn.lpstrFilter		  = szFilter;
				ofn.nFilterIndex	  = 1; 

				ofn.lpstrFile		  = szFile; 
				ofn.nMaxFile		  = sizeof(szFile); 
				
				ofn.lpstrInitialDir   = szDirName; 
				ofn.lpstrTitle		  = "Save Genesis File [elements]";
				
				ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST|OFN_LONGNAMES|OFN_HIDEREADONLY; 

				if (!saveGFX(&ofn)) return;

				FILE *FP=fopen(szFile,"wb"); 

				SetCursor(LoadCursor(NULL,IDC_WAIT));
				
				//*****************************************************************
				// Save Selected Elements
					int n=0;
					for(i=0;i!=data.LensFlare.GetNoFlares();i++)
					if (data.SelectedElement[i]) n++;

					char FileMode[10]="GFX101";
					_FileWriteS(FP,FileMode,10);

					_FileWriteI(FP,data.AntiAliasing);
					_FileWriteI(FP,data.Single);
					_FileWriteI(FP,data.CSE);
					_FileWriteI(FP,data.MaskInside);
					_FileWriteF(FP,data.CameraDepth);
					_FileWriteF(FP,data.ExternalSmoothness);
					_FileWriteF(FP,data.InternalSmoothness);
					_FileWriteI(FP,data.Colourise);
					_FileWriteF(FP,data.ColouriseSmoothness);

					_FileWriteS(FP,data.LensFlare.GetName(),256);
					_FileWriteI(FP,n);
					_FileWriteI(FP,data.LensFlare.ZSort);

					for(i=0;i!=data.LensFlare.GetNoFlares();i++) 
					if (data.SelectedElement[i])
							{ Flare *tmp=data.LensFlare.GetFlare(i);
					          tmp->Save(FP);
							}

				//*****************************************************************
				if( SetTimer(hwnd,ID_TIMER,1000,TimerProc)!=0 )
					Disabled=TRUE;

				SetCursor(LoadCursor(NULL,IDC_ARROW));

				fclose(FP);
}
