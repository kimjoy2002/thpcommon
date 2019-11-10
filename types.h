#ifndef H_TYPES___
#define H_TYPES___

#include "TCHAR.h"
#include <string>
#include <d3d9.h>
#include <d3dx9math.h>

typedef struct ground_info{
	int x;
	int y;
	bool ground;
} type_ground;

typedef struct{
	int flg;
	int left;
	int top;
	int right;
	int bottom;
	WCHAR	text[MAX_GAME_ITEM_TEXT];
	BYTE cost;
} TITEM_RESOURCE_INFO;

// �X�e?�W���W
typedef struct TSTAGE_POS
{
	short x;
	short y;
} TStagePos, *PStagePos;

typedef struct{
    HWND hwnd;
    RECT rc;
	SOCKET sockListener;
    BOOL thread_end;
} NETWORK, *PNETWORK;

// �X�e?�W�p�X�N���v�g���
typedef struct {
	WCHAR path[_MAX_PATH*2+1];
	SIZE	size;
} TSCR_IMG_INFO;

typedef struct type_base_scr_info{
	int scr_index;										// �X�N���v�g�ԍ�
	int res_index;									// ��??�X�ԍ�
	int ID;												// ID
	LPDIRECT3DTEXTURE9	pTexture;		// �e�N�X?��
	char	md5[MD5_LENGTH+1];				// �n�b�V���l
	BOOL flg;											// �g�p�t���O
	int se_count;										// SE�̐�
	BOOL send;										// ���M
	WCHAR scr_path[_MAX_PATH*2+1];	// �X�N���v�g�̃p�X
} TBASE_SCR_INFO, *PBASE_SCR_INFO;

// �X�e?�W�e�p���
typedef struct{
	RECT rec_blt_tex;					// ��??�X�͈�
	int blt_atk;
	int hit_range;						// �����蔼�a
	int bomb_range;					// �������a
	BOOL ephemeris;					// �O��
} TSTAGE_BULLET_INFO;

typedef struct type_stage_scr_info : public type_base_scr_info{
//	int scr_index;
//	int res_index;
//	int ID;
	WCHAR name[MAX_STAGE_NAME+1];
	TSCR_IMG_INFO thumnail;						// �T?�l�C���摜
	TSCR_IMG_INFO stage;							// �X�e?�W�摜
	TSCR_IMG_INFO bg;								// �w�i�摜
	TSTAGE_BULLET_INFO blt_info[MAX_STAGE_BULLET_TYPE];		// �e���
	int blt_type_count;											// �e��ސ�
	WCHAR bgm[_MAX_PATH*2+1];				// BGM�p����
} TSTAGE_SCR_INFO, *PSTAGE_SCR_INFO;

// �L�����e�p���
typedef struct TCharaBulletInfo{
	RECT rec_blt_icon;				// �L�����Z���p�A�C�R���̃�??�X�͈�
	RECT rec_blt_tex;					// ��??�X�͈�
	int blt_atk;
	int blt_delay;
	int hit_range;						// �����蔼�a
	int bomb_range;					// �������a
	BOOL ephemeris;			// �O���̓o?
} TCHARA_BULLET_INFO;

// �L�����e�p���
typedef struct TCharaSpellcardInfo : public TCharaBulletInfo{	
	WCHAR name [MAX_SPELLCARD_NAME+1];
	int exp;
	int max_exp;
} TCHARA_SPELLCARD_INFO;

// �L�����p�X�N���v�g���
typedef struct type_chara_scr_info : public type_base_scr_info{
//	int scr_index;
//	int res_index;													// ��??�XIndex
//	int ID;
	WCHAR name[MAX_CHARACTER_NAME+1];		// �L������
	WCHAR tex_path[_MAX_PATH*2+1];				// �L�����p�e�N�X?���̃p�X
	int tex_chr_num;												// �L�������s�A�j���e�N�X?����
	RECT rec_tex_chr;												// �L�����e�N�X?���͈�
	int tex_trg_num;												// �e���˃A�j���e�N�X?����
	RECT rec_tex_trg;												// �e���˃A�j���e�N�X?���͈�
	RECT rec_sel_icon;											// �L�����Z���p�A�C�R���̃�??�X�͈�
	int delay;															// �f�B���C�l
	int move;															// �ړ��l
	int max_hp;														// HP�ő�l
	int angle_range_min;											// �e���ˊp�x�ŏ��͈�
	int angle_range_max;										// �e���ˊp�x�ő�͈�
	BYTE draw_w;													// �L��������
	BYTE draw_h;													// �L�����c��
	char shot_h;														// ���˂̍���
	TCHARA_BULLET_INFO blt_info[MAX_CHARA_BULLET_TYPE];		// �e���
	TCHARA_SPELLCARD_INFO sc_info;					// �X�y���J?�h���
	int blt_type_count;											// �e��ސ�
	int blt_sel_count;												// �I���ł���e�̎�ސ�
} TCHARA_SCR_INFO, *PCHARA_SCR_INFO;

