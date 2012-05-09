#include "../OgitorSample/OgitorSample.h"
/*
/ ************************************************************************ /
/ *  Add for memory leak.                                                                    * /
/ ************************************************************************ /
// 在入口函数cpp中添加以下定义和头文件  
#define CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
/ ************************************************************************ /
*/

/************************************************************************/
/*   Visual Leak Detector.                                                                     */
/************************************************************************/
//#include "vld.h"
/************************************************************************/


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
	extern "C"
	{
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		INT WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR strCmdLine,INT)
#else
		int main(int argc,char *argv[])
#endif
		{
/*
			/ ************************************************************************ /
			/ *  Add for memory leak.                                                                    * /
			/ ************************************************************************ /
			// 在入口函数一开始添加以下代码  
			_CrtDumpMemoryLeaks();  
			_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );  

			//num就是刚刚检测出来的内存泄露的地方大括号内的数字  
			_CrtSetBreakAlloc(163);   // ...这截没用啊。。。。
			_CrtSetBreakAlloc(167);  
			_CrtSetBreakAlloc(164);  
			_CrtSetBreakAlloc(3506);  
			_CrtSetBreakAlloc(3499);  
			_CrtSetBreakAlloc(3441);  
			_CrtSetBreakAlloc(3442);  
			/ ************************************************************************ /
*/


			setlocale(LC_ALL,"Chinese-simplified");
			OgitorSampleApp app;
			try
			{
				app.run();
			}
			catch(Ogre::Exception &e)
			{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
 				MessageBoxA(NULL,e.getFullDescription().c_str(),"Wrong~~",MB_OK|MB_ICONERROR|MB_TASKMODAL);
#else
				std::cerr << "出错喽~~" << e.getFullDescription.c_str() << std::endl;
#endif							
			}

			return 0;
		}
#ifdef __cplusplus
	}
#endif