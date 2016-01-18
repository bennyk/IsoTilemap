//
//  Player.hpp
//  IsoTilemap
//
//  Created by Benny Khoo on 18/01/2016.
//
//

#ifndef Player_hpp
#define Player_hpp

#include "cocos2d.h"

class Player : public cocos2d::Sprite
{
public:
    static Player *createWithTileMap(cocos2d::TMXTiledMap *tileMap);
    void updateVertexZ(cocos2d::Vec2 tilePos);
    
private:
    bool initWithTileMap(cocos2d::TMXTiledMap *tileMap);
    
private:
    cocos2d::TMXTiledMap *_tileMap;
};

#endif /* Player_hpp */