// �F�،��ʔԍ���
enum E_TYPE_AUTH_RESULT : BYTE {
	AUTH_RESULT_SUCCESS=0,			// ����
	AUTH_RESULT_INVALID_PWD,		// �p�X��?�h�s��v
	AUTH_RESULT_NO_UNIQUE_USER,		// �g�p�ς݃�?�U��
	AUTH_RESULT_INVALID_USER_NAME,	// ��?�U?�����s��
	AUTH_RESULT_GAME_LOAD,	// �Q??���͓���Ȃ�
	AUTH_RESULT_GAME_PHASE,	// �Q??���͓���Ȃ�
	AUTH_RESULT_GAME_INVALID,	// �Q??���͓���Ȃ�
	AUTH_RESULT_TIMEOUT,			// ?�C?�A�E�g
	AUTH_RESULT_INVALID_HASH,	// �n�b�V���l������Ȃ�
	AUTH_RESULT_FILESEND_ERROR,	// �t?�C�����M���s
	AUTH_RESULT_FILESEND_END,	// �t?�C�����M�I��
	AUTH_RESULT_FILESEND_DISABLE,
	AUTH_RESULT_INVALID_SCR_CHARA_COUNT,	// �L�����̃X�N���v�g������Ȃ�
	AUTH_RESULT_INVALID_SCR_STAGE_COUNT,	// �L�����̃X�N���v�g������Ȃ�
	AUTH_RESULT_INVALID_SCR_CHARA_NOT_FOUND,	// �L�����̃X�N���v�g������Ȃ�
	AUTH_RESULT_INVALID_SCR_STAGE_NOT_FOUND,	// �L�����̃X�N���v�g������Ȃ�
};

#define DEF_CHARA_DEAD_KILL		(1)
#define DEF_CHARA_DEAD_DROP		(2)
#define DEF_CHARA_DEAD_CLOSE		(3)

enum E_TYPE_CHARA_DEAD : BYTE {
	CHARA_DEAD_KILL = DEF_CHARA_DEAD_KILL,
	CHARA_DEAD_DROP = DEF_CHARA_DEAD_DROP,
	CHARA_DEAD_CLOSE = DEF_CHARA_DEAD_CLOSE,
};

enum E_TYPE_USER_DIRECTION: BYTE {
	USER_DIRECTION_LEFT=0,
	USER_DIRECTION_RIGHT,
	USER_DIRECTION_MAX,
};

// BYTE DATA
enum E_TYPE_CONN_STATE : BYTE {
	CONN_STATE_EMPTY,		// �F�ؑO
	CONN_STATE_DESTROY,		// �ؒf
	CONN_STATE_AUTHED,		// �F�؍ς�
};

#define PROC_FLG_OBJ_REMOVE						(0x0001)
#define PROC_FLG_OBJ_UPDATE_VEC				(0x0002)
#define PROC_FLG_OBJ_UPDATE_POS				(0x0004)
#define PROC_FLG_OBJ_UPDATE_STATE			(0x0008)
#define PROC_FLG_OBJ_SET_EXDATA				(0x0010)
#define PROC_FLG_OBJ_UPDATE_ANGLE			(0x0020)
#define PROC_FLG_OBJ_UPDATE_TYPE				(0x0040)
#define PROC_FLG_OBJ_SET_TEX_INDEX			(0x0080)

#define PROC_FLG_OBJ_WAIT_SERVER_MSG		(PROC_FLG_OBJ_REMOVE|PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS|PROC_FLG_OBJ_UPDATE_STATE|PROC_FLG_OBJ_UPDATE_TYPE)

#define OBJ_STATE_NONE								(0x08000000)
#define OBJ_STATE_ROOM								(0x10000000)
#define OBJ_STATE_LOAD								(0x20000000)
#define OBJ_STATE_GAME								(0x40000000)
#define OBJ_STATE_RESULT							(0x80000000)

// �N���C�A���g��on_DrawChara��Lua�ɓn�����̒ʏ�e���X�y���̔��f�p�t���O
#define CHARA_DRAW_TRIGGER_FLG_SPELL	0x00100000

#define OBJ_STATE_MAIN_WAIT_FLG			(0x00000001)
#define OBJ_STATE_MAIN_ACTIVE_FLG			(0x00000002)
#define OBJ_STATE_MAIN_DEAD_FLG			(0x00000004)
#define OBJ_STATE_MAIN_DROP_FLG			(0x00000008)
#define OBJ_STATE_MAIN_PAUSE_FLG			(0x00000010)
#define OBJ_STATE_MAIN_SHOTPOWER_FLG	(0x00000020)
#define OBJ_STATE_MAIN_TRIGGER_BULLET_FLG	(0x00000040)
#define OBJ_STATE_MAIN_TRIGGER_SPELL_FLG	(0x00000080)
#define OBJ_STATE_MAIN_SHOOTING_FLG	(0x00000100)
#define OBJ_STATE_MAIN_SHOT_FLG			(0x00000200)
#define OBJ_STATE_MAIN_GALLERY_FLG		(0x00000400)
#define OBJ_STATE_MAIN_MASK					(0x000007FF)

