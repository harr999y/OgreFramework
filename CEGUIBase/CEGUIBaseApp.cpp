#include "CEGUIBaseApp.h"

static bool gEditKeysEnable = true;
//static bool gKeyDelay = false;
static bool gFullScreen = false;
static CEGUI::Window * oldActiveWindow = NULL;
static int oldActiveWindowCaraIndex = 0;

LRESULT CALLBACK ChineseHook(int nCode,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK KeyJudge(int nCode,WPARAM wParam,LPARAM lParam);

void CEGUIBaseListener::initialise()
{
	using namespace CEGUI;
	Ogre::LogManager::getSingleton().logMessage("Being Iniltilizing OIS");
	OIS::ParamList pl;
	size_t windowHnd  = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW",&windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"),windowHndStr.str()));

//  	pl.insert(std::make_pair(std::string("w32_mouse"),"DISCL_FOREGROUND"));
//  	if (!gFullScreen)
//  	{
//  		pl.insert(std::make_pair(std::string("w32_mouse"),"DISCL_NONEXCLUSIVE"));
//  	}

	mInputMgr = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*> (mInputMgr->createInputObject(OIS::OISKeyboard,true));
	mMouse = static_cast<OIS::Mouse*> (mInputMgr->createInputObject(OIS::OISMouse,true));
	//const OIS::MouseState & mouseState = mMouse->getMouseState();
	//mouseState.width = mWindow->getWidth();
	//mouseState.height = mWindow->getHeight();
	ShowCursor(false);

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	windowResized(mWindow);

	Ogre::WindowEventUtilities::addWindowEventListener(mWindow,this);

	createGUI();

	doSome();

	mRoot->addFrameListener(this);

	//gHook = SetWindowsHookEx(WH_GETMESSAGE,ChineseHook,NULL,GetCurrentThreadId());

	//gHook2 = SetWindowsHookEx(WH_KEYBOARD,KeyJudge,NULL,GetCurrentThreadId());
}


void CEGUIBaseListener::doSome()
{}


bool CEGUIBaseListener::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if(mWindow->isClosed())
		return false;
	if(mShutDown)
		return false;

	mMouse->capture();
	mKeyboard->capture();

	updateFps(evt);	
	updateSpeed(evt);
	updateArrow(evt);

	mCameraMan->frameRenderingQueued(evt);

	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

	mApp->frameRenderingQueued(evt);

	return true;
}


CEGUI::MouseButton CEGUIBaseListener::convertOISButtonToCegui(int buttonID)
{
	using namespace OIS;

	switch (buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;
	case OIS::MB_Right:
		return CEGUI::RightButton;
	case OIS::MB_Middle:
		return CEGUI::MiddleButton;
	default:
		return CEGUI::LeftButton;
	}
}


bool CEGUIBaseListener::frameStarted(const Ogre::FrameEvent& evt)
{	
	mApp->frameStarted(evt);
	return true;
}

bool CEGUIBaseListener::frameEnded(const Ogre::FrameEvent& evt)
{
	mApp->frameEnded(evt);
	return true;
}

bool CEGUIBaseListener::keyPressed(const OIS::KeyEvent &arg)
{
	//CEGUI::Windo
	if(arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
		return true;
	}

	mCameraMan->injectKeyDown(arg);

	if (gEditKeysEnable && GetKeyState(VK_LCONTROL) & 0xF0)
	{
		if(arg.key == OIS::KC_1 )
		{
			mDeltaSpeedMouseChanged = 0.05f;
			mMouseSpeedChanged = true;
		}
		else if(arg.key == OIS::KC_2)
		{
			mDeltaSpeedMouseChanged = -0.05f;
			mMouseSpeedChanged = true;
		}
		else if(arg.key == OIS::KC_4)
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
		else if(arg.key == OIS::KC_3)
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		else if(arg.key == OIS::KC_5)
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
		else if(arg.key == OIS::KC_R)
			mCameraMan->getCamera()->setPolygonMode(Ogre::PM_WIREFRAME);
		else if(arg.key == OIS::KC_E)
			mCameraMan->getCamera()->setPolygonMode(Ogre::PM_SOLID);
		else if(arg.key == OIS::KC_T)
			mCameraMan->getCamera()->setPolygonMode(Ogre::PM_POINTS);
		else if(arg.key == OIS::KC_A)
		{
			handleCopyPasteCut(ALL);
		}
		else if (arg.key == OIS::KC_C)
		{
			handleCopyPasteCut(COPY);
		}
		else if (arg.key == OIS::KC_V)
		{
			handleCopyPasteCut(PASTE);
		}
		else if (arg.key == OIS::KC_X)
		{
			handleCopyPasteCut(CUT);
		}
		return true;
	}

	// do event injection
	CEGUI::System& cegui = CEGUI::System::getSingleton();

	//先获取当前正在输入的窗口的输入法句柄
	/*HWND hwnd = GetActiveWindow();
	
	//mWindow->getCustomAttribute("Window",(void *)&hwnd);
	HIMC hIMC = ImmGetContext(hwnd);

	// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.
	DWORD dwSize = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);*/



	/*if (gEditKeysEnable)  
	{
		if (gKeyDelay)
		{
			gKeyDelay = false;
		}
	}*/

	// key down
	if (gEditKeysEnable)
	{
		if (arg.key == OIS::KC_BACK || arg.key == OIS::KC_RETURN)
			return true;
		cegui.injectKeyDown(arg.key);
		if(arg.key == OIS::KC_LEFT)   //mLeftArrow,mRightArrow 是为了控制一直按键时光标迅速左右移动
			mLeftArrow = true;
		else if(arg.key == OIS::KC_RIGHT)
			mRightArrow = true;
	}
	

	return mApp->keyPressed(arg);
}

	/*HKL * hkl;
	int a = GetKeyboardLayoutList(5,hkl);
	LPSTR * lp;

	GetKeyboardLayoutName(*lp);
	if (ImmIsIME(hkl[1]))
	{
		LPMSG msg = 0;
		HWND hwnd;
		mWindow->getCustomAttribute("Window",(void *)&hwnd);
		GetMessageW(msg,hwnd,WM_IME_CHAR,WM_IME_CHAR);
		if (msg)
		{
			CEGUI::utf8 * string = (CEGUI::utf8 *)msg->wParam;
			CEGUI::System::getSingleton().injectChar(*(CEGUI::utf8 *)string);
		}
	}
	else if(ImmIsIME(hkl[0]))
	{
		cegui.injectKeyDown(arg.key);
		CEGUI::System::getSingleton().injectChar(arg.text);
	}
	// now character
	//cegui.injectChar(arg.text);*/

