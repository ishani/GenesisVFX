#define STRICT
#include <windows.h>
#include <stdio.h>
#include "Exception.h"
#include <string.h>
#include "resource.h"
#include <TCHAR.H>
#include "PhotoUtil.h"
#include "Utils.h"
#include "FColour.h"
#include "Flare.h"
#include "LensFlares.h"
#include "LensF.h"
#include "GFXfileman.h"



previewExplorer *explorerHandler = NULL;

BOOL globalExplorerPreviewFlag = FALSE;
BOOL globalThumbnailFlag = TRUE;

// global size flags for cataloging thumbnails

BOOL catalogSize1 = TRUE;

// effect size multiplication, must be set in the rendering routine
// after seting up rinfo, you should do 
// if(globalPreviewScale) rinfo.SizeMultiplyParticle = globalPreviewScale;

int globalPreviewScale = 0;


// directory handling routines - If not done, LW will screwup every scene file it saves!
// but this is also beneficial for other ports because Genesis will remember the last
// directory it has accessed...

char genesisDir[1024] = { 0x00 };


// function to open GFX files, to be used instead of GetOpenFileName()

BOOL openGFX( LPOPENFILENAME ofn)
{

    char savedDir[1024];

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !PROVIDE SYSTEM VERSION DETECTION! <- bypass hook install for NT 3.51 :(
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    or add proper handling of allmost
                                          // everything. window sizing, messages... etc.

    ofn->Flags |= OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_EXPLORER;
    ofn->hInstance = hInst;
    ofn->lpTemplateName = MAKEINTRESOURCE(IDD_FILEPREVIEW);

    ofn->lpfnHook = (LPOFNHOOKPROC) explorerHook;


    explorerHandler = new previewExplorer;  



    GetCurrentDirectory(1024,savedDir);

    if(genesisDir[0])
        SetCurrentDirectory(genesisDir);
	else {
		// setup default cd-rom drive letter
		char root[256];
		strcpy(root,"a:\\gfx");
		char c;
		for(c='A';c<='Z';c++) {
			root[0]=c;
			if( GetDriveType(root)==DRIVE_CDROM )
				break;
		}
		if( c<='Z' )
			if( !SetCurrentDirectory(root) ) {
				// locate registry entry for photoshop
				HKEY hkey;
				DWORD type;
				unsigned long size=255;
				int retval=ERROR_SUCCESS;
				retval=RegOpenKeyEx(HKEY_CLASSES_ROOT,"Photoshop.Plugin\\shell\\open\\command",0,KEY_QUERY_VALUE,&hkey);
				if( retval==ERROR_SUCCESS ) {
					retval=RegQueryValueEx(hkey,NULL,NULL,&type,(byte*)root,&size);
					if( retval==ERROR_SUCCESS ) {
						char drive[_MAX_DRIVE];
						char path[_MAX_PATH];
						_splitpath(root,drive,path,NULL,NULL);
						sprintf(root,"%s%sgfx",drive,path);
					}
					RegCloseKey(hkey);
				}
			}
		}

    BOOL result = GetOpenFileName( ofn );

    GetCurrentDirectory(1024,genesisDir);
    SetCurrentDirectory(savedDir);




    delete explorerHandler;
    explorerHandler = NULL; // just in case

    return result;

}



BOOL saveGFX( LPOPENFILENAME ofn)
{

    char savedDir[1024];

    
    GetCurrentDirectory(1024,savedDir);

    if(genesisDir[0])
        SetCurrentDirectory(genesisDir);

    BOOL result = GetSaveFileName( ofn );

    GetCurrentDirectory(1024,genesisDir);
    SetCurrentDirectory(savedDir);

    return result;

}



UINT APIENTRY explorerHook(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{

    try
    {
        return (explorerHandler->hookProc(hWnd,message,wParam,lParam));
    }
    catch(exception ex)
    {
        ex.handle();
    }

    return FALSE;
}



BOOL previewExplorer::hookProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
            {
                initialise(hDlg);
            } break;

        case WM_NOTIFY:
            {
                processNotification((OFNOTIFY*)lParam);
            } break;

        case WM_PAINT:
            {
                // in case we receive paint before init, check handle validity.

                if(previewImage) paint();

                return DefWindowProc(hWnd,message,wParam,lParam);

            } break;

        case WM_COMMAND:
            {
                switch(LOWORD(wParam))
                {
                    case IDC_PREVIEW:
                        {
                            enabled = !enabled;

                            enabled ? enable() : disable();

                            if(enabled)
                            {
                                generatePreview();
								ValidateRect(GetDlgItem(hWnd,IDC_PREVIEW_DIB),NULL);
                            }

                        } break;


                    case IDC_GENERATE_THUMBNAILS:
                        {

                              createThumbnails();

                        } break;
                }
            } break;
    }

    return FALSE;
}





