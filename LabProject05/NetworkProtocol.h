#pragma once
constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 25;
constexpr int PASSWORD_SIZE = 25;

constexpr int W_WIDTH = 8;
constexpr int W_HEIGHT = 8;

// Packet ID
// Client -> Server
constexpr char CS_LOGIN = 0;
constexpr char CS_MOUSE = 1;
constexpr char CS_KEYINPUT = 2;

// Server -> Client
constexpr char SC_LOGIN_INFO = 11;
constexpr char SC_ADD_PLAYER = 12;
constexpr char SC_REMOVE_PLAYER = 13;
constexpr char SC_MOVE_PLAYER = 14;

#pragma pack (push, 1)

///////////////////////////////////////////////
// Client -> Server
///////////////////////////////////////////////

// �α��ν� ��Ŷ
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	int 	id;
	char	name[NAME_SIZE];
	char	password[PASSWORD_SIZE];
};

// TODO: �ش� ��Ŷ�� ���콺 ������ ������ ĳ���Ͱ� ��� �����̴����� ���� �ٲ� ����
// �̵� �� ��ų ��� Ű��ǲ ��Ŷ
struct CS_MOUSE_PACKET {
	unsigned char size;
	char	type;
	float	Rotation;		// ���콺 ���⿡ ���� ĳ���Ͱ� �ٶ󺸴� ���� 
};

// �̵� �� ��ų ��� Ű��ǲ ��Ŷ
struct CS_KEYINPUT_PACKET {
	unsigned char size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT, 4~ SKILL
};

///////////////////////////////////////////////
// Server -> Client
///////////////////////////////////////////////

// Ŭ���̾�Ʈ ���� ������ ������ �÷��̾� ����
struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char	type;
	int 	id;
	int 	x, y, z;
};

// Ŭ���̾�Ʈ ���� ������ ������ �÷��̾� ����
struct SC_ADD_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int 	id;
	int 	x, y, z;
	char	nickname[NAME_SIZE];
};

// Ŭ���̾�Ʈ ���� ������ ������ �÷��̾� ����
struct SC_REMOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int		id;
};

struct SC_MOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int		id;				// ������ ������ �ִ� ĳ���� ��ȣ
	int 	x, y, z;		// ������ ��ġ
};

#pragma pack (pop)