// An NVIDIA PhysX demo application, using debug visualization and joints
// by Simon Schirm
#ifndef _NXTRUCK_
#define _NXTRUCK_

#include <stdio.h>
#include "NxOgreSample_Terrain.h"

class Truck
{
public:
	Truck(Critter::RenderSystem * rs): force(0.0), steerAngle(0), mRenderSystem(rs) {}

	struct Wheel
	{
		void create(Critter::RenderSystem * rs,NxOgre::Scene & scene, const NxOgre::Vec3 & pos, float rad, NxActor * holder,NxOgre::MaterialIdentifier wheelMat)
		{
			mRenderSystem = rs;
			//NxActorDesc actorDesc;
			//NxBodyDesc bodyDesc;
			//NxSphereShapeDesc sphereDesc;
			Critter::BodyDescription bodyDesc;
			NxOgre::SphereDescription sphereDesc;
			
			bodyDesc.mSolverIterationCount = 20;
			// wheel
			sphereDesc.mRadius = rad;
			sphereDesc.mMaterial = wheelMat;
			//sphereDesc.materialIndex = wheelMaterial;
			bodyDesc.mMass = 400.0;
			//wheel = scene.createActor(actorDesc);
			wheel = mRenderSystem->createBody(sphereDesc,pos,"sphere.mesh",bodyDesc);
			wheel->getSceneNode()->setScale(.015f,.015f,.015f);
			//mRenderSystem->createSceneNodeEntityPair("sphere.mesh",NxOgre::Vec3(pos.x,pos.y,pos.z));

			// roll axis
			NxBodyDesc bd;
			bd.mass = 500.0;
			bd.massSpaceInertia = NxVec3(10,10,10);
			NxActorDesc ad;
			ad.body = &bd;
			ad.shapes.clear();
			ad.globalPose.t = NxVec3(pos.x,pos.y,pos.z);
			rollAxis = scene.getScene()->createActor(ad);
			//rollAxis = mRenderSystem->createBody(sphereDesc,pos,bodyDesc);
			// revolution joint connecting wheel with rollAxis
			NxRevoluteJointDesc revJointDesc;
			revJointDesc.projectionMode = NX_JPM_POINT_MINDIST;
			revJointDesc.actor[0] = wheel->getNxActor();
			revJointDesc.actor[1] = rollAxis;
			revJointDesc.setGlobalAnchor(NxVec3(pos.x,pos.y,pos.z));
			revJointDesc.setGlobalAxis(NxVec3(0,0,1));
			rollJoint = scene.getScene()->createJoint(revJointDesc);

			// prismatic joint connecting rollAxis with holder
			NxPrismaticJointDesc prisJointDesc;
			prisJointDesc.actor[0] = rollAxis;
			prisJointDesc.actor[1] = holder;
			prisJointDesc.setGlobalAnchor(NxVec3(pos.x,pos.y,pos.z));
			prisJointDesc.setGlobalAxis(NxVec3(0,1,0));
			scene.getScene()->createJoint(prisJointDesc);

			//add springs and dampers to the suspension (i.e. the related actors)
			float springLength = 0.2f;
			NxSpringAndDamperEffector * springNdamp = scene.getScene()->createSpringAndDamperEffector(NxSpringAndDamperEffectorDesc());

			springNdamp->setBodies(rollAxis, NxVec3(pos.x,pos.y,pos.z), holder, NxVec3(pos.x,pos.y + springLength,pos.z));
			springNdamp->setLinearSpring(0, springLength, 2*springLength, 100000, 100000);
			springNdamp->setLinearDamper(-1,1, 1e5, 1e5);

			//disable collision detection 
			if (holder->getShapes() != NULL)
			{
				scene.getScene()->setActorPairFlags(*(wheel->getNxActor()), *holder, NX_IGNORE_PAIR);
			}							
		}

		Critter::Body * wheel;
		NxActor * rollAxis;
		NxJoint * rollJoint;
		Critter::RenderSystem * mRenderSystem;
	};