#define OBJ_STATE_MAIN_NOLIVE_FLG			(OBJ_STATE_MAIN_DEAD_FLG|OBJ_STATE_MAIN_DROP_FLG)
#define OBJ_STATE_MAIN_NOACT_FLG			(OBJ_STATE_MAIN_DEAD_FLG|OBJ_STATE_MAIN_WAIT_FLG)

#define DEF_STATE_EMPTY							OBJ_STATE_EMPTY
#define DEF_STATE_WAIT								OBJ_STATE_MAIN_WAIT_FLG
#define DEF_STATE_ACTIVE							OBJ_STATE_MAIN_ACTIVE_FLG
#define DEF_STATE_DEAD								OBJ_STATE_MAIN_DEAD_FLG
#define DEF_STATE_DROP								OBJ_STATE_MAIN_DROP_FLG
#define DEF_STATE_SHOTPOWER					OBJ_STATE_MAIN_SHOTPOWER_FLG
#define DEF_STATE_TRIGGER_BULLET			OBJ_STATE_MAIN_TRIGGER_BULLET_FLG
#define DEF_STATE_TRIGGER_SPELL				OBJ_STATE_MAIN_TRIGGER_SPELL_FLG
#define DEF_STATE_SHOT								OBJ_STATE_MAIN_SHOT_FLG
#define DEF_STATE_GALLERY						OBJ_STATE_MAIN_GALLERY_FLG

#define OBJ_STATE_ACT_FLG						(OBJ_STATE_MAIN_ACTIVE_FLG|OBJ_STATE_MAIN_SHOTPOWER_FLG|OBJ_STATE_MAIN_TRIGGER_BULLET_FLG|OBJ_STATE_MAIN_TRIGGER_SPELL_FLG|OBJ_STATE_MAIN_SHOOTING_FLG|OBJ_STATE_MAIN_SHOT_FLG)

// BYTE DATA
enum E_TYPE_OBJ_STATE : DWORD {	// userState
	OBJ_STATE_EMPTY								=	(OBJ_STATE_NONE|0),
	OBJ_STATE_ROOM_READY						=	(OBJ_STATE_ROOM|0),
	OBJ_STATE_ROOM_MOVE						=	(OBJ_STATE_ROOM|1),
	OBJ_STATE_ROOM_READY_OK				=	(OBJ_STATE_ROOM|2),

	OBJ_STATE_LOADING							=	(OBJ_STATE_LOAD|0),
	OBJ_STATE_LOADCOMPLETE					=	(OBJ_STATE_LOAD|1),

	OBJ_STATE_MAIN_WAIT						= (OBJ_STATE_GAME|OBJ_STATE_MAIN_WAIT_FLG),
	OBJ_STATE_MAIN_ACTIVE						= (OBJ_STATE_GAME|OBJ_STATE_MAIN_ACTIVE_FLG),
	OBJ_STATE_MAIN_DEAD						= (OBJ_STATE_GAME|OBJ_STATE_MAIN_DEAD_FLG),
	OBJ_STATE_MAIN_DROP						= (OBJ_STATE_GAME|OBJ_STATE_MAIN_DROP_FLG),

	OBJ_STATE_MAIN_PAUSE						= (OBJ_STATE_GAME|OBJ_STATE_MAIN_PAUSE_FLG),
	OBJ_STATE_MAIN_SHOTPOWER				= (OBJ_STATE_GAME|OBJ_STATE_MAIN_SHOTPOWER_FLG),
	OBJ_STATE_MAIN_TRIGGER_BULLET		= (OBJ_STATE_GAME|OBJ_STATE_MAIN_TRIGGER_BULLET_FLG),
	OBJ_STATE_MAIN_TRIGGER_SPELL		= (OBJ_STATE_GAME|OBJ_STATE_MAIN_TRIGGER_SPELL_FLG),
	OBJ_STATE_MAIN_SHOOTING				= (OBJ_STATE_GAME|OBJ_STATE_MAIN_SHOOTING_FLG),
	OBJ_STATE_MAIN_SHOT						= (OBJ_STATE_GAME|OBJ_STATE_MAIN_SHOT_FLG),

	OBJ_STATE_MAIN_GALLERY					= (OBJ_STATE_GAME|OBJ_STATE_MAIN_GALLERY_FLG),

	OBJ_STATE_RESULT_CONFIRMING			= (OBJ_STATE_RESULT|0),
	OBJ_STATE_RESULT_CONFIRMED				= (OBJ_STATE_RESULT|1),

};

