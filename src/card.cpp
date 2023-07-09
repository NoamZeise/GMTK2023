#include "card.h"

#include <GameHelper/vector_maths.h>

Hand::Hand(Manager* manager) {
    cardTexs[(int)CounterType::None] = manager->render->LoadTexture("textures/card.png");
    cardTexs[(int)CounterType::Forest] = manager->render->LoadTexture("textures/forest_card.png");
    cardTexs[(int)CounterType::Mountain] = manager->render->LoadTexture("textures/mountain_card.png");
    cardTexs[(int)CounterType::Lake] = manager->render->LoadTexture("textures/water_card.png");
    
    Card card;
    card.tex = cardTexs[(int)CounterType::None];
    for(int i = 0; i < HAND_SIZE; i++) {
	Card n = card;
	n.cType = CounterType::None;
	n.update(glm::vec4(100.0f + 110.0f * i,
			  500.0f,
			   card.tex.dim.x,
			   card.tex.dim.y));
	n.baseRect = n.rect;
	cards.push_back(n);
    }
}

void Hand::setLevel(Level level, Board &board) {
    board.reset();
    for(auto &f: level.forest)
	board.set(f.x, f.y, CounterType::Forest);
    for(auto &f: level.lake)
	board.set(f.x, f.y, CounterType::Lake);
    for(auto &f: level.mountain)
	board.set(f.x, f.y, CounterType::Mountain);
    for(int i = 0; i < HAND_SIZE; i++) {
	cards[i].cType = level.hand[i];
	cards[i].selected = false;
	cards[i].update(cards[i].baseRect);
	cards[i].disabled = false;
	cards[i].colour = glm::vec4(1.0f);
	if(cards[i].cType == CounterType::None)
	    cards[i].disabled = true;
    }
    empty = false;
    cardSelected = false;
}

void Hand::update(Manager *manager, Board &board) {
    for(int i = 0; i < cards.size(); i++) {
	if(!cards[i].disabled && gh::contains(manager->correctedMouse, cards[i].rect)) {
	    cards[i].colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	    if(manager->input.m.hold(GLFW_MOUSE_BUTTON_LEFT) && (!cardSelected || cards[i].selected)) {
		cards[i].selected = true;
		cardSelected = true;
		cards[i].colour = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f);
		cards[i].rect.x = manager->correctedMouse.x -
		    cards[i].rect.z / 2.0f;
		cards[i].rect.y = manager->correctedMouse.y -
		    cards[i].rect.w / 3.0f;
		cards[i].depth = 1.0f;
		cards[i].update(cards[i].rect);
	    } else if(cards[i].selected) {
		cardSelected = false;
		cards[i].selected = false;
		cards[i].depth = 0.0f;
		cards[i].update(cards[i].baseRect);
		if(board.set(cards[i].cType)) {
		    cards[i].selected = false;
		    cards[i].disabled = true;
		    empty = checkEmpty();
		}
	    }
	} else if(cards[i].selected) {
		cardSelected = false;
		cards[i].selected = false;
		cards[i].depth = 0.0f;
		cards[i].update(cards[i].baseRect);
	} else {
	    cards[i].colour = glm::vec4(1.0f);
	}
    }
}

void Hand::draw(Render *render) {
    for(auto& card: cards) {
	if(!card.disabled) {
	    render->DrawQuad(cardTexs[(int)card.cType], card.model, card.colour);
	    render->DrawQuad(card.tex, card.model, card.colour);
	}
    }
}

bool Hand::checkEmpty() {
    for(auto& h: cards)
	if(!h.disabled)
	    return false;
    return true;
}

bool Hand::isEmpty() {
    return empty;
}