bool CEGUIBaseListener::keyReleased(const OIS::KeyEvent &arg)
{
	if(arg.key == OIS::KC_1 || arg.key == OIS::KC_2 || arg.key == OIS::KC_LCONTROL)
	{
		mDeltaSpeedMouseChanged = 0.0f;
	}
	if (gEditKeysEnable)
	{
		CEGUI::System::getSingleton().injectKeyUp(arg.key);
		if(arg.key == OIS::KC_LEFT )
		{
			mArrowTimePassed = 0.0f;
			mLeftArrow = false;
		}
		else if(arg.key == OIS::KC_RIGHT)
		{
			mArrowTimePassed = 0.0f;
			mRightArrow = false;
		}
		CEGUI::System::getSingleton().injectKeyUp(arg.key);
	}
	mCameraMan->injectKeyUp(arg);
	return mApp->keyReleased(arg);
}

void IMMFollow();

LRESULT CALLBACK ChineseHook(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(gHook,nCode,wParam,lParam);
	}
/*	if (!ImmIsIME(GetKeyboardLayout(0)))    //这个有点奇怪，在我这儿就是不好使，不过幸好反正也用不到~
	{
		return CallNextHookEx(gHook,nCode,wParam,lParam);
	}*/

	MSG * msg = (MSG *) lParam;

	switch(msg->message)   
	{   
	case WM_CHAR:   
		{   	
			if (!(GetKeyState(VK_LCONTROL) & 0xF0))  //过滤LControl
			{
				CEGUI::utf32 word = (CEGUI::utf32)msg->wParam;
				if (word >= 32)
					CEGUI::System::getSingleton().injectChar(word);  //将工程设为Unicode模式，这儿可以直接接收，而网上那个在多字节模式下的貌似有点问题，码和CEGUI的识别对不上~~~
				else if (word == 8)
					CEGUI::System::getSingleton().injectKeyDown(OIS::KC_BACK);
				else if (word == 13)
					CEGUI::System::getSingleton().injectKeyDown(OIS::KC_RETURN);
			}
		}
		break;
	case WM_IME_STARTCOMPOSITION:
		{		
			if (gFullScreen)
			{
				CEGUI::Editbox * eb = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("IME/Composition");
				eb->show();
				oldActiveWindow = CEGUI::System::getSingleton().getGUISheet()->getActiveChild();
				CEGUI::String type = oldActiveWindow->getType();
				if (type == "TaharezLook/MultiLineEditbox" || type == "Vanilla/MultiLineEditbox" || type == "WindowsLook/MultiLineEditbox")
				{
					oldActiveWindowCaraIndex = static_cast<CEGUI::MultiLineEditbox*>(oldActiveWindow)->getCaratIndex();
				}
				else if(type == "TaharezLook/Editbox" || type == "Vanilla/Editbox" || type == "WindowsLook/Editbox")
				{
					oldActiveWindowCaraIndex = static_cast<CEGUI::Editbox*>(oldActiveWindow)->getCaratIndex();
				}
				CEGUI::Window * win = CEGUI::WindowManager::getSingleton().getWindow("IME/CandidateList");
				win->show();
				eb->activate();	
				//win->activate();
			}
			gEditKeysEnable  = false;
			//gKeyDelay = true;
			IMMFollow();	
		}
		break;
	case WM_IME_ENDCOMPOSITION:
		{
			if (gFullScreen)
			{
				CEGUI::Editbox * eb = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("IME/Composition");
				CEGUI::String string(eb->getText().data());
				
				if (oldActiveWindow)
				{
					CEGUI::String type = oldActiveWindow->getType();
					if (type == "TaharezLook/MultiLineEditbox" || type == "Vanilla/MultiLineEditbox" || type == "WindowsLook/MultiLineEditbox" || type == "TaharezLook/Editbox" || type == "Vanilla/Editbox" || type == "WindowsLook/Editbox")
					{
						oldActiveWindow->activate();
						oldActiveWindow->insertText(string,oldActiveWindowCaraIndex);
					}
				}
				
				eb->hide();
				CEGUI::Window * win = CEGUI::WindowManager::getSingleton().getWindow("IME/CandidateList");
				win->hide();
				//win->deactivate();
			}	
			gEditKeysEnable  = true;
			//gKeyDelay = true;
		}
		break;
	/*case WM_IME_NOTIFY:
		{
			switch (wParam)
			{
			case IMN_CHANGECANDIDATE:
				{
					HIMC hIMC = ImmGetContext(GetActiveWindow());
					CEGUI::Window * win = CEGUI::WindowManager::getSingleton().getWindow("IME/CandidateList");
					LPCANDIDATELIST pList;
					LONG dwSize = ImmGetCandidateListA(hIMC,0,NULL,0);
					pList = (LPCANDIDATELIST)GlobalAlloc( GPTR, dwSize);
					ImmGetCandidateListA(hIMC,0,pList,dwSize);
					//dwSize /= 2;
					DWORD * dwOffset = &pList->dwOffset[0];
					dwOffset += pList->dwPageStart;
					char buf[355];
					DWORD j = 0;
					for (DWORD i=0;i<pList->dwCount;i++)
					{
						LPSTR s = (LPSTR)pList + *dwOffset++;			
						sprintf( buf + j,"%d.%s ",i+1,s);
						j += strlen(s) + 3;
					}
					buf[j + 1] = '\0';
					int num = MultiByteToWideChar(CP_ACP, NULL, buf, j + 1, NULL, 0);
					WCHAR * pstr = new WCHAR[num + 1];
					MultiByteToWideChar(CP_ACP, NULL, buf, strlen(buf), pstr, num);
					pstr[num] = L'\0';
					int utf8Num = WideCharToMultiByte(CP_UTF8, NULL, pstr, wcslen(pstr), NULL, 0, NULL, NULL);
					char * str2 = new char[utf8Num + 1];
					WideCharToMultiByte(CP_UTF8, NULL, pstr, wcslen(pstr), str2, utf8Num, NULL, NULL);
					str2[utf8Num] = '\0';
					CEGUI::String string2((CEGUI::utf8*)str2);
					int weight = win->getFont()->getTextExtent(string2);
					win->setWidth(CEGUI::UDim(0.0f,weight));
					win->setText(string2.data());
				}
				break;
			}
		}
		break;*/
	case WM_IME_COMPOSITION:
		{
			if (gFullScreen)
			{
				//1.获得并设置输入框
				//先获取当前正在输入的窗口的输入法句柄
				HIMC hIMC = ImmGetContext(GetActiveWindow());

				// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.
				LONG dwSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
				WCHAR * lpstr = new WCHAR[dwSize + 1];

				// 再调用一次.ImmGetCompositionString获取字符串
				ImmGetCompositionStringW(hIMC, GCS_COMPSTR , lpstr, dwSize);
				lpstr[dwSize] = L'\0';

				/*int num = MultiByteToWideChar(CP_ACP, NULL, lpstr, dwSize, NULL, 0);
				WCHAR * pstr = new WCHAR[num + 1];
				MultiByteToWideChar(CP_ACP, NULL, lpstr, dwSize, pstr, num);
				pstr[num] = L'\0';*/

				int utf8Num = WideCharToMultiByte(CP_UTF8, NULL, lpstr, dwSize, NULL, 0, NULL, NULL);
				char * str = new char[utf8Num + 1];
				WideCharToMultiByte(CP_UTF8, NULL, lpstr, dwSize, str, utf8Num, NULL, NULL);
				str[utf8Num] = '\0';

				CEGUI::String string((CEGUI::utf8*)str);
				CEGUI::Editbox * eb = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("IME/Composition");
				float weight = eb->getFont()->getTextExtent(string,1.0f);//getTextExtent(string.data());
				eb->setWidth(CEGUI::UDim(0.0f,weight));
				eb->setText(string.data());
				LONG cursorIndex = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0);
				eb->setCaratIndex(cursorIndex);


				//2.获得并设置候选框
				CEGUI::Window * win = CEGUI::WindowManager::getSingleton().getWindow("IME/CandidateList");
				LPCANDIDATELIST pList;
				dwSize = ImmGetCandidateListA(hIMC,0,NULL,0);
				pList = (LPCANDIDATELIST)GlobalAlloc( GPTR, dwSize);
				ImmGetCandidateListA(hIMC,0,pList,dwSize);
				//dwSize /= 2;
				DWORD * dwOffset = &pList->dwOffset[0];
				dwOffset += pList->dwPageStart;
				char buf[355];
				DWORD j = 0;
				for (DWORD i=0;i<pList->dwCount;i++)
				{
					LPSTR s = (LPSTR)pList + *dwOffset++;			
					sprintf( buf + j,"%d.%s ",i+1,s);
					j += strlen(s) + 3;
				}
				buf[j + 1] = '\0';
				int num = MultiByteToWideChar(CP_ACP, NULL, buf, j + 1, NULL, 0);
				WCHAR * pstr = new WCHAR[num + 1];
				MultiByteToWideChar(CP_ACP, NULL, buf, strlen(buf), pstr, num);
				pstr[num] = L'\0';
				utf8Num = WideCharToMultiByte(CP_UTF8, NULL, pstr, wcslen(pstr), NULL, 0, NULL, NULL);
				char * str2 = new char[utf8Num + 1];
				WideCharToMultiByte(CP_UTF8, NULL, pstr, wcslen(pstr), str2, utf8Num, NULL, NULL);
				str2[utf8Num] = '\0';
				CEGUI::String string2((CEGUI::utf8*)str2);
				weight = win->getFont()->getTextExtent(string2);
				win->setWidth(CEGUI::UDim(0.0f,weight));
				win->setText(string2.data());
				
			}
		}
		break;
	}

	return CallNextHookEx(gHook,nCode,wParam,lParam);   
}

	/*case WM_KEYDOWN:
		{
			//CEGUI::utf32 word = (CEGUI::utf32)msg->wParam;

			//先获取当前正在输入的窗口的输入法句柄
			HIMC hIMC = ImmGetContext(msg->hwnd);

			// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.
			DWORD dwSize = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
			if (dwSize == 0)
			{
				if (gKeyDelay)
				{
					gKeyDelay = false;
				}
				else
					gEditKeysEnable = true;
			}
			else
			{
				gEditKeysEnable = false;
				gKeyDelay = true;
			}
			//CEGUI::utf32 code = (msg->lParam & 0x00FF0000 ) >> 16;
			
			//CEGUI::System::getSingleton().injectKeyDown(code);
			//CEGUI::System::getSingleton().injectChar(word);

		}
		break;
	case WM_KEYUP:
		{
			//先获取当前正在输入的窗口的输入法句柄
			HIMC hIMC = ImmGetContext(msg->hwnd);

			// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.
			DWORD dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
			if (dwSize == 0)
			{
				big = true;
			}
			else
				big = false;
			//CEGUI::utf32 word = (CEGUI::utf32)msg->wParam;
			//CEGUI::utf32 code = (msg->lParam & 0x00FF0000 ) >> 16;
			
			//CEGUI::System::getSingleton().injectKeyUp(code);

		}
		break;  */
	/*case WM_IME_COMPOSITION:
		{
			HIMC hIMC;
			HWND hWnd=msg->hwnd;
			DWORD dwSize;
			wchar_t lpstr[20];
			if(msg->lParam & GCS_RESULTSTR)
			{
				//先获取当前正在输入的窗口的输入法句柄
				hIMC = ImmGetContext(hWnd);

				// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.
				dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);

				// 缓冲区大小要加上字符串的NULL结束符大小,
				//   考虑到UNICODE
				dwSize += sizeof(WCHAR);

				memset(lpstr, 0, 20);
				// 再调用一次.ImmGetCompositionString获取字符串
				ImmGetCompositionString(hIMC, GCS_RESULTSTR, lpstr, dwSize);

				//现在lpstr里面即是输入的汉字了。你可以处理lpstr,当然也可以保存为文件...
				MessageBoxW(NULL, lpstr, lpstr, MB_OK);
				ImmReleaseContext(hWnd, hIMC);
			}
		}
		break;*/			/*if ( word >= 0 && word <= 32)
			{
				CEGUI::System::getSingleton().injectKeyDown(CEGUI::uint(word));
				//CEGUI::System::getSingleton().injectChar((CEGUI::utf8)msg->wParam);
			}
			CEGUI::utf8 code_point = (CEGUI::utf8)(msg->wParam);   
			CEGUI::System::getSingleton().injectChar(utf);   
			return true;   
			static char s_tempChar[3] = "";
			static wchar_t s_tempWchar[2] = L"";
			static bool s_flag = false;
			unsigned char uch = (unsigned char)code_point;
			if( uch >= 0xA1 )
			{
			if( !s_flag )
			{
			s_tempChar[0] = (char)uch; //第一个字节
			s_flag = true;
			return true;
			}
			else if( uch >= 0xA1 )
			{
			s_tempChar[1] = (char)uch; //第二个字节
			s_flag = false;
			MultiByteToWideChar( 0, 0, s_tempChar, 2, s_tempWchar, 1); //转成宽字节
			s_tempWchar[1] = L'\0';
			CEGUI::utf32 code = (CEGUI::utf32)s_tempWchar[0];
			return CEGUI::System::getSingleton().injectChar( code );
			}
			else
			{
			return CEGUI::System::getSingleton().injectChar(code_point);
			}
			}
			else
			{
			s_flag = false;
			return CEGUI::System::getSingleton().injectChar(code_point);
			}*/
