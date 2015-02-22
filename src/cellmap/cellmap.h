#ifndef CELLMAP_H
#define CELLMAP_H

#include <vector>

#include "util/debstr.h"

#include "Envir.h"
#include "Obj.h"
#include "Creature.h"




class VPoint { // Points in the viewport coord system' - not used much.
public:
  CPoint p;
};


class Cell {
public:
  Envir envir;
  ObjSlot item; 
  Creature creature;
  char cc;
  CPoint overlay;

  Cell() {
    clearChar(); 
  }

  bool charEmpty() const { return cc == '\0';  }
  void clearChar() { cc = '\0';  }

  bool light() const { return envir.permLight;  } // So far, only envir/floor can contribute to 'cell lighted', but it might be e.g. a glowing monster instead.
  void markWalls() { envir.markWalls();  }
  void lightCells() { envir.lightCells();  }

  bool blocked() const {
    // debstr() << "blocked, envir:" << envir.blocked() << "/" << envir.type << ", mob:" << creature.blocked() << "\n";
    return envir.blocked() || creature.blocked();
  }

  bool hasOverlay() const { return !!(overlay == CPoint(0, 0));  }
  void clearOverlay() { overlay = CPoint(0, 0);  }
};



class CellColumn {
public:
  enum ColumnConst { Height = 60 }; //30 }; // 30
  Cell cells[Height];
  Cell& operator [] (int y); // { return cells[y];  }
};



class Map {
public:
  enum MapConst { 
    Width = 101, //100, //48, //16, 
    Height = CellColumn::Height 
  };
private:
  CellColumn cellColumns[Width];
public:
  CellColumn& operator [] (int x); // { return cellColumns[x];  }
  Cell& operator [] (CPoint p); // { return cellColumns[p.x][p.y];  }

  bool legalPos(CPoint pos);

  void moveMob(class Mob& m, CPoint newpos);
  void addObj(class Obj& o, CPoint pos);

  Map();

  void initWorld(); // JG, FIXME: All this shouldn't really clutter Map/CellMap -'initWorld'  should go somewhere OUTSIDE basic structures' impls.

  // static Map map;

  static CPoint key2dir(char key);


  void addRandomMob();
  void addRandomObj();
  void addObjAtPos(CPoint pos);
  void scatterObjsAtPos(CPoint pos, int n);
};




class Viewport {
public:
  enum Sizes { 
	  Width = 32, 
	  Height = 20, // 25, 
	  SweetspotPct = 20};
  CPoint offset; // Offset is WORLD coordinates, of upper left (0,0) VIEWPORT corner. (e.g. 1,1 for first offset.) So you must ADD it, to go from  v2W
  CRect sweetspotArea;
  Viewport();
  bool adjust(CPoint wpos); // True if adjust happens.

  CPoint w2v(CPoint w);
  CPoint v2w(CPoint v);

  static Viewport vp;
};


#endif // CELLMAP_H
