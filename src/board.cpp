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
    wateredTileModel = render->Load3DModel("models/watered.obj");
    counterBase = modelMat;
}

void Board::reset() {
    won = false;
    wateredTiles.clear();
    for(int x = 0; x < 5; x++) {
	for(int y = 0; y < 5; y++) {
	    this->board[x * BOARD_WIDTH + y] = CounterType::None;
	}
    }
    boardUpdateRequired = true;
}

bool Board::wonBoard() { return won; }

bool Board::updating() { return boardUpdateRequired; }

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
    for(auto &w: wateredTiles) {
	Obj3D h = squares[w.x * BOARD_WIDTH + w.y];
	h.colour = glm::vec4(0.1f, 0.2f, 0.7f, 1.0f);
	h.modelMat = glm::translate(h.modelMat, glm::vec3(0.0f, 0.4f, 0.0f));
	h.draw(render);
    }
}

CounterType Board::getCounter(int x, int y) {
    if(x < 0|| x >= BOARD_WIDTH)
	return CounterType::Mountain;
    if(y < 0 || y >= BOARD_HEIGHT)
	return CounterType::Mountain;
    return board[x * BOARD_WIDTH + y];
}

Surrounding Board::getSurrounding(int x, int y) {
    Surrounding s;
    s.c[(int)Dir::Up] = getCounter(x, y - 1);
    s.pos[(int)Dir::Up] = {x, y-1};
    s.c[(int)Dir::Down] = getCounter(x, y + 1);
    s.pos[(int)Dir::Down] = {x, y+1};
    s.c[(int)Dir::Left] = getCounter(x - 1, y);
    s.pos[(int)Dir::Left] = {x-1, y};
    s.c[(int)Dir::Right] = getCounter(x + 1, y);
    s.pos[(int)Dir::Right] = {x+1, y};
    s.c[(int)Dir::UpLeft] = getCounter(x-1, y - 1);
    s.pos[(int)Dir::UpLeft] = {x-1, y-1};
    s.c[(int)Dir::UpRight] = getCounter(x + 1, y - 1);
    s.pos[(int)Dir::UpRight] = {x + 1, y-1};
    s.c[(int)Dir::DownLeft] = getCounter(x - 1, y+1);
    s.pos[(int)Dir::DownLeft] = {x-1, y+1};
    s.c[(int)Dir::DownRight] = getCounter(x + 1, y+1);
    s.pos[(int)Dir::DownRight] = {x+1, y+1};
    return s;
}

void Board::set(unsigned int x, unsigned int y, CounterType type) {
    if(x >= BOARD_WIDTH || y >= BOARD_HEIGHT)
	throw std::runtime_error("Board Pos out of range!");
    board[x * BOARD_WIDTH + y] = type;
    boardUpdateRequired = true;
}

bool Board::set(CounterType type) {
    if(highlightX == -1 || highlightY == -1 || boardUpdateRequired)
	return false;
    if(board[highlightX * BOARD_WIDTH + highlightY] == CounterType::Mountain)
	return false;
    set(highlightX, highlightY, type);
    return true;
}

#include <iostream>

void Board::stepBoard() {
    boardUpdateRequired = false;
    bool emptyTile = false;
    wateredTiles.clear();
    std::vector<SetBoard> sets;
    
    for(int x = 0; x < BOARD_WIDTH; x++) {
	for(int y = 0; y < BOARD_HEIGHT; y++) {
	    CounterType t = board[x * BOARD_WIDTH + y];
	    if(t == CounterType::None)
		emptyTile = true;
	    if(t == CounterType::Mountain)
		continue;
	    Surrounding s = getSurrounding(x, y);
	    bool hasLake = false;
	    for(int i = 0; i < 4; i++)
		if(s.c[i] == CounterType::Lake)
		    hasLake = true;
	    if(hasLake || t == CounterType::Lake)
		wateredTiles.push_back(BoardPos(x, y));
	    if(sets.size() > 0)
		continue;
	    if(t == CounterType::Forest) {
		if(hasLake)
		    for(int i = 0; i < 4; i++)
			if(s.c[i] == CounterType::None) {
			    set(s.pos[i].x, s.pos[i].y, CounterType::Forest);
			    SetBoard setB;
			    setB.c = CounterType::Forest;
			    setB.pos = s.pos[i];
			    sets.push_back(setB);
			    break;
			}
	    }
	}
    }

    if(!emptyTile)
	won = true;
    if(sets.size() > 0) {
	set(sets[0].pos.x, sets[0].pos.y, sets[0].c);
    }
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
