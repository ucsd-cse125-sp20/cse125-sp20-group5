#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <boost/serialization/string.hpp>

#define CRLF "\r\n\r\n"

#define OPCODE_NOP 0
#define OPCODE_PLAYER_MOVE_UP 1
#define OPCODE_PLAYER_MOVE_DOWN 2
#define OPCODE_PLAYER_MOVE_LEFT 3
#define OPCODE_PLAYER_MOVE_RIGHT 4
#define OPCODE_PLAYER_MOVE_UPPER_LEFT 5
#define OPCODE_PLAYER_MOVE_UPPER_RIGHT 6
#define OPCODE_PLAYER_MOVE_LOWER_LEFT 7
#define OPCODE_PLAYER_MOVE_LOWER_RIGHT 8
#define OPCODE_PLAYER_MOVE_FREEZE 9
#define OPCODE_PLAYER_INTERACT 10
#define OPCODE_PLAYER_START_ACTION 12
#define OPCODE_PLAYER_END_ACTION 13

// Level select
#define OPCODE_LEVEL_SELECT 14

// Has game begun or not
#define OPCODE_GAME_STARTED 15
#define OPCODE_GAME_NOT_STARTED 16

class Message {
public:
	Message() : opCode(OPCODE_NOP), levelName(std::string()) {}
	Message(int op_code) : opCode(op_code), levelName(std::string()) {}
	Message(int op_code, std::string level_name) : opCode(op_code), levelName(level_name) {}
	
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & opCode;
		ar & levelName;
	}

	int getOpCode() {
		return opCode;
	}

	std::string getLevelName() {
		return levelName;
	}

private:
	int opCode;
	std::string levelName; // used in OPCODE_LEVEL_SELECT
};

#endif