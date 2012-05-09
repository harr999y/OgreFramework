
#include "OgreSample23.h"

#define ANIMATIONS_PER_SECOND 100.0f

WaterMesh::WaterMesh(const String& meshName, Real planeSize, int complexity)
{
	int x,y,b; // I prefer to initialize for() variables inside it, but VC doesn't like it ;(

	this->meshName = meshName ;
	this->complexity =  complexity ;
	numFaces = 2 * complexity * complexity;
	numVertices = (complexity + 1) * (complexity + 1) ;
	lastTimeStamp = 0 ;
	lastAnimationTimeStamp = 0;
	lastFrameTime = 0 ;

	// initialize algorithm parameters
	PARAM_C = 0.3f ; // ripple speed
	PARAM_D = 0.4f ; // distance
	PARAM_U = 0.15f ; // viscosity
	PARAM_T = 0.13f ; // time
	useFakeNormals = true ;

	// allocate space for normal calculation
	vNormals = new Vector3[numVertices];

	// create mesh and submesh
	mesh = MeshManager::getSingleton().createManual(meshName,
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	subMesh = mesh->createSubMesh();
	subMesh->useSharedVertices=false;

	// Vertex buffers
	subMesh->vertexData = new VertexData();
	subMesh->vertexData->vertexStart = 0;
	subMesh->vertexData->vertexCount = numVertices;

	VertexDeclaration* vdecl = subMesh->vertexData->vertexDeclaration;
	VertexBufferBinding* vbind = subMesh->vertexData->vertexBufferBinding;


	vdecl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
	vdecl->addElement(1, 0, VET_FLOAT3, VES_NORMAL);
	vdecl->addElement(2, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES);

	// Prepare buffer for positions - todo: first attempt, slow
	posVertexBuffer =
         HardwareBufferManager::getSingleton().createVertexBuffer(
            3*sizeof(float),
			numVertices,
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	vbind->setBinding(0, posVertexBuffer);

	// Prepare buffer for normals - write only
	normVertexBuffer =
         HardwareBufferManager::getSingleton().createVertexBuffer(
            3*sizeof(float),
			numVertices,
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	vbind->setBinding(1, normVertexBuffer);

	// Prepare texture coords buffer - static one
	// todo: optimize to write directly into buffer
	float *texcoordsBufData = new float[numVertices*2];
	for(y=0;y<=complexity;y++) {
		for(x=0;x<=complexity;x++) {
			texcoordsBufData[2*(y*(complexity+1)+x)+0] = (float)x / complexity ;
			texcoordsBufData[2*(y*(complexity+1)+x)+1] = 1.0f - ((float)y / (complexity)) ;
		}
	}
	texcoordsVertexBuffer =
         HardwareBufferManager::getSingleton().createVertexBuffer(
            2*sizeof(float),
			numVertices,
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	texcoordsVertexBuffer->writeData(0,
		texcoordsVertexBuffer->getSizeInBytes(),
		texcoordsBufData,
		true); // true?
	delete [] texcoordsBufData;
    vbind->setBinding(2, texcoordsVertexBuffer);

	// Prepare buffer for indices
	indexBuffer =
		HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT,
			3*numFaces,
			HardwareBuffer::HBU_STATIC, true);
	unsigned short *faceVertexIndices = (unsigned short*)
		indexBuffer->lock(0, numFaces*3*2, HardwareBuffer::HBL_DISCARD);
	for(y=0 ; y<complexity ; y++) {
		for(int x=0 ; x<complexity ; x++) {
			unsigned short *twoface = faceVertexIndices + (y*complexity+x)*2*3;
			int p0 = y*(complexity+1) + x ;
			int p1 = y*(complexity+1) + x + 1 ;
			int p2 = (y+1)*(complexity+1) + x ;
			int p3 = (y+1)*(complexity+1) + x + 1 ;
			twoface[0]=p2; //first tri
			twoface[1]=p1;
			twoface[2]=p0;
			twoface[3]=p2; //second tri
			twoface[4]=p3;
			twoface[5]=p1;
		}
	}
	indexBuffer->unlock();
	// Set index buffer for this submesh
	subMesh->indexData->indexBuffer = indexBuffer;
	subMesh->indexData->indexStart = 0;
	subMesh->indexData->indexCount = 3*numFaces;

	/*	prepare vertex positions
	 *	note - we use 3 vertex buffers, since algorighm uses two last phases
	 *	to calculate the next one
	 */
	for(b=0;b<3;b++) {
		vertexBuffers[b] = new float[numVertices * 3] ;
		for(y=0;y<=complexity;y++) {
			for(x=0;x<=complexity;x++) {
				int numPoint = y*(complexity+1) + x ;
				float* vertex = vertexBuffers[b] + 3*numPoint ;
				vertex[0]=(float)(x) / (float)(complexity) * (float) planeSize ;
				vertex[1]= 0 ; // rand() % 30 ;
				vertex[2]=(float)(y) / (float)(complexity) * (float) planeSize ;
			}
		}
	}

	AxisAlignedBox meshBounds(0,0,0,
		planeSize,0, planeSize);
	mesh->_setBounds(meshBounds);

	currentBuffNumber = 0 ;
	posVertexBuffer->writeData(0,
		posVertexBuffer->getSizeInBytes(), // size
		vertexBuffers[currentBuffNumber], // source
		true); // discard?

    mesh->load();
    mesh->touch();
}
/* ========================================================================= */
WaterMesh::~WaterMesh ()
{
 	delete[] vertexBuffers[0];
 	delete[] vertexBuffers[1];
 	delete[] vertexBuffers[2];

	delete[] vNormals;
}
/* ========================================================================= */
void WaterMesh::push(Real x, Real y, Real depth, bool absolute)
{
	float *buf = vertexBuffers[currentBuffNumber]+1 ;
	// scale pressure according to time passed
	depth = depth * lastFrameTime * ANIMATIONS_PER_SECOND ;
#define _PREP(addx,addy) { \
	float *vertex=buf+3*((int)(y+addy)*(complexity+1)+(int)(x+addx)) ; \
	float diffy = y - floor(y+addy); \
	float diffx = x - floor(x+addx); \
	float dist=sqrt(diffy*diffy + diffx*diffx) ; \
	float power = 1 - dist ; \
	if (power<0)  \
		power = 0; \
	if (absolute) \
		*vertex = depth*power ;  \
	else \
		*vertex += depth*power ;  \
} /* #define */
	_PREP(0,0);
	_PREP(0,1);
	_PREP(1,0);
	_PREP(1,1);
#undef _PREP
}
/* ========================================================================= */
Real WaterMesh::getHeight(Real x, Real y)
{
#define hat(_x,_y) buf[3*((int)_y*(complexity+1)+(int)(_x))]
	float *buf = vertexBuffers[currentBuffNumber] ;
	Real xa = floor(x);
	Real xb = xa + 1 ;
	Real ya = floor(y);
	Real yb = ya + 1 ;
	Real yaxavg = hat(xa,ya) * (1.0f-fabs(xa-x)) + hat(xb,ya) * (1.0f-fabs(xb-x));
	Real ybxavg = hat(xa,yb) * (1.0f-fabs(xa-x)) + hat(xb,yb) * (1.0f-fabs(xb-x));
	Real yavg = yaxavg * (1.0f-fabs(ya-y)) + ybxavg * (1.0f-fabs(yb-y)) ;
	return yavg ;
}
/* ========================================================================= */
void WaterMesh::calculateFakeNormals()
{
	int x,y;
	float *buf = vertexBuffers[currentBuffNumber] + 1;
	float *pNormals = (float*) normVertexBuffer->lock(
		0,normVertexBuffer->getSizeInBytes(), HardwareBuffer::HBL_DISCARD);
	for(y=1;y<complexity;y++) {
		float *nrow = pNormals + 3*y*(complexity+1);
		float *row = buf + 3*y*(complexity+1) ;
		float *rowup = buf + 3*(y-1)*(complexity+1) ;
		float *rowdown = buf + 3*(y+1)*(complexity+1) ;
		for(x=1;x<complexity;x++) {
			Real xdiff = row[3*x+3] - row[3*x-3] ;
			Real ydiff = rowup[3*x] - rowdown[3*x-3] ;
			Vector3 norm(xdiff,30,ydiff);
			norm.normalise();
			nrow[3*x+0] = norm.x;
			nrow[3*x+1] = norm.y;
			nrow[3*x+2] = norm.z;
		}
	}
	normVertexBuffer->unlock();
}
/* ========================================================================= */
void WaterMesh::calculateNormals()
{
	int i,x,y;
	float *buf = vertexBuffers[currentBuffNumber] + 1;
	// zero normals
	for(i=0;i<numVertices;i++) {
		vNormals[i] = Vector3::ZERO;
	}
	// first, calculate normals for faces, add them to proper vertices
	buf = vertexBuffers[currentBuffNumber] ;
	unsigned short* vinds = (unsigned short*) indexBuffer->lock(
		0, indexBuffer->getSizeInBytes(), HardwareBuffer::HBL_READ_ONLY);
	float *pNormals = (float*) normVertexBuffer->lock(
		0, normVertexBuffer->getSizeInBytes(), HardwareBuffer::HBL_DISCARD);
	for(i=0;i<numFaces;i++) {
		int p0 = vinds[3*i] ;
		int p1 = vinds[3*i+1] ;
		int p2 = vinds[3*i+2] ;
		Vector3 v0(buf[3*p0], buf[3*p0+1], buf[3*p0+2]);
		Vector3 v1(buf[3*p1], buf[3*p1+1], buf[3*p1+2]);
		Vector3 v2(buf[3*p2], buf[3*p2+1], buf[3*p2+2]);
		Vector3 diff1 = v2 - v1 ;
		Vector3 diff2 = v0 - v1 ;
		Vector3 fn = diff1.crossProduct(diff2);
		vNormals[p0] += fn ;
		vNormals[p1] += fn ;
		vNormals[p2] += fn ;
	}
	// now normalize vertex normals
	for(y=0;y<=complexity;y++) {
		for(x=0;x<=complexity;x++) {
			int numPoint = y*(complexity+1) + x ;
			Vector3 n = vNormals[numPoint] ;
			n.normalise() ;
			float* normal = pNormals + 3*numPoint ;
			normal[0]=n.x;
			normal[1]=n.y;
			normal[2]=n.z;
		}
	}
	indexBuffer->unlock();
	normVertexBuffer->unlock();
}
/* ========================================================================= */
void WaterMesh::updateMesh(Real timeSinceLastFrame)
{
	int x, y ;

	lastFrameTime = timeSinceLastFrame ;
	lastTimeStamp += timeSinceLastFrame ;

	// do rendering to get ANIMATIONS_PER_SECOND
	while(lastAnimationTimeStamp <= lastTimeStamp) {

		// switch buffer numbers
		currentBuffNumber = (currentBuffNumber + 1) % 3 ;
		float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
		float *buf1 = vertexBuffers[(currentBuffNumber+2)%3] + 1 ;
		float *buf2 = vertexBuffers[(currentBuffNumber+1)%3] + 1;

		/* we use an algorithm from
		 * http://collective.valve-erc.com/index.php?go=water_simulation
		 * The params could be dynamically changed every frame ofcourse
		 */
		double C = PARAM_C; // ripple speed
		double D = PARAM_D; // distance
		double U = PARAM_U; // viscosity
		double T = PARAM_T; // time
		Real TERM1 = ( 4.0f - 8.0f*C*C*T*T/(D*D) ) / (U*T+2) ;
		Real TERM2 = ( U*T-2.0f ) / (U*T+2.0f) ;
		Real TERM3 = ( 2.0f * C*C*T*T/(D*D) ) / (U*T+2) ;
		for(y=1;y<complexity;y++) { // don't do anything with border values
			float *row = buf + 3*y*(complexity+1) ;
			float *row1 = buf1 + 3*y*(complexity+1) ;
			float *row1up = buf1 + 3*(y-1)*(complexity+1) ;
			float *row1down = buf1 + 3*(y+1)*(complexity+1) ;
			float *row2 = buf2 + 3*y*(complexity+1) ;
			for(x=1;x<complexity;x++) {
				row[3*x] = TERM1 * row1[3*x]
					+ TERM2 * row2[3*x]
					+ TERM3 * ( row1[3*x-3] + row1[3*x+3] + row1up[3*x]+row1down[3*x] ) ;
			}
		}

		lastAnimationTimeStamp += (1.0f / ANIMATIONS_PER_SECOND);
	}

	if (useFakeNormals) {
		calculateFakeNormals();
	} else {
		calculateNormals();
	}

	// set vertex buffer
	posVertexBuffer->writeData(0,
		posVertexBuffer->getSizeInBytes(), // size
		vertexBuffers[currentBuffNumber], // source
		true); // discard?
}


#include <iostream>
// Mesh stuff
#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_PREFIX "Examples/Water"
#define MATERIAL_NAME "Examples/Water8"
#define COMPLEXITY 64 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 3000.0f
#define CIRCLES_MATERIAL "Examples/Water/Circles"

/* Some global variables */
SceneNode *headNode ;
Overlay* waterOverlay ;
ParticleSystem *particleSystem ;
ParticleEmitter *particleEmitter ;
SceneManager *sceneMgr ;

void prepareCircleMaterial()
{
	char *bmap = new char[256 * 256 * 4] ;
	memset(bmap, 127, 256 * 256 * 4);
	for(int b=0;b<16;b++) {
		int x0 = b % 4 ;
		int y0 = b >> 2 ;
		Real radius = 4.0f + 1.4 * (float) b ;
		for(int x=0;x<64;x++) {
			for(int y=0;y<64;y++) {
				Real dist = Math::Sqrt((x-32)*(x-32)+(y-32)*(y-32)); // 0..ca.45
				dist = fabs(dist -radius -2) / 2.0f ;
				dist = dist * 255.0f;
				if (dist>255)
					dist=255 ;
				int colour = 255-(int)dist ;
				colour = (int)( ((Real)(15-b))/15.0f * (Real) colour );

				bmap[4*(256*(y+64*y0)+x+64*x0)+0]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+1]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+2]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+3]=colour ;
			}
		}
	}

	DataStreamPtr imgstream(new MemoryDataStream(bmap, 256 * 256 * 4));
	//~ Image img;
	//~ img.loadRawData( imgstream, 256, 256, PF_A8R8G8B8 );
	//~ TextureManager::getSingleton().loadImage( CIRCLES_MATERIAL , img );
	TextureManager::getSingleton().loadRawData(CIRCLES_MATERIAL,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		imgstream, 256, 256, PF_A8R8G8B8);
	MaterialPtr material =
		MaterialManager::getSingleton().create( CIRCLES_MATERIAL,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	TextureUnitState *texLayer = material->getTechnique(0)->getPass(0)->createTextureUnitState( CIRCLES_MATERIAL );
	texLayer->setTextureAddressingMode( TextureUnitState::TAM_CLAMP );
	material->setSceneBlending( SBT_ADD );
	material->setDepthWriteEnabled( false ) ;
	material->load();
	// finished with bmap so release the memory
	delete [] bmap;
}


