#include "board.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <stdexcept>

const glm::vec3 OFFSET(-7.2f, 0.0f, -7.2f);
const glm::vec3 SPACE(3.6f, 0.0f, 3.6f);
const glm::vec3 SCALE(1.7f, 0.1f, 1.7f);
const glm::vec4 PIECE_COLOUR(0.45f, 0.4f, 0.2f, 1.0f);

Obj3D::Obj3D(Resource::Model model, glm::mat4 modelMat) {
    this->model = model;
    this->modelMat = modelMat;
    this->normalMat = glm::inverseTranspose(modelMat);
}

void Obj3D::draw(Render *render) {
    render->DrawModel(model, modelMat, normalMat, colour);
}

Board::Board(Render *render) {
    glm::mat4 modelMat = glm::rotate(glm::mat4(1.0f),
				     glm::radians(90.0f),
				     glm::vec3(1.0f, 0.0f, 0.0f));
    
    boardObj = Obj3D(render->Load3DModel("models/board.obj"),
		     glm::translate(
			     glm::scale(modelMat, glm::vec3(1.0f, 2.0f, 1.0f)),
			     glm::vec3(0.0f, -0.1f, 0.0f)));
    Resource::Model square = render->Load3DModel("models/unitCube.obj");
    modelMat =
	glm::translate(modelMat, OFFSET);
    for(int x = 0; x < 5; x++) {
	for(int y = 0; y < 5; y++) {
	    glm::mat4 pos =
		    glm::translate(
			    modelMat,
			    glm::vec3(x * SPACE.x, 0.0f,
				      y * SPACE.z));
	    this->board[x * BOARD_WIDTH + y] = CounterType::None;
	    this->boardTransforms[x * BOARD_WIDTH + y] = Obj3D(Resource::Model(),
							       pos);
	    squares.push_back(Obj3D(square, glm::scale(pos, SCALE)));
			      
	    squares.back().colour = PIECE_COLOUR;
	}
    }
    modelType[(size_t)CounterType::Forest] = 
	    render->Load3DModel("models/forest.obj");
    modelType[(size_t)CounterType::Mountain] = 
	render->Load3DModel("models/mountain.obj");
    modelType[(size_t)CounterType::Lake] =
	render->Load3DModel("models/lake.obj");
    counterBase = modelMat;

    set(1, 1, CounterType::Forest);
    set(2, 3, CounterType::Lake);
    set(4, 4, CounterType::Mountain);
}

#include <iostream>

void Board::draw(Render *render) {
    for(auto& sq: squares)
	sq.draw(render);
    boardObj.draw(render);
    for(int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; i++) {
	if(board[i] != CounterType::None) {
	    render->DrawModel(modelType[(int)board[i]],
			      boardTransforms[i].modelMat,
			      boardTransforms[i].normalMat);
	}
	if(highlightX != -1 && highlightY != -1) {
	    if(i == highlightX * BOARD_WIDTH + highlightY) {
		Obj3D h = squares[i];
		h.colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		h.modelMat = glm::translate(h.modelMat, glm::vec3(0.0f, 0.5f, 0.0f));
		h.draw(render);
	    }
	}
    }
}

void Board::set(unsigned int x, unsigned int y, CounterType type) {
    if(x >= BOARD_WIDTH || y >= BOARD_HEIGHT)
	throw std::runtime_error("Board Pos out of range!");
    board[x * BOARD_WIDTH + y] = type;
}


void Board::intersect(glm::vec3 pos) {
    int x = (pos.x + 9.0f) / SPACE.x;
    int y = -(pos.y - 9.0f) / SPACE.z;
    
    if(x < 0 || x > BOARD_WIDTH)
	x = -1;
    if(y < 0 || y > BOARD_HEIGHT)
	y = -1;

    if(abs(pos.x) > 9.0)
	x = -1;
    if(abs(pos.y) > 9.0)
	y = -1;
    
    highlightX = x;
    highlightY = y;
}
