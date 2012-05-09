#ifndef _OGRESAMPLE23_
#define _OGRESAMPLE23_

#define _OGRESAMPLE23_

#include "../CEGUIBase/CEGUIBaseApp.h"
#include <OgrePlatform.h>
#include <OgreBillboardParticleRenderer.h>

using namespace Ogre ;

class WaterMesh
{
private:
	MeshPtr mesh ;
	SubMesh *subMesh ; 
	float *vertexBuffers[3] ; // we need 3 vertex buffers
	int currentBuffNumber ;
	int complexity ;
	String meshName ;
	int numFaces ;
	int numVertices ;
	Vector3* vNormals ;

	HardwareVertexBufferSharedPtr posVertexBuffer ;
	HardwareVertexBufferSharedPtr normVertexBuffer ;
	HardwareVertexBufferSharedPtr texcoordsVertexBuffer ;
	HardwareIndexBufferSharedPtr indexBuffer ;

	Real lastTimeStamp ;
	Real lastAnimationTimeStamp;
	Real lastFrameTime ;
	
	void calculateFakeNormals();
	void calculateNormals();
public:
	WaterMesh(const String& meshName, Real planeSize, int complexity) ;
    
    virtual ~WaterMesh ();


	/** "pushes" a mesh at position [x,y]. Note, that x,y are float, hence 
	*	4 vertices are actually pushed
	*	@note 
	*		This should be replaced by push with 'radius' parameter to simulate
	*  		big objects falling into water
	*/
	void push(Real x, Real y, Real depth, bool absolute=false) ;

	/** gets height at given x and y, takes average value of the closes nodes */
	Real getHeight(Real x, Real y);

	/** updates mesh */
	void updateMesh(Real timeSinceLastFrame) ;
	
	Real PARAM_C ; // ripple speed 
	Real PARAM_D ; // distance
	Real PARAM_U ; // viscosity
	Real PARAM_T ; // time
	bool useFakeNormals ;
} ;

class WaterCircle;

class OgreSample23App : public CEGUIBaseApp
{
protected:
	WaterMesh * waterMesh;
	Entity * waterEntity;
	AnimationState * mAnimState;

	Real timeoutDelay;
	Real headDepth;
	typedef vector<WaterCircle *>::type WaterCircles;
	WaterCircles circles;

#define RAIN_HEIGHT_RANDOM 5
#define RAIN_HEIGHT_CONSTANT 5

public:
	OgreSample23App() : headDepth(1.8f)
	{}
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createScene();
	void animatedHead(Real timeSinceLastFrame);
	void processParticles();
	void processCircles(Real timeSinceLastFrame);
	void destroyScene();
};

#endif