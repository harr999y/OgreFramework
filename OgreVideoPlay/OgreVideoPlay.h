/// Do not include this file directly, always use UtilsOgreDshow.h instead.// Ogre Dshow: small wrapper for video reproduction in Ogre, using Direct Show 9./*Wrapper for video reproduction using Direct Show in the Ogre 3d engine.Coded by H. Hernán Moraldo from Moraldo Gameswww.hernan.moraldo.com.ar/pmenglish/field.php--------------------Copyright (c) 2007 Horacio Hernan MoraldoThis software is provided 'as-is', without any express orimplied warranty. In no event will the authors be held liablefor any damages arising from the use of this software.Permission is granted to anyone to use this software for anypurpose, including commercial applications, and to alter it andredistribute it freely, subject to the following restrictions:1. The origin of this software must not be misrepresented; youmust not claim that you wrote the original software. If you usethis software in a product, an acknowledgment in the productdocumentation would be appreciated but is not required.2. Altered source versions must be plainly marked as such, andmust not be misrepresented as being the original software.3. This notice may not be removed or altered from any sourcedistribution.*/


#ifndef _OGREVEDIOPLAY_
#define _OGREVEDIOPLAY_

#include "../BaseApp/BaseClasses.h"
#include <Qedit.h>// for sample grabber
#include <dshow.h>

namespace hGenUtils 
{  
	// convert a char* to a wchar*  
	WCHAR* convertCStringToWString(const char* string)
	{
		const int MAX_STRINGZ=500;   
		static WCHAR wtext[MAX_STRINGZ+2];    
		if (strlen(string)>MAX_STRINGZ)   
		{        
			throw("hGenUtils::convertCStringToWString buffer isn't big enough"); 
		}    
		// convert text to wchar   
		if (MultiByteToWideChar(      
			CP_ACP,
			// ansi code page     
			0,
			// flags       
			string,
			// orig string      
			-1,
			// calculate len    
			wtext,
			// where to put the string     
			MAX_STRINGZ)
			// maximum allowed path   
			==0)    
		{      
			throw("hGenUtils::convertCStringToWString failed with no extra error info"); 
		}    
		return wtext;
	}
}  

#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__ 

namespace OgreUtils
{
	struct DirectShowData;

	/// A class for playing movies in an ogre texture
	class DirectShowMovieTexture
	{
	public:
		// cons / decons
		/// Initializes the dshow object, and creates a texture with the given dimensions.
		/**
		If dontModifyDimensions is false, the system might modify the texture dimensions
		by setting them to the nearest power of two (useful for old computers).
		(Ie, 1024x512 if the original dimensions were 640x480).
		*/
		DirectShowMovieTexture(int width, int height, bool dontModifyDimensions=true);
		/// Destroys the dshow object
		virtual ~DirectShowMovieTexture();

		// basic movie methods
		/// Loads a given movie
		/**
		/param moviePath A string telling the full path of the file to be loaded.
		/param horizontalMirroring A bool telling whether the video should be rendered
		as if seen through a mirror, or not.
		*/
		//已经强制在载入视频后重新修改纹理的大小,所以最后一个参数现在已经失效
		void loadMovie(const Ogre::String& moviePath, bool horizontalMirroring=false);
		/// Obtains the dimensions of the current movie
		Ogre::Vector2 getMovieDimensions();
		/// Unloads the current movie
		void unloadMovie();

		// methods for movie control
		/// Pauses the current movie
		void pauseMovie();
		/// Starts playing the current movie
		void playMovie();
		/// Makes the current movie rewind
		void rewindMovie();
		/// Stops the current movie
		void stopMovie();
		/// Is the latest video put to play, now playing?
		/** (This is an old implementation of mine; I guess I should re-check this) */
		bool isPlayingMovie();

		// methods on movie texture
		/// Obtain the ogre texture where the movie is rendered
		Ogre::TexturePtr getMovieTexture();
		/// Render a movie frame in the ogre texture
		void updateMovieTexture();
	protected:
		/// Texture where to render the movie
		Ogre::TexturePtr mTexture;
		/// Real texture width
		Ogre::Real mTexWidth;
		/// Real texture height
		Ogre::Real mTexHeight;
		/// Direct Show specific data
		DirectShowData* dsdata;
		/// Do we do horizontal mirroring by software?
		bool mHorizontalMirroring;

		/// Clean the full texture (paint it all black)
		void cleanTextureContents();

		void ResetSize(Ogre::Real w ,Ogre::Real h)
		{
			mTexWidth=w;
			mTexHeight=h;
		}

	};


	//对DirectShowTexture进行一次包装, 直接生成Overlay或者材质
	class DirectShowControl
	{
	public:
		friend class DirectShowManager;

		DirectShowControl(Ogre::String name,Ogre::String filename,int width,int height,Ogre::Viewport *vp,bool overlay=true);
		~DirectShowControl();

		void Destroy();
		Ogre::MaterialPtr getMaterial();
		void PlayMovie();
		void StopMovie();


	protected:
		void createOverlay(); //目前先针对全屏, 以后再扩展任意位置,大小
		void createMaterial();

	private:
		DirectShowMovieTexture *mDirectshowTexture;
		Ogre::MaterialPtr mMaterial;
		int mWidth;
		int mHeight;
		Ogre::String mFilename;
		Ogre::String mName;
		Ogre::Overlay *mOverlay;
		bool isOverlay;
		Ogre::Viewport *mVp;

	};

	typedef std::list<DirectShowControl*> DirectShowControlList;

	class DirectShowManager : public Ogre::Singleton<DirectShowManager>,public Ogre::FrameListener
	{
	public:
		DirectShowManager(Ogre::Viewport *vp);
		~DirectShowManager();

		static DirectShowManager &getSingleton()
		{
			return *ms_Singleton;
		}
		static DirectShowManager *getSingletonPtr()
		{
			return ms_Singleton;
		}
		virtual bool frameStarted(const Ogre::FrameEvent& evt) ;
		DirectShowControl *createDirectshowControl(Ogre::String name,Ogre::String filename,int VWidth,int VHeight,bool overlay=true);

		void DestroyAll();

	private:
		DirectShowControlList mDirectCtrlList;
		Ogre::Viewport *mVP;

	};
}

namespace OgreUtils
{
	struct DirectShowData
	{
		/// Graph object
		IGraphBuilder *pGraph;
		/// Media control object
		IMediaControl *pControl;
		/// Media event object
		IMediaEvent *pEvent;
		/// Grabber filter
		IBaseFilter *pGrabberF;
		/// Grabber object
		ISampleGrabber *pGrabber;
		/// Interface for seeking object
		IMediaSeeking *pSeeking;
		/// Window interface
		/** Useful for some configuration
		*/
		IVideoWindow *pWindow;

		/// Video output width
		int videoWidth;
		/// Video output height
		int videoHeight;
	};

	/// Util function for converting C strings to wide strings
	/** (as needed for path in directshow). */
	WCHAR* util_convertCStringToWString(const char* string);
}

#endif