/*LRESULT CALLBACK KeyJudge(int nCode,WPARAM wParam,LPARAM lParam)
{
	static bool gKeyDelay = false;
	if (nCode < 0)
	{
		return CallNextHookEx(gHook,nCode,wParam,lParam);
	}

	//先获取当前正在输入的窗口的输入法句柄
	HIMC hIMC = ImmGetContext(0);

	// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.
	DWORD dwSize = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
	if (dwSize == 0)
	{
		if (gKeyDelay)
		{
			gKeyDelay = false;
		}
		else
			gEditKeysEnable = true;
	}
	else
	{
		gEditKeysEnable = false;
		gKeyDelay = true;
	}

	return CallNextHookEx(gHook,nCode,wParam,lParam);   
}*/
 
bool CEGUIBaseListener::mouseMoved(const OIS::MouseEvent &arg)
{
	CEGUI::System& cegui = CEGUI::System::getSingleton();

	if (!gFullScreen)
	{
		if (arg.state.X.abs == 0 || arg.state.X.abs == mWindow->getWidth() || arg.state.Y.abs == 0 ||
			arg.state.Y.abs == mWindow->getHeight())
		{
			if (CEGUI::MouseCursor::getSingleton().isVisible())
			{
				CEGUI::MouseCursor::getSingleton().setVisible(false);
				ShowCursor(true);
				mMouseIn = false;
			}
			return true;
		}
		else
		{
			if (!CEGUI::MouseCursor::getSingleton().isVisible())
			{
				CEGUI::MouseCursor::getSingleton().setVisible(true);
				ShowCursor(false);
				mMouseIn = true;
			}

			cegui.injectMousePosition( arg.state.X.abs, arg.state.Y.abs );	
			cegui.injectMouseMove(arg.state.X.rel * mMouseSpeed, arg.state.Y.rel * mMouseSpeed);
			cegui.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
		}
	}
	else
	{
		cegui.injectMouseMove(arg.state.X.rel * mMouseSpeed, arg.state.Y.rel * mMouseSpeed);
		cegui.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	}	

	if(mRMouseDown)
	{
		mCameraMan->getCamera()->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCameraMan->getCamera()->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
	}
	return mApp->mouseMoved(arg);
}

