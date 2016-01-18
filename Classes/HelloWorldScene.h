#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    
    cocos2d::Vec2 tilePosFromLocation(cocos2d::Vec2 location);
    void centerTileMapOnTileCoord(cocos2d::Vec2 tilePos);
        
private:
    cocos2d::TMXTiledMap *_tileMap;
};

#endif // __HELLOWORLD_SCENE_H__
