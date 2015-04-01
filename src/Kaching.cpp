#include "stdafx.h"
#include "Kaching.h"




void Blob::run(CDC* dc, Blobs& queue) {
  for (int i = 0; i < Count; ++i) {
    queue.blobs.push_back(new Blob(i + 1));
  }

  std::vector<Blob*> output;
  while (queue.blobs.size() > 0) {
    int ix = rnd::Rnd(queue.blobs.size());
    Blob* pick = queue.blobs[ix];
    if (!queue.growSomeSide(*pick, dc)) {
      queue.remove(ix);
      output.push_back(pick);
      pick->addDoor(dc, queue);
      pick->clearInner(dc, queue);
    }
  }

  /*
  std::vector<Blob*>::iterator i;
  for (i = output.begin(); i != output.end(); ++i) {
    Blob* ptr = *i;
    delete ptr;
  }
  */
}

bool Blobs::growSomeSide(Blob& b, CDC* dc) {
  // Determine which b sides are still open:
  int sides[4];
  int sideCount = 0;
  for (int i = 0; i < 4; ++i) {
    if (!b.deadSides[i]) {
      sides[sideCount] = i;
      ++sideCount;
    }
  }
  if (sideCount == 0) { return false; }
  // pick an open side:
  int side = sides[rnd::Rnd(sideCount)];
  exploreSide(side, b, dc);
  return true;
}


bool Blobs::isLegal(CPoint a) {
  if (a.x < 0 || a.y < 0 || a.x >= Blob::Side || a.y >= Blob::Side) { return false; }
  return true;
}

bool Blobs::occupied(CPoint a) {
  if (a.x < 0 || a.y < 0 || a.x >= Blob::Side || a.y >= Blob::Side) { return true; }
  return marks[a.x][a.y].visited();
}



enum { S_Left = 0, S_Top = 1, S_Right = 2, S_Bottom = 3 };

bool Blobs::sideFree(CPoint a, CPoint b, CPoint dir, CPoint nei) { 
  for (CPoint p = a;; p += dir) {
    if (occupied(p)) { return false; }
    // But we also want the neighbour free:
    CPoint p2 = p + nei;
    if (occupied(p2)) { return false; }
    if (p == b) {
      break;
    }
  }
  return true;
}

void Blobs::occupySide(CPoint a, CPoint b, CPoint dir, Blob& z, CDC* dc) {
  for (CPoint p = a;; p += dir) {
    if (isLegal(p)) { marks[p.x][p.y].occupyMark(z.id); }
    if (p == b) {
      break;
    }
  }
  plotBox(a, b, z.color, dc);
}

void Blobs::plotBox(CPoint a, CPoint b, COLORREF color, CDC* dc) {
  if (dc != NULL) {
    const int sc = 12;
    CPoint sa(a.x*sc, a.y*sc);
    CPoint sb(b.x*sc + sc - 1, b.y*sc + sc - 1);
    CRect sr(sa, sb);
    CBrush brush(color); // RGB(240, 250, 230));
    dc->FillRect(&sr, &brush);
    //dc.MoveTo(a);
    //dc.LineTo(b);
  }
}

void Blobs::exploreSide(int side, Blob& z, CDC* dc) {
  // side is left,right,top,bottom 0 1 2 3.
  /* we need to check if they are free/unoccupied. */
  CRect& r = z.r;

  CPoint a, b, dir, nei;
  switch (side) {
  case S_Left:   a = CPoint(r.left - 1, r.top); b = CPoint(r.left - 1, r.bottom); dir = CPoint(0, 1); nei = CPoint(-1, 0);  break;
  case S_Top:    a = CPoint(r.left, r.top - 1); b = CPoint(r.right, r.top - 1); dir = CPoint(1, 0); nei = CPoint(0, -1); break;
  case S_Right:  a = CPoint(r.right + 1, r.top); b = CPoint(r.right + 1, r.bottom); dir = CPoint(0, 1);  nei = CPoint(1, 0); break;
  case S_Bottom: a = CPoint(r.left, r.bottom + 1); b = CPoint(r.right, r.bottom + 1); dir = CPoint(1, 0);  nei = CPoint(0, 1); break;
  }
  if (!sideFree(a, b, dir, nei)) {
    z.deadSides[side] = true; // It's dead then.
    return;
  }

  occupySide(a, b, dir, z, dc);

  switch (side) {
  case S_Left:   r.left--; break;
  case S_Top:    r.top--; break;
  case S_Right:  r.right++; break;
  case S_Bottom: r.bottom++; break;
  }

}


void Blob::addDoor(CDC* dc, Blobs& blobs) {
  bool hor = !!rnd::Rnd(2);
  int pos = (hor ? rnd::Rnd(r.left + 1, r.right) : rnd::Rnd(r.top + 1, r.bottom)); // we avoid the corners.
  bool side = !!rnd::Rnd(2);
  if (hor) {
    door.x = pos; door.y = (side ? r.top : r.bottom);
  } else { // ie vert.
    door.x = (side ? r.left : r.right);  door.y = pos;
  }

  blobs.marks[door.x][door.y].occupyMark(-1000); // -1000 is door. 

  if (dc) {
    COLORREF color = RGB(0, 0, 0);
    Blobs::plotBox(door, door, color, dc);
  }
}

void Blob::clearInner(CDC* dc, Blobs& blobs) {
  for (int x = r.left + 1; x < r.right; ++x) {
    for (int y = r.top + 1; y < r.bottom; ++y) {
      CPoint i(x, y);
      MarkB& inner = blobs.marks[i.x][i.y]; // door.x][door.y];
      inner.c = -inner.c;
      if (dc) {
        COLORREF color2 = RGB(255-GetRValue(color), 255-GetGValue(color), 255-GetBValue(color));
        Blobs::plotBox(i, i, color2, dc);
      }

    }
  }
}