#ifndef _MESSAGE_H_
#define _MESSAGE_H_

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

// Chat message related op code
#define OPCODE_PLAYER_CHAT_0 100
#define OPCODE_PLAYER_CHAT_1 101
#define OPCODE_PLAYER_CHAT_2 102
#define OPCODE_PLAYER_CHAT_3 103
#define OPCODE_PLAYER_CHAT_4 104
#define OPCODE_PLAYER_CHAT_5 105
#define OPCODE_PLAYER_CHAT_6 106
#define OPCODE_PLAYER_CHAT_7 107
#define OPCODE_PLAYER_CHAT_8 108
#define OPCODE_PLAYER_CHAT_9 109
#define OPCODE_PLAYER_CHAT_SECRET 110
#define OPCODE_PLAYER_CHAT_ZHU 111
#define OPCODE_PLAYER_CHAT_SUGUMAR 112
#define OPCODE_PLAYER_CHAT_JOYAAN  113
#define OPCODE_PLAYER_CHAT_MINGQI 114
#define OPCODE_PLAYER_CHAT_YANG 115


class Message {
public:
	Message() : opCode(OPCODE_NOP), levelId(0) {}
	Message(int op_code) : opCode(op_code), levelId(0) {}
	Message(int op_code, int level_id) : opCode(op_code), levelId(level_id) {}
	Message(int op_code, unsigned int num_players, bool fake) : opCode(op_code), numPlayers(num_players) {}

	template <class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & opCode;
		ar & levelId;
		ar & numPlayers;
	}

	int getOpCode() {
		return opCode;
	}

	int getLevelId() {
		return levelId;
	}

	int getNumPlayers() {
		return numPlayers;
	}

private:
	int opCode;
	int levelId; // used in OPCODE_LEVEL_SELECT
	unsigned int numPlayers; // num players in lobby
};

#endif