void previewExplorer::initialise(HWND hDlg)
{
    hWnd = hDlg;

    SendMessage(GetDlgItem(hWnd,IDC_PREVIEW),BM_SETCHECK,enabled,NULL);
}



void previewExplorer::processNotification(OFNOTIFY *notify)
{

    switch(notify->hdr.code)
    {
        case CDN_INITDONE:
            {
                positionPreviewDIB();
                enabled ? enable() : disable();

                positionDialog();

                initialiseDIB();

                return;

            } break;

        case CDN_FILEOK:
            {
                cleanupDIB();

                return;                
                
            } break;

        case CDN_SELCHANGE:
            {
                if(dirChange)
                {
                    dirChange = FALSE;
                    return;
                }

                SendMessage(GetParent(hWnd),CDM_GETFILEPATH,MAX_PREVIEW_PATH,(LPARAM)filename);

                if(!enabled) return;

                generatePreview();

                paint();

                return;

            } break;

        case CDN_FOLDERCHANGE:
            {
                filename[0] = NULL;
                resetDIB();
            } break;

        default:
            {
                if(previewImage) paint();
            } break;


    }
}



void previewGFX::initialiseDIB(void)
{
    previewImageInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    previewImageInfo.bmiHeader.biPlanes = 1;
    previewImageInfo.bmiHeader.biBitCount = 24;
    previewImageInfo.bmiHeader.biCompression = BI_RGB;
    previewImageInfo.bmiHeader.biClrUsed = 0;
    previewImageInfo.bmiHeader.biClrImportant = 0;
    previewImageInfo.bmiHeader.biWidth = width;
    previewImageInfo.bmiHeader.biHeight = height;
    previewImageInfo.bmiHeader.biSizeImage = width * height * 3;

    previewImageData = new byte[width*height*3];
    memset(previewImageData,0,width*height*3);
}

void previewGFX::resetDIB(void)
{
    if(previewImageData)
      memset(previewImageData,0,width*height*3);
}


void previewGFX::cleanupDIB(void)
{
    delete [] previewImageData;
}


void previewGFX::initialiseRGB(void)
{
    Alloc2D(R,byte,height,width);
    Alloc2D(G,byte,height,width);
    Alloc2D(B,byte,height,width);
    Alloc2D(A,byte,height,width);
}

void previewGFX::cleanupRGB(void)
{
    Free2D(R,height);
    Free2D(G,height);
    Free2D(B,height);
    Free2D(A,height);
}




void previewExplorer::initialiseDIB(void)
{

    previewGFX::initialiseDIB();

    // this is a cheat for faster performance
    // normally you allocate RGB buffers only if
    // needed, and previewFileRender allocates it's own
    // but in here we want to do it only once...
    //
    // globalRGB will let previewFileRender know if
    // there is global RGB buffers.
    //

    previewGFX::initialiseRGB();

    globalRGB = TRUE;

    // --- get handle of the paint control

    previewImage = GetDlgItem(hWnd,IDC_PREVIEW_DIB);
}

void previewExplorer::cleanupDIB(void)
{
    previewGFX::cleanupDIB();

    // refer to previewExplorer::initialiseRGB for comments

    previewGFX::cleanupRGB();
}


