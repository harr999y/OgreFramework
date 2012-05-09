#include "AudioDemo.h"

void AudioDemo::createScene()
{
	Ogre::SceneNode * sn = mSceneMgr->getRootSceneNode()->createChildSceneNode("listener");
	SoundManager * soundMgr = new SoundManager();
	soundMgr->Initialize();
	int sound = 0;
	sound = soundMgr->CreateSound(Ogre::String("chuanqi.mp3"));
	int channel = 0;
	soundMgr->PlaySound(sound,sn,&channel);
	FMOD::Channel * cha = soundMgr->GetSoundChannel(channel);
	cha->setVolume(1.0f);
}

void AudioDemo::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	SoundManager::getSingleton().FrameStarted(mSceneMgr->getSceneNode("listener"),evt.timeSinceLastFrame);	
}