	struct SteerWheel
	{
		void create(Critter::RenderSystem * rs,NxOgre::Scene & scene, const NxOgre::Vec3 & pos, float rad, NxActor * holder,	NxOgre::MaterialIdentifier wheelMat)
		{
			mRenderSystem = rs;
			NxActorDesc actorDesc;
			NxBodyDesc bodyDesc;
			bodyDesc.solverIterationCount = 20;

			// steer axis
			bodyDesc.mass = 500.0;
			bodyDesc.massSpaceInertia = NxVec3(10,10,10);
			//NxOgre::BoxDescription shape;
			//shape.mSize = NxOgre::Vec3(0.00001,0.00001,0.00001);
			//steerAxis = mRenderSystem->createBody(shape,pos,"sphere.mesh",bodyDesc);//scene.createActor(actorDesc);
			actorDesc.body = &bodyDesc;
			actorDesc.shapes.clear();
			actorDesc.globalPose.t = NxVec3(pos.x,pos.y,pos.z);
			steerAxis = scene.getScene()->createActor(actorDesc);
			wheel.create(mRenderSystem, scene, pos, rad, steerAxis,wheelMat); //
			//mRenderSystem->createSceneNodeEntityPair("sphere.mesh",NxOgre::Vec3(pos.x,pos.y,pos.z));

			// revolution joint connecting steerAxis with the holder
			NxRevoluteJointDesc revJointDesc;
			revJointDesc.projectionMode = NX_JPM_POINT_MINDIST;
			revJointDesc.actor[0] = steerAxis;
			revJointDesc.actor[1] = holder;
			revJointDesc.setGlobalAnchor(NxVec3(pos.x,pos.y,pos.z));
			revJointDesc.setGlobalAxis(NxVec3(0,1,0));
			NxJoint * joint = scene.getScene()->createJoint(revJointDesc);
			steerJoint = static_cast<NxRevoluteJoint*>(joint);

			// disable collision detection 
			scene.getScene()->setActorPairFlags(*(wheel.wheel->getNxActor()), *holder, NX_IGNORE_PAIR);
		}

		Wheel wheel;	
		NxActor * steerAxis;
		NxRevoluteJoint * steerJoint;
		Critter::RenderSystem * mRenderSystem;
	};

	void create(NxOgre::Scene & scene, NxActor& ground,NxOgre::MaterialIdentifier wheelMat)
	{

		//NxBodyDesc bodyDesc;	
		//NxBoxShapeDesc boxDesc0;
		//NxBoxShapeDesc boxDesc1;
		Critter::BodyDescription bodyDesc;
		NxOgre::BoxDescription boxDesc0;
		NxOgre::BoxDescription boxDesc1;

		bodyDesc.mSolverIterationCount = 20;

		// tractor actor (two shapes)
		//NxActorDesc tractoractorDesc;
		bodyDesc.mMass = 3000;
		//tractoractorDesc.body			= &bodyDesc;
		//tractoractorDesc.globalPose.t   = NxVec3(0,0,0);

		boxDesc0.mSize = NxOgre::Vec3(8.5,6,7.8);
		//boxDesc0.mLocalPose = NxOgre::Vec3(3.5,4,0);
		//tractoractorDesc.shapes.pushBack(&boxDesc0);

		NxOgre::ShapeDescriptions shapes;
		shapes.push_back(&boxDesc0);

		boxDesc1.mSize = NxOgre::Vec3(22,0.7,5.8);
		boxDesc1.mLocalPose = NxOgre::Vec3(6.2,-2.7,0);
		shapes.push_back(&boxDesc1);

		//tractor = scene.createActor(tractoractorDesc);

		tractor = mRenderSystem->createBody(shapes,NxOgre::Vec3(5.5,4,0),"cube.1m.mesh",bodyDesc);
		tractor->getSceneNode()->setScale(8.5,6,7.8);
		//tractor->getSceneNode()->setp
		//tractor->getSceneNode()->setPosition(1228,1228,1208);
		// trailer actor (also two shapes)
		//NxActorDesc traileractorDesc;
		bodyDesc.mMass = 6000;
		//traileractorDesc.body			= &bodyDesc;
		//traileractorDesc.globalPose.t   = NxVec3(0,0,0);

		boxDesc0.mSize = NxOgre::Vec3(26.5,4.8,7.8);
		//boxDesc0.mLocalPose = NxOgre::Vec3(24.0,4,0);

		boxDesc1.mSize = NxOgre::Vec3(15,0.7,5.8);
		boxDesc1.mLocalPose = NxOgre::Vec3(4,-2.0,0);

		//trailer = scene.createActor(traileractorDesc);

		trailer = mRenderSystem->createBody(shapes,NxOgre::Vec3(24.0,4,0),"cube.1m.mesh",bodyDesc);
		trailer->getSceneNode()->setScale(22.5,4.8,7.8);
		//trailer->getSceneNode()->setPosition(31,2.0,0);
		// remove collision between lower truck parts and ground
		//scene.getScene()->setShapePairFlags(*tractor->getNxActor()->getShapes()[0], *ground.getShapes()[0], NX_IGNORE_PAIR);
		//scene.getScene()->setShapePairFlags(*trailer->getNxActor()->getShapes()[0], *ground.getShapes()[0], NX_IGNORE_PAIR);

		// wheels
		float staticFriction  = 1.2f;
		float dynamicFriction = 1.0f;

		steerWheels[0].create(mRenderSystem, scene, NxVec3( 6,1.7, 4), 1.5f, tractor->getNxActor(),wheelMat);
		steerWheels[1].create(mRenderSystem, scene, NxVec3( 6,1.7,-4), 1.5f, tractor->getNxActor(),wheelMat);
		frontWheels[0].create(mRenderSystem, scene, NxVec3(17.5,1.7, 4), 1.5f, tractor->getNxActor(),wheelMat);
		frontWheels[1].create(mRenderSystem, scene, NxVec3(17.5,1.7,-4), 1.5f, tractor->getNxActor(),wheelMat);

		//scene.getScene()->setActorPairFlags(*steerWheels[0].wheel.wheel->getNxActor(),*tractor->getNxActor(), NX_IGNORE_PAIR);
		//scene.getScene()->setActorPairFlags(*steerWheels[1].wheel.wheel->getNxActor(),*tractor->getNxActor(), NX_IGNORE_PAIR);

		//additionally remove collision between front wheels and the trailer
		scene.getScene()->setActorPairFlags(*frontWheels[0].wheel->getNxActor(),*trailer->getNxActor(), NX_IGNORE_PAIR);
		scene.getScene()->setActorPairFlags(*frontWheels[1].wheel->getNxActor(),*trailer->getNxActor(), NX_IGNORE_PAIR);
	
		backWheels[0].create(mRenderSystem, scene, NxOgre::Vec3(26,1.7, 4), 1.5f, trailer->getNxActor(),wheelMat);	
		backWheels[1].create(mRenderSystem, scene, NxOgre::Vec3(33.6,1.7, 4), 1.5f, trailer->getNxActor(),wheelMat);
		backWheels[2].create(mRenderSystem, scene, NxOgre::Vec3(26,1.7,-4), 1.5f, trailer->getNxActor(),wheelMat);
		backWheels[3].create(mRenderSystem, scene, NxOgre::Vec3(33.6,1.7,-4), 1.5f, trailer->getNxActor(),wheelMat);

		// create rotation joint for the tractor trailer connection
		NxSphericalJointDesc sphJointDesc;
		sphJointDesc.actor[0] = tractor->getNxActor();
		sphJointDesc.actor[1] = trailer->getNxActor();
		sphJointDesc.setGlobalAnchor(NxVec3(14,0.8,0));
		//sphJointDesc.setGlobalAnchor(NxVec3(14,1.2,0));

		sphJointDesc.setGlobalAxis(NxVec3(0,1,0));
		sphJointDesc.swingAxis.set(0,0,1);

		sphJointDesc.twistLimit.low.value = NxMath::degToRad( -46.0f);
		sphJointDesc.twistLimit.high.value = NxMath::degToRad( 46.0f);
		sphJointDesc.swingLimit.value = NxMath::degToRad( 10.0f);

		sphJointDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
		sphJointDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;

		// NxJointLimitDesc swingLimit;
		mJoint = scene.getScene()->createJoint(sphJointDesc);
	}

