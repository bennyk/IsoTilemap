#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.hpp"

class HelloWorld : public cocos2d::Layer
{    
public:
    using Vec2 = cocos2d::Vec2;
    using Rect = cocos2d::Rect;
    
    enum EMoveDirection
    {
        MoveDirectionNone = 0,
        MoveDirectionUpperLeft,
        MoveDirectionLowerLeft,
        MoveDirectionUpperRight,
        MoveDirectionLowerRight,
        
        MAX_MoveDirections,
    };
    

public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    virtual void update(float delta) override;
    
private:
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    
    Vec2 ensureTilePosIsWithinBounds(Vec2 tilePos);
    Vec2 floatTilePosFromLocation(Vec2 location);
    cocos2d::Vec2 tilePosFromLocation(Vec2 location);
    void centerTileMapOnTileCoord(Vec2 tilePos);
    bool isTilePosBlocked(Vec2 tilePos);
        
private:
    cocos2d::TMXTiledMap *_tileMap;
    cocos2d::Vec2 _playableAreaMin, _playableAreaMax;
    Player *_player;
    
    Vec2 _screenCenter;
    Rect _upperLeft, _lowerLeft, _upperRight, _lowerRight;
    Vec2 _moveOffsets[MAX_MoveDirections];
    EMoveDirection _currentMoveDirection;
};

#endif // __HELLOWORLD_SCENE_H__
