#include "level.h"

#include <GameHelper/vector_maths.h>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

char* loadTextFile(std::string filename)
{
	std::ifstream in(filename, std::ios::binary | std::ios::ate);
	if (!in.is_open())
		throw std::runtime_error("failed to load text file at " + filename);

	size_t fileSize = (size_t)in.tellg();
	char* text = new char[fileSize + 1]; //+1 for null terminator

	in.seekg(0);
	in.read(text, fileSize);
	in.close();
	text[fileSize] = '\0';
	return text;
}


//from: https://stackoverflow.com/questions/13172158/c-split-string-by-line
std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

std::vector<BoardPos> getCoords(std::string line) {
    std::vector<BoardPos> board;
    std::vector<std::string> coords = split_string(line, " ");
    for(int i = 1; i < coords.size(); i++) {
	//std::cout << coords[i] << std::endl;
	//std::cout << "size: " << coords[i].size() << std::endl;
	
	if(coords[i].size() >= 3) {
	    int x = coords[i][0] - '0';
	    int y = coords[i][2] - '0';
	    board.push_back(BoardPos(x, y));
	}
    }
    return board;
}

std::vector<Level> loadLevels() {
    std::vector<Level> levels;
    int i = 0;
    std::string name = "levels/";
    while(gh::exists("levels/" + std::to_string(i) + ".txt")) {
	char* textData = loadTextFile("levels/" + std::to_string(i++) + ".txt");
	Level l;
	auto lines = split_string(std::string(textData), "\n");
	delete[] textData;
	if(lines.size() != 4) {
	    std::cerr << " Wrong number of liens in level file\n";
	    continue;
	}
	std::string F = lines[0];
	std::string L = lines[1];
	std::string M = lines[2];
	std::string H = lines[3];

	l.forest = getCoords(F);
	l.lake = getCoords(L);
	l.mountain = getCoords(M);

	std::vector<std::string> hand = split_string(H, " ");
	int i = i;
	for(i = 1 ; i < hand.size() && i <= HAND_SIZE; i++) {
	    if(hand[i] == "F") {
		l.hand[i - 1] = CounterType::Forest;
	    }
	    if(hand[i] == "L") {
		l.hand[i - 1] = CounterType::Lake;
	    }
	} for(;i <= HAND_SIZE; i++) {
	    l.hand[i - 1] = CounterType::None;
	}
	
	levels.push_back(l);
    }
    return levels;
}
