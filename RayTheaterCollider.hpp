#ifndef RayTheaterCollider_H
#define RayTheaterCollider_H 1

#include <raylib.h>
#include <vector>

namespace Theater {

// BM: Collider - Class
//==============================================================================
class ColliderPoint;
class ColliderCircle;
class ColliderRect;
class ColliderZone;

class Collider {
  virtual bool isCollidingWithPoint(ColliderPoint *) = 0;
  virtual bool isCollidingWithRect(ColliderRect *) = 0;
  virtual bool isCollidingWithCircle(ColliderCircle *) = 0;
  virtual bool isCollidingWithZone(ColliderZone *) = 0;

  virtual bool containsPoint(float x, float y) = 0;

public:
  static bool zoneContainsPoint(std::vector<Vector2> *zoneborder,
                                Vector2 point) {

    switch (zoneborder->size()) {
    case 0:
      return false;
      break;

    case 1:
      auto dot = zoneborder->at(0);
      return dot.x == point.x && dot.y == point.y;
      break;
    }

    auto connectDots = [](std::vector<Vector2> *dots, int d1, int d2,
                          Vector2 point) {
      auto pos1 = dots->at(d1);
      auto pos2 = dots->at(d2);

      unsigned char b1 = ((pos1.y <= point.y ? 1 : 0) << 0) |
                         ((pos1.y > point.y ? 1 : 0) << 1);

      unsigned char b2 = ((pos2.y <= point.y ? 1 : 0) << 0) |
                         ((pos2.y > point.y ? 1 : 0) << 1);

      if ((b1 ^ b2) == 3) {
        float lineX = pos2.x - pos1.x;
        float lineY = pos2.y - pos1.y;
        float lineL = lineX * lineX + lineY * lineY;

        float circleX = point.x - pos1.x;
        float circleY = point.y - pos1.y;

        float t =
            fmax(0, fmin(lineL, (lineX * circleX + lineY * circleY))) / lineL;

        float offsetX = pos1.x + ((pos2.x - pos1.x) * t);

        return offsetX > point.x ? 1 : 0;
      };

      return 0;
    };

    unsigned int intersections = 0;
    for (int a = 0; a < zoneborder->size() - 1; a++)
      intersections += connectDots(zoneborder, a, a + 1, point);

    intersections += connectDots(zoneborder, zoneborder->size() - 1, 0, point);

    return (intersections & 1) == 1;
  }
};

// BM: Collider - Point - Class
//==============================================================================
class ColliderPoint : public Collider {
public:
  // Interface
  //---------------------------------------------------------------------------
  virtual Vector2 getPosition() = 0;

  // Don't touch
  //---------------------------------------------------------------------------
  bool isCollidingWithPoint(ColliderPoint *) override;
  bool isCollidingWithRect(ColliderRect *) override;
  bool isCollidingWithCircle(ColliderCircle *) override;
  bool isCollidingWithZone(ColliderZone *) override;

  bool containsPoint(float x, float y) override;

}; // namespace Theater

// BM: Collider - Rect - Class
//==============================================================================
class ColliderRect : public Collider {
public:
  // Interface
  //---------------------------------------------------------------------------
  virtual Rectangle getRect() = 0;

  // Don't touch
  //---------------------------------------------------------------------------
  bool isCollidingWithPoint(ColliderPoint *) override;
  bool isCollidingWithRect(ColliderRect *) override;
  bool isCollidingWithCircle(ColliderCircle *) override;
  bool isCollidingWithZone(ColliderZone *) override;

  bool rectContainsPoint(Rectangle r, Vector2 p);
  bool containsPoint(float x, float y) override;

private:
  bool rectInRect(Rectangle r1, Rectangle r2);

}; // namespace Theater

// BM: Collider - Circle - Class
//==============================================================================
class ColliderCircle : public Collider {
public:
  // Interface
  //---------------------------------------------------------------------------
  virtual Vector2 getPosition() = 0;
  virtual float getRadius() = 0;

  // Don't touch
  //---------------------------------------------------------------------------
  bool isCollidingWithPoint(ColliderPoint *) override;
  bool isCollidingWithRect(ColliderRect *) override;
  bool isCollidingWithCircle(ColliderCircle *) override;
  bool isCollidingWithZone(ColliderZone *) override;

  bool containsPoint(float x, float y) override;
  bool containsPoint(Vector2 p);

private:
  bool lineHitsCircle(float cx, float cy, float radius, float x1, float y1,
                      float x2, float y2);