void previewExplorer::paint(void)
{
    previewDC = GetDC(previewImage);

    SetDIBitsToDevice(previewDC,0,0,width,height,0,0,0,height,
                      previewImageData,&previewImageInfo,DIB_RGB_COLORS);

    ReleaseDC(previewImage,previewDC);
}



void previewExplorer::generatePreview(void)
{

    if(!enabled) return;
    
    if((!strstr(filename,".gfx")) && (!strstr(filename,".GFX"))) return;  

    // validate accessibility trap...  had to do this because something is weird.

    FILE *fp = fopen(filename,"rb");
    if(!fp)
    {
        resetDIB();
        return; 
    }
    else fclose(fp);

    previewGFX::generatePreview();
}



void previewGFX::generatePreview(char *filename)
{

    renderFilePreview *previewGenerator;

    previewGenerator = new renderFilePreview(this); // load/setup effect & allocate local RGB buffer)

    if(!previewGenerator->loadThumbnail())
    {
        previewGenerator->generatePreview();            // render effect
        previewGenerator->saveThumbnail();
    }

    // deleting save thumbnail as well;

    delete previewGenerator;                        // delete effect & local RGB buffer
}







// Please note that this function is LW oriented. 
// It enabled GLW style status and checks for abort;

volatile int globalAbort;

void previewExplorer::createThumbnails(void)
{


    // force enabled on for generate preview

    int enableState = enabled;
    int sizeState = (int)size;

    enabled = TRUE;


    // for LW, move this window to the top because of the status bar...

//    RECT rect;
  //  GetWindowRect(GetParent(hWnd),&rect);
//    int top = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
 //   SetWindowPos(GetParent(hWnd),HWND_TOP,rect.left,top,0,0,SWP_NOSIZE);

    // get into file processing

    char *dir = new char[1024];

    lstrcpy(dir,filename);

    int len = lstrlen(dir);

    for(; len >= 0; len--)
        if(dir[len] != '\\')
          dir[len] = NULL;
        else
          break;

    char *sDir = new char[1024];
    lstrcpy(sDir,dir);

    lstrcat(dir,"*.gfx");

    HANDLE file;
    WIN32_FIND_DATA fData;



    renderFilePreview *previewGenerator = new renderFilePreview(this); 
    
    file = FindFirstFile(dir, &fData);

    while(file != INVALID_HANDLE_VALUE)
    {


        lstrcpy(filename,sDir);
        lstrcat(filename,fData.cFileName);

        if(catalogSize1)
        {
            size = 1.0f;

            if(/*!previewGenerator->loadThumbnail()*/ TRUE )
            {
                previewGenerator->generatePreview();        
                if(globalAbort) break;
                previewGenerator->saveThumbnail();
                paint();
            }

        }


        if(!FindNextFile(file,&fData)) break;

    };

//    FindClose(file);

    delete previewGenerator;

    delete [] dir;
    delete [] sDir;

    enabled = enableState;
    size = (float)sizeState;

    // reset dialog to original position

//    positionDialog();

    globalAbort = FALSE;

//    MessageBox(NULL,"Thumbnail Generation Done.","Genesis VFX",MB_ICONINFORMATION | MB_SYSTEMMODAL | MB_OK);
}




// set renderFilePreview object to get data from the current previewGFX 
// (which is represented by previewExplorer.)


renderFilePreview::renderFilePreview(previewGFX *configData)
{

    config = configData;

    height = config->height;
    width = config->width;

}


// init or get RGBs load effect and setup all data. that is IF no thumbnail

void renderFilePreview::initialise(void)
{

    if(config->globalRGB)
    {
        R = config->R;
        G = config->G;
        B = config->B;
        A = config->A;
    }
    else
    {
        Alloc2D(R,byte,height,width);
        Alloc2D(G,byte,height,width);
        Alloc2D(B,byte,height,width);
        Alloc2D(A,byte,height,width);
    }

    for(int y = 0; y < height; y++)
      for(int x = 0; x < width; x++)
      {
        R[y][x] = G[y][x] = B[y][x] = A[y][x] = NULL;
      }

    currentEffect = new ImageFilter_LensF;

    currentEffect->Load(NULL,config->filename);

    currentEffect->data.ImageWidth = width;
    currentEffect->data.ImageHeight = height;

    currentEffect->data.PosX = -0.55f; // .875

    currentEffect->Img_R = R;
    currentEffect->Img_G = G;
    currentEffect->Img_B = B;
    currentEffect->Img_A = A;

    currentEffect->data.LensFlare.GetFrame(config->getFrame());
//    currentEffect->Time = config->getFrame(); ?

    valid = TRUE;
}


