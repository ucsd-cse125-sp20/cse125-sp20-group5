#ifndef _MESSAGE_H_
#define _MESSAGE_H_


#define CRLF "\r\n\r\n"

#define OPCODE_NOP 0
#define OPCODE_PLAYER_MOVE_UP 1
#define OPCODE_PLAYER_MOVE_DOWN 2
#define OPCODE_PLAYER_MOVE_LEFT 3
#define OPCODE_PLAYER_MOVE_RIGHT 4
#define OPCODE_PLAYER_INTERACT 5

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