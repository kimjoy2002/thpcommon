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

// ステ?ジ座標
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

// ステ?ジ用スクリプト情報
typedef struct {
	WCHAR path[_MAX_PATH*2+1];
	SIZE	size;
} TSCR_IMG_INFO;

typedef struct type_base_scr_info{
	int scr_index;										// スクリプト番号
	int res_index;									// リ??ス番号
	int ID;												// ID
	LPDIRECT3DTEXTURE9	pTexture;		// テクス?ャ
	char	md5[MD5_LENGTH+1];				// ハッシュ値
	BOOL flg;											// 使用フラグ
	int se_count;										// SEの数
	BOOL send;										// 送信
	WCHAR scr_path[_MAX_PATH*2+1];	// スクリプトのパス
} TBASE_SCR_INFO, *PBASE_SCR_INFO;

// ステ?ジ弾用情報
typedef struct{
	RECT rec_blt_tex;					// リ??ス範囲
	int blt_atk;
	int hit_range;						// 当たり半径
	int bomb_range;					// 爆発半径
	BOOL ephemeris;					// 軌道
} TSTAGE_BULLET_INFO;

typedef struct type_stage_scr_info : public type_base_scr_info{
//	int scr_index;
//	int res_index;
//	int ID;
	WCHAR name[MAX_STAGE_NAME+1];
	TSCR_IMG_INFO thumnail;						// サ?ネイル画像
	TSCR_IMG_INFO stage;							// ステ?ジ画像
	TSCR_IMG_INFO bg;								// 背景画像
	TSTAGE_BULLET_INFO blt_info[MAX_STAGE_BULLET_TYPE];		// 弾情報
	int blt_type_count;											// 弾種類数
	WCHAR bgm[_MAX_PATH*2+1];				// BGM用音声
} TSTAGE_SCR_INFO, *PSTAGE_SCR_INFO;

// キャラ弾用情報
typedef struct TCharaBulletInfo{
	RECT rec_blt_icon;				// キャラセレ用アイコンのリ??ス範囲
	RECT rec_blt_tex;					// リ??ス範囲
	int blt_atk;
	int blt_delay;
	int hit_range;						// 当たり半径
	int bomb_range;					// 爆発半径
	BOOL ephemeris;			// 軌道の登?
} TCHARA_BULLET_INFO;

// キャラ弾用情報
typedef struct TCharaSpellcardInfo : public TCharaBulletInfo{	
	WCHAR name [MAX_SPELLCARD_NAME+1];
	int exp;
	int max_exp;
} TCHARA_SPELLCARD_INFO;

// キャラ用スクリプト情報
typedef struct type_chara_scr_info : public type_base_scr_info{
//	int scr_index;
//	int res_index;													// リ??スIndex
//	int ID;
	WCHAR name[MAX_CHARACTER_NAME+1];		// キャラ名
	WCHAR tex_path[_MAX_PATH*2+1];				// キャラ用テクス?ャのパス
	int tex_chr_num;												// キャラ歩行アニメテクス?ャ数
	RECT rec_tex_chr;												// キャラテクス?ャ範囲
	int tex_trg_num;												// 弾発射アニメテクス?ャ数
	RECT rec_tex_trg;												// 弾発射アニメテクス?ャ範囲
	RECT rec_sel_icon;											// キャラセレ用アイコンのリ??ス範囲
	int delay;															// ディレイ値
	int move;															// 移動値
	int max_hp;														// HP最大値
	int angle_range_min;											// 弾発射角度最小範囲
	int angle_range_max;										// 弾発射角度最大範囲
	BYTE draw_w;													// キャラ横幅
	BYTE draw_h;													// キャラ縦幅
	char shot_h;														// 発射の高さ
	TCHARA_BULLET_INFO blt_info[MAX_CHARA_BULLET_TYPE];		// 弾情報
	TCHARA_SPELLCARD_INFO sc_info;					// スペルカ?ド情報
	int blt_type_count;											// 弾種類数
	int blt_sel_count;												// 選択できる弾の種類数
} TCHARA_SCR_INFO, *PCHARA_SCR_INFO;