#define CHARA_STATE_COUNT					(20)					// ��Ԃ̐�
#define CHARA_STATE_STEALTH_INDEX	(0)					// �X�e���X
#define CHARA_STATE_BLIND_INDEX		(1)					// �Â��Ȃ�
#define CHARA_STATE_PAIN_INDEX			(2)					/// ��
#define CHARA_STATE_SHIELD_INDEX		(3)					/// �V?���h
#define CHARA_STATE_POWER_INDEX		(4)					/// �p��?�A�b�v
#define CHARA_STATE_UPMOVE_INDEX		(5)					// �ړ�����
#define CHARA_STATE_NOMOVE_INDEX		(6)					/// �ړ��s��
#define CHARA_STATE_NOANGLE_INDEX	(7)					/// �p�x�ύX�s��
#define CHARA_STATE_DOUBLE_INDEX		(8)					// ��A���g�p��
#define CHARA_STATE_REVERSE_INDEX	(9)					/// �t��
#define CHARA_STATE_ITEM_REVERSE_INDEX	(10)			/// �t���e���ˑO���
#define CHARA_STATE_ITEM_BLIND_INDEX	(11)				/// �Â��Ȃ�e���ˏ��
#define CHARA_STATE_ITEM_REPAIRBLT_INDEX	(12)		/// �񕜂���e���ˏ��
#define CHARA_STATE_ITEM_TELEPORT_INDEX	(13)			/// ���e?�Ɉړ�����e���ˏ��
#define CHARA_STATE_ITEM_DRAIN_INDEX	(14)				/// HP�z������e���ˏ��
#define CHARA_STATE_ITEM_FETCH_INDEX	(15)				/// �����񂹂�e���ˏ��
#define CHARA_STATE_ITEM_EXCHANGE_INDEX	(16)		/// �ʒu����ւ��e���ˏ��
#define CHARA_STATE_ITEM_NOANGLE_INDEX	(17)			/// �p�x�ύX�s�e���ˏ��
#define CHARA_STATE_ITEM_NOMOVE_INDEX	(18)			/// �ړ��s�e���ˏ��
#define CHARA_STATE_ITEM_STEAL_INDEX	(19)			/// �ړ��s�e���ˏ��

#define CHARA_STATE_STEALTH				CHARA_STATE_STEALTH_INDEX
#define CHARA_STATE_BLIND					CHARA_STATE_BLIND_INDEX
#define CHARA_STATE_PAIN						CHARA_STATE_PAIN_INDEX
#define CHARA_STATE_SHIELD					CHARA_STATE_SHIELD_INDEX
#define CHARA_STATE_POWER					CHARA_STATE_POWER_INDEX
#define CHARA_STATE_UPMOVE				CHARA_STATE_UPMOVE_INDEX
#define CHARA_STATE_NOMOVE				CHARA_STATE_NOMOVE_INDEX
#define CHARA_STATE_NOANGLE				CHARA_STATE_NOANGLE_INDEX
#define CHARA_STATE_DOUBLE					CHARA_STATE_DOUBLE_INDEX
#define CHARA_STATE_REVERSE				CHARA_STATE_REVERSE_INDEX
#define CHARA_STATE_ITEM_REVERSE		CHARA_STATE_ITEM_REVERSE_INDEX
#define CHARA_STATE_ITEM_BLIND			CHARA_STATE_ITEM_BLIND_INDEX
#define CHARA_STATE_ITEM_REPAIRBLT	CHARA_STATE_ITEM_REPAIRBLT_INDEX
#define CHARA_STATE_ITEM_TELEPORT	CHARA_STATE_ITEM_TELEPORT_INDEX
#define CHARA_STATE_ITEM_DRAIN			CHARA_STATE_ITEM_DRAIN_INDEX
#define CHARA_STATE_ITEM_FETCH			CHARA_STATE_ITEM_FETCH_INDEX
#define CHARA_STATE_ITEM_EXCHANGE	CHARA_STATE_ITEM_EXCHANGE_INDEX
#define CHARA_STATE_ITEM_NOANGLE		CHARA_STATE_ITEM_NOANGLE_INDEX
#define CHARA_STATE_ITEM_NOMOVE		CHARA_STATE_ITEM_NOMOVE_INDEX
#define CHARA_STATE_ITEM_STEAL			CHARA_STATE_ITEM_STEAL_INDEX

