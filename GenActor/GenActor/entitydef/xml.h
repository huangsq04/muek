/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef KBE_XMLP_H
#define KBE_XMLP_H

#include <string>
//#include "helper/debug_helper.h"
#include "common.h"
//#include "common/smartpointer.h"
#include "tinyxml.h"
#include <algorithm>
#include <functional>
#include <cctype>
#pragma warning (disable : 4706)
namespace KBEngine{
	inline std::string &kbe_ltrim(std::string &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	inline std::string &kbe_rtrim(std::string &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}


	inline std::string kbe_trim(std::string s)
	{
		return kbe_ltrim(kbe_rtrim(s));
	}
#define XML_FOR_BEGIN(node)																\
		do																				\
		{																				\
		if(node->Type() != TiXmlNode::TINYXML_ELEMENT)									\
				continue;																\
			
#define XML_FOR_END(node)																\
	}while((node = node->NextSibling()));												\
			
class  XML 
{
public:
	XML(void):
		txdoc_(NULL),
		rootElement_(NULL),
		isGood_(false)
	{
	}

	XML(const char* xmlFile):
		txdoc_(NULL),
		rootElement_(NULL),
		isGood_(false)
	{
		openSection(xmlFile);
	}
	
	~XML(void){
		if(txdoc_){
			txdoc_->Clear();
			delete txdoc_;
			txdoc_ = NULL;
			rootElement_ = NULL;
		}
	}

	bool isGood() const{ return isGood_; }

	bool openSection(const char* xmlFile)
	{
		char pathbuf[MAX_PATH];
		_snprintf_s(pathbuf, MAX_PATH, "%s", xmlFile);

		txdoc_ = new TiXmlDocument((char*)&pathbuf);

		if(!txdoc_->LoadFile())
		{
			isGood_ = false;
			return false;
		}

		rootElement_ = txdoc_->RootElement();
		isGood_ = true;
		return true;
	}

	/**获取根元素*/
	TiXmlElement* getRootElement(void){return rootElement_;}

	/**获取根节点， 带参数key为范围根节点下的某个子节点根*/
	TiXmlNode* getRootNode(const char* key = "")
	{
		if(rootElement_ == NULL)
			return rootElement_;

		if(strlen(key) > 0){
			TiXmlNode* node = rootElement_->FirstChild(key);
			if(node == NULL)
				return NULL;
			return node->FirstChild();
		}
		return rootElement_->FirstChild();
	}

	/**直接返回要进入的key节点指针*/
	TiXmlNode* enterNode(TiXmlNode* node, const char* key)
	{
		do
		{
			if(node->Type() != TiXmlNode::TINYXML_ELEMENT)
				continue;

			if (getKey(node) == key)
			{
				TiXmlNode* childNode = node->FirstChild();
				do
				{
					if (!childNode || childNode->Type() != TiXmlNode::TINYXML_COMMENT)
						break;
				}
				while ((childNode = childNode->NextSibling()));

				return childNode;
			}

		}
		while((node = node->NextSibling()));

		return NULL;
	}

	/**是否存在这样一个key*/
	bool hasNode(TiXmlNode* node, const char* key)
	{
		do{
			if(node->Type() != TiXmlNode::TINYXML_ELEMENT)
				continue;

			if(getKey(node) == key)
				return true;

		}while((node = node->NextSibling()));

		return false;	
	}
	
	TiXmlDocument* getTxdoc() const { return txdoc_; }

	std::string getKey(const TiXmlNode* node)
	{
		if(node == NULL)
			return "";
		return kbe_trim(node->Value());
	}

	std::string getValStr(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if(ptext == NULL)
			return "";
		return kbe_trim(node->Value());
	}

	std::string getVal(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if(ptext == NULL)
			return "";

		return ptext->Value();
	}

	int getValInt(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if(ptext == NULL)
			return 0;

		return atoi(kbe_trim(ptext->Value()).c_str());
	}

	double getValFloat(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if(ptext == NULL)
			return 0.f;

		return atof(kbe_trim(ptext->Value()).c_str());
	}

protected:
	TiXmlDocument* txdoc_;
	TiXmlElement* rootElement_;
	bool isGood_;
};

}
 
#endif // KBE_XMLP_H
