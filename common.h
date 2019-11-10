#ifndef H_COMMON___
#define H_COMMON___

#include "windows.h"
#include <cassert>
#include <map>
#include <vector>
#include <algorithm>
#include "CCriticalSection.h"
#include "util.h"

#include <lua.hpp>
#include "tolua++.h"
//#include "tolua_glue/thg_glue.h"
#include "LuaHelper.h"
#include "CMainStage.h"
#include "../clx/clx/md5.h"
#include <string>

#if OUT_HASH_CODE
#define CHARA_TEST					0
#define STAGE_TEST						0
#else
#define CHARA_TEST					0	// 1/0
#define CHARA_TEST_COUNT		4	// 4

#define STAGE_TEST						0	// 1/0
#define STATE_TEST_COUNT			7
#endif

#define TEST_CODE						0xB303
extern int g_nMaxLoginNum;

namespace common
{
	void GetExeDir(WCHAR* path);
	BOOL EncodeFileHash(const char* path, clx::md5* pmd);
	BOOL WCharToMB(char** ppDest, int* pLen, WCHAR* src);
	namespace scr
	{
		BOOL LoadLoaderScript(lua_State *pL, char* luafile);
		BOOL LoadAllCharaScript(lua_State *pL , LuaHelper* pLuah, std::map < int, TCHARA_SCR_INFO >* pDestMapScrInfo);
		int LoadCharaScript(int num, const char* exedir, const char* modulefile, const char* module,LuaHelper* pLuah, std::map < int, TCHARA_SCR_INFO >* pDestMapScrInfo);
		TCHARA_SCR_INFO* FindCharaScrInfoFromCharaType(int chara_type, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);

		BOOL LoadAllStageScript(lua_State *pL , LuaHelper* pLuah, std::map < int, TSTAGE_SCR_INFO >* pDestMapScrInfo);
		int LoadStageScript(int num, const char* exedir, const char* modulefile, const char* module,LuaHelper* pLuah, std::map < int, TSTAGE_SCR_INFO >* pDestMapScrInfo);
		TSTAGE_SCR_INFO* FindStageScrInfoFromStageID(int stage_id, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo);
		TSTAGE_SCR_INFO* FindStageScrInfoFromStageIndex(int stage_index, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo);

		BOOL FindStageScrInfoFromStageIndex(int stage_index, TSTAGE_SCR_INFO* pStageScrInfo, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo);

		// Luaの関数呼び出し
		BOOL CallLuaFunc(LuaHelper* pLuah, char* func_name, LuaFuncParam* pLuaResults, int nResults, LuaFuncParam* pLuaParams, CCriticalSection* pCriticalSection);

		// スクリプトで発射処理
		BOOL CallShootingFunc(LuaHelper* pLuah, ptype_session shot_sess, int nProcType, int nBltType, int nShotAngle, int nShotPower, TCHARA_SCR_INFO* pCharaScrInfo, int nCharaObjNo, int nFrame, int nIndicatorAngle, int nIndicatorPower, CCriticalSection* pCriticalSection);

		// 各スクリプトリ??スのハッシュ値を取得
		// 戻り値-1:ハッシュ値が何らかの理由で取得できなかった/ 0:これ以上のフ?イルは無い / 1:取得成功
		int GetCharaFileHash(LuaHelper* pLuah, int nID, int nFileNo, char* md5, WCHAR* path, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection);
		int GetStageFileHash(LuaHelper* pLuah, int nID, int nFileNo, char* md5, WCHAR* path, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection);

		// SEのハッシュ値を取得
		// 戻り値FALSE:ハッシュ値が何らかの理由で取得できなかった
		BOOL GetCharaSEHash(LuaHelper* pLuah, int nCharaIndex, int nFileNo, char* md5, WCHAR* path,CCriticalSection* pCriticalSection);
		BOOL GetStageSEHash(LuaHelper* pLuah, int nStageIndex, int nFileNo, char* md5, WCHAR* path,CCriticalSection* pCriticalSection);

		BOOL GetCharaFilePath(WCHAR* path, LuaHelper* pLuah, int nID, int nFileNo, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection);
		BOOL GetStageFilePath(WCHAR* path, LuaHelper* pLuah, int nID, int nFileNo, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection);
	};

	namespace session
	{
		void GetSessionName(ptype_session sess, WCHAR name[MAX_USER_NAME+1]);
	}

	namespace obj
	{
		void SetLuaFlg(type_obj* obj, DWORD flg, BOOL on);
		// 当たり判定
		BOOL IsHit(int range, D3DXVECTOR2* pos, ptype_obj obj);
	}

	enum E_CHECK_GROUND_RESULT
	{
		CHECK_GROUND_RESULT_GROUND,		// 接地
		CHECK_GROUND_RESULT_GETDOWN,	// 着地
		CHECK_GROUND_RESULT_FALLDOWN,	// 落下
		CHECK_GROUND_RESULT_FALLING,		// 落下中
		CHECK_GROUND_RESULT_DROPOUT,		// 落下?
	};

