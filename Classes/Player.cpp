//
//  Player.cpp
//  IsoTilemap
//
//  Created by Benny Khoo on 18/01/2016.
//
//

#include "Player.hpp"

using namespace cocos2d;

Player* Player::createWithTileMap(TMXTiledMap *tileMap)
{
    Player *ret = new (std::nothrow) Player();
    if (ret && ret->initWithTileMap(tileMap))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Player::initWithTileMap(cocos2d::TMXTiledMap *tileMap)
{
    if (Sprite::initWithFile("ninja.png")) {
        _tileMap = tileMap;
        return true;
    }
    return false;
}

void Player::updateVertexZ(cocos2d::Vec2 tilePos)
{
    // Lowest Z value is at the origin point and its value is equal to map width + height.
    // This is because automatic vertexZ values are not counted along rows or columns of tilemap coordinates,
    // but horizontally (diagonally in tilemap coordinates). Eg the tiles at coordinates:
    // 0,4 / 1,3 / 2,2 / 3,1 / 4,0 all have the same vertexZ value.
    float lowestZ = -(_tileMap->getMapSize().width + _tileMap->getMapSize().height);
    
    // Current Z value is simply the sum of the current tile coordinates.
    float currentZ = tilePos.x + tilePos.y;
    
    // Subtract 1 to always make the player appear behind the objects he is positioned at.
    this->setPositionZ(lowestZ + currentZ - 1.5f);
    
    CCLOG("vertexZ: %.3f at tile pos: (%.0f, %.0f) -- lowestZ: %.1f, currentZ: %.0f",
          this->getPositionZ(), tilePos.x, tilePos.y, lowestZ, currentZ);
    
#if 0
    auto layer = _tileMap->getLayer("Objects");
    auto tile = layer->getTileAt(tilePos);
    if (tile != nullptr) {
        CCLOG("tileZ %.3f", tile->getPositionZ());
    }
#endif
    
}