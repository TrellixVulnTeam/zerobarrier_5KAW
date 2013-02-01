
void PointToGridCoordinate(const vector2d &p, int gridCellWidth, int gridCellHeight, int *xIndexOut, int *yIndexOut) {
  *xIndexOut = (int)floorf(p.x / gridCellWidth);
  *yIndexOut = (int)floorf(p.y / gridCellHeight);
}

void AABB2dToGridCoordinateRange(const aabb2d &b, int gridCellWidth, int gridCellHeight, int *xStartOut, int *yStartOut, int *xEndOut, int *yEndOut) {
  PointToGridCoordinate(b.tl(), gridCellWidth, gridCellHeight, xStartOut, yEndOut);
  PointToGridCoordinate(b.br(), gridCellWidth, gridCellHeight, xEndOut, yStartOut);
}

void PlatformPhysics2dEditor::Init(Camera2d *camera) {
  this->camera = camera;

  cellWidth = 32;
  cellHeight = 32;
}

void PlatformPhysics2dEditor::Tick(void) {
  TimeStamp currentTime = zb_time::Time();
  const f32 dt = (f32)zbmin(0.5, zb_time::TimeDifference(lastTick, currentTime));
  lastTick = currentTime;

  vector2d cameraMove;
  const f32 cameraSpeed = cellWidth * 20.0f;
  if (CONTROLS->KeyDownTranslate("a")) {
    cameraMove.x -= cameraSpeed * dt;
  }

  if (CONTROLS->KeyDownTranslate("d")) {
    cameraMove.x += cameraSpeed * dt;
  }

  if (CONTROLS->KeyDownTranslate("s")) {
    cameraMove.y -= cameraSpeed * dt;
  }

  if (CONTROLS->KeyDownTranslate("w")) {
    cameraMove.y += cameraSpeed * dt;
  }

  const f32 zoomSpeed = 0.5f;
  if (CONTROLS->KeyDownTranslate("z")) {
    camera->Zoom(zoomSpeed * dt);
  }

  if (CONTROLS->KeyDownTranslate("x")) {
    camera->Zoom(-zoomSpeed * dt);
  }

  if (CONTROLS->KeyDownTranslate("c")) {
    camera->SetZoom(1.0f);
  }

  camera->Move(cameraMove);

  if (CONTROLS->MouseButtonPressed(ControlMouseButton::Left)) {
    vector2d click;
    CONTROLS->MousePosition(&click.x, &click.y);

    i32 x = 0;
    i32 y = 0;
    PointToGridCoordinate(camera->ViewportToWorld(click), cellWidth, cellHeight, &x, &y);

    CollisionTile &tile = editorTiles[GridCoordinate(x, y)];
    if (tile.Type == CollisionType::Empty) {
      tile.Type = CollisionType::Solid;
    }
    else {
      tile.Type = CollisionType::Empty;
    }

    CollisionTile &up = editorTiles[GridCoordinate(x, y+1)];
    if (up.Type == CollisionType::Solid) {
      if (tile.Type == CollisionType::Solid) {
        tile.TopEdge = CollisionType::Empty;
        up.BottomEdge = CollisionType::Empty;
      }
      else
      {
        tile.TopEdge = CollisionType::Solid;
        up.BottomEdge = CollisionType::Solid;
      }
    }
    else {
      tile.TopEdge = tile.Type;
      up.BottomEdge = tile.Type;
    }

    CollisionTile &down = editorTiles[GridCoordinate(x, y-1)];
    if (down.Type == CollisionType::Solid) {
      if (tile.Type == CollisionType::Solid) {
        tile.BottomEdge = CollisionType::Empty;
        down.TopEdge = CollisionType::Empty;
      }
      else
      {
        tile.BottomEdge = CollisionType::Solid;
        down.TopEdge = CollisionType::Solid;
      }
    }
    else {
      tile.BottomEdge = tile.Type;
      down.TopEdge = tile.Type;
    }

    CollisionTile &left = editorTiles[GridCoordinate(x-1, y)];
    if (left.Type == CollisionType::Solid) {
      if (tile.Type == CollisionType::Solid) {
        tile.LeftEdge = CollisionType::Empty;
        left.RightEdge = CollisionType::Empty;
      }
      else
      {
        tile.LeftEdge = CollisionType::Solid;
        left.RightEdge = CollisionType::Solid;
      }
    }
    else {
      tile.LeftEdge = tile.Type;
      left.RightEdge = tile.Type;
    }

    CollisionTile &right = editorTiles[GridCoordinate(x+1, y)];
    if (right.Type == CollisionType::Solid) {
      if (tile.Type == CollisionType::Solid) {
        tile.RightEdge = CollisionType::Empty;
        right.LeftEdge = CollisionType::Empty;
      }
      else
      {
        tile.RightEdge = CollisionType::Solid;
        right.LeftEdge = CollisionType::Solid;
      }
    }
    else {
      tile.RightEdge = tile.Type;
      right.LeftEdge = tile.Type;
    }
  }
}