	namespace chr
	{
		enum E_MOVE_STAGE_RESULT
		{
			MOVE_STAGE_RESULT_NONE,				// 変更なし
			MOVE_STAGE_RESULT_MOVED,				// 移動した
			MOVE_STAGE_RESULT_VEC_CHANGE,	// 移動値変更（落下⇔着地・移動⇔落下)
			MOVE_STAGE_RESULT_DROP,				// 落下?
			MOVE_STAGE_RESULT_REMOVE,			// 削除
		};
		enum E_MOVE_GROUND_RESULT
		{
			MOVE_GROUND_RESULT_NONE,				// 移動不要
			MOVE_GROUND_RESULT_MOVED,			// 移動した
			MOVE_GROUND_RESULT_FALL,				// 落下する
			MOVE_GROUND_RESULT_WALL,				// 移動できない
		};
		E_MOVE_STAGE_RESULT MoveStage(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);

		E_MOVE_STAGE_RESULT MoveOnStage(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);

		enum E_GETDOWN_RESULT
		{
			GETDOWN_RESULT_DOWN,
			GETDOWN_RESULT_ON,		// 接地済み
			GETDOWN_RESULT_NOW,		// 今接地
			GETDOWN_RESULT_UP,			// 上に
		};
		E_GETDOWN_RESULT GetDown(CMainStage* pStage, short* px, short* py, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);

		enum E_CHARA_GETDOWN_RESULT
		{
			CHARA_GETDOWN_RESULT_DOWN,					// 落下
			CHARA_GETDOWN_RESULT_ON_GETDOWN,		// 接地済み
			CHARA_GETDOWN_RESULT_NOW_GETDOWN,	// 今接地
			CHARA_GETDOWN_RESULT_UP_GETDOWN,		// 上に接地
		};
		E_CHARA_GETDOWN_RESULT CharaGetDown(CMainStage* pStage, short* px, short* py, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);

		// 
		E_MOVE_GROUND_RESULT MoveGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);
		E_MOVE_GROUND_RESULT CharaMoveGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);

		common::E_CHECK_GROUND_RESULT CheckGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);
		// 着地後の傾きを取得
		BOOL GetDownStage(CMainStage* pStage, ptype_session sess);

	};

	namespace blt
	{
		enum E_MOVE_ACT_BULLET_RESULT
		{
			MOVE_ACT_BULLET_RESULT_NONE,				// 変更なし
			MOVE_ACT_BULLET_RESULT_MOVED,				// 移動した
			MOVE_ACT_BULLET_RESULT_VEC_CHANGE,	// 移動値変更（落下⇔着地・移動⇔落下)
			MOVE_ACT_BULLET_RESULT_DROP,				// 落下?
			MOVE_ACT_BULLET_RESULT_REMOVE,			// 削除
			MOVE_ACT_BULLET_RESULT_STAGE_HIT,		// ステ?ジに当たった
			MOVE_ACT_BULLET_RESULT_CHARA_HIT,		// キャラに当たった
			MOVE_ACT_BULLET_RESULT_OBJECT_HIT,		// オブジェクトに当たった
		};

		enum E_MOVE_WAIT_BULLET_RESULT
		{
			MOVE_WAIT_BULLET_RESULT_NONE,				// 変更なし
			MOVE_WAIT_BULLET_RESULT_GROUND,		// 接地
			MOVE_WAIT_BULLET_RESULT_GETDOWN,		// 着地
			MOVE_WAIT_BULLET_RESULT_FALLDOWN,		// 落下
			MOVE_WAIT_BULLET_RESULT_FALLING,			// 落下中
			MOVE_WAIT_BULLET_RESULT_DROPOUT,		// 落下?
			MOVE_WAIT_BULLET_RESULT_REMOVED,		// 削除
			MOVE_WAIT_BULLET_RESULT_VEC_CHANGE,	// 移動値変更
		};

		common::E_CHECK_GROUND_RESULT CheckGround(CMainStage* pStage, ptype_blt blt, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);
		void UpdateBulletVec(D3DXVECTOR2* out,ptype_blt blt, int wind, double dAddElapsed=1);
		
		common::blt::E_MOVE_ACT_BULLET_RESULT		MoveActBullet(CMainStage* pStage, ptype_blt blt, std::vector< type_session* > *pVecCharacters, LuaHelper* pLuah, int nWind, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, std::map<int, type_obj*>* pMapObjects, CCriticalSection* pCriticalSection, BOOL bSrv);

		common::blt::E_MOVE_WAIT_BULLET_RESULT	MoveWaitBullet(CMainStage* pStage, ptype_blt blt, std::vector< type_session* > *pVecCharacters, LuaHelper* pLuah, int nWind, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, std::map<int, type_obj*>* pMapObjects, CCriticalSection* pCriticalSection, BOOL bSrv);
		
		BOOL GetSlideVec(CMainStage* pStage,int nAngle, D3DXVECTOR2 *out, D3DXVECTOR2 *pos, int hit_range);

	};
};

#endif