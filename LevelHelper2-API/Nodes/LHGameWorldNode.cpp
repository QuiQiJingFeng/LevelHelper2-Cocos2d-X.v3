//
//  LHGameWorldNode.cpp
//  LevelHelper2-Cocos2d-X-v3
//
//  Created by Bogdan Vladu on 31/03/14.
//  Copyright (c) 2014 GameDevHelper.com. All rights reserved.
//

#include "LHGameWorldNode.h"
#include "LHDictionary.h"
#include "LHScene.h"
#include "LHDevice.h"
#include "LHConfig.h"

#if LH_USE_BOX2D
#include "LHBox2dDebugDrawNode.h"
#include "Box2D/Box2D.h"
#endif

LHGameWorldNode::LHGameWorldNode()
{
#if LH_USE_BOX2D
    _box2dWorld = nullptr;
    _debugNode = nullptr;
#endif
}

LHGameWorldNode::~LHGameWorldNode()
{
#if LH_USE_BOX2D
    //we need to first destroy all children and then destroy box2d world
    this->removeAllChildren();
    delete _box2dWorld;
    _box2dWorld = nullptr;
#endif

}

LHGameWorldNode* LHGameWorldNode::nodeWithDictionary(LHDictionary* dict, Node* prnt)
{
    LHGameWorldNode *ret = new LHGameWorldNode();
    if (ret && ret->initWithDictionary(dict, prnt))
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

bool LHGameWorldNode::initWithDictionary(LHDictionary* dict, Node* prnt)
{
    if(Node::init())
    {
        _physicsBody = NULL;
        prnt->addChild(this);
        
        this->loadGenericInfoFromDictionary(dict);
        
        this->setPosition(Point(0,0));
        this->setContentSize(prnt->getScene()->getContentSize());
        
        this->loadChildrenFromDictionary(dict);
        
        this->scheduleUpdate();
        
        return true;
    }
    return false;
}

void LHGameWorldNode::update(float delta)
{
#if LH_USE_BOX2D
    this->step(delta);
#endif
}

#if COCOS2D_VERSION >= 0x00030200
void LHGameWorldNode::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
#else
void LHGameWorldNode::visit(Renderer *renderer, const Mat4& parentTransform, bool parentTransformUpdated)
#endif
{
#if LH_USE_BOX2D
    #if LH_DEBUG
        #if COCOS2D_VERSION >= 0x00030200
            _debugNode->onDraw(parentTransform, parentFlags);
        #else
            _debugNode->onDraw(parentTransform, parentTransformUpdated);
        #endif
    #endif
#endif
    
    if(renderer)
    {
#if COCOS2D_VERSION >= 0x00030200
        Node::visit(renderer, parentTransform, parentFlags);
#else
        Node::visit(renderer, parentTransform, parentTransformUpdated);
#endif
    }
}

#pragma mark - BOX2D SUPPORT


#if LH_USE_BOX2D

b2World* LHGameWorldNode::getBox2dWorld()
{
    if(!_box2dWorld){
        
        b2Vec2 gravity;
        gravity.Set(0.f, 0.0f);
        _box2dWorld = new b2World(gravity);
        _box2dWorld->SetAllowSleeping(true);
        _box2dWorld->SetContinuousPhysics(true);

#if LH_DEBUG
        LHBox2dDebugDrawNode* drawNode = LHBox2dDebugDrawNode::create();
        drawNode->setLocalZOrder(99999);
        _box2dWorld->SetDebugDraw(drawNode->getDebug());
        drawNode->getDebug()->setRatio(((LHScene*)this->getScene())->getPtm());
        
        uint32 flags = 0;
        flags += b2Draw::e_shapeBit;
        flags += b2Draw::e_jointBit;
        //        //        flags += b2Draw::e_aabbBit;
        //        //        flags += b2Draw::e_pairBit;
        //        //        flags += b2Draw::e_centerOfMassBit;
        drawNode->getDebug()->SetFlags(flags);
        
        this->addChild(drawNode);
        _debugNode = drawNode;
#endif
        
    }
    return _box2dWorld;
}

const float32 FIXED_TIMESTEP = 1.0f / 24.0f;
const float32 MINIMUM_TIMESTEP = 1.0f / 600.0f;
const int32 VELOCITY_ITERATIONS = 8;
const int32 POSITION_ITERATIONS = 8;
const int32 MAXIMUM_NUMBER_OF_STEPS = 24;

void LHGameWorldNode::step(float dt)
{
    if(!this->getBox2dWorld())return;
        
	float32 frameTime = dt;
	int stepsPerformed = 0;
	while ( (frameTime > 0.0) && (stepsPerformed < MAXIMUM_NUMBER_OF_STEPS) ){
		float32 deltaTime = std::min( frameTime, FIXED_TIMESTEP );
		frameTime -= deltaTime;
		if (frameTime < MINIMUM_TIMESTEP) {
			deltaTime += frameTime;
			frameTime = 0.0f;
		}
		this->getBox2dWorld()->Step(deltaTime,VELOCITY_ITERATIONS,POSITION_ITERATIONS);
		stepsPerformed++;
	}
	this->getBox2dWorld()->ClearForces ();
}

Point LHGameWorldNode::getGravity(){
    b2Vec2 grv = this->getBox2dWorld()->GetGravity();
    return Point(grv.x, grv.y);
}
void LHGameWorldNode::setGravity(Point gravity){
    b2Vec2 grv;
    grv.Set(gravity.x, gravity.y);
    this->getBox2dWorld()->SetGravity(grv);
}

#endif
