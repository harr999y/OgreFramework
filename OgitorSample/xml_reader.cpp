#include "assert.h"
#include <iostream>

#include "xml_reader.h"
#include "Ogre.h"

namespace Util
{
	//////////////////////////////////////////////////////////////////////////
	bool XmlReader::advanceFirstChildNode(const Ogre::String &nodeName)
	{
		mCurrentNode = mRootNode->first_node(nodeName.c_str());

		assert(mCurrentNode && "Dont have this node!");
		if (!mCurrentNode)
			return false;

		return true;
	}

	bool XmlReader::advanceNextSiblingNode(const Ogre::String &nodeName)
	{
		mCurrentNode = mCurrentNode->next_sibling(nodeName.c_str());

		if (!mCurrentNode)
			return false;

		return true;
	}

	Ogre::String XmlReader::getAttribute(const Ogre::String &attributeName)
	{
		rapidxml::xml_attribute<> * attribute = mCurrentNode->first_attribute(attributeName.c_str());
		assert(attribute && "Dont have this attribute.");

		return attribute->value();
	}

	void XmlReader::init(const Ogre::String &fileName)
	{
		/// 这是一种尝试。
/*
		typedef Ogre::String::value_type CharType;
		std::ifstream fileStream(fileName);
		fileStream.seekg(0, ios::end);
		std::vector<CharType> vec(
			std::istreambuf_iterator<CharType>(fileStream.rdbuf()),
			std::istreambuf_iterator<CharType>());
		mDoc.parse<0>(&vec.front());*/

		mFileStream = new file(fileName.c_str());
		 
		mDoc.parse<0>(mFileStream->data());

		mRootNode = mDoc.first_node("root");

		assert(mRootNode && "This xml dont has root node!");
	}

}
