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

		// Lua�̊֐��Ăяo��
		BOOL CallLuaFunc(LuaHelper* pLuah, char* func_name, LuaFuncParam* pLuaResults, int nResults, LuaFuncParam* pLuaParams, CCriticalSection* pCriticalSection);

		// �X�N���v�g�Ŕ��ˏ���
		BOOL CallShootingFunc(LuaHelper* pLuah, ptype_session shot_sess, int nProcType, int nBltType, int nShotAngle, int nShotPower, TCHARA_SCR_INFO* pCharaScrInfo, int nCharaObjNo, int nFrame, int nIndicatorAngle, int nIndicatorPower, CCriticalSection* pCriticalSection);

		// �e�X�N���v�g��??�X�̃n�b�V���l���擾
		// �߂�l-1:�n�b�V���l�����炩�̗��R�Ŏ擾�ł��Ȃ�����/ 0:����ȏ�̃t?�C���͖��� / 1:�擾����
		int GetCharaFileHash(LuaHelper* pLuah, int nID, int nFileNo, char* md5, WCHAR* path, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection);
		int GetStageFileHash(LuaHelper* pLuah, int nID, int nFileNo, char* md5, WCHAR* path, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection);

		// SE�̃n�b�V���l���擾
		// �߂�lFALSE:�n�b�V���l�����炩�̗��R�Ŏ擾�ł��Ȃ�����
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
		// �����蔻��
		BOOL IsHit(int range, D3DXVECTOR2* pos, ptype_obj obj);
	}

	enum E_CHECK_GROUND_RESULT
	{
		CHECK_GROUND_RESULT_GROUND,		// �ڒn
		CHECK_GROUND_RESULT_GETDOWN,	// ���n
		CHECK_GROUND_RESULT_FALLDOWN,	// ����
		CHECK_GROUND_RESULT_FALLING,		// ������
		CHECK_GROUND_RESULT_DROPOUT,		// ����?
	};

	namespace chr
	{
		enum E_MOVE_STAGE_RESULT
		{
			MOVE_STAGE_RESULT_NONE,				// �ύX�Ȃ�
			MOVE_STAGE_RESULT_MOVED,				// �ړ�����
			MOVE_STAGE_RESULT_VEC_CHANGE,	// �ړ��l�ύX�i�����̒��n�E�ړ��̗���)
			MOVE_STAGE_RESULT_DROP,				// ����?
			MOVE_STAGE_RESULT_REMOVE,			// �폜
		};
		enum E_MOVE_GROUND_RESULT
		{
			MOVE_GROUND_RESULT_NONE,				// �ړ��s�v
			MOVE_GROUND_RESULT_MOVED,			// �ړ�����
			MOVE_GROUND_RESULT_FALL,				// ��������
			MOVE_GROUND_RESULT_WALL,				// �ړ��ł��Ȃ�
		};
		E_MOVE_STAGE_RESULT MoveStage(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);

		E_MOVE_STAGE_RESULT MoveOnStage(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);

		enum E_GETDOWN_RESULT
		{
			GETDOWN_RESULT_DOWN,
			GETDOWN_RESULT_ON,		// �ڒn�ς�
			GETDOWN_RESULT_NOW,		// ���ڒn
			GETDOWN_RESULT_UP,			// ���
		};
		E_GETDOWN_RESULT GetDown(CMainStage* pStage, short* px, short* py, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);

		enum E_CHARA_GETDOWN_RESULT
		{
			CHARA_GETDOWN_RESULT_DOWN,					// ����
			CHARA_GETDOWN_RESULT_ON_GETDOWN,		// �ڒn�ς�
			CHARA_GETDOWN_RESULT_NOW_GETDOWN,	// ���ڒn
			CHARA_GETDOWN_RESULT_UP_GETDOWN,		// ��ɐڒn
		};
		E_CHARA_GETDOWN_RESULT CharaGetDown(CMainStage* pStage, short* px, short* py, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);

		// 
		E_MOVE_GROUND_RESULT MoveGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);
		E_MOVE_GROUND_RESULT CharaMoveGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo);

		common::E_CHECK_GROUND_RESULT CheckGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);
		// ���n��̌X�����擾
		BOOL GetDownStage(CMainStage* pStage, ptype_session sess);

	};

	namespace blt
	{
		enum E_MOVE_ACT_BULLET_RESULT
		{
			MOVE_ACT_BULLET_RESULT_NONE,				// �ύX�Ȃ�
			MOVE_ACT_BULLET_RESULT_MOVED,				// �ړ�����
			MOVE_ACT_BULLET_RESULT_VEC_CHANGE,	// �ړ��l�ύX�i�����̒��n�E�ړ��̗���)
			MOVE_ACT_BULLET_RESULT_DROP,				// ����?
			MOVE_ACT_BULLET_RESULT_REMOVE,			// �폜
			MOVE_ACT_BULLET_RESULT_STAGE_HIT,		// �X�e?�W�ɓ�������
			MOVE_ACT_BULLET_RESULT_CHARA_HIT,		// �L�����ɓ�������
			MOVE_ACT_BULLET_RESULT_OBJECT_HIT,		// �I�u�W�F�N�g�ɓ�������
		};

		enum E_MOVE_WAIT_BULLET_RESULT
		{
			MOVE_WAIT_BULLET_RESULT_NONE,				// �ύX�Ȃ�
			MOVE_WAIT_BULLET_RESULT_GROUND,		// �ڒn
			MOVE_WAIT_BULLET_RESULT_GETDOWN,		// ���n
			MOVE_WAIT_BULLET_RESULT_FALLDOWN,		// ����
			MOVE_WAIT_BULLET_RESULT_FALLING,			// ������
			MOVE_WAIT_BULLET_RESULT_DROPOUT,		// ����?
			MOVE_WAIT_BULLET_RESULT_REMOVED,		// �폜
			MOVE_WAIT_BULLET_RESULT_VEC_CHANGE,	// �ړ��l�ύX
		};

		common::E_CHECK_GROUND_RESULT CheckGround(CMainStage* pStage, ptype_blt blt, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo);
		void UpdateBulletVec(D3DXVECTOR2* out,ptype_blt blt, int wind, double dAddElapsed=1);
		
		common::blt::E_MOVE_ACT_BULLET_RESULT		MoveActBullet(CMainStage* pStage, ptype_blt blt, std::vector< type_session* > *pVecCharacters, LuaHelper* pLuah, int nWind, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, std::map<int, type_obj*>* pMapObjects, CCriticalSection* pCriticalSection, BOOL bSrv);

		common::blt::E_MOVE_WAIT_BULLET_RESULT	MoveWaitBullet(CMainStage* pStage, ptype_blt blt, std::vector< type_session* > *pVecCharacters, LuaHelper* pLuah, int nWind, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, std::map<int, type_obj*>* pMapObjects, CCriticalSection* pCriticalSection, BOOL bSrv);
		
		BOOL GetSlideVec(CMainStage* pStage,int nAngle, D3DXVECTOR2 *out, D3DXVECTOR2 *pos, int hit_range);

	};
};

#endif