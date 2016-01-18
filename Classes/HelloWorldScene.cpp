#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    _tileMap = TMXTiledMap::create("isometric.tmx");
    addChild(_tileMap);
    
    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    eventListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    return true;
}

Vec2 HelloWorld::tilePosFromLocation(cocos2d::Vec2 location)
{
    Vec2 pos = location - _tileMap->getPosition();
    
    float halfMapWidth = _tileMap->getMapSize().width * 0.5f;
    float mapHeight = _tileMap->getMapSize().height;
    float pointWidth = _tileMap->getTileSize().width / CC_CONTENT_SCALE_FACTOR();
    float pointHeight = _tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR();
    
    Vec2 tilePosDiv = Vec2(pos.x / pointWidth, pos.y / pointHeight);
    float inverseTileY = mapHeight - tilePosDiv.y;
    
    // Cast to int makes sure that result is in whole numbers, tile coordinates will be used as array indices
    float posX = (int)(inverseTileY + tilePosDiv.x - halfMapWidth);
    float posY = (int)(inverseTileY - tilePosDiv.x + halfMapWidth);
    
    // make sure coordinates are within isomap bounds
    posX = MAX(0, posX);
    posX = MIN(_tileMap->getMapSize().width - 1, posX);
    posY = MAX(0, posY);
    posY = MIN(_tileMap->getMapSize().height - 1, posY);
    
    pos = Vec2(posX, posY);
    
    CCLOG("touch at (%.0f, %.0f) is at tileCoord (%i, %i)", location.x, location.y, (int)pos.x, (int)pos.y);
    //CCLOG(@"\tinverseY: %.2f -- tilePosDiv: (%.2f, %.2f) -- halfMapWidth: %.0f\n", inverseTileY, tilePosDiv.x, tilePosDiv.y, halfMapWidth);
    
    return pos;
}

void HelloWorld::centerTileMapOnTileCoord(cocos2d::Vec2 tilePos)
{
    Size winSize = Director::getInstance()->getWinSize();
    Vec2 screenCenter = Vec2 { winSize.width * .5f, winSize.height * .5f};
    
    auto layer = _tileMap->getLayer("Ground");
    
    // internally tile Y coordinates are off by 1, this fixes the returned pixel coordinates
    tilePos.y -= 1;
    
    // get the pixel coordinates for a tile at these coordinates
    Vec2 scrollPosition = layer->getPositionAt(tilePos);
    // negate the position for scrolling
    scrollPosition *= -1;
    // add offset to screen center
    scrollPosition += screenCenter;
    
    CCLOG("tilePos: (%i, %i) moveTo: (%.0f, %.0f)", (int)tilePos.x, (int)tilePos.y, scrollPosition.x, scrollPosition.y);
    
    auto move = MoveTo::create(.2f, scrollPosition);
    _tileMap->stopAllActions();
    _tileMap->runAction(move);
}

bool HelloWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    Vec2 touchLocation = touch->getLocationInView();
    auto loc = Director::getInstance()->convertToGL(touchLocation);
    
    auto tilePos = tilePosFromLocation(loc);
    centerTileMapOnTileCoord(tilePos);
    
    return true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
}

void HelloWorld::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
}

void HelloWorld::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{

}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
