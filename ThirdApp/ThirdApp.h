#ifndef _THIRD_
#define _THIRD_

#include "../CEGUIBase/CEGUIBaseApp.h"


using namespace Ogre;

class SelectionRectangle : public ManualObject
{
public:
	SelectionRectangle(const Ogre::String & name)
		: ManualObject(name)
	{
		setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
		setUseIdentityProjection(true);
		setUseIdentityView(true);
		setQueryFlags(0);
	}

	void setCorners(float left, float top,float right,float bottom)
	{
		left = left * 2 - 1;
		right = right * 2 - 1;
		top = 1 - top * 2;
		bottom = 1 - bottom * 2;
		clear();
		begin("",Ogre::RenderOperation::OT_LINE_STRIP);
		position(left, top, -1);
		position(right, top, -1);
		position(right, bottom, -1);
		position(left, bottom, -1);
		position(left, top, -1);
		end();

		Ogre::AxisAlignedBox box;
		box.setInfinite();
		setBoundingBox(box);
	}

	void setCorners(const Vector2 &topLeft, const Vector2 &bottomRight)
	{
		setCorners(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	}
};

class ThirdListener : public BaseListener
{
public:
	ThirdListener(Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw,OgreBites::SdkCameraMan* cm): 
	  BaseListener(root,sm,rw,cm),mSelecting(false)
	  {} 

	  ~ThirdListener()
	  {
		  mSceneMgr->destroyQuery(mVolQuery);
		  delete mRect;
	  }

	  /* MouseListener callbacks. */
	  bool mouseMoved(const OIS::MouseEvent &arg)
	  {
		  if(mSelecting)
		  {
			  CEGUI::MouseCursor *mouse = CEGUI::MouseCursor::getSingletonPtr();
			  mStop.x = mouse->getPosition().d_x / (float)arg.state.width;
			  mStop.y = mouse->getPosition().d_y / (float)arg.state.height;

			  mRect->setCorners(mStart,mStop);  //draw inside this function
		  }
		  CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
		  return true;
	  }

	  bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	  {
		  if(id == OIS::MB_Left)
		  {
			  CEGUI::MouseCursor *mouse = CEGUI::MouseCursor::getSingletonPtr();
			  mStart.x = mouse->getPosition().d_x/(float)arg.state.width;
			  mStart.y = mouse->getPosition().d_y/(float)arg.state.height;
			  mStop = mStart;

			  mSelecting = true;
			  mRect->clear();
			  mRect->setVisible(true);
		  }
		  return true;
	  }

	  bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	  {
		  if(id == OIS::MB_Left)
		  {
			  performSelection(mStart,mStop);
			  mRect->setVisible(false);
			  mSelecting = false;
		  }

		  return true;
	  }

	  void performSelection(const Ogre::Vector2 & first , const Ogre::Vector2 & second)
	  {
		  float left = first.x,right = second.x,top = first.y,bottom = second.y;
		  if(left > right)
			  swap(left,right);
		  if(top > bottom)
			  swap(top,bottom);

		  if ((right - left) * (bottom - top) < 0.00001)
			  return;

		  Ogre::Camera * cam = mCameraMan->getCamera();
		  Ogre::Ray topLeft = cam->getCameraToViewportRay(left,top);
		  Ogre::Ray topRight = cam->getCameraToViewportRay(right,top);
		  Ogre::Ray bottomLeft = cam->getCameraToViewportRay(left,bottom);
		  Ogre::Ray bottomRight = cam->getCameraToViewportRay(right,bottom);	

		  PlaneBoundedVolume vol;
		  vol.planes.push_back(Plane(topLeft.getPoint(5),topRight.getPoint(5),bottomLeft.getPoint(5)));  //The front plane
		  vol.planes.push_back(Plane(topLeft.getOrigin(),topLeft.getPoint(50),topRight.getPoint(50)));   //The up plane
		  vol.planes.push_back(Plane(topLeft.getOrigin(),bottomLeft.getOrigin(),bottomLeft.getPoint(50))); //The left plane
		  vol.planes.push_back(Plane(topRight.getOrigin(),topRight.getPoint(50),bottomRight.getPoint(50))); //The right plane
		  vol.planes.push_back(Plane(bottomLeft.getOrigin(),bottomRight.getOrigin(),bottomRight.getPoint(50))); //The down plane

		  PlaneBoundedVolumeList volList;
		  volList.push_back(vol);

		  mVolQuery->setVolumes(volList);
		  SceneQueryResult result = mVolQuery->execute();

		  Ogre::RenderQueue * queue = mSceneMgr->getRenderQueue();
		  deselectObject();
		  SceneQueryResultMovableList::iterator itr;
		  for(itr = result.movables.begin(); itr != result.movables.end(); itr++)
			  selectObject(*itr);
	  }

	  void deselectObject()
	  {
		  std::list<MovableObject*>::iterator itr;
		  for(itr = mSelected.begin();itr != mSelected.end();itr++)
			  (*itr)->getParentSceneNode()->showBoundingBox(false);
	  }

	  void selectObject(MovableObject *obj)
	  {
		  obj->getParentSceneNode()->showBoundingBox(true);
		  mSelected.push_back(obj);
	  }

	  void doSome();

private:
	Vector2 mStart,mStop;
	Ogre::PlaneBoundedVolumeListSceneQuery *mVolQuery;
	std::list<Ogre::MovableObject *> mSelected;
	SelectionRectangle *mRect;
	bool mSelecting;

	static void swap(float &x,float &y)
	{
		float temp = x;
		x = y;
		y = temp;
	}
};

class ThirdApp:public BaseApp
{
public:
	ThirdApp()
	{}

	~ThirdApp() 
	{}

	void createScene()
	{
		mCameraMan->getCamera()->setPosition(-60, 100, -60);
		mCameraMan->getCamera()->lookAt(60, 0, 60);

		mSceneMgr->setAmbientLight(ColourValue::White);
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				Entity *ent = mSceneMgr->createEntity("Robot" + StringConverter::toString(i + j * 10), "robot.mesh");
				SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(i * 15, 0, j * 15));
				node->attachObject(ent);
				node->setScale(0.1f, 0.1f, 0.1f);
			}
		}
	}

	void ThirdApp::createGUISystem();
	void ThirdApp::initialiseGUI();

	void createFrameListener()
	{
		ThirdListener * listener = new ThirdListener(mRoot, mSceneMgr,mWindow, mCameraMan);
		listener->initialise();
		mRoot->addFrameListener(listener);
	}

protected:
	CEGUI::OgreRenderer * mGUIRenderer;

};


#endif