void renderFilePreview::cleanup()
{
    if(!config->globalRGB)
    {
        Free2D(R,height);
        Free2D(G,height);
        Free2D(B,height);
        Free2D(A,height);
    }

    delete currentEffect;
}


BOOL renderFilePreview::loadThumbnail(void)
{

    loadedThumbnail = FALSE;

    if(!config->previewImageData) return FALSE; 

    thumbnailHeader th;


    char *fname = getThumbnailFileName();

    FILE *fp = fopen(fname,"rb");

    delete fname;

    if(!fp) return FALSE;

    fread(&th,sizeof(thumbnailHeader),1,fp);

    if(th.version != THUMBNAIL_VERSION) 
    {
        fclose(fp);
        return FALSE;
    }


    // check last write time  <- check if this works fine.

    FILETIME *timeData = getFileTime();
    
    if((timeData->dwLowDateTime != th.lowTime) || (timeData->dwHighDateTime != th.highTime))
    {
        delete timeData;
        fclose(fp);
        return FALSE;
    }

    delete timeData;


    if((th.width != width) || (th.height != height)) 
    {
        fclose(fp);
        return FALSE;
    }


    
//    fread(config->previewImageData,th.size,1,fp);
//      ^
//   before compression

    inputImageData(fp);  // <- use zLib to read and decompress

    int checksum;        // jsut see if get proper character. simple :)
    fread(&checksum,sizeof(int),1,fp);

    if(th.size != checksum)
    {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);

    return TRUE;
}

void renderFilePreview::inputImageData(FILE *fp)
{
    if(!fp) return;
    
    unsigned long dataSize;

    fread(&dataSize,sizeof(unsigned long),1,fp);

    byte *data = new byte[dataSize];

    fread(data,dataSize,1,fp);

    unsigned long destSize = width*height*3;

    uncompress(config->previewImageData,&destSize,data,dataSize);

    delete [] data;
}

void renderFilePreview::outputImageData(FILE *fp)
{
    if(!fp) return;

    unsigned long dataSize = width*height*3;

    unsigned long srcSize = width*height*3;

    dataSize = (dataSize / 3) * 4; // increase by 1/3


    byte *data = new byte[dataSize];

    compress(data,&dataSize,config->previewImageData,srcSize);

    fwrite(&dataSize,sizeof(unsigned long),1,fp);

    fwrite(data,dataSize,1,fp);

    delete [] data;

}

void renderFilePreview::saveThumbnail(void)
{

    // validity check

 //   if(loadedThumbnail) return;
    if(!config->previewImageData) return;

    thumbnailHeader th;

    th.version = THUMBNAIL_VERSION;

    // set last write time

    FILETIME *timeData = getFileTime();

    if(!timeData) return;  // file not found... ??

    th.lowTime = timeData->dwLowDateTime;
    th.highTime = timeData->dwHighDateTime;

    delete timeData;

    th.width = width;
    th.height = height;
    th.size = width*height*3;

    // all data is filled, now save thumbnail

    char *fname = getThumbnailFileName();  // take filename from config.

    FILE *fp = fopen(fname,"wb");

    delete fname;

    if(!fp) return;

    fwrite(&th,sizeof(thumbnailHeader),1,fp);

    outputImageData(fp);

    //fwrite(config->previewImageData,th.size,1,fp);
    //  ^
    // before compression

    fwrite(&th.size,sizeof(int),1,fp);

    fclose(fp);
}

