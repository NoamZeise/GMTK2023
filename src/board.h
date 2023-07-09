#ifndef BOARD_H
#define BOARD_H

#include <render.h>
#include <glm/glm.hpp>

struct Obj3D {
    Obj3D(){}
    Obj3D(Resource::Model model,
	  glm::mat4 modelMat);
    void draw(Render *render);

    Resource::Model model;
    glm::mat4 modelMat;
    glm::mat4 normalMat;
    glm::vec4 colour = glm::vec4(0.0f);
};

enum class CounterType {
  None,
  Forest,
  Mountain,
  Lake,
};

enum class Dir {
  Up,
  Down,
  Left,
  Right,
  UpLeft,
  UpRight,
  DownLeft,
  DownRight,
};

struct BoardPos {
    int x;
    int y;
    BoardPos() { this->x = -1; this->y = -1;}
    BoardPos(int x, int y) {
	this->x = x;
	this->y = y;
    }
};

struct SetBoard {
    BoardPos pos;
    CounterType c;
};

struct Surrounding {
    BoardPos pos[8];
    CounterType c[8];
};

const int BOARD_WIDTH = 5;
const int BOARD_HEIGHT = 5;

class Board {
 public:
    Board(){}
    Board(Render *render);
    void reset();
    void draw(Render *render);
    void set(unsigned int x, unsigned int y, CounterType type);
    bool set(CounterType type);
    CounterType getCounter(int x, int y);
    Surrounding getSurrounding(int x, int y);
    void intersect(glm::vec3 pos);
    bool wonBoard();
    bool updating();
    void stepBoard();
 private:
    bool boardUpdateRequired = true;
    Obj3D boardObj;
    std::vector<Obj3D> squares;

   CounterType board[BOARD_WIDTH * BOARD_HEIGHT];
   Obj3D boardTransforms[BOARD_WIDTH * BOARD_HEIGHT];
    Resource::Model wateredTileModel;
   glm::mat4 counterBase;
   Resource::Model modelType[(size_t)CounterType::Lake + 1];
    int highlightX = -1;
    int highlightY = -1;
    std::vector<BoardPos> wateredTiles;
    bool won = false;
};

#endif