bool CEGUIBaseListener::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if (!mMouseIn)
	{
		return true;
	}
	if(id == OIS::MB_Right)
	{
		mRMouseDown = true;
	}
	CEGUI::System::getSingleton().injectMouseButtonDown(convertOISButtonToCegui(id));
	mCameraMan->injectMouseDown(arg,id);
	return mApp->mousePressed(arg,id);
}

bool CEGUIBaseListener::mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if (!mMouseIn)
	{
		return true;
	}
	if(id == OIS::MB_Right)
	{
		mRMouseDown = false;
	}
	CEGUI::System::getSingleton().injectMouseButtonUp(convertOISButtonToCegui(id));	
	mCameraMan->injectMouseUp(arg,id);
	return mApp->mouseReleased(arg,id);
}

void CEGUIBaseListener::windowResized(Ogre::RenderWindow* rw)
{
	CEGUI::System* const sys = CEGUI::System::getSingletonPtr();
	if (sys)
		sys->notifyDisplaySizeChanged(
		CEGUI::Size(static_cast<float>(rw->getWidth()),
		static_cast<float>(rw->getHeight())));

	gFullScreen = mWindow->isFullScreen();
	const OIS::MouseState & mouseState = mMouse->getMouseState();
	mouseState.width = mWindow->getWidth();
	mouseState.height = mWindow->getHeight();
	mOgreRenderer->setDisplaySize(CEGUI::Size(mWindow->getWidth(),mWindow->getHeight()));
}

