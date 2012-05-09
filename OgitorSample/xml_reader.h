#ifndef _XML_READER_H_
#define _XML_READER_H_

#include "rapidxml.hpp"

#include "commonClasses-inl.h"
#include "OgreString.h"
#include "rapidxml_utils.hpp"

namespace Util
{
	/**
	Rapid_Xml wrapper.
	*/
	class XmlReader
	{
	public:
		XmlReader(const Ogre::String &fileName)
		  : mCurrentNode(NULL),
		    mRootNode(NULL),
			mFileStream(NULL)
		{
			init(fileName);
		}
		~XmlReader()
		{
			delete mFileStream;
			mFileStream = NULL;
		}

	public:
		bool advanceFirstChildNode(const Ogre::String &nodeName);
		bool advanceNextSiblingNode(const Ogre::String &nodeName);
		Ogre::String getAttribute(const Ogre::String &attributeName);

	private:
		DISALLOW_COPY_AND_ASSIGN(XmlReader);

		void init(const Ogre::String &fileName);

	private:
		rapidxml::xml_document<> mDoc;
		rapidxml::xml_node<> *mRootNode;
		rapidxml::xml_node<> *mCurrentNode;

		typedef Ogre::String::value_type CharType;
		typedef rapidxml::file<CharType> file;
		file *mFileStream;
	};
}

#endif
