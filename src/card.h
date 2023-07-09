#ifndef CARD_H
#define CARD_H

#include <graphics/glm_helper.h>
#include <render.h>
#include <manager.h>

#include <vector>

#include "board.h"
#include "level.h"

struct Card {
    CounterType cType;
    Resource::Texture tex;
    glm::vec4 baseRect;
    glm::vec4 rect;
    glm::mat4 model;
    glm::vec4 colour = glm::vec4(1.0f);
    float depth = 0.0f;
    bool selected = false;
    bool disabled = false;
    void update(glm::vec4 rect) {
	this->rect = rect;
        this->model = glmhelper::calcMatFromRect(
		rect, 0.0f, depth);
    }
};

class Hand {
 public:
    Hand(){}
    Hand(Manager *manager);
    void setLevel(Level level, Board &board);
    void update(Manager *manager, Board &board);
    void draw(Render *render);
    bool isEmpty();
 private:
    Resource::Texture cardTexs[(int)CounterType::Lake + 1];
    std::vector<Card> cards;
    bool cardSelected = false;
    bool empty = false;
    bool checkEmpty();
};

#endif