void CEGUIBaseListener::updateFps(const Ogre::FrameEvent &evt)
{
	mFps++;
	mFpsTime += evt.timeSinceLastFrame;
	if(mFpsTime >= 1.0f)
	{
		mFpsTime = 0.0f;
		mFPS = mFps;
		mFps = 0;
	}
}

void CEGUIBaseListener::updateSpeed(const Ogre::FrameEvent &evt)
{
	mMouseSpeed += mDeltaSpeedMouseChanged;	
}

void CEGUIBaseListener::updateArrow(const Ogre::FrameEvent &evt) //写在frameRenderingQueued()里
{
	if(mLeftArrow)
	{
		mArrowTimePassed += evt.timeSinceLastFrame;
		if(mArrowTimePassed > 0.6f)
		{
			if((int)(mArrowTimePassed * 1000) % 13 == 0)
				CEGUI::System::getSingleton().injectKeyDown(OIS::KC_LEFT);
		}
	}
	else if(mRightArrow)
	{
		mArrowTimePassed += evt.timeSinceLastFrame;
		if(mArrowTimePassed > 0.6f)
		{
			if((int)(mArrowTimePassed * 1000) % 13 == 0)
				CEGUI::System::getSingleton().injectKeyDown(OIS::KC_RIGHT);
		}
	}
}

