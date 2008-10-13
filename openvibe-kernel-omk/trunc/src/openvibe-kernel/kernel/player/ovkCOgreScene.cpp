#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "OgreRenderSystem.h"
#include "OgreStringConverter.h"

#include "ovkCOgreObject.h"
#include "ovkCOgreVisualisation.h"
#include "ovkCOgreScene.h"

#if defined( __WIN32__ ) || defined( _WIN32 )
#  include <gdk/gdkwin32.h>
#else
#  include <gdk/gdkx.h>
#endif

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

COgreScene::COgreScene(const OpenViBE::Kernel::IKernelContext& rKernelContext, const Ogre::String& rName, COgreVisualisation* pOgreVis) :
	m_rKernelContext(rKernelContext),
	m_sName(rName),
	m_pOgreVis(pOgreVis),
	m_pSceneManager(NULL)
{
}

COgreScene::~COgreScene()
{
	//destroy objects
	std::map<OpenViBE::CIdentifier, COgreObject*>::iterator it = m_mOgreObjectIds.begin();
	while(it != m_mOgreObjectIds.end())
	{
		delete it->second;
		m_mOgreObjectIds.erase(it);
		it = m_mOgreObjectIds.begin();
	}

	//destroy lights

	//destroy scene manager
	if(m_pSceneManager != NULL)
	{
		m_pOgreVis->getOgreRoot()->destroySceneManager(m_pSceneManager);
	}
}

boolean COgreScene::init()
{
	//initialize once only
	if(m_pSceneManager != NULL)
	{
		return true;
	}

	//retrieve Ogre root
	Ogre::Root* l_pRoot = m_pOgreVis->getOgreRoot();
	if(l_pRoot == NULL)
	{
		//TODO : display error msg
		return false;
	}

	//create scene manager
	try
	{
		m_pSceneManager = l_pRoot->createSceneManager("DefaultSceneManager", string(m_sName) + "_SceneManager");
	}
	catch ( Ogre::ItemIdentityException& e)
	{
		m_rKernelContext.getLogManager()
			<< LogLevel_Trace << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to create scene manager : " << e.what() << "\n";
		return false;
	}

	//create default lights
	m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1, 1));
	//m_pSceneManager->setAmbientLight(Ogre::ColourValue(0, 0, 0, 1));

	//front light
	Ogre::Light* l_pLight1 = m_pSceneManager->createLight(string(m_sName) + "_Light_0");
	l_pLight1->setType(Ogre::Light::LT_DIRECTIONAL);
	l_pLight1->setDirection(Ogre::Real(0), Ogre::Real(0), Ogre::Real(-1));	
	l_pLight1->setDiffuseColour(1, 1, 1);

	//back light
	Ogre::Light* l_pLight2 = m_pSceneManager->createLight(string(m_sName) + "_Light_1");
	l_pLight2->setType(Ogre::Light::LT_DIRECTIONAL);
	l_pLight2->setDirection(Ogre::Real(0), Ogre::Real(0), Ogre::Real(1));
	l_pLight2->setDiffuseColour(1, 1, 1);

	//top light
	Ogre::Light* l_pLight3 = m_pSceneManager->createLight(string(m_sName) + "_Light_2");
	l_pLight3->setType(Ogre::Light::LT_DIRECTIONAL);
	l_pLight3->setDirection(Ogre::Real(0), Ogre::Real(-1), Ogre::Real(0));
	l_pLight3->setDiffuseColour(1, 1, 1);

	return true;
}

const Ogre::String& COgreScene::getName()
{
	return m_sName;
}

Ogre::SceneManager* COgreScene::getSceneManager()
{
	return m_pSceneManager;
}