char *renderFilePreview::getThumbnailFileName(void)
{
    int len = lstrlen(config->filename);

    char *data = new char[len+10];

    lstrcpy(data,config->filename);

    len -= 4;

    data[len] = NULL;

    int size = (int) config->getSize();

    switch(size)
    {
        case 1:
            {
                lstrcat(data,".x1.gft");
            } break;

        case 2:
            {
                lstrcat(data,".x2.gft");
            } break;
        case 4:
            {
                lstrcat(data,".x4.gft");
            } break;
    }

    return data;
}

FILETIME *renderFilePreview::getFileTime(void)
{
    WIN32_FIND_DATA fData;

    HANDLE fHandle = FindFirstFile(config->filename,&fData);
    
    if(fHandle == INVALID_HANDLE_VALUE) return NULL;

    FindClose(fHandle);

    FILETIME *fTime = new FILETIME;

//    fTime->dwLowDateTime = fData.ftCreationTime.dwLowDateTime;
//    fTime->dwHighDateTime = fData.ftCreationTime.dwHighDateTime;
    fTime->dwLowDateTime = fData.ftLastWriteTime.dwLowDateTime;
    fTime->dwHighDateTime = fData.ftLastWriteTime.dwHighDateTime;


    return fTime;
}




void renderFilePreview::generatePreview(void)
{

	SetCursor(LoadCursor(NULL,IDC_APPSTARTING));  // IDC_APPSTARTING

    initialise();


    // I pass the size to the renderer via a global var which otherwise is allways NULL...

    globalPreviewScale = (int)config->getSize();

    SetCursor(LoadCursor(NULL,IDC_APPSTARTING));

    try
    {
		// render effect - this has been modified
		RenderNFO rinfo; rinfo.Init();

		rinfo.xres=width; rinfo.yres=height;
		rinfo.CameraDepth=currentEffect->data.CameraDepth;
		rinfo.Time=0.0f;

		rinfo.Img_R=R;
		rinfo.Img_G=G;
		rinfo.Img_B=B;
		rinfo.Img_A=A;


		rinfo.xpos=width/4.0f;
		rinfo.ypos=height/2.0f;

		rinfo.UseSingle=1;

		StatusText("File Preview");
		globalAbort=currentEffect->data.LensFlare.RenderHQArea(rinfo);

//        currentEffect->Render();
    }

    catch(exabort exa) {}    

    globalPreviewScale = NULL;

    setData();

    cleanup();

	SetCursor(LoadCursor(NULL,IDC_ARROW));

}

void renderFilePreview::setRGBData(void)
{
    for(int y = 0; y < height; y++)
      for(int x = 0; x < width; x++)
      {
        config->R[y][x] = R[y][x];
        config->G[y][x] = G[y][x];
        config->B[y][x] = B[y][x];
        config->A[y][x] = A[y][x];
      }
}

void renderFilePreview::setDIBData(void)
{
    byte *data = config->previewImageData;

    for(int y = height-1; y >= 0; y--)     // flip the bitmap for DIB
      for(int x = 0; x < width; x++)
      {
            *(data++) = B[y][x];
            *(data++) = G[y][x];
            *(data++) = R[y][x];
      }
}








// thumbnail catalog dialog proc

BOOL CALLBACK CatalogDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch(message)
    {
        case WM_INITDIALOG:
			break;

        case WM_COMMAND:
            {
                switch(LOWORD(wParam))
                {

                    case IDOK:
                        {
                            EndDialog(hWnd,1);
                            return TRUE;

                        } break;

                    case IDCANCEL:
                        {
                            EndDialog(hWnd,0);
                            return TRUE;

                        } break;

                };

                if( !catalogSize1 )
                {
                    catalogSize1 = TRUE;
                }

            } break;
    }

    return FALSE;
}



// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------








// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------

// Window positioning...  ugh...


// nightmare --

// position image control and the progress bar...