/* �p�P�b�g�w�b?��? */
enum E_TYPE_PACKET_HEADER : BYTE{  // BYTE DATA
		PK_SYN=0,								//
		PK_ACK=1,								//
		PK_USER_AUTH=2,					// ���F�F�؃p�P�b�g
		PK_REQ_HASH=3,						// ���F�n�b�V���l�v���p�P�b�g
		PK_RET_HASH=4,						// ���F�n�b�V���l�ԓ��p�P�b�g
		PK_USER_CHAT=10,					// ���F?���b�g���b�Z?�W�p�P�b�g
		PK_USER_ROOMINFO=11,			// ���F��??��?�U?���
		PK_USER_LOAD=12,					// ��?�h
		PK_USER_MAININFO=13,			// ���F�Q??��?�U?���
		PK_USER_CONFIRMED,
		PK_REQ_TEAM_RAND,					// ???����??��
		PK_REQ_LOC=20,						// �v���F�ʒu
		PK_CMD_MV=30,						// �R?���h�F�ړ�
		PK_CMD_ROOM_CHARA_SEL,		// �R?���h�F�L�����I��
		PK_CMD_ROOM_READY,				// �R?���h�F?����ԕύX
		PK_CMD_ROOM_RULE,				// �R?���h�F��?���ύX
		PK_CMD_ROOM_RULE_TURN_LIMIT,		// �R?���h�F����??�����ύX
		PK_CMD_ROOM_RULE_ACT_TIME_LIMIT,		// �R?���h�F�������ԕύX
		PK_CMD_ROOM_MV,					// �R?���h�F�ړ�
		PK_CMD_MAIN_FLIP,			// �R?���h�F�X�V�X�V
		PK_CMD_ROOM_ITEM_SEL,		// �R?���h�F�A�C�e?�I��
		PK_CMD_ROOM_TEAM_COUNT,	// �R?���h�F???���ύX
		PK_CMD_ROOM_STAGE_SEL,		// �R?���h�F�X�e?�W�I��

		PK_REQ_LOAD_COMPLETE = 50,			// ��?�h�����v��
		PK_CMD_LOAD_COMPLETE,		// �R?���h�F��?�h����

		PK_CMD_MAIN_MV,					// �R?���h�F�ړ�
		PK_CMD_MAIN_SHOTPOWER,		// �R?���h�F�V���b�g�p��?�J�n
		PK_CMD_MAIN_TRIGGER,			// �R?���h�F�g���K?
		PK_REQ_TRIGGER_END,				// �g���K?�I��
		PK_USER_MAIN_TRIGGER_END,		// �R?���h�F�g���K?�I��
		PK_CMD_MAIN_SHOT,				// �R?���h�F����
		PK_CMD_MAIN_PASS,				// �R?���h�F�p�X
		PK_CMD_MAIN_ITEM,				// �R?���h�F�A�C�e?
		PK_CMD_MAIN_TURN_PASS,		// �R?���h�F��??���p�X

		PK_CMD_CONFIRM,

		PK_OBJ_UPDATE_ACT,
		PK_OBJ_REMOVE,
		PK_USER_DISCON = 100,			// ���F�ؒf
		PK_REQ_FILE_DATA = 200,					// �t?�C���v��
		PK_REQ_FILE_HASH = 201,					// �t?�C���v��
		PK_USER_FILEINFO = 202,					// �t?�C�����M
		PK_CHK_FILE_HASH = 203,					// �t?�C���v��
		PK_NOOP=0xFF,						//
};

enum E_TYPE_PACKET_FILEINFO_HEADER:BYTE{
	PK_USER_FILEINFO_OPEN,					// �t?�C���ʒu�A�n�b�V���l�A�T�C�Y�Ȃ�
	PK_USER_FILEINFO_SEND,					// �t?�C���f??���M
	PK_USER_FILEINFO_CLOSE,						// �t?�C�����M�I��	
};

/* �p�P�b�g�F��??��?�U?���w�b?��? */
enum E_TYPE_PACKET_MAININFO_HEADER : BYTE{
	PK_USER_MAININFO_START,					// �X??�g
	PK_USER_MAININFO_ACTIVE,				// �A�N�e�B�u���(���ԑ҂��Ȃ�)
	PK_USER_MAININFO_ITEM_USE,			// �A�C�e?
	PK_USER_MAININFO_CHARA_MV,			// �ړ��p�P�b�g���
	PK_USER_MAININFO_FLIP,			// �R?���h�F�X�V�X�V
	PK_USER_MAININFO_UPDATEPOS,			// �R?���h�F�X�V
	PK_USER_MAININFO_CHARA_DEAD_KILL,		// �L����?�S�p�P�b�g���
	PK_USER_MAININFO_CHARA_DEAD_DROP,		// �L����?�S�p�P�b�g���
	PK_USER_MAININFO_CHARA_DEAD_CLOSE,	// �L����?�S�p�P�b�g���
	PK_USER_MAININFO_CHARA_UPDATE_STATUS,	// �L�����̃X�e??�X��ύX
	PK_USER_MAININFO_CHARA_UPDATE_STATE,	// ��ԍX�V
	PK_USER_MAININFO_WIND,						// �������ݒ�
	PK_USER_MAININFO_ADD_ITEM,				// �A�C�e?�ǉ�

	PK_USER_MAININFO_BULLET_TRIGGER,	// ���ˉ��o�p�P�b�g���
	PK_REQ_MAININFO_BULLET_SHOT,		// �e���˃p�P�b�g�v��
	PK_REJ_MAININFO_BULLET_SHOT,		// �e���˃p�P�b�g����
	PK_REJ_MAININFO_TURN_PASS,			// ??���p�X�p�P�b�g����
	PK_USER_MAININFO_BULLET_SHOT,		// �e���˃p�P�b�g���
	PK_USER_MAININFO_BULLET_MV,			// �e�ʒu�p�P�b�g���
	PK_USER_MAININFO_BULLET_VEC,		// �e�����p�P�b�g���
	PK_USER_MAININFO_BULLET_UPDATE_TYPE,			// �e?�C�v�X�V�p�P�b�g���
	PK_USER_MAININFO_OBJECT_MV,			// �e�ړ��p�P�b�g���
	PK_USER_MAININFO_OBJECT_RM,			// �e�폜�p�P�b�g���
	PK_USER_MAININFO_OBJECT_BOMB,		// �e�����p�P�b�g���
	PK_USER_MAININFO_PASTE_IMAGE,		// �X�e?�W�ɉ摜?��t���p�P�b�g���
	PK_USER_MAININFO_OBJECT_UPDATE_STATE,		// �e��ԍX�V�p�P�b�g���
	PK_USER_MAININFO_TURNEND,			// ??���G���h
	PK_USER_MAININFO_GAMEEND,			// �Q??�G���h
};

/* �p�P�b�g�F?���b�g�w�b?��? */
enum E_TYPE_PACKET_CHAT_HEADER : BYTE{
	PK_USER_CHAT_WIS,							// �l��
	PK_USER_CHAT_ALL=MAXLOGINUSER,		// �S����
	PK_USER_CHAT_TEAM,							// ???��
	PK_USER_CHAT_SERVER_INFO,				// �T?�o������
	PK_USER_CHAT_SERVER_WARNING,		// �T?�o����x��

	PK_USER_CHAT_NONE = 0xFF,
};

/* �p�P�b�g�F��??��?�U?���w�b?��? */
enum E_TYPE_PACKET_ROOMINFO_HEADER : BYTE{
	PK_USER_ROOMINFO_IN,						// ��??�C��
	PK_USER_ROOMINFO_NEW,					// ��??�C��
	PK_USER_ROOMINFO_MASTER,				// ?�X??�ݒ�
	PK_USER_ROOMINFO_READY,				// ?����ԕύX
	PK_USER_ROOMINFO_RULE,					// ��?���ύX���
	PK_USER_ROOMINFO_RULE_TURN_LIMIT,	// ����??�����ύX���
	PK_USER_ROOMINFO_RULE_ACT_TIME_LIMIT,	// �������ԕύX���
	PK_USER_ROOMINFO_CHARA_SEL,			// �L�����I�����
	PK_USER_ROOMINFO_MOVE,					// �ړ����
	PK_USER_ROOMINFO_TEAM_RAND,				// ???����??��
	PK_USER_ROOMINFO_ITEM_SEL,			// �A�C�e?�I��
	PK_USER_ROOMINFO_TEAM_COUNT,		// ???�ݒ�ύX
	PK_USER_ROOMINFO_STAGE_SEL,			// �X�e?�W�I��
	PK_USER_ROOMINFO_RE_ENTER,			// �ē������

//	PK_ROOMINFO_OUT,						// PK_USER_DISCON�g�p�̂��ߖ��g�p
};

// ��?���F�Q??��?��
enum E_TYPE_GAME_GENERAL_RULE : BYTE{
	GAME_GENERAL_RULE_ITEM_ADVENT = 0,				// �A�C�e?�o���L��
	GAME_GENERAL_RULE_STAGE_STATE = 1,				// �X�e?�W��ԕύX

	GAME_GENERAL_RULE_MAX
};

// �Q??�S�̂̏��
enum E_STATE_GAME_PHASE : BYTE{
	GAME_PHASE_ROOM,			// ?������
	GAME_PHASE_LOAD,			// ��?�f�B���O
	GAME_PHASE_MAIN,			// ���C��
	GAME_PHASE_RESULT,		// ���ʉ��
	GAME_PHASE_RETURN,		// �߂�

	GAME_PHASE_INVALID,		// INVALID
};