void COgreScene::getBoundingBox(float32& rMinX, float32& rMinY, float32& rMinZ, float32& rMaxX, float32& rMaxY, float32& rMaxZ)
{
	if(m_mOgreObjectIds.size() == 0)
	{
		rMinX = rMinY = rMinZ = rMaxX = rMaxY = rMaxZ = 0;
	}
	else
	{
		std::map<OpenViBE::CIdentifier, COgreObject*>::iterator it;

		rMinX = FLT_MAX;
		rMinY = FLT_MAX;
		rMinZ = FLT_MAX;
		rMaxX = -FLT_MAX;
		rMaxY = -FLT_MAX;
		rMaxZ = -FLT_MAX;

		//get each object's bounding box and update global bounding box
		for(it = m_mOgreObjectIds.begin(); it != m_mOgreObjectIds.end(); it++)
		{
			float32 l_f32ObjectMinX, l_f32ObjectMinY, l_f32ObjectMinZ;
			float32 l_f32ObjectMaxX, l_f32ObjectMaxY, l_f32ObjectMaxZ;

			if(it->second->getWorldBoundingBox(l_f32ObjectMinX, l_f32ObjectMinY, l_f32ObjectMinZ,
				l_f32ObjectMaxX, l_f32ObjectMaxY, l_f32ObjectMaxZ) == true)
			{
				if(l_f32ObjectMinX < rMinX)
					rMinX = l_f32ObjectMinX;
				if(l_f32ObjectMaxX > rMaxX)
					rMaxX = l_f32ObjectMaxX;

				if(l_f32ObjectMinY < rMinY)
					rMinY = l_f32ObjectMinY;
				if(l_f32ObjectMaxY > rMaxY)
					rMaxY = l_f32ObjectMaxY;

				if(l_f32ObjectMinZ < rMinZ)
					rMinZ = l_f32ObjectMinZ;
				if(l_f32ObjectMaxZ > rMaxZ)
					rMaxZ = l_f32ObjectMaxZ;
			}
		}
	}
}

COgreObject* COgreScene::createObject(CIdentifier rObjectIdentifier, const CString& rGeometryFileName, const CNameValuePairList& rNameValuePairList)
{
	if(!m_pSceneManager)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Could not create object " << rGeometryFileName << " because scene manager is not yet initialized\n";
		return NULL;
	}

	COgreObject* l_pObject = new COgreObject(m_rKernelContext, rObjectIdentifier, m_pOgreVis, m_pSceneManager, rGeometryFileName);

	if(l_pObject->loadGeometry() == false)
	{
		delete l_pObject;
		return NULL;
	}

	//should mesh be cloned?
	boolean l_bCloneMesh = false;
	rNameValuePairList.getValue("CloneMesh", l_bCloneMesh);
	if(l_bCloneMesh == true)
	{
		l_pObject->cloneMeshes();
	}
	
	//should materials be cloned?
	boolean l_bCloneMaterials = false;
	rNameValuePairList.getValue("CloneMaterials", l_bCloneMaterials);
	if(l_bCloneMaterials)
	{
		l_pObject->cloneMaterials();
	}

	l_pObject->createAnimators();

	//store object in Ids map
	m_mOgreObjectIds[rObjectIdentifier] = l_pObject;

	return l_pObject;
}

COgreObject* COgreScene::createPlane(CIdentifier rObjectIdentifier, const CNameValuePairList& rNameValuePairList)
{
	COgreObject* l_pObject = new COgreObject(m_rKernelContext, rObjectIdentifier, m_pOgreVis, m_pSceneManager);

	if(l_pObject->createPlane(rNameValuePairList) == false)
	{
		delete l_pObject;
		return NULL;
	}

	l_pObject->createAnimators();

	//store object in Ids map
	m_mOgreObjectIds[rObjectIdentifier] = l_pObject;

	return l_pObject;
}

boolean COgreScene::removeObject(CIdentifier rObjectIdentifier)
{
	//ensure object exists
	if(m_mOgreObjectIds.find(rObjectIdentifier) == m_mOgreObjectIds.end())
	{
		return false;
	}	
	COgreObject* l_pObject = m_mOgreObjectIds[rObjectIdentifier];

	//remove object from Ids map	
	m_mOgreObjectIds.erase(rObjectIdentifier);

	//delete object
	delete l_pObject;

	return true;
}

COgreObject* COgreScene::getOgreObject(CIdentifier rObjectIdentifier)
{
	if(m_mOgreObjectIds.find(rObjectIdentifier) == m_mOgreObjectIds.end() || m_mOgreObjectIds[rObjectIdentifier] == NULL)
	{
		return NULL;
	}
	return m_mOgreObjectIds[rObjectIdentifier];
}
