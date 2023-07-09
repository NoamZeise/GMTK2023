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

const int BOARD_WIDTH = 5;
const int BOARD_HEIGHT = 5;

class Board {
 public:
    Board(){}
    Board(Render *render);
    void draw(Render *render);
    void set(unsigned int x, unsigned int y, CounterType type);
    void intersect(glm::vec3 pos);
 private:
    
    Obj3D boardObj;
    std::vector<Obj3D> squares;

   CounterType board[BOARD_WIDTH * BOARD_HEIGHT];
   Obj3D boardTransforms[BOARD_WIDTH * BOARD_HEIGHT];
   
   glm::mat4 counterBase;
   Resource::Model modelType[(size_t)CounterType::Lake + 1];
    int highlightX = -1;
    int highlightY = -1;
};

#endif