	void control( bool keyLeft, bool keyRight, bool keyUp, bool keyDown, float dt)
	{
		const NxReal maxSteerAngle = 45.0f;
		NxReal sangle; 

		if (keyLeft ) steerAngle += 5;
		if (keyRight) steerAngle -= 5;
		if (!keyLeft && !keyRight) 
		{
			if (steerAngle < 0.0) 
			{
				steerAngle = NxMath::min(steerAngle + 1.0, 0.0);
			} 
			else if (steerAngle > 0.0) 
			{
				steerAngle = NxMath::max(steerAngle - 1.0, 0.0);
			}
		}

		if (steerAngle > 0.0) 
			sangle = NxMath::min(steerAngle,  maxSteerAngle);
		else				  
			sangle = NxMath::max(steerAngle, -maxSteerAngle);	
		steerAngle = sangle;

		steerWheels[0].steerJoint->setSpring(NxSpringDesc(5e4, 100, NxMath::degToRad(sangle)));
		steerWheels[1].steerJoint->setSpring(NxSpringDesc(5e4, 100, NxMath::degToRad(sangle)));

		if (dt == 0)
			return;

		float gdvAcc = dt * 30.0f;
		float gdvBrk = dt * 10.0f;

		if( keyUp && !keyDown)
			force = gdvAcc;
		else if(!keyUp &&  keyDown) 
			force = -gdvBrk;				
		else	
			force = 0.0f;

		unsigned i;
		float maxvel = NxMath::sign(force) * 1e5f;
		for (i=0; i<2; i++)
			static_cast<NxRevoluteJoint*>(frontWheels[i].rollJoint)->setMotor(NxMotorDesc(maxvel, 1.0f * NxMath::abs(force), false));
		for (i=0; i<2; i++)
			static_cast<NxRevoluteJoint*>(steerWheels[i].wheel.rollJoint)->setMotor(NxMotorDesc(maxvel, 1.0f * NxMath::abs(force), false));
	}

	Critter::Body *  tractor;
	Critter::Body *  trailer;
	SteerWheel steerWheels[2];
	Wheel      frontWheels[2];
	Wheel	   backWheels[4];
	NxOgre::MaterialIdentifier wheelMat;
	NxJoint * mJoint;

	float force;
	float steerAngle;

	Critter::RenderSystem * mRenderSystem;

};

#endif