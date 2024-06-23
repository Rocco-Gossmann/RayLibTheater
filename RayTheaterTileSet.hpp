#ifndef RAY_THEATER_TILE_SET_HPP
#define RAY_THEATER_TILE_SET_HPP

#include "raylib.h"
#include <iostream>
#include <unordered_map>
#include <utility>
namespace Theater {

enum TileSetFlags {
  TF_0 = 1,
  TF_1 = 2,
  TF_2 = 4,
  TF_3 = 8,
  TF_4 = 16,
  TF_5 = 32,
  TF_6 = 64,
  TF_7 = 128,
  TF_8 = 256,
  TF_9 = 256 * 2,
  TF_10 = 256 * 4,
  TF_11 = 256 * 8,
  TF_12 = 256 * 16,
  TF_13 = 256 * 32,
  TF_14 = 256 * 64,
  TF_15 = 256 * 128,
};

// BM: Class
//==============================================================================
class TileSet {

public:
  typedef unsigned int Index;
  typedef unsigned short Flags;

  TileSet(Texture2D texture);
  ~TileSet();

  Index AddRect(Rectangle, Flags f = 0);
  void UpdateRect(Index, Rectangle);

  void DrawTile(Index);

private:
  Index m_rect_autoincrement;
  Texture2D m_texture;
  std::unordered_map<Index, Rectangle> m_rects;
  std::unordered_map<Index, Flags> m_flags;
};

// BM: Implementation
//==============================================================================
inline TileSet::TileSet(Texture2D tex)
    : m_rect_autoincrement(0), m_texture(tex), m_rects(), m_flags() {
  std::cout << "build TileSet" << std::endl;
}

inline TileSet::~TileSet() {
  std::cout << "delete TileSet" << std::endl;
  m_flags.clear();
  m_rects.clear();
}

inline TileSet::Index TileSet::AddRect(Rectangle r, TileSet::Flags f) {
  m_rect_autoincrement++;
  Index idx = m_rect_autoincrement;

  m_rects.insert(std::make_pair(idx, r));
  m_flags.insert(std::make_pair(idx, f));

  return idx;
}

inline void TileSet::DrawTile(Index idx) {
  auto find = m_rects.find(idx);
  if (find == m_rects.end())
    return;

  DrawTextureRec(m_texture, find->second, {0, 0}, WHITE);
}

} // namespace Theater
#endif