// 認証結果番号列挙
enum E_TYPE_AUTH_RESULT : BYTE {
	AUTH_RESULT_SUCCESS=0,			// 成功
	AUTH_RESULT_INVALID_PWD,		// パスワ?ド不一致
	AUTH_RESULT_NO_UNIQUE_USER,		// 使用済みユ?ザ名
	AUTH_RESULT_INVALID_USER_NAME,	// ユ?ザ?名が不正
	AUTH_RESULT_GAME_LOAD,	// ゲ??中は入れない
	AUTH_RESULT_GAME_PHASE,	// ゲ??中は入れない
	AUTH_RESULT_GAME_INVALID,	// ゲ??中は入れない
	AUTH_RESULT_TIMEOUT,			// ?イ?アウト
	AUTH_RESULT_INVALID_HASH,	// ハッシュ値があわない
	AUTH_RESULT_FILESEND_ERROR,	// フ?イル送信失敗
	AUTH_RESULT_FILESEND_END,	// フ?イル送信終了
	AUTH_RESULT_FILESEND_DISABLE,
	AUTH_RESULT_INVALID_SCR_CHARA_COUNT,	// キャラのスクリプトが足りない
	AUTH_RESULT_INVALID_SCR_STAGE_COUNT,	// キャラのスクリプトが足りない
	AUTH_RESULT_INVALID_SCR_CHARA_NOT_FOUND,	// キャラのスクリプトが足りない
	AUTH_RESULT_INVALID_SCR_STAGE_NOT_FOUND,	// キャラのスクリプトが足りない
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
	CONN_STATE_EMPTY,		// 認証前
	CONN_STATE_DESTROY,		// 切断
	CONN_STATE_AUTHED,		// 認証済み
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

// クライアントのon_DrawCharaでLuaに渡す時の通常弾かスペルの判断用フラグ
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

#define CHARA_STATE_COUNT					(20)					// 状態の数
#define CHARA_STATE_STEALTH_INDEX	(0)					// ステルス
#define CHARA_STATE_BLIND_INDEX		(1)					// 暗くなる
#define CHARA_STATE_PAIN_INDEX			(2)					/// 毒
#define CHARA_STATE_SHIELD_INDEX		(3)					/// シ?ルド
#define CHARA_STATE_POWER_INDEX		(4)					/// パワ?アップ
#define CHARA_STATE_UPMOVE_INDEX		(5)					// 移動増加
#define CHARA_STATE_NOMOVE_INDEX		(6)					/// 移動不可
#define CHARA_STATE_NOANGLE_INDEX	(7)					/// 角度変更不可
#define CHARA_STATE_DOUBLE_INDEX		(8)					// 二連発使用中
#define CHARA_STATE_REVERSE_INDEX	(9)					/// 逆さ
#define CHARA_STATE_ITEM_REVERSE_INDEX	(10)			/// 逆さ弾発射前状態
#define CHARA_STATE_ITEM_BLIND_INDEX	(11)				/// 暗くなる弾発射状態
#define CHARA_STATE_ITEM_REPAIRBLT_INDEX	(12)		/// 回復する弾発射状態
#define CHARA_STATE_ITEM_TELEPORT_INDEX	(13)			/// 着弾?に移動する弾発射状態
#define CHARA_STATE_ITEM_DRAIN_INDEX	(14)				/// HP吸収する弾発射状態
#define CHARA_STATE_ITEM_FETCH_INDEX	(15)				/// 引き寄せる弾発射状態
#define CHARA_STATE_ITEM_EXCHANGE_INDEX	(16)		/// 位置入れ替え弾発射状態
#define CHARA_STATE_ITEM_NOANGLE_INDEX	(17)			/// 角度変更不可弾発射状態
#define CHARA_STATE_ITEM_NOMOVE_INDEX	(18)			/// 移動不可弾発射状態
#define CHARA_STATE_ITEM_STEAL_INDEX	(19)			/// 移動不可弾発射状態

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

/* パケットヘッ?定? */
enum E_TYPE_PACKET_HEADER : BYTE{  // BYTE DATA
		PK_SYN=0,								//
		PK_ACK=1,								//
		PK_USER_AUTH=2,					// 情報：認証パケット
		PK_REQ_HASH=3,						// 情報：ハッシュ値要求パケット
		PK_RET_HASH=4,						// 情報：ハッシュ値返答パケット
		PK_USER_CHAT=10,					// 情報：?ャットメッセ?ジパケット
		PK_USER_ROOMINFO=11,			// 情報：ル??ユ?ザ?情報
		PK_USER_LOAD=12,					// ロ?ド
		PK_USER_MAININFO=13,			// 情報：ゲ??ユ?ザ?情報
		PK_USER_CONFIRMED,
		PK_REQ_TEAM_RAND,					// ???ラン??化
		PK_REQ_LOC=20,						// 要求：位置
		PK_CMD_MV=30,						// コ?ンド：移動
		PK_CMD_ROOM_CHARA_SEL,		// コ?ンド：キャラ選択
		PK_CMD_ROOM_READY,				// コ?ンド：?備状態変更
		PK_CMD_ROOM_RULE,				// コ?ンド：ル?ル変更
		PK_CMD_ROOM_RULE_TURN_LIMIT,		// コ?ンド：制限??ン数変更
		PK_CMD_ROOM_RULE_ACT_TIME_LIMIT,		// コ?ンド：制限時間変更
		PK_CMD_ROOM_MV,					// コ?ンド：移動
		PK_CMD_MAIN_FLIP,			// コ?ンド：更新更新
		PK_CMD_ROOM_ITEM_SEL,		// コ?ンド：アイテ?選択
		PK_CMD_ROOM_TEAM_COUNT,	// コ?ンド：???数変更
		PK_CMD_ROOM_STAGE_SEL,		// コ?ンド：ステ?ジ選択

