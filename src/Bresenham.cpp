#include "stdafx.h"
#include "Bresenham.h"

// #include "./Dungeons.h" // was only for bresenExample.

// Bresenham::Bresenham() {}
// Bresenham::~Bresenham() {}



// void putpixel(int x, int y, int color){}

// Example impl: (not active)
void bres_line(int x,int y,int x2, int y2, std::vector<CPoint>& pixels) { // int color) {
  // Bresenham.
  // Alternatively, this should be a visitor-iterator, where 'putpixel' becomes the callback.
  // Source: http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
  int w = x2 - x; 
  int h = y2 - y; 
  int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0; 
  if (w<0) { dx1 = -1; } else if (w>0) { dx1 = 1; }
  if (h<0) { dy1 = -1; } else if (h>0) { dy1 = 1; }
  if (w<0) { dx2 = -1; } else if (w>0) { dx2 = 1; }
  int longest = abs(w); 
  int shortest = abs(h); 
  if (!(longest>shortest)) {
    longest = abs(h); 
    shortest = abs(w); 
    if (h<0) { dy2 = -1; } else if (h>0) { dy2 = 1; }
    dx2 = 0;             
  }
  int numerator = longest >> 1; 
  for (int i=0; i<=longest; i++) {
    pixels.push_back(CPoint(x, y)); // putpixel(x, y, color);
    numerator += shortest;
    if (!(numerator<longest)) {
        numerator -= longest; 
        x += dx1; y += dy1;
    } else {
        x += dx2; y += dy2;
    }
  }
}



 
BresIter::BresIter(int x1, int y1, int x2, int y2) {
  // Bresenham.
  // Alternatively, this should be a visitor-iterator, where 'putpixel' becomes the callback.
  // Source: http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
  x = x1; y = y1;

  int w = x2 - x;
  int h = y2 - y;

  dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
  if (w < 0) { dx1 = -1; }
  else if (w > 0) { dx1 = 1; }
  if (h < 0) { dy1 = -1; }
  else if (h > 0) { dy1 = 1; }
  if (w < 0) { dx2 = -1; }
  else if (w > 0) { dx2 = 1; }
  longest = abs(w);
  shortest = abs(h);
  if (!(longest > shortest)) {
    longest = abs(h);
    shortest = abs(w);
    if (h < 0) { dy2 = -1; }
    else if (h > 0) { dy2 = 1; }
    dx2 = 0;
  }
  numerator = longest >> 1;

  i = 0;
}



CPoint BresIter::next() {
  CPoint curPoint(x, y);
  pixels.push_back(curPoint); // putpixel(x, y, color);

  numerator += shortest;
  if (!(numerator<longest)) {
    numerator -= longest; 
    x += dx1; y += dy1;
  } else {
    x += dx2; y += dy2;
  }

  ++i;
  return curPoint;
}


/*
void bresenExample() {
  BresIter i(2, 9, 29, 14);
  for ( ; !i.done(); ) {
    CPoint p = i.next();
    CL->map[p].envir.type = EN_Green;
    // Use p
  } 
}
*/