void previewExplorer::positionPreviewDIB(void)
{
    RECT pRect;
    GetWindowRect(GetParent(hWnd),&pRect);

    RECT cRect;
    GetClientRect(GetParent(hWnd),&cRect);

    RECT dibRect;
    GetWindowRect(GetDlgItem(hWnd,IDC_PREVIEW_DIB),&dibRect);

    int margin = dibRect.left - pRect.left - GetSystemMetrics(SM_CXDLGFRAME);

    int yOffset = GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYCAPTION);

    dibRect.top -= pRect.top;
    dibRect.top -= yOffset;

    dibRect.left = margin;
    dibRect.right = cRect.right - margin*2;

    dibRect.bottom = cRect.right - margin*2;

    dibRect.bottom = (dibRect.bottom / 4) * 3;


    RECT hRect;
    GetWindowRect(hWnd,&hRect);

    hRect.top -= pRect.top;
    hRect.top -= yOffset;


    // this is very confusing but our window (child) is the same size and position as explorer's
    // window...  (explorer sizes it by itself first)  and repositions controls to be below explorers controls :-/

    int hBottom = dibRect.top + dibRect.bottom + margin ;  

    // size main explorer window

    SetWindowPos(hWnd,NULL,0,0,dibRect.right+margin,hBottom, SWP_NOMOVE | SWP_NOZORDER);


//    dibRect.right &= 0xfffffff0;  // well we now use fixed width - 400


// w&h are now predefined at the top of .hpp

//    width = 400;
//    height = 300;

    // position image frame

	GetClientRect(GetDlgItem(hWnd,IDC_PREVIEW_DIB),&dibRect);
    SetWindowPos(GetDlgItem(hWnd,IDC_PREVIEW_DIB),HWND_TOP,dibRect.left,dibRect.top,dibRect.right,height+2,SWP_NOMOVE);

}


// pull down the window and enable controls

void previewExplorer::enable(void)
{

    EnableWindow(GetDlgItem(hWnd,IDC_GENERATE_THUMBNAILS),TRUE);


    // ---

    RECT pRect;
    GetWindowRect(GetParent(hWnd),&pRect);

    RECT dibRect;
    GetWindowRect(GetDlgItem(hWnd,IDC_PREVIEW_DIB),&dibRect);

    int margin = dibRect.left - pRect.left;

    pRect.bottom = dibRect.bottom + margin;

    pRect.bottom -= pRect.top;
    pRect.right -= pRect.left;

    SetWindowPos(GetParent(hWnd),HWND_TOP,pRect.left,pRect.top,pRect.right,pRect.bottom,SWP_NOMOVE);
}

// pull-up the window and disable controls

void previewExplorer::disable(void)
{
    EnableWindow(GetDlgItem(hWnd,IDC_GENERATE_THUMBNAILS),FALSE);


    // ---
    
    RECT pRect;

    GetWindowRect(GetParent(hWnd),&pRect);

    pRect.right -= pRect.left;
    pRect.bottom -= pRect.top;

    RECT dibRect;

    GetWindowRect(GetDlgItem(hWnd,IDC_PREVIEW_DIB),&dibRect);

    int bottom = dibRect.top - pRect.top;

    SetWindowPos(GetParent(hWnd),HWND_TOP,pRect.left,pRect.top,pRect.right,  bottom, SWP_NOMOVE);

}

// position the dialog box
//
//  if preview enabled, center it on the screen
//  if not place it in the center of the upper half of the screen
//

void previewExplorer::positionDialog(void)
{

    RECT rect;
    GetWindowRect(GetParent(hWnd),&rect);

    rect.right -= rect.left;
    rect.bottom -= rect.top;

    int cy = GetSystemMetrics(SM_CYSCREEN);
    int cx = GetSystemMetrics(SM_CXSCREEN);

    int left = cx/2 - rect.right/2;

    if(cy == 600) // 800x600
    {
        SetWindowPos(GetParent(hWnd),HWND_TOP, left, 0 , 0 , 0 , SWP_NOSIZE);
        return;
    }


    int centerY = cy / 2;

    int newY = centerY;

    if(!enabled)
    {
        newY -= rect.bottom;   // bottom is at the center
    }
    else
    {
        newY -= rect.bottom/2; // whole box is centered

    }

    SetWindowPos(GetParent(hWnd),HWND_TOP, left, newY , 0 , 0 , SWP_NOSIZE);

}



