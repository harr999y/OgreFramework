#ifndef _OGRESAMPLE13_
#define _OGRESAMPLE13_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

#define maxObjectsPerBatch 80
#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+28F
#endif

const size_t numTypeMeshes = 4;

enum CurrentGeomOpt
{
	INSTANCE_OPT,
	STATIC_OPT,
	ENTITY_OPT
};

class OgreSample13App : public CEGUIBaseApp
{
public:
	void createScene();
	bool handleSliders(const CEGUI::EventArgs & args);
	bool handleSelected(const CEGUI::EventArgs & args);
	bool handleCheckboxs(const CEGUI::EventArgs & args);
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void burnCPU();
	void destroyCurrentGeomOpt();
	void createCurrentGeomOpt();
	void createInstanceGeom();
	void destroyInstanceGeom();
	void createStaticGeom();
	void destroyStaticGeom();
	void createEntityGeom();
	void destroyEntityGeom();
	void setCurrentGeometryOpt(CurrentGeomOpt opt);
	void createGUI();
	void setupInstancedMaterialToEntity(Entity*ent);
	String buildInstancedMaterial(const String &originalMaterialName);
	
protected:

	double mAvgFrameTime;
	size_t mSelectedMesh;
	size_t mNumMeshes;
	size_t objectCount;
	String mDebugText;
	CurrentGeomOpt mCurrentGeomOpt;
	size_t mNumRendered;
	Ogre::Timer * mTimer;
	double mLastTime,mBurnAmount;
	vector <InstancedGeometry *>::type renderInstance;
	vector <StaticGeometry *>::type renderStatic;
	vector <Entity *>::type renderEntity;
	vector <SceneNode *>::type nodes;
	vector <Vector3 *>::type posMatrices;
};
#endif