		PK_REQ_LOAD_COMPLETE = 50,			// ロ?ド完了要求
		PK_CMD_LOAD_COMPLETE,		// コ?ンド：ロ?ド完了

		PK_CMD_MAIN_MV,					// コ?ンド：移動
		PK_CMD_MAIN_SHOTPOWER,		// コ?ンド：ショットパワ?開始
		PK_CMD_MAIN_TRIGGER,			// コ?ンド：トリガ?
		PK_REQ_TRIGGER_END,				// トリガ?終了
		PK_USER_MAIN_TRIGGER_END,		// コ?ンド：トリガ?終了
		PK_CMD_MAIN_SHOT,				// コ?ンド：発射
		PK_CMD_MAIN_PASS,				// コ?ンド：パス
		PK_CMD_MAIN_ITEM,				// コ?ンド：アイテ?
		PK_CMD_MAIN_TURN_PASS,		// コ?ンド：自??ンパス

		PK_CMD_CONFIRM,

		PK_OBJ_UPDATE_ACT,
		PK_OBJ_REMOVE,
		PK_USER_DISCON = 100,			// 情報：切断
		PK_REQ_FILE_DATA = 200,					// フ?イル要求
		PK_REQ_FILE_HASH = 201,					// フ?イル要求
		PK_USER_FILEINFO = 202,					// フ?イル送信
		PK_CHK_FILE_HASH = 203,					// フ?イル要求
		PK_NOOP=0xFF,						//
};

enum E_TYPE_PACKET_FILEINFO_HEADER:BYTE{
	PK_USER_FILEINFO_OPEN,					// フ?イル位置、ハッシュ値、サイズなど
	PK_USER_FILEINFO_SEND,					// フ?イルデ??送信
	PK_USER_FILEINFO_CLOSE,						// フ?イル送信終了	
};

/* パケット：ル??ユ?ザ?情報ヘッ?定? */
enum E_TYPE_PACKET_MAININFO_HEADER : BYTE{
	PK_USER_MAININFO_START,					// ス??ト
	PK_USER_MAININFO_ACTIVE,				// アクティブ情報(順番待ちなど)
	PK_USER_MAININFO_ITEM_USE,			// アイテ?
	PK_USER_MAININFO_CHARA_MV,			// 移動パケット情報
	PK_USER_MAININFO_FLIP,			// コ?ンド：更新更新
	PK_USER_MAININFO_UPDATEPOS,			// コ?ンド：更新
	PK_USER_MAININFO_CHARA_DEAD_KILL,		// キャラ?亡パケット情報
	PK_USER_MAININFO_CHARA_DEAD_DROP,		// キャラ?亡パケット情報
	PK_USER_MAININFO_CHARA_DEAD_CLOSE,	// キャラ?亡パケット情報
	PK_USER_MAININFO_CHARA_UPDATE_STATUS,	// キャラのステ??スを変更
	PK_USER_MAININFO_CHARA_UPDATE_STATE,	// 状態更新
	PK_USER_MAININFO_WIND,						// 風向き設定
	PK_USER_MAININFO_ADD_ITEM,				// アイテ?追加