void CEGUIBaseApp::createGUISystem()
{
	using namespace CEGUI;
	mOgreRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	mOgreRenderer->setFrameControlExecutionEnabled(false);
	initialiseGUI();

	mCEGUIRenderQueueListener = new CEGUIRQListener(mOgreRenderer,Ogre::RENDER_QUEUE_OVERLAY,false);
	mSceneMgr->addRenderQueueListener(mCEGUIRenderQueueListener);

	// clearing this queue actually makes sure it's created(!)
	mOgreRenderer->getDefaultRenderingRoot().clearGeometry(CEGUI::RQ_OVERLAY);
	// subscribe handler to render overlay items
	mOgreRenderer->getDefaultRenderingRoot().subscribeEvent(CEGUI::RenderingSurface::EventRenderQueueStarted,
		CEGUI::Event::Subscriber(&CEGUIBaseApp::updateGUI, this));

	CEGUI::WindowManager & winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::SchemeManager::getSingleton().create("WindowsLook.scheme");
	CEGUI::FontManager::getSingleton().create("chinese-26.font");
	CEGUI::FontManager::getSingleton().create("chinese-15.font");
	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook","MouseArrow");

	mRootWindow = (DefaultWindow *)(winMgr.createWindow("DefaultWindow","RootWindow"));
	System::getSingleton().setGUISheet(mRootWindow);
	CEGUI::ImagesetManager::getSingleton().create("DriveIcons.imageset");
	mRootWindow->addChildWindow(winMgr.loadWindowLayout("OgreSampleBase.layout"));
	mRootWindow->addChildWindow(winMgr.loadWindowLayout("IME.layout"));
	

	/*winMgr.getWindow("Root/Show/fps")->subscribeEvent(RenderingSurface::EventRenderQueueStarted,Event::Subscriber(&CEGUIBaseApp::updateGUI,this));
	winMgr.getWindow("Root/Show/speed")->subscribeEvent(RenderingSurface::EventRenderQueueStarted,Event::Subscriber(&CEGUIBaseApp::updateGUI,this));*/

	WindowManager::getSingleton().getWindow("Root/Show/speed")->setText("1.00");
	//setup the elements you want to implement
	createGUI();
}

void CEGUIBaseApp::initialiseGUI()
{
	//1.Initialise default resource groups
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	//2. setup default group for validation schemas
	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");      
}

void CEGUIBaseApp::createScene()
{}

void CEGUIBaseApp::createFrameListener()
{
	CEGUIBaseListener * listener = new CEGUIBaseListener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}

void CEGUIBaseApp::doSome()
{}

/*bool CEGUIBaseApp::updateGUI(const CEGUI::EventArgs& args)
{
using namespace CEGUI;

if (static_cast<const RenderQueueEventArgs&>(args).queueID != RQ_OVERLAY)
return false;

mListener->updateAll(args);

return true;
}*/

bool CEGUIBaseListener::updateAll(const CEGUI::EventArgs& args)
{
	/*using namespace CEGUI;

	if (static_cast<const RenderQueueEventArgs&>(args).queueID != RQ_OVERLAY)
		return false;

	CEGUI::Font* fnt = CEGUI::System::getSingleton().getDefaultFont();
	if (fnt)
	{
		//1. render FPS:
		mFpsGeometry->reset();
		fnt->drawText(*mFpsGeometry, mFpsString, CEGUI::Vector2(0.f, 0.f), 0,CEGUI::colour(0xFFFFFFFF));
		mFpsGeometry->draw();

		//2. render the mouse speed
		if(mSpeedShowTime > 0.0f)
		{
			mSpeedGeometry->reset();
			fnt->drawText(*mSpeedGeometry,mMouseTimeString,CEGUI::Vector2(610.0f,0.0f),0,colour(0xffff0000));
			mSpeedGeometry->draw();
		}
	}
	return true;*/
	return true;
}

void CEGUIBaseListener::createGUI()
{
	//setup the FPS
	/*const CEGUI::Rect scrn1(CEGUI::Vector2(0.0f, 0.0f),CEGUI::Size(160.0f,20.0f));
	mFpsGeometry = &mOgreRenderer->createGeometryBuffer();
	mFpsGeometry->setClippingRegion(scrn1);

	const CEGUI::Rect scrn2(CEGUI::Vector2(600.0f, 0.0f),CEGUI::Size(180.0f,20.0f));
	mSpeedGeometry = &mOgreRenderer->createGeometryBuffer();
	mSpeedGeometry->setClippingRegion(scrn2);*/
}

void CEGUIBaseApp::createGUI()
{
	/*	using namespace CEGUI;

	//init the mouse
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook","MouseArrow");
	//CEGUI::MouseCursor::getSingleton().setImage("TaharezLook","MouseArrow");
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::DefaultWindow* root = (CEGUI::DefaultWindow*)winMgr.createWindow("DefaultWindow","Root");
	CEGUI::System::getSingleton().setGUISheet(root);
	CEGUI::FrameWindow* wnd = (CEGUI::FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow","Demo Window");
	root->addChildWindow(wnd);
	wnd->setPosition(CEGUI::UVector2(cegui_reldim(0.25f),cegui_reldim(0.25f)));
	wnd->setSize(UVector2(cegui_reldim(0.5f),cegui_reldim(0.5f)));
	wnd->setMaxSize(UVector2(cegui_reldim(1.0f),cegui_reldim(1.0f)));
	wnd->setMinSize(UVector2(cegui_reldim(0.1f),cegui_reldim(0.1f)));
	wnd->setText("Hello,my love~!");*/

}

