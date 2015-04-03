#ifndef CELLMAP_H
#define CELLMAP_H

#include <vector>

#include "util/debstr.h"

#include "Envir.h"
#include "Obj.h"
#include "Creature.h"

#include "../LOS.h"



class VPoint { // Points in the viewport coord system' - not used much.
public:
  CPoint p;
};


class Cell {
public:
  Envir envir;
  ObjSlot item; 
  Creature creature;
  CPoint overlay; // doesn't need save.

  Cell() { }

  bool is_lit() const { return envir.permLight;  } // So far, only envir/floor can contribute to 'cell lighted', but it might be e.g. a glowing monster instead.
  void markWalls(CPoint pos) { envir.markWalls(pos);  }
  void lightCells(CPoint pos) { envir.lightCells(pos);  }

  bool blocked() const {
    // debstr() << "blocked, envir:" << envir.blocked() << "/" << envir.type << ", mob:" << creature.blocked() << "\n";
    return envir.blocked() || creature.blocked();
  }

  bool hasOverlay() const { return !!(overlay != CPoint(0, 0));  }
  void clearOverlay() { overlay = CPoint(0, 0);  }

  bool persist(class Persist& p) {
    envir.persist(p);
    // item.persist(p); // FIXME: No, they should be output separately.
    // creature.persist(p); // FIXME: No, they should be output separately.
    return true;
  }

};



/// class CellColumn {
/// public:
///   enum ColumnConst { Height = 60 }; //30 }; // 30
///   Cell cells[Height];
///   Cell& operator [] (int y); // { return cells[y];  }
/// };



class Map {
public:
  int Width2; // can't be const, because of persist().
  int Height2;

  enum MapConst { 
    // Width1 = 101, // Fixme, these should be  dynamic, and fixme2: height should be  independent of width.. 
    // Height1 = 60, //CellColumn::Height,

    DefWidth = 44, //101, // FIXME: with these 'dynamic', try to make them very small!
    DefHeight= 26, //60
  };

  LightMap lightmap;

private:
  std::vector < std::vector< Cell > > cells_i;

  //std::vector <CellColumn> cellColumns_i;
  // CellColumn& cellColumns(int colIx); // [Width];

public:
  //CellColumn& operator [] (int x); 
  Cell& operator [] (CPoint p); 
  Cell* cell(CPoint p); // NULL if outside.

  bool legalPos(CPoint pos);
  bool canSee(CPoint a, CPoint b, bool onlyEnvir);

  void clearMob(class Mob& m); // helper for using stairs
  void moveMobImpl(class Mob& m, CPoint newpos, bool bInvalidate);
  void addObj(class Obj& o, CPoint pos);
  void moveMob(class Mob& m, CPoint newpos);
  void moveMobNoInv(class Mob& m, CPoint newpos); // { // , bool bInvalidate) {

  void setMobForce(class Mob& m, CPoint newpos, bool bInvalidate); // used for swapping two mobs.
  void adjustViewport(CPoint newpos, bool bInvalidate);

  Map(int w, int h);

  void resizeMap();

  void initWorld(int level); // JG, FIXME: All this shouldn't really clutter Map/CellMap -'initWorld'  should go somewhere OUTSIDE basic structures' impls.

  void initTown(int level);
  void initTunnels(int level);
  void initOuterBorders();

  void addColDemo(int x, int y);

  void initTunnels2(int level);

  void initKachingBlob(int level);

  void initTunnelsBrown(int level);

  // static Map map;

  static CPoint key2dir(char key);


  void addRandomMob(int level);
  bool addRandomMobAtPos(CPoint pos, int level);
  void scatterMobsAtPos(CPoint pos, int n, int level, int radius);

  void addRandomObj(int level);
  void addObjAtPos(CPoint pos, int level);
  void scatterObjsAtPos(CPoint pos, int n, int level, int radius);


  void addStairs(int theWidth, int theHeight);
  void addStair(EnvirEnum type, int theWidth, int theHeight);

  CPoint findFreeEnvir(EnvirEnum type, int theWidth, int theHeight);
  CPoint findNextEnvir(CPoint start, EnvirEnum type); // , int theWidth);

  CPoint findNextMob(CPoint start, CreatureEnum ctype);
  // CPoint findMobFree(CreatureEnum type);


  bool persist(class Persist& p);
  bool transferObj(Persist& p); // Only works for obj IN, to map:

  static  CPoint dirs[4];
  static  CPoint diag[4];
};




class Viewport {
public:
  enum Sizes { 
	  VP_Width = 32, 
	  VP_Height = 18, // 20, // 25,  // Note well, this is NOT term height! (it's smaller.)
	  SweetspotPct = 20,
    Y_Offset = 2 // todo/fixme: this must be included in all term<->viewport<->dung-map transforms.
  };
  CPoint offset; // Offset is WORLD coordinates, of upper left (0,0) VIEWPORT corner. (e.g. 1,1 for first offset.) So you must ADD it, to go from  v2W
  CRect sweetspotArea;
  Viewport();
  bool adjust(CPoint wpos, class Map& map); // True if adjust happens.

  CPoint w2v(CPoint w);
  CPoint v2w(CPoint v);

  bool persist(class Persist& p) {
    p.transfer(offset.x, "offset_x"); 
    p.transfer(offset.y, "offset_y"); 
    return true;
  }

  static Viewport vp;
};


#endif // CELLMAP_H
