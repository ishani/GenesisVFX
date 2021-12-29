//
//  Customisation of explorer file open dialog box for previewing effects.
//
//  Written by Anton S. Yemelyanov for Genesis LW.
//
//  --  I tried to make this easy to understand and copy into other ports of Genesis
//      but at the same time I put this in OO so that these classes can be used for 
//      other purposes.  For instance this is easy to use to make mass thumbnail generator.
//
//  enjoy :)
//
//  07.24.97
//
//  NOTE: if moving this to other Genesis Ports, make sure to change:
//
//  ImageFilter_LensF::Load
//  ImageFilter_LensF::LoadElements
//
//  to receive filename on input.  Check lensf.h & flareeditfile.cpp to see how this is done.
//
//  please add the folowing to the RenderFixed routine (after rinfo initialisation):
//  
//  if(globalPreviewScale) rinfo.SizeMultiplyParticle = globalPreviewScale;
//
//  and ofcourse you will need to declare: extern int globalPreviewScale;  :)
//
//
//  07.24.97 - 7:47 am - no sleep
//
//  Something weird with notification messages.  I added if(!FP) return; into all loading routines...
//  not sure why...  might be some sort of a sharing violation... will check later.
//
//  This is simple: 
//                  previewGFX has all params and data in it.
//
//                  renderFilePreview interfaces with previewGFX to render preview
//
//                  previewExplorer enhances previewGFX to hook explorer
//                  and because it inherits previewGFX, we still use same renderFilePreveiw
//                  to get the preview generated.
//
//
//  Later I will add support to thumbnails.
//


#ifndef _GFX_FILEMAN_
#define _GFX_FILEMAN_



// copy zlib.lib from LW directory into your project dir.
// this is compression utilities.  They are also used for PNG.

// please place genesis/lw/zlib in include path.

#include "zlib.h"


// ---


#define MAX_PREVIEW_PATH  1024


// default data interfacing for previewGFX class (for further enhancements)

#define DDI_DIB  0x01
#define DDI_RGB  0x02

// thumbnails

#define THUMBNAIL_VERSION  0x02

#define THUMBNAIL_WIDTH    400
#define THUMBNAIL_HEIGHT   300


// use this function to open files instead of GetOpenFileName() or GetSaveFileName()

BOOL openGFX( LPOPENFILENAME ofn);
BOOL saveGFX( LPOPENFILENAME ofn);


// was preview enabled last time we opened file ?

extern BOOL globalExplorerPreviewFlag; 
extern BOOL globalThumbnailFlag;

class previewGFX
{
  protected:

      friend class renderFilePreview;     

      char filename[MAX_PREVIEW_PATH];      // filename of the file.

      int width,height;

      int frame;

      float size;

      //
      // 2 ways we can get back image.  must be preallocated before previewing.
      //
      // defaultDataInterface member triggers what we receive back.
      //

      BITMAPINFO  previewImageInfo; // BITMAPINFO for DIB      
      byte *previewImageData;       // DIB               (DDI_DIB)

      byte **R,**G,**B, **A;        // Genesis Bitmap    (DDI_RGB)

      BOOL globalRGB;
      BOOL enableThumbnails;

      int defaultDataInterface; // DDI_DIB or DDI_RGB

      previewGFX()
      {
          globalRGB = FALSE;
          enableThumbnails = FALSE;
          frame = 1;
          size = 1.0f;

          width = THUMBNAIL_WIDTH; height = THUMBNAIL_HEIGHT;

      };

      virtual void initialiseDIB(void);  // allocate and cleanup data based on width & height
      virtual void initialiseRGB(void);
      virtual void cleanupDIB(void);
      virtual void cleanupRGB(void);
      void resetDIB(void);

      
      virtual void generatePreview(void) 
      { 
          if(( !(defaultDataInterface & DDI_DIB)) && ( !(defaultDataInterface & DDI_RGB)))
              throw exception("class previewGFX : Default Data Interface is not defined !");

          generatePreview(filename); 
      };   

      virtual void generatePreview(char *); // generate preview from file (path+filename)

  public:

      // overload this if you need preview for differend frames...
      virtual int getFrame(void) { return frame; };
      virtual float getSize(void) { return size; };

};


// class for interfacing with NT explorer

class previewExplorer : public previewGFX
{
  private:

    HWND hWnd;                          // handle for custom explorer extension
                                        // note: this is a child controls of the
                                        // explorer window.

    BOOL enabled;                       // is preview enabled ?
    BOOL dirChange;
    BOOL validFile;                     // is file valid ?
    BOOL thumbnailsEnabled;             // generate & view thumbnails?

    HWND        previewImage;           // handle for the image control.
    HDC         previewDC;


    void positionDialog(void);          // position explorer dialog in upper half of the screen
    void positionPreviewDIB(void);      // size image control and get width/height
    void enable(void);                  // show image control & enable zoom controls
    void disable(void);                 // hide image control & disable zoom controls

    void initialiseDIB(void);           // init bitmapinfo, data + get paint window handle
    void cleanupDIB(void);              // also, for this particular case, allocate global R G B

    void paint(void);                   // paint preview window

  public:

    previewExplorer() 
    { 
        defaultDataInterface = DDI_DIB;  // we need output only to DIB by renderFilePreview
        
        enabled = globalExplorerPreviewFlag; 
        thumbnailsEnabled = globalThumbnailFlag;
        previewImage = NULL; 

    };

   ~previewExplorer() 
   { 
       globalExplorerPreviewFlag = enabled; 
       globalThumbnailFlag = thumbnailsEnabled;
   };

    void initialise(HWND);

    BOOL hookProc(HWND,UINT,WPARAM,LPARAM);     // hook procedure (as child of explorer window)
    void processNotification(OFNOTIFY*);        // process notifications from explorer control

    virtual void generatePreview(void);         

    void createThumbnails(void);
};


// is this needed ?  

class previewSaver : public previewGFX
{
    previewSaver()
    {
        defaultDataInterface = DDI_DIB;
    }

    ~previewSaver()
    {

    }
};



UINT APIENTRY explorerHook(HWND,UINT,WPARAM,LPARAM);


typedef struct _thumbnailHeader
{
    int version;

    DWORD lowTime;
    DWORD highTime;

    int width;
    int height;
    int size;   

} thumbnailHeader;


class renderFilePreview
{

  private: 

    int valid;                                 // if everything is successfull (ASY - NOT ENABLED)

    BOOL thumbnail;
    BOOL loadedThumbnail;

    previewGFX *config;
      
    ImageFilter_LensF *currentEffect;

    int width, height;

    byte **R,**G,**B,**A;  // deleted after destruction


    // note: destination data SHOULD be preallocated.

    void setDIBData(void); // output DIB data to previewGFX
    void setRGBData(void); // output RGB data to previewGFX

  public:

    renderFilePreview(previewGFX *);  // get source previewGFX
    void initialise(void);            // alloc memory and init data
    void cleanup(void);               // free memory

    void generatePreview(void);       

    void setData(void)
    {
        if(!valid) return;
        if(config->defaultDataInterface & DDI_DIB) setDIBData();
        if(config->defaultDataInterface & DDI_RGB) setRGBData();
    }

    BOOL isThumbnail(void) { return thumbnail; };

    FILETIME *getFileTime(void);
    char *getThumbnailFileName(void);
    BOOL loadThumbnail(void);
    void saveThumbnail(void);

    void inputImageData(FILE *fp);
    void outputImageData(FILE *fp);

};


// the x1 x2 x4 question.

BOOL CALLBACK CatalogDlgProc(HWND, UINT, WPARAM, LPARAM);




#endif _GFX_FILEMAN_