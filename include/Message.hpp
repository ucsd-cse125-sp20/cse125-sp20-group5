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
#define OPCODE_PLAYER_ACTION 11

class Message {
public:
	Message() : opCode(OPCODE_NOP){}
	Message(int op_code) : opCode(op_code) {}
	
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & opCode;
	}

	int getOpCode() {
		return opCode;
	}

private:
	int opCode;
};

#endif