int CEGUIBaseListener::getFps() const 
{
	return mFPS;
}

float CEGUIBaseListener::getMouseSpeed() const
{
	return mMouseSpeed;
}

bool CEGUIBaseApp::updateGUI(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;
	if (static_cast<const RenderQueueEventArgs&>(args).queueID != RQ_OVERLAY)
		return false;

	static int fps = 0;
	if(fps != mListener->getFps())
	{
		fps = mListener->getFps();
		WindowManager::getSingleton().getWindow("Root/Show/fps")->setText(PropertyHelper::intToString(fps));
	}
	if(mListener->getMouseSpeedChanged())
	{
		float speed = mListener->getMouseSpeed();
		char a[5] = "\0";
		sprintf(a,"%.2f",speed);
		WindowManager::getSingleton().getWindow("Root/Show/speed")->setText(a);
	}
	return true;
}

void CEGUIBaseApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{}

void IMMFollow()
{
	using namespace CEGUI;
	//CEGUI::WindowManager::WindowIterator windowIt = CEGUI::WindowManager::getSingleton().getIterator();

	CEGUI::Vector2 point;
	if (gFullScreen)
	{
		Editbox * eb = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("IME/Composition");
		point.d_x = oldActiveWindow->getArea().d_min.d_x.d_scale;
		point.d_y = oldActiveWindow->getArea().d_max.d_y.d_scale;
		eb->setPosition(UVector2(UDim(point.d_x,20.0f),UDim(point.d_y,-20.0f)));
		Window * win = CEGUI::WindowManager::getSingleton().getWindow("IME/CandidateList");
		win->setPosition(UVector2(UDim(point.d_x,20.0f),UDim(point.d_y,20.0f)));
	}
	else
	{
		Window * window = System::getSingleton().getGUISheet()->getActiveChild();
		const CEGUI::String& type = window->getType();
		if (type == "TaharezLook/MultiLineEditbox" || type == "Vanilla/MultiLineEditbox" || type == "WindowsLook/MultiLineEditbox")
		{		
			CEGUI::MultiLineEditbox* mleb = static_cast<CEGUI::MultiLineEditbox*>(window);
			point.d_x = mleb->getXPosition().d_offset;
			point.d_y = mleb->getTextRenderArea().getHeight() + mleb->getYPosition().d_offset - 50.0f;

			point = CEGUI::CoordConverter::windowToScreen(*mleb,point);
		}
		else if(type == "TaharezLook/Editbox" || type == "Vanilla/Editbox" || type == "WindowsLook/Editbox")
		{
			//CEGUI::Font * curFont;
			CEGUI::Editbox * eb = static_cast<CEGUI::Editbox*>(window);
			/*size_t caraIndex = eb->getSelectionStartIndex();
			curFont = eb->getFont();
			point.d_y = curFont->getFontHeight();*/
			//point.d_y = caraIndex * curFont->getFontHeight();
			point.d_x = eb->getXPosition().asAbsolute(0.0f);
			point.d_y = eb->getYPosition().asAbsolute(0.0f) + eb->getHeight().asAbsolute(0.0f) -20.0f;
			CEGUI::Vector2 point = CEGUI::CoordConverter::windowToScreen(*eb, point);
		}

		HWND hWnd = GetActiveWindow();
		HIMC hImc = ImmGetContext(hWnd);
		COMPOSITIONFORM form;
		ImmGetCompositionWindow(hImc,&form);
		form.ptCurrentPos.x = point.d_x;
		form.ptCurrentPos.y = point.d_y;
		ImmSetCompositionWindow(hImc,&form);
	}
}