enum E_STATE_GAME_MAIN_PHASE:WORD{
	GAME_MAIN_PHASE_NONE				= 0x0001,
	GAME_MAIN_PHASE_ACT				= 0x0002,	// ??��
	GAME_MAIN_PHASE_SHOTPOWER	= 0x0004,	// �e�̃p��?�𒙂߂�Ƃ���
	GAME_MAIN_PHASE_TRIGGER		= 0x0008,	// �e�����Ƃ���
	GAME_MAIN_PHASE_SHOT_REQ		= 0x0010,	// �e����Ԃ̂�҂�
	GAME_MAIN_PHASE_SHOOTING		= 0x0020,	// �e�������Ă鏊
	GAME_MAIN_PHASE_SHOT				= 0x0040,	// �e�����ł鏊
	GAME_MAIN_PHASE_DOUBLE			= 0x0080,	// �񔭖ڂ����ł鏊
	GAME_MAIN_PHASE_CHECK			= 0x0100,	// �e����яI������Ƃ����?�F�b�N
	GAME_MAIN_PHASE_SYNC				= 0x0200,	// ���ւ�PHASE�̓��������
	GAME_MAIN_PHASE_TURNEND		= 0x0400,	// ??���G���h
	GAME_MAIN_PHASE_RETURN			= 0x0800,	// ��??���֑J��
};

#define CAN_MODIFY_SHOT_ANGLE_PHASE (GAME_MAIN_PHASE_ACT|GAME_MAIN_PHASE_SHOTPOWER|GAME_MAIN_PHASE_NONE|GAME_MAIN_PHASE_CHECK|GAME_MAIN_PHASE_SYNC|GAME_MAIN_PHASE_TURNEND|GAME_MAIN_PHASE_RETURN)

#define OBJ_TYPE_BLT					(0x80)
#define OBJ_TYPE_ITEM				(0x40)
#define OBJ_TYPE_NONECHARA		(0xC0)

#define OBJ_TYPE_SOLID				(0x01)	// �ʏ�̒e
#define OBJ_TYPE_GAS					(0x02)	// �X�e?�W�Ɠ����蔻�肪�Ȃ��e
#define OBJ_TYPE_LIQUID				(0x04)	// �X�e?�W�𔇂��e
#define OBJ_TYPE_STAGE				(0x08)	// ���ɉe���̖���GAS
#define OBJ_TYPE_TACTIC			(0x10)	// ���ɂ������蔻�肪�����e
																// ??���G���h��??���X??�g�C�x���g�̂ݔ���(TACTIC=��p)
#define OBJ_TYPE_SLIDE				(0x20)	// �n�ʂ�����e
#define OBJ_TYPE_MASK				(0x3F)

enum E_OBJ_TYPE:BYTE
{
	OBJ_TYPE_CHARA			= 0x00,
	OBJ_TYPE_BLT_SOLID	= (OBJ_TYPE_BLT|OBJ_TYPE_SOLID),
	OBJ_TYPE_BLT_GAS		= (OBJ_TYPE_BLT|OBJ_TYPE_GAS),
	OBJ_TYPE_BLT_LIQUID	= (OBJ_TYPE_BLT|OBJ_TYPE_LIQUID),
	OBJ_TYPE_BLT_STAGE	= (OBJ_TYPE_BLT|OBJ_TYPE_STAGE),
	OBJ_TYPE_BLT_TACTIC	= (OBJ_TYPE_BLT|OBJ_TYPE_TACTIC),
	OBJ_TYPE_BLT_SLIDE	= (OBJ_TYPE_BLT|OBJ_TYPE_SLIDE),

	OBJ_TYPE_ITEM_SOLID	= (OBJ_TYPE_ITEM|OBJ_TYPE_SOLID),
	OBJ_TYPE_ITEM_GAS	= (OBJ_TYPE_ITEM|OBJ_TYPE_GAS),
	OBJ_TYPE_ITEM_LIQUID= (OBJ_TYPE_ITEM|OBJ_TYPE_LIQUID),
	OBJ_TYPE_ITEM_STAGE= (OBJ_TYPE_ITEM|OBJ_TYPE_STAGE),
	OBJ_TYPE_ITEM_TACTIC=(OBJ_TYPE_ITEM|OBJ_TYPE_TACTIC),
	OBJ_TYPE_ITEM_SLIDE=(OBJ_TYPE_ITEM|OBJ_TYPE_SLIDE),
};

// �I�u�W�F�N�g����������
enum E_OBJ_RM_TYPE:BYTE
{
	OBJ_RM_TYPE_NORMAL = 0,
	OBJ_RM_TYPE_OUT		= 1,
	OBJ_RM_TYPE_BOMB		= 2,
	OBJ_RM_TYPE_SHIELD	= 3,
};

#define BLT_PROC_TYPE_SCR_CHARA			(0)
#define BLT_PROC_TYPE_SCR_STAGE			(1)
#define BLT_PROC_TYPE_SCR_SPELL				(2)
#define BLT_PROC_TYPE_ITEM						(3)

#define STAGE_OBJ_NO									(0xFFFF)
#define DEF_STAGE_OBJ_NO							STAGE_OBJ_NO
#define DEF_STAGE_ID									(0xFF)
#define DEF_SYSTEM_ID								(0xFE)

#define MAX_SHOT_INDICATOR			(320)

#define CHARA_FACE_FINE				(0)
#define CHARA_FACE_NORMAL			(1)
#define CHARA_FACE_HURT				(2)