  bool pointHitsCircle(float cx, float cy, float radius, float px, float py);

  bool circleHitsPolyShape(Vector2 circleOrigin, float circleRadius,
                           std::vector<Vector2> lst);
};

// BM: Collider - Zone - Class
//==============================================================================
class ColliderZone : public Collider {
public:
  // Interface
  //---------------------------------------------------------------------------
  virtual std::vector<Vector2> *getZoneBorder() = 0;

  // Don't touch
  //---------------------------------------------------------------------------
  bool isCollidingWithPoint(ColliderPoint *) override;
  bool isCollidingWithRect(ColliderRect *) override;
  bool isCollidingWithCircle(ColliderCircle *) override;
  bool isCollidingWithZone(ColliderZone *) override;

  bool containsPoint(float x, float y) override;

private:
  bool containsOneOfPoints(std::vector<Vector2> *shape,
                           std::vector<Vector2> points);
};

// BM: Collider - Circle - Implementation
//==============================================================================
inline bool ColliderCircle::pointHitsCircle(float cx, float cy, float rad,
                                            float px, float py) {
  auto rad2 = rad * rad;
  auto dstx1 = abs(cx - px);
  auto dsty1 = abs(cy - py);

  return dstx1 * dstx1 + dsty1 * dsty1 > rad2;
}

inline bool ColliderCircle::containsPoint(float x, float y) {
  auto p = this->getPosition();
  auto r = this->getRadius();
  return pointHitsCircle(p.x, p.y, r, x, y);
};

inline bool ColliderCircle::lineHitsCircle(float cx, float cy, float cr,
                                           float x1, float y1, float x2,
                                           float y2) {

  float lineX = x2 - x1; // Get distances of Line Vector
  float lineY = y2 - y1;
  float lineL = lineX * lineX + lineY * lineY;

  float circleX = cx - x1; // Get Distances for Circle to Line Vector
  float circleY = cy - y1;

  // Find Closest normalized point on Line to get from line to Circle Origin
  float t = fmax(0, fmin(lineL, (lineX * circleX + lineY * circleY))) / lineL;

  // Translate that normalized point back to the point on the Line
  float offsetX = x1 + ((x2 - x1) * t);
  float offsetY = y1 + ((y2 - y1) * t);

  // do a simple circle to point collision check with the found point on the
  // Line
  return (((cx - offsetX) * (cx - offsetX) + (cy - offsetY) * (cy - offsetY)) <=
          (cr * cr));
}

inline bool ColliderCircle::circleHitsPolyShape(Vector2 circleOrigin,
                                                float circleRadius,
                                                std::vector<Vector2> lst) {
  bool hit = false;
  for (auto a = 0; a < lst.size() - 1; a++) {
    auto s = lst.at(a);
    auto e = lst.at(a + 1);
    hit = hit || this->lineHitsCircle(circleOrigin.x, circleOrigin.y,
                                      circleRadius, s.x, s.y, e.x, e.y);
  }

  return hit;
}

inline bool ColliderCircle::containsPoint(Vector2 p) {
  auto rad = getRadius();
  auto rad2 = rad * rad;

  auto pc = getPosition();

  auto dstx1 = abs(pc.x - p.x);
  auto dsty1 = abs(pc.y - p.y);

  return dstx1 * dstx1 + dsty1 * dsty1 <= rad2;
}

inline bool ColliderCircle::isCollidingWithPoint(ColliderPoint *p) {
  return containsPoint(p->getPosition());
}

inline bool ColliderCircle::isCollidingWithCircle(ColliderCircle *c) {
  auto rad = this->getRadius() + c->getRadius();
  auto rad2 = rad * rad;

  auto p = getPosition();
  auto pc = c->getPosition();

  auto dstx1 = abs(pc.x - p.x);
  auto dsty1 = abs(pc.y - p.y);

  return dstx1 * dstx1 + dsty1 * dsty1 > rad2;
}

inline bool ColliderCircle::isCollidingWithRect(ColliderRect *rc) {
  auto o = this->getPosition();
  auto r = rc->getRect();
  auto rad = this->getRadius();

  return rc->rectContainsPoint(r, o) ||
         circleHitsPolyShape(o, rad,
                             {{r.x, r.y},
                              {r.x + r.width, r.y},
                              {r.x + r.width, r.y + r.height},
                              {r.x, r.y + r.height},
                              {r.x, r.y}});
}

inline bool ColliderCircle::isCollidingWithZone(ColliderZone *z) {
  auto zoneShape = z->getZoneBorder();
  auto circPos = getPosition();
  auto circRad = getRadius();

  return z->containsPoint(circPos.x, circPos.y) ||
         circleHitsPolyShape(circPos, circRad, *zoneShape);
}

//==============================================================================
// BM: Collider - Zone - Implementation
//==============================================================================
inline bool ColliderZone::containsOneOfPoints(std::vector<Vector2> *shape,
                                              std::vector<Vector2> points) {
  for (Vector2 point : points) {
    if (zoneContainsPoint(shape, point))
      return true;
  }

  return false;
}

inline bool ColliderZone::containsPoint(float x, float y) {
  return zoneContainsPoint(getZoneBorder(), {x, y});
}

inline bool ColliderZone::isCollidingWithPoint(ColliderPoint *p) {
  return zoneContainsPoint(getZoneBorder(), p->getPosition());
}

inline bool ColliderZone::isCollidingWithRect(ColliderRect *r) {
  auto rect = r->getRect();
  auto rectZone =
      std::vector<Vector2>({{rect.x, rect.y},
                            {rect.x + rect.width, rect.y},
                            {rect.x + rect.width, rect.y + rect.height},
                            {rect.x, rect.y + rect.height}});

  auto shapeZone = getZoneBorder();

  bool ret = containsOneOfPoints(shapeZone, rectZone);

  if (!ret)
    ret = containsOneOfPoints(&rectZone, *shapeZone);

  return ret;
};

inline bool ColliderZone::isCollidingWithCircle(ColliderCircle *c) {
  return c->isCollidingWithZone(this);
}

inline bool ColliderZone::isCollidingWithZone(ColliderZone *z) {
  auto mePoints = getZoneBorder();
  auto zPoints = z->getZoneBorder();

  bool ret = containsOneOfPoints(mePoints, *zPoints);

  if (!ret)
    return containsOneOfPoints(zPoints, *mePoints);

  return ret;
}

//=============================================================================
// BM: Collider - Point - Implementation
//==============================================================================
inline bool ColliderPoint::containsPoint(float x, float y) {
  auto p1 = this->getPosition();
  return (p1.x == x && p1.y == y);
};

inline bool ColliderPoint::isCollidingWithPoint(ColliderPoint *p) {
  auto p2 = p->getPosition();
  return containsPoint(p2.x, p2.y);
}

inline bool ColliderPoint::isCollidingWithRect(ColliderRect *r) {
  return r->rectContainsPoint(r->getRect(), this->getPosition());
}

inline bool ColliderPoint::isCollidingWithCircle(ColliderCircle *c) {
  return c->containsPoint(getPosition());
}

inline bool ColliderPoint::isCollidingWithZone(ColliderZone *z) {
  return zoneContainsPoint(z->getZoneBorder(), this->getPosition());
}

//==============================================================================
// BM: Collider - Rect - Implementation
//==============================================================================
inline bool ColliderRect::rectContainsPoint(Rectangle r, Vector2 p) {
  return p.x >= r.x && p.x <= r.x + r.width && p.y >= r.y &&
         p.y <= r.y + r.height;
}

inline bool ColliderRect::containsPoint(float x, float y) {
  auto r = this->getRect();
  return x >= r.x && x <= r.x + r.width && y >= r.y && y <= r.y + r.height;
};

inline bool ColliderRect::rectInRect(Rectangle r1, Rectangle r2) {

  bool tlIn = rectContainsPoint(r1, {r2.x, r2.y});
  bool trIn = rectContainsPoint(r1, {r2.x + r2.width, r2.y});
  bool blIn = rectContainsPoint(r1, {r2.x, r2.y + r2.height});
  bool brIn = rectContainsPoint(r1, {r2.x + r2.width, r2.y + r2.height});

  return (tlIn || trIn || blIn || brIn);
}

inline bool ColliderRect::isCollidingWithPoint(ColliderPoint *p) {
  return this->rectContainsPoint(this->getRect(), p->getPosition());
}

inline bool ColliderRect::isCollidingWithCircle(ColliderCircle *c) {
  return c->isCollidingWithRect(this);
}

inline bool ColliderRect::isCollidingWithRect(ColliderRect *r) {
  auto r1 = this->getRect();
  auto r2 = r->getRect();
  return rectInRect(r1, r2) || rectInRect(r2, r2);
}

inline bool ColliderRect::isCollidingWithZone(ColliderZone *z) {
  return z->isCollidingWithRect(this);
}

}; // namespace Theater
#endif