	PK_USER_MAININFO_BULLET_TRIGGER,	// 発射演出パケット情報
	PK_REQ_MAININFO_BULLET_SHOT,		// 弾発射パケット要求
	PK_REJ_MAININFO_BULLET_SHOT,		// 弾発射パケット拒否
	PK_REJ_MAININFO_TURN_PASS,			// ??ンパスパケット拒否
	PK_USER_MAININFO_BULLET_SHOT,		// 弾発射パケット情報
	PK_USER_MAININFO_BULLET_MV,			// 弾位置パケット情報
	PK_USER_MAININFO_BULLET_VEC,		// 弾方向パケット情報
	PK_USER_MAININFO_BULLET_UPDATE_TYPE,			// 弾?イプ更新パケット情報
	PK_USER_MAININFO_OBJECT_MV,			// 弾移動パケット情報
	PK_USER_MAININFO_OBJECT_RM,			// 弾削除パケット情報
	PK_USER_MAININFO_OBJECT_BOMB,		// 弾爆発パケット情報
	PK_USER_MAININFO_PASTE_IMAGE,		// ステ?ジに画像?り付けパケット情報
	PK_USER_MAININFO_OBJECT_UPDATE_STATE,		// 弾状態更新パケット情報
	PK_USER_MAININFO_TURNEND,			// ??ンエンド
	PK_USER_MAININFO_GAMEEND,			// ゲ??エンド
};

/* パケット：?ャットヘッ?定? */
enum E_TYPE_PACKET_CHAT_HEADER : BYTE{
	PK_USER_CHAT_WIS,							// 個人へ
	PK_USER_CHAT_ALL=MAXLOGINUSER,		// 全員へ
	PK_USER_CHAT_TEAM,							// ???へ
	PK_USER_CHAT_SERVER_INFO,				// サ?バから情報
	PK_USER_CHAT_SERVER_WARNING,		// サ?バから警告

