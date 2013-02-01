#pragma once

class PlatformPhysics2dEditor {
public:
  PlatformPhysics2dEditor(void)
  : camera(0x0)
  , cellWidth(0)
  , cellHeight(0)
  , lastTick(0) {
    // Do nothing.
  }

  void Init(Camera2d *camera);

  void Tick(void);
  void Draw(void);

  void SetCellDimensions(i32 width, i32 height);

private:
  Camera2d *camera;

  i32 cellWidth;
  i32 cellHeight;

  typedef zbpair(i32, i32) GridCoordinate;
  typedef zbmap(GridCoordinate, CollisionTile) EditorTileStorage;
  EditorTileStorage editorTiles;

  TimeStamp lastTick;
};

void PointToGridCoordinate(const vector2d &p, i32 gridCellWidth, i32 gridCellHeight, i32 *xIndexOut, i32 *yIndexOut);
void AABB2dToGridCoordinateRange(const aabb2d &b, i32 gridCellWidth, i32 gridCellHeight, i32 *xStartOut, i32 *yStartOut, i32 *xEndOut, i32 *yEndOut);