enum E_CHARA_FACE_TYPE
{
	CHARA_FACE_TYPE_FINE = CHARA_FACE_FINE,
	CHARA_FACE_TYPE_NORMAL = CHARA_FACE_NORMAL,
	CHARA_FACE_TYPE_HURT = CHARA_FACE_HURT,
};

// �I�u�W�F�N�g��� size 28byte
typedef struct obj_info
{
	E_TYPE_OBJ_STATE	obj_state;		// ���
	BYTE		obj_type;							// PC / BLT / ITEM

	short		vx;		// �ړ�����x���W
	short		vy;		// �ړ�����y���W

	short		ax;		// ���݈ʒux
	short		ay;		// ���݈ʒuy

	short		angle;	// �̂̌X��(360��)

	short		obj_no;			// USER_NO / NPC_NO �c
	BYTE		sess_index;		// USER_INDEX / NPC_INDEX ...
	WORD proc_flg;			// lua proc flg
	WORD	frame_count;	// Frame
	WORD	turn_count;	// Turn
	TBASE_SCR_INFO* scrinfo;
	
	DWORD	extdata1;
	DWORD	extdata2;
	DWORD	option[OBJ_OPTION_COUNT];
} type_obj, *ptype_obj, **pptype_obj;

// �L�������
typedef struct connection_info : public obj_info
{
	WCHAR	name[MAX_USER_NAME];	// �I�u�W�F�N�g��(24byte)
	BYTE		name_len;							// 

	short		lx;		// ���r?x
	short		ly;		// ���r?y

	int			sock;				// �������ꂽ?�P�b�g�ԍ�
	INT	    addr;				// �N���C�A���g�A�h���X
	INT	    sessionkey;	// ?���b�g�T?�o?�ł͕s�g�p - �T?�o?�ړ����̔閧�L?
	INT 		clientver;		// �Q??�o?�W����
	INT		port;				// ??�g�ԍ��i?���b�g�T?�o?�ł͕s�g�p�j

//	WORD	direction;		// ����(E_DIRECTION_NUMBER)

	BYTE		cost;
	// �������
	DWORD		items[GAME_ITEM_STOCK_MAX_COUNT];	// �A�C�e?�ԍ�
	E_TYPE_USER_DIRECTION	dir;			// ����
	BYTE		master;									// ?�X??����=1/ ��=0
	BYTE		game_ready;							// �Q???����� OK=1/NG=0
	BYTE		team_no;								// ???No

	short		MV_c;	// �ړ�����
	short		MV_m;	// �ړ�����MAX
	short		HP_c;	// HP
	short		HP_m;	// HP MAX
	short		delay;	// �f�B���C
	short		EXP_c;	// EXP
	WORD	live_count;

	BYTE		entity;
	BYTE		chara_type;
//	WORD	chara_state;
	char		chara_state[CHARA_STATE_COUNT];
	BYTE		connect_state;		// �N���C�A���g�̐ڑ����
} type_session, *ptype_session, **pptype_session;

// �e��� size 20byte + 28byte(obj_info)
typedef struct bullet_info : public obj_info
{
	BYTE proc_type;		// 0�F�X�N���v�g/?1�F�X�N���v�g�g�p���Ȃ�
	BYTE hit_range;			// �����蔻��̔��a
	short chr_obj_no;		// �e���������L�����ԍ�(�X�e?�W�̏ꍇ STAGE_OBJ_NO )
	BYTE chara_type;		// �e���������L�����X�N���v�g�ԍ�

	BYTE bullet_type;		// �X�N���v�g��̒e�ԍ�
	BYTE tex_index;

	short bx;				// �ړ��̊ɘa
	short by;				// �ړ��̊ɘa

	char adx;
	char ady;
} type_blt, *ptype_blt, **pptype_blt;

// �ڑ����
typedef struct connInfo{
	type_session	s;
	int				sockNo;
	char			flag;  // 0:�Z�b�V�������g�p�A1:�g�p��
} t_sessionInfo;

typedef struct packet {
	int		cli_sock;
	int		size;
	BYTE    data[MAX_PACKET_SIZE];
	void *		session;
}type_packet, *ptype_packet;

typedef struct queues{
	type_packet  * packet;
	struct queues * next;
}type_queue, *ptype_queue;

typedef struct objlist {
    struct objlist *	next;
    void				*data;
} type_objlist, *ptype_objlist, **pptype_objlist;

typedef struct mapsys{
	type_objlist *	objL;
}type_map;


typedef struct my_png_buffer_ {
    unsigned char *data;
    unsigned long data_len;
    unsigned long data_offset;
} my_png_buffer;


// SetData(2Byte)
inline INT SetShortData(BYTE* data, short s)
{
	memcpy(data, &s, sizeof(short));
	return 2;
};

// SetData(1Byte)
inline INT SetCharData(BYTE* data, char c)
{
	data[0] = c;
	return 1;
};

#endif