	PK_USER_CHAT_NONE = 0xFF,
};

/* パケット：ル??ユ?ザ?情報ヘッ?定? */
enum E_TYPE_PACKET_ROOMINFO_HEADER : BYTE{
	PK_USER_ROOMINFO_IN,						// ル??イン
	PK_USER_ROOMINFO_NEW,					// ル??イン
	PK_USER_ROOMINFO_MASTER,				// ?ス??設定
	PK_USER_ROOMINFO_READY,				// ?備状態変更
	PK_USER_ROOMINFO_RULE,					// ル?ル変更情報
	PK_USER_ROOMINFO_RULE_TURN_LIMIT,	// 制限??ン数変更情報
	PK_USER_ROOMINFO_RULE_ACT_TIME_LIMIT,	// 制限時間変更情報
	PK_USER_ROOMINFO_CHARA_SEL,			// キャラ選択情報
	PK_USER_ROOMINFO_MOVE,					// 移動情報
	PK_USER_ROOMINFO_TEAM_RAND,				// ???ラン??化
	PK_USER_ROOMINFO_ITEM_SEL,			// アイテ?選択
	PK_USER_ROOMINFO_TEAM_COUNT,		// ???設定変更
	PK_USER_ROOMINFO_STAGE_SEL,			// ステ?ジ選択
	PK_USER_ROOMINFO_RE_ENTER,			// 再入室情報

//	PK_ROOMINFO_OUT,						// PK_USER_DISCON使用のため未使用
};

// ル?ル：ゲ??ル?ル
enum E_TYPE_GAME_GENERAL_RULE : BYTE{
	GAME_GENERAL_RULE_ITEM_ADVENT = 0,				// アイテ?出現有無
	GAME_GENERAL_RULE_STAGE_STATE = 1,				// ステ?ジ状態変更

	GAME_GENERAL_RULE_MAX
};

// ゲ??全体の状態
enum E_STATE_GAME_PHASE : BYTE{
	GAME_PHASE_ROOM,			// ?備部屋
	GAME_PHASE_LOAD,			// ロ?ディング
	GAME_PHASE_MAIN,			// メイン
	GAME_PHASE_RESULT,		// 結果画面
	GAME_PHASE_RETURN,		// 戻る