void CEGUIBaseListener::handleCopyPasteCut(CopyState copyState)
{
	using namespace CEGUI;
	CEGUI::Window * window = CEGUI::System::getSingleton().getGUISheet()->getActiveChild();
	const CEGUI::String& type = window->getType();
	CEGUI::MultiLineEditbox * mleb = 0;
	CEGUI::Editbox * eb = 0;
	if (type == "TaharezLook/MultiLineEditbox" || type == "Vanilla/MultiLineEditbox" || type == "WindowsLook/MultiLineEditbox")
	{
		mleb = static_cast<CEGUI::MultiLineEditbox*>(window);
	}
	else if(type == "TaharezLook/Editbox" || type == "Vanilla/Editbox" || type == "WindowsLook/Editbox")
	{
		eb = static_cast<CEGUI::Editbox*>(window);	
	}

	if(mleb)
	{
		if ((copyState == COPY || copyState == CUT) && mleb->getSelectionLength() > 0)
		{	
			HWND hwnd = GetActiveWindow();
			assert(OpenClipboard(hwnd));
			EmptyClipboard();
			CEGUI::String string(mleb->getText().data());
			string.erase(mleb->getSelectionEndIndex(),mleb->getText().size() - mleb->getSelectionEndIndex());
			string.erase(0,mleb->getSelectionStartIndex());

			HGLOBAL clipBuffer;
			WCHAR* buffer;
			const char* str = string.c_str();
			int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, str, strlen(str), NULL, 0);

			wchar_t* wszString = new wchar_t[wcsLen];
			//转换
			::MultiByteToWideChar(CP_UTF8, NULL, str, strlen(str), wszString, wcsLen);
			wszString[wcsLen] = '\0';
			clipBuffer = GlobalAlloc(GMEM_DDESHARE,wcsLen * 2 + 2);
			buffer = (WCHAR *)GlobalLock(clipBuffer);
	
			wcscpy(buffer,wszString);
			buffer[wcsLen] = L'\0';
			GlobalUnlock(clipBuffer); 
			SetClipboardData(CF_UNICODETEXT,clipBuffer);
			CloseClipboard();	
			if (copyState == CUT)
			{
				CEGUI::String s(mleb->getText().data());
				s.erase((CEGUI::String::size_type)(mleb->getSelectionStartIndex()),(CEGUI::String::size_type)mleb->getSelectionLength());
				mleb->setCaratIndex(mleb->getSelectionStartIndex());
				mleb->setText(s.data());
				mleb->ensureCaratIsVisible();
			}

			//mleb->getText().copy(ss,mleb->getSelectionLength(),mleb->getSelectionStartIndex());
	
			/*mBuff.clear();
			mBuff.append(mleb->getText().data());
			
			mBuff.erase(mleb->getSelectionEndIndex(),mleb->getText().size() - mleb->getSelectionEndIndex());
			mBuff.erase(0,mleb->getSelectionStartIndex());
			if (mCutState)
			{
				CEGUI::String s(mleb->getText().data());
				s.erase((CEGUI::String::size_type)(mleb->getSelectionStartIndex()),(CEGUI::String::size_type)mleb->getSelectionLength());
				mleb->setCaratIndex(mleb->getSelectionStartIndex());
				mleb->setText(s.data());
				mleb->ensureCaratIsVisible();
			}*/
			//buffer[mleb->getSelectionLength() * 2 + 1] = '\0';
			//MultiByteToWideChar(CP_UTF8,MB_PRECOMPOSED,buffer,-1,(TCHAR*)buffer,sizeof(TCHAR) * (mleb->getSelectionLength() + 1));
		}
		else if (copyState == PASTE)
		{
			HWND hwnd = GetActiveWindow();
			assert(OpenClipboard(hwnd));
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);
			WCHAR * buffer;
			buffer = (WCHAR*)GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();

			int utf8Num = WideCharToMultiByte(CP_UTF8, NULL, buffer, wcslen(buffer), NULL, 0, NULL, NULL);
			char * str = new char[utf8Num + 1];
			WideCharToMultiByte(CP_UTF8, NULL, buffer, wcslen(buffer), str, utf8Num, NULL, NULL);
			str[utf8Num] = '\0';
			CEGUI::String string(mleb->getText().data());
			string.erase(mleb->getSelectionStartIndex(),mleb->getSelectionLength());
			CEGUI::String strNum((CEGUI::utf8*)str);
			string.insert(mleb->getSelectionStartIndex(),strNum);
			mleb->setText(string.data());
			mleb->setCaratIndex(mleb->getSelectionStartIndex() + strNum.size());
			mleb->ensureCaratIsVisible();
			/*CEGUI::String string(mleb->getText().data());
			string.erase(mleb->getSelectionStartIndex(),mleb->getSelectionLength());
			string.insert(mleb->getSelectionStartIndex(),mBuff.data());
			mleb->setText(string.data());
			mleb->setCaratIndex(mleb->getSelectionEndIndex() + mBuff.size());
			mleb->ensureCaratIsVisible();
			mPastState = false;*/
		}
		else if (copyState == ALL)
		{
			mleb->setSelection(0,mleb->getText().size());
			mleb->setCaratIndex(mleb->getText().size());
		}
	}
	else if (eb)
	{
		if ((copyState == COPY || copyState == CUT) && mleb->getSelectionLength() > 0)
		{	
			mBuff.clear();
			mBuff.append(eb->getText().data());

			mBuff.erase(eb->getSelectionEndIndex(),eb->getText().size() - eb->getSelectionEndIndex());
			mBuff.erase(0,eb->getSelectionStartIndex());
			if (copyState == CUT)
			{
				CEGUI::String s(mleb->getText().data());
				s.erase((CEGUI::String::size_type)(eb->getSelectionStartIndex()),(CEGUI::String::size_type)eb->getSelectionLength());
				eb->setCaratIndex(mleb->getSelectionStartIndex());
				eb->setText(s.data());
			}
		}
		else if (copyState == PASTE)
		{
			CEGUI::String string(eb->getText().data());
			string.erase(eb->getSelectionStartIndex(),eb->getSelectionLength());
			string.insert(eb->getSelectionStartIndex(),mBuff.data());
			eb->setText(string.data());
			eb->setCaratIndex(eb->getSelectionEndIndex() + mBuff.size());
		}
		else if (copyState == ALL)
		{
			eb->setSelection(0,mleb->getText().size());
			eb->setCaratIndex(mleb->getText().size());
		}
	}
}

void CEGUIBaseApp::frameStarted(const Ogre::FrameEvent &evt)
{}
void CEGUIBaseApp::frameEnded(const Ogre::FrameEvent &evt)
{}