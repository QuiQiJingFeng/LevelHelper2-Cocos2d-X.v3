
#include "LHSceneSubclass.h"

USING_NS_CC;

LHSceneSubclass* LHSceneSubclass::createWithContentOfFile(const std::string& plistLevelFile)
{
    LHSceneSubclass *ret = new LHSceneSubclass();
    if (ret && ret->initWithContentOfFile(plistLevelFile))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}
LHSceneSubclass::LHSceneSubclass()
{
    /*INITIALIZE YOUR CONTENT HERE*/
    /*AT THIS POINT NOTHING IS LOADED*/
}

LHSceneSubclass::~LHSceneSubclass()
{
    //nothing to release
}

bool LHSceneSubclass::initWithContentOfFile(const std::string& plistLevelFile)
{
    bool retValue = LHScene::initWithContentOfFile(plistLevelFile);
    
    /*INITIALIZE YOUR CONTENT HERE*/
    /*AT THIS POINT EVERYTHING IS LOADED AND YOU CAN ACCESS YOUR OBJECTS*/
    
    LHAsset* assetInLevel = (LHAsset*)this->getChildNodeWithName("OfficerAsset");
    
    printf("Found the following asset in the loaded level %p name %s\n", assetInLevel, assetInLevel->getName().c_str());
    
    
    
    LHAsset* asset = LHAsset::createWithName("myNewAsset", "OfficerAsset.lhasset", this->getGameWorld());
    asset->setPosition(Point(110,180));

    return retValue;
}