	GAME_PHASE_INVALID,		// INVALID
};

enum E_STATE_GAME_MAIN_PHASE:WORD{
	GAME_MAIN_PHASE_NONE				= 0x0001,
	GAME_MAIN_PHASE_ACT				= 0x0002,	// ??ン
	GAME_MAIN_PHASE_SHOTPOWER	= 0x0004,	// 弾のパワ?を貯めるところ
	GAME_MAIN_PHASE_TRIGGER		= 0x0008,	// 弾を撃つところ
	GAME_MAIN_PHASE_SHOT_REQ		= 0x0010,	// 弾が飛ぶのを待つ
	GAME_MAIN_PHASE_SHOOTING		= 0x0020,	// 弾を撃ってる所
	GAME_MAIN_PHASE_SHOT				= 0x0040,	// 弾が飛んでる所
	GAME_MAIN_PHASE_DOUBLE			= 0x0080,	// 二発目が飛んでる所
	GAME_MAIN_PHASE_CHECK			= 0x0100,	// 弾が飛び終わったところで?ェック
	GAME_MAIN_PHASE_SYNC				= 0x0200,	// 次へのPHASEの同期を取る
	GAME_MAIN_PHASE_TURNEND		= 0x0400,	// ??ンエンド
	GAME_MAIN_PHASE_RETURN			= 0x0800,	// 次??ンへ遷移
};

#define CAN_MODIFY_SHOT_ANGLE_PHASE (GAME_MAIN_PHASE_ACT|GAME_MAIN_PHASE_SHOTPOWER|GAME_MAIN_PHASE_NONE|GAME_MAIN_PHASE_CHECK|GAME_MAIN_PHASE_SYNC|GAME_MAIN_PHASE_TURNEND|GAME_MAIN_PHASE_RETURN)

#define OBJ_TYPE_BLT					(0x80)
#define OBJ_TYPE_ITEM				(0x40)
#define OBJ_TYPE_NONECHARA		(0xC0)

#define OBJ_TYPE_SOLID				(0x01)	// 通常の弾
#define OBJ_TYPE_GAS					(0x02)	// ステ?ジと当たり判定がない弾
#define OBJ_TYPE_LIQUID				(0x04)	// ステ?ジを這う弾
#define OBJ_TYPE_STAGE				(0x08)	// 風に影響の無いGAS
#define OBJ_TYPE_TACTIC			(0x10)	// 何にも当たり判定が無い弾
																// ??ンエンドと??ンス??トイベントのみ反応(TACTIC=戦術)
#define OBJ_TYPE_SLIDE				(0x20)	// 地面を滑る弾
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

// オブジェクトが消える種類
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

// オブジェクト情報 size 28byte
typedef struct obj_info
{
	E_TYPE_OBJ_STATE	obj_state;		// 状態
	BYTE		obj_type;							// PC / BLT / ITEM

	short		vx;		// 移動方向x座標
	short		vy;		// 移動方向y座標

	short		ax;		// 現在位置x
	short		ay;		// 現在位置y

	short		angle;	// 体の傾き(360°)

	short		obj_no;			// USER_NO / NPC_NO …
	BYTE		sess_index;		// USER_INDEX / NPC_INDEX ...
	WORD proc_flg;			// lua proc flg
	WORD	frame_count;	// Frame
	WORD	turn_count;	// Turn
	TBASE_SCR_INFO* scrinfo;
	
	DWORD	extdata1;
	DWORD	extdata2;
	DWORD	option[OBJ_OPTION_COUNT];
} type_obj, *ptype_obj, **pptype_obj;

// キャラ情報
typedef struct connection_info : public obj_info
{
	WCHAR	name[MAX_USER_NAME];	// オブジェクト名(24byte)
	BYTE		name_len;							// 

	short		lx;		// ロビ?x
	short		ly;		// ロビ?y

	int			sock;				// 生成された?ケット番号
	INT	    addr;				// クライアントアドレス
	INT	    sessionkey;	// ?ャットサ?バ?では不使用 - サ?バ?移動時の秘密キ?
	INT 		clientver;		// ゲ??バ?ジョン
	INT		port;				// ??ト番号（?ャットサ?バ?では不使用）

//	WORD	direction;		// 方向(E_DIRECTION_NUMBER)

	BYTE		cost;
	// 部屋情報
	DWORD		items[GAME_ITEM_STOCK_MAX_COUNT];	// アイテ?番号
	E_TYPE_USER_DIRECTION	dir;			// 向き
	BYTE		master;									// ?ス??権限=1/ 他=0
	BYTE		game_ready;							// ゲ???備状態 OK=1/NG=0
	BYTE		team_no;								// ???No

	short		MV_c;	// 移動距離
	short		MV_m;	// 移動距離MAX
	short		HP_c;	// HP
	short		HP_m;	// HP MAX
	short		delay;	// ディレイ
	short		EXP_c;	// EXP
	WORD	live_count;

	BYTE		entity;
	BYTE		chara_type;
//	WORD	chara_state;
	char		chara_state[CHARA_STATE_COUNT];
	BYTE		connect_state;		// クライアントの接続状態
} type_session, *ptype_session, **pptype_session;

// 弾情報 size 20byte + 28byte(obj_info)
typedef struct bullet_info : public obj_info
{
	BYTE proc_type;		// 0：スクリプト/?1：スクリプト使用しない
	BYTE hit_range;			// 当たり判定の半径
	short chr_obj_no;		// 弾を撃ったキャラ番号(ステ?ジの場合 STAGE_OBJ_NO )
	BYTE chara_type;		// 弾を撃ったキャラスクリプト番号

	BYTE bullet_type;		// スクリプト上の弾番号
	BYTE tex_index;

	short bx;				// 移動の緩和
	short by;				// 移動の緩和

	char adx;
	char ady;
} type_blt, *ptype_blt, **pptype_blt;

// 接続情報
typedef struct connInfo{
	type_session	s;
	int				sockNo;
	char			flag;  // 0:セッション未使用、1:使用中
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
