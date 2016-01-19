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
    
    _tileMap = TMXTiledMap::create("isometric-with-border.tmx");
    
    auto collisionsLayer = _tileMap->getLayer("Collisions");
    collisionsLayer->setVisible(false);
    
    addChild(_tileMap);
    
//    _tileMap->setPosition(Vec2 {-500, -500});
    
    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    eventListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    
    const int borderSize = 10;
    _playableAreaMin = Vec2(borderSize, borderSize);
    _playableAreaMax = Vec2(_tileMap->getMapSize().width - 1 - borderSize,
                            _tileMap->getMapSize().height - 1 - borderSize);
    
    
    auto screenSize = Director::getInstance()->getWinSize();
    
    // Create the player and add it
    _player = Player::createWithTileMap(_tileMap);
    _player->setPosition(screenSize.width / 2, screenSize.height / 2);
    // offset player's texture to best match the tile center position
    _player->setAnchorPoint(Vec2{0.3f, 0.1f});
    addChild(_player);
    
    // divide the screen into 4 areas
    _screenCenter = Vec2(screenSize.width / 2, screenSize.height / 2);
    _upperLeft = Rect(0, _screenCenter.y, _screenCenter.x, _screenCenter.y);
    _lowerLeft = Rect(0, 0, _screenCenter.x, _screenCenter.y);
    _upperRight = Rect(_screenCenter.x, _screenCenter.y, _screenCenter.x, _screenCenter.y);
    _lowerRight = Rect(_screenCenter.x, 0, _screenCenter.x, _screenCenter.y);

    // to move in any of these directions means to add/subtract 1 to/from the current tile coordinate
    _moveOffsets[MoveDirectionNone] = Vec2::ZERO;
    _moveOffsets[MoveDirectionUpperLeft] = Vec2(-1, 0);
    _moveOffsets[MoveDirectionLowerLeft] = Vec2(0, 1);
    _moveOffsets[MoveDirectionUpperRight] = Vec2(0, -1);
    _moveOffsets[MoveDirectionLowerRight] = Vec2(1, 0);
    
    _currentMoveDirection = MoveDirectionNone;
    
    scheduleUpdate();

    return true;
}

bool HelloWorld::isTilePosBlocked(Vec2 tilePos)
{
    auto layer = _tileMap->getLayer("Collisions");
    CC_ASSERT(layer != nullptr);
    
    bool isBlocked = false;
    auto tileGID = layer->getTileGIDAt(tilePos);
    if (tileGID > 0) {
        auto tileProperties = _tileMap->getPropertiesForGID(tileGID);
        CC_ASSERT(tileProperties.getType() != Value::Type::NONE);
        
        auto valueMap = tileProperties.asValueMap();
        auto v = valueMap.find("blocks_movement");
        isBlocked = v != valueMap.end();
    }
    return isBlocked;
}

Vec2 HelloWorld::ensureTilePosIsWithinBounds(Vec2 tilePos)
{
    tilePos.x = MAX(_playableAreaMin.x, tilePos.x);
    tilePos.x = MIN(_playableAreaMax.x, tilePos.x);
    tilePos.y = MAX(_playableAreaMin.y, tilePos.y);
    tilePos.y = MIN(_playableAreaMax.y, tilePos.y);
    
    return tilePos;
}

Vec2 HelloWorld::floatTilePosFromLocation(Vec2 location)
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
    return Vec2(posX, posY);
}

Vec2 HelloWorld::tilePosFromLocation(cocos2d::Vec2 location)
{
    Vec2 pos = floatTilePosFromLocation(location);
    
    // make sure coordinates are within bounds of the playable area, and cast to int
    pos = ensureTilePosIsWithinBounds(Vec2((int)pos.x, (int)pos.y));
    
//    CCLOG("touch at (%.0f, %.0f) is at tileCoord (%i, %i)", location.x, location.y, (int)pos.x, (int)pos.y);
    
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
    
    if (_upperLeft.containsPoint(loc)) {
        _currentMoveDirection = MoveDirectionUpperLeft;
    }
    else if (_lowerLeft.containsPoint(loc)) {
        _currentMoveDirection = MoveDirectionLowerLeft;
    }
    else if (_upperRight.containsPoint(loc)) {
        _currentMoveDirection = MoveDirectionUpperRight;
    }
    else if (_lowerRight.containsPoint(loc)) {
        _currentMoveDirection = MoveDirectionLowerRight;
    }
    return true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
}

void HelloWorld::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    _currentMoveDirection = MoveDirectionNone;
}

void HelloWorld::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{

}

void HelloWorld::update(float delta)
{
    // if the tilemap is currently being moved, wait until it's done moving
    if (_tileMap->getNumberOfRunningActions() == 0) {
        if (_currentMoveDirection != MoveDirectionNone) {
            // player is always standing on the tile which is centered on the screen
            Vec2 tilePos = tilePosFromLocation(_screenCenter);
            
            // get the tile coordinate offset for the direction we're moving to
            CC_ASSERT(_currentMoveDirection < MAX_MoveDirections);
            Vec2 offset = _moveOffsets[_currentMoveDirection];
            
            // offset the tile position and then make sure it's within bounds of the playable area
            tilePos = Vec2(tilePos.x + offset.x, tilePos.y + offset.y);
            tilePos = ensureTilePosIsWithinBounds(tilePos);
            
            if (!isTilePosBlocked(tilePos)) {
                centerTileMapOnTileCoord(tilePos);
            }
        }
    }
    
    // continuously fix the player's Z position
    Vec2 tilePos = floatTilePosFromLocation(_screenCenter);
    _player->updateVertexZ(tilePos);
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