/* =========================================================================*/
/*               WaterCircle class                                          */
/* =========================================================================*/
#define CIRCLE_SIZE 500.0
#define CIRCLE_TIME 0.5f
class WaterCircle
{
private:
	String name ;
	SceneNode *node ;
	MeshPtr mesh ;
	SubMesh *subMesh ;
	Entity *entity ;
	Real tm ;
	static bool first ;
	// some buffers shared by all circles
	static HardwareVertexBufferSharedPtr posnormVertexBuffer ;
	static HardwareIndexBufferSharedPtr indexBuffer ; // indices for 2 faces
	static HardwareVertexBufferSharedPtr *texcoordsVertexBuffers ;

	float *texBufData;
	void _prepareMesh()
	{
		int i,lvl ;

		mesh = MeshManager::getSingleton().createManual(name,
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME) ;
		subMesh = mesh->createSubMesh();
		subMesh->useSharedVertices=false;

		int numVertices = 4 ;

		if (first) { // first Circle, create some static common data
			first = false ;

			// static buffer for position and normals
			posnormVertexBuffer =
				HardwareBufferManager::getSingleton().createVertexBuffer(
				6*sizeof(float), // size of one vertex data
				4, // number of vertices
				HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage
				false); // no shadow buffer
			float *posnormBufData = (float*) posnormVertexBuffer->
				lock(HardwareBuffer::HBL_DISCARD);
			for(i=0;i<numVertices;i++) {
				posnormBufData[6*i+0]=((Real)(i%2)-0.5f)*CIRCLE_SIZE; // pos X
				posnormBufData[6*i+1]=0; // pos Y
				posnormBufData[6*i+2]=((Real)(i/2)-0.5f)*CIRCLE_SIZE; // pos Z
				posnormBufData[6*i+3]=0 ; // normal X
				posnormBufData[6*i+4]=1 ; // normal Y
				posnormBufData[6*i+5]=0 ; // normal Z
			}
			posnormVertexBuffer->unlock();

			// static buffers for 16 sets of texture coordinates
			texcoordsVertexBuffers = new HardwareVertexBufferSharedPtr[16];
			for(lvl=0;lvl<16;lvl++) {
				texcoordsVertexBuffers[lvl] =
					HardwareBufferManager::getSingleton().createVertexBuffer(
					2*sizeof(float), // size of one vertex data
					numVertices, // number of vertices
					HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage
					false); // no shadow buffer
				float *texcoordsBufData = (float*) texcoordsVertexBuffers[lvl]->
					lock(HardwareBuffer::HBL_DISCARD);
				float x0 = (Real)(lvl % 4) * 0.25 ;
				float y0 = (Real)(lvl / 4) * 0.25 ;
				y0 = 0.75-y0 ; // upside down
				for(i=0;i<4;i++) {
					texcoordsBufData[i*2 + 0]=
						x0 + 0.25 * (Real)(i%2) ;
					texcoordsBufData[i*2 + 1]=
						y0 + 0.25 * (Real)(i/2) ;
				}
				texcoordsVertexBuffers[lvl]->unlock();
			}

			// Index buffer for 2 faces
			unsigned short faces[6] = {2,1,0,  2,3,1};
			indexBuffer =
				HardwareBufferManager::getSingleton().createIndexBuffer(
				HardwareIndexBuffer::IT_16BIT,
				6,
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			indexBuffer->writeData(0,
				indexBuffer->getSizeInBytes(),
				faces,
				true); // true?
		}

		// Initialize vertex data
		subMesh->vertexData = new VertexData();
		subMesh->vertexData->vertexStart = 0;
		subMesh->vertexData->vertexCount = 4;
		// first, set vertex buffer bindings
		VertexBufferBinding *vbind = subMesh->vertexData->vertexBufferBinding ;
		vbind->setBinding(0, posnormVertexBuffer);
		vbind->setBinding(1, texcoordsVertexBuffers[0]);
		// now, set vertex buffer declaration
		VertexDeclaration *vdecl = subMesh->vertexData->vertexDeclaration ;
		vdecl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
		vdecl->addElement(0, 3*sizeof(float), VET_FLOAT3, VES_NORMAL);
		vdecl->addElement(1, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES);

		// Initialize index data
		subMesh->indexData->indexBuffer = indexBuffer;
		subMesh->indexData->indexStart = 0;
		subMesh->indexData->indexCount = 6;

		// set mesh bounds
		AxisAlignedBox circleBounds(-CIRCLE_SIZE/2.0f, 0, -CIRCLE_SIZE/2.0f,
			CIRCLE_SIZE/2.0f, 0, CIRCLE_SIZE/2.0f);
		mesh->_setBounds(circleBounds);
		mesh->load();
		mesh->touch();
	}
public:
	int lvl ;
	void setTextureLevel()
	{
		subMesh->vertexData->vertexBufferBinding->setBinding(1, texcoordsVertexBuffers[lvl]);
	}
	WaterCircle(const String& name, Real x, Real y)
	{
		this->name = name ;
		_prepareMesh();
		node = static_cast<SceneNode*> (sceneMgr->getRootSceneNode()->createChild(name));
		node->translate(x*(PLANE_SIZE/COMPLEXITY), 10, y*(PLANE_SIZE/COMPLEXITY));
		entity = sceneMgr->createEntity(name, name);
		entity->setMaterialName(CIRCLES_MATERIAL);
		node->attachObject(entity);
		tm = 0 ;
		lvl = 0 ;
		setTextureLevel();
	}
	~WaterCircle()
	{
		MeshManager::getSingleton().remove(mesh->getHandle());
		sceneMgr->destroyEntity(entity->getName());
		static_cast<SceneNode*> (sceneMgr->getRootSceneNode())->removeChild(node->getName());
	}
	void animate(Real timeSinceLastFrame)
	{
		int lastlvl = lvl ;
		tm += timeSinceLastFrame ;
		lvl = (int) ( (Real)(tm)/CIRCLE_TIME * 16 );
		if (lvl<16 && lvl!=lastlvl) {
			setTextureLevel();
		}
	}
	static void clearStaticBuffers()
	{
		posnormVertexBuffer = HardwareVertexBufferSharedPtr() ;
		indexBuffer = HardwareIndexBufferSharedPtr() ;
		for(int i=0;i<16;i++) {
			texcoordsVertexBuffers[i] = HardwareVertexBufferSharedPtr() ;
		}
		delete [] texcoordsVertexBuffers;
	}
} ;
bool WaterCircle::first = true ;
HardwareVertexBufferSharedPtr WaterCircle::posnormVertexBuffer =
HardwareVertexBufferSharedPtr() ;
HardwareIndexBufferSharedPtr WaterCircle::indexBuffer =
HardwareIndexBufferSharedPtr() ;
HardwareVertexBufferSharedPtr* WaterCircle::texcoordsVertexBuffers = 0 ;

void OgreSample23App::createScene()
{
	mSceneMgr->setAmbientLight(ColourValue(0.75, 0.75, 0.75));
	sceneMgr = mSceneMgr;
	// Create a light
	Light* l = mSceneMgr->createLight("MainLight");
	// Accept default settings: point light, white diffuse, just set position
	// NB I could attach the light to a SceneNode if I wanted it to move automatically with
	//  other objects, but I don't
	l->setPosition(300,300,100);

	waterMesh = new WaterMesh(MESH_NAME,PLANE_SIZE,COMPLEXITY);
	waterEntity = mSceneMgr->createEntity(ENTITY_NAME,MESH_NAME);
	waterEntity->setMaterialName(MATERIAL_NAME);
	SceneNode *waterNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	waterNode->attachObject(waterEntity);

	// Add a head, give it it's own node
	headNode = waterNode->createChildSceneNode();
	Entity *ent = mSceneMgr->createEntity("head", "ogrehead.mesh");
	headNode->attachObject(ent);

	// Create light node
	SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(l);

	// set up spline animation of light node
	Animation* anim = mSceneMgr->createAnimation("WaterLight", 20);
	NodeAnimationTrack *track ;
	TransformKeyFrame *key ;
	// create a random spline for light
	track = anim->createNodeTrack(0, lightNode);
	key = track->createNodeKeyFrame(0);
	for(int ff=1;ff<=19;ff++) {
		key = track->createNodeKeyFrame(ff);
		Vector3 lpos (
			rand()%(int)PLANE_SIZE , //- PLANE_SIZE/2,
			rand()%300+100,
			rand()%(int)PLANE_SIZE //- PLANE_SIZE/2
			);
		key->setTranslate(lpos);
	}
	key = track->createNodeKeyFrame(20);

	// Create a new animation state to track this
	mAnimState = mSceneMgr->createAnimationState("WaterLight");
	mAnimState->setEnabled(true);

	// Put in a bit of fog for the hell of it
	//mSceneMgr->setFog(FOG_EXP, ColourValue::White, 0.0002);

	// Let there be rain
	particleSystem = mSceneMgr->createParticleSystem("rain",
		"Examples/Water/Rain");
	particleEmitter = particleSystem->getEmitter(0);
	SceneNode* rNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	rNode->translate(PLANE_SIZE/2.0f, 3000, PLANE_SIZE/2.0f);
	rNode->attachObject(particleSystem);
	// Fast-forward the rain so it looks more natural
	particleSystem->fastForward(20);
	static_cast<BillboardParticleRenderer*>(particleSystem->getRenderer())->setBillboardOrigin(BBO_BOTTOM_CENTER);

	prepareCircleMaterial();

	timeoutDelay = 0.0f;

	particleEmitter->setEmissionRate(20.0f);

	mCameraMan->getCamera()->setPosition(400, 235, 15);
	mCameraMan->setTopSpeed(1400);
	//mCameraMan->getCamera()->lookAt(200,0,0);
	//mCameraMan->getCamera()->yaw(Degree(-45));
}

void OgreSample23App::destroyScene()
{
	unsigned int activeCircles = this->circles.size();

	for (unsigned int i=0;i<activeCircles;i++)
	{
		delete (this->circles[i]);
	}

	delete waterMesh;
	waterMesh = 0;
}

void OgreSample23App::processCircles(Ogre::Real timeSinceLastFrame)
{
	for (unsigned int i=0;i<circles.size();i++)
	{
		circles[i]->animate(timeSinceLastFrame);
	}
	bool found;
	do 
	{
		found = false;
		for (WaterCircles::iterator it = circles.begin();it != circles.end();it++)
		{
			if ((*it)->lvl >= 16)
			{
				delete (*it);
				circles.erase(it);
				found = true;
				break;
			}
		}
	} while (found);
}

void OgreSample23App::processParticles()
{
	static int pindex = 0 ;
	ParticleIterator pit = particleSystem->_getIterator() ;
	while(!pit.end()) 
	{
		Particle *particle = pit.getNext();
		Vector3 ppos = particle->position;
		if (ppos.y<=0 && particle->timeToLive>0) 
		{ // hits the water!
			// delete particle
			particle->timeToLive = 0.0f;
			// push the water
			float x = ppos.x / PLANE_SIZE * COMPLEXITY ;
			float y = ppos.z / PLANE_SIZE * COMPLEXITY ;
			float h = rand() % RAIN_HEIGHT_RANDOM + RAIN_HEIGHT_CONSTANT ;
			if (x<1) x=1 ;
			if (x>COMPLEXITY-1) x=COMPLEXITY-1;
			if (y<1) y=1 ;
			if (y>COMPLEXITY-1) y=COMPLEXITY-1;
			waterMesh->push(x,y,-h) ;
			/*WaterCircle *circle = new WaterCircle(
				"Circle#"+StringConverter::toString(pindex++),
				x, y);
			circles.push_back(circle);*/
		}
	}
}

void OgreSample23App::animatedHead(Ogre::Real timeSinceLastFrame)
{
	static double sines[4] = {0,100,200,300};
	static const double adds[4] = {0.3f,-1.6f,1.1f,0.5f};
	static Vector3 oldPos = Vector3::UNIT_Z;
	for (int i=0;i<4;i++)
	{
		sines[i] += adds[i] * timeSinceLastFrame;
	}

	Real tx = ((sin(sines[0]) + sin(sines[1])) / 4 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	Real ty = ((sin(sines[2]) + sin(sines[3])) / 4 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
	waterMesh->push(tx,ty, -headDepth);
	Real step = PLANE_SIZE / COMPLEXITY ;

	headNode->resetToInitialState();
	headNode->scale(3,3,3);
	Vector3 newPos = Vector3(step*tx, headDepth, step*ty);
	Vector3 diffPos = newPos - oldPos ;
	Quaternion headRotation = Vector3::UNIT_Z.getRotationTo(diffPos);
	oldPos = newPos ;
	headNode->translate(newPos);
	headNode->rotate(headRotation);
}

void OgreSample23App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mAnimState->addTime(evt.timeSinceLastFrame);
	//processCircles(evt.timeSinceLastFrame);

	processParticles();

	timeoutDelay -= evt.timeSinceLastFrame;
	if (timeoutDelay <= 0)
	{
		timeoutDelay = 0;
	}
	animatedHead(evt.timeSinceLastFrame);
	waterMesh->updateMesh(evt.timeSinceLastFrame);
}