void PlatformPhysics2dEditor::Draw(void) {
  RENDERTHREAD->SetCamera(*camera);

  const aabb2d cameraWorldBounds = camera->GetViewportWorldBounds();
  Draw::Rect(cameraWorldBounds, Color(1.0f, 1.0f, 1.0f, 0.3f), true);

  i32 xStart = 0;
  i32 yStart = 0;
  i32 xEnd = 0;
  i32 yEnd = 0;
  AABB2dToGridCoordinateRange(cameraWorldBounds, cellWidth, cellHeight, &xStart, &yStart, &xEnd, &yEnd);

  // Draw the background grid.
  const f32 clipLeft = cameraWorldBounds.left();
  const f32 clipRight = cameraWorldBounds.right();
  for (i32 y = yStart; y <= yEnd; ++y) {
    const f32 yOffset = (f32)y*cellHeight;
    Draw::Line(vector2d(clipLeft, yOffset), vector2d(clipRight, yOffset), Color::Blue);
  }

  const f32 clipTop = cameraWorldBounds.top();
  const f32 clipBottom = cameraWorldBounds.bottom();
  for (i32 x = xStart; x <= xEnd; ++x) {
    const f32 xOffset = (f32)x*cellWidth;
    Draw::Line(vector2d(xOffset, clipTop), vector2d(xOffset, clipBottom), Color::Blue);
  }

  // Draw the collision info.
  const f32 gridCellWidth = (f32)cellWidth;
  const f32 gridCellHeight = (f32)cellHeight;

  for (i32 y = yStart; y <= yEnd; ++y) {
    for (i32 x = xStart; x <= xEnd; ++x) {
      EditorTileStorage::iterator iTile = editorTiles.find(GridCoordinate(x, y));
      if (iTile != editorTiles.end()) {
        const CollisionTile &tile = iTile->second;

        if (tile.Type == CollisionType::Solid) {
          Draw::Rect(aabb2d(x*gridCellWidth + gridCellWidth / 2.0f, y*gridCellHeight + gridCellHeight / 2.0f, gridCellWidth / 2.0f, gridCellHeight / 2.0f), Color(1.0f, 0.0f, 0.0f, 0.2f), true);
        }

        switch (tile.TopEdge) {
          case CollisionType::Solid: Draw::ThickLine(vector2d(x*gridCellWidth, y*gridCellHeight + gridCellHeight), vector2d(x*gridCellWidth + gridCellWidth, y*gridCellHeight + gridCellHeight), 5.0f, Color::Red); break;
          default: break;
        }

        switch (tile.BottomEdge) {
          case CollisionType::Solid: Draw::ThickLine(vector2d(x*gridCellWidth, y*gridCellHeight), vector2d(x*gridCellWidth + gridCellWidth, y*gridCellHeight), 5.0f, Color::Red); break;
          default: break;
        }

        switch (tile.LeftEdge) {
          case CollisionType::Solid: Draw::ThickLine(vector2d(x*gridCellWidth, y*gridCellHeight), vector2d(x*gridCellWidth, y*gridCellHeight + gridCellHeight), 5.0f, Color::Red); break;
          default: break;
        }

        switch (tile.RightEdge) {
          case CollisionType::Solid: Draw::ThickLine(vector2d(x*gridCellWidth + gridCellWidth, y*gridCellHeight), vector2d(x*gridCellWidth + gridCellWidth, y*gridCellHeight + gridCellHeight), 5.0f, Color::Red); break;
          default: break;
        }
      }
    }
  }
}
