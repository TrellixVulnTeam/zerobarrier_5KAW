#pragma once

ZB_NAMESPACE_BEGIN

namespace CollisionType {
  enum CollisionType {
    Empty,
    Solid,
    Complex,
  };
}

struct CollisionTile {
  // Tile type, as placed in the editor.
  CollisionType::CollisionType Type : 2;

  // Edge types, determined from tile type and surrounding tile types.
  CollisionType::CollisionType TopEdge : 2;
  CollisionType::CollisionType LeftEdge : 2;
  CollisionType::CollisionType BottomEdge : 2;
  CollisionType::CollisionType RightEdge : 2;

  // Index to be used to lookup more complex collision primitives.
  u32 ComplexTileIndex : 22;
};

struct CollisionGrid {
  zbvector(CollisionTile) tiles;
  int width;
  int height;
  int cellWidth;
  int cellHeight;
};

struct Platformer2d {
  aabb2d body;
};

class PlatformPhysics2d {
public:

private:

};

ZB_NAMESPACE_END
