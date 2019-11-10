#include "common.h"
#if (TRIAL|RTRIAL)
#include "hash.h"
#endif

using namespace common;
using namespace common::scr;
using namespace common::chr;
using namespace common::blt;

int g_nMaxLoginNum = 12;

// common
BOOL common::WCharToMB(char** ppDest, int* pLen, WCHAR* src)
{
	*pLen = 0;
	int nLen = WideCharToMultiByte(CP_THREAD_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	if (!nLen) return FALSE;
	*ppDest = new char[nLen+1];
	char* pDest = *ppDest;
	ZeroMemory(pDest, sizeof(char)*nLen+1);
	if (WideCharToMultiByte(CP_THREAD_ACP, 0, src, -1, pDest, nLen, NULL, NULL) == 0)
	{
		MessageBox(NULL, L"WCharToMB convert error", src, MB_OK);
		SafeDeleteArray(pDest);
		return FALSE;
	}
	*pLen = nLen;
	return TRUE;
}

void common::GetExeDir(WCHAR* path)
{
	WCHAR exepath[_MAX_PATH*2+1];
	WCHAR pDrive[_MAX_DRIVE+1];
	WCHAR pDir[_MAX_DIR+1];
	GetModuleFileName(NULL, exepath, _MAX_PATH*2);
	_wsplitpath(exepath, pDrive, pDir, NULL,NULL);
	int nLen = wcslen(pDir);
	if (!(pDir[nLen-1] == L'/' || pDir[nLen-1] == L'\\'))
	{
		pDir[nLen-1] = L'\\';
		pDir[nLen] = NULL;
	}
	_wmakepath(path, pDrive, pDir, NULL, NULL);
}

BOOL common::EncodeFileHash(const char* path, clx::md5* pmd)
{
#if SCR_HASH_CHECK
	FILE* f = NULL;
	if (fopen_s(&f, path, "rb") != NULL)
		return FALSE;
	long lSize = _filelength(_fileno(f));
	fseek(f,0L, SEEK_SET);

	char* buf = new char[lSize];
	if (!buf)
	{
		fclose(f);
		MessageBoxA(NULL, path, "フ?イル読み込み失敗", MB_OK);
		return FALSE;
	}
	if (lSize != fread(buf, 1, lSize, f))
	{
		fclose(f);
		SafeDeleteArray(buf);
		MessageBoxA(NULL, path, "フ?イル読み込み失敗", MB_OK);
		return FALSE;
	}
	fclose(f);
	pmd->update(buf, lSize);
	SafeDeleteArray(buf);
	return TRUE;
#else
	return TRUE;
#endif
}

BOOL common::scr::LoadLoaderScript(lua_State *pL, char* luafile)
{
	BOOL bLoaded = FALSE;
	while (1)
	{
		int top = lua_gettop(pL);
		int ret = luaL_dofile(pL, luafile);
		bLoaded = (!ret);
		if (ret)
		{
			char str[1024];
			strcpy_s(str, 1024, lua_tostring(pL, -1));
			int len = strlen(str);
			wchar_t *wstr = new wchar_t[len+1]();
			if (wstr)
			{
				if (MultiByteToWideChar(CP_THREAD_ACP, 0, str, len, wstr, len) != len)
					MessageBox(NULL, L"script load error", wstr, MB_OK);
				delete[] wstr;
			}
		}
		else	break;
		lua_settop(pL, top);
	}
	return bLoaded;
}


BOOL common::scr::LoadAllStageScript(lua_State *pL , LuaHelper* pLuah, std::map < int, TSTAGE_SCR_INFO >* pDestMapScrInfo)
{
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;

	pDestMapScrInfo->clear();

	//フ?イルの検索
	_finddata_t	fileinfo;
	long handle; 
	char name[_MAX_FNAME*2+1];
	char scrmodule[_MAX_FNAME*2+1];
	char exe_path[_MAX_PATH*2+1];
	std::string exe_dir;

	GetModuleDirA(exe_path, _MAX_PATH*2+1);
	exe_dir = exe_path;
	if (exe_dir[exe_dir.length()-1] != '\\')
		exe_dir += "\\";
	std::string wc = exe_dir;
	wc += LUA_STGSCR_WC;
	handle = _findfirst(wc.c_str(),&fileinfo);

	_splitpath_s(fileinfo.name, NULL, NULL, NULL, NULL, name, _MAX_FNAME*2, NULL, NULL);
	SafePrintfA(scrmodule, _MAX_FNAME*2+1, "%s%s", LUA_STGSCR_MOD, name);
	int scr_num = 1;
	int ret = 0;
	// load lua "data.scr.chr.chara.lua"
	ret = LoadStageScript(scr_num, exe_dir.c_str(), fileinfo.name, scrmodule, pLuah, pDestMapScrInfo);
	if (ret == -1)
		return FALSE;
	scr_num+=ret;

	while(!_findnext(handle, &fileinfo))
	{
		_splitpath_s(fileinfo.name, NULL, NULL, NULL, NULL, name, _MAX_FNAME*2, NULL, NULL);
		SafePrintfA(scrmodule, _MAX_FNAME*2+1, "%s%s", LUA_STGSCR_MOD, name);
		ret = LoadStageScript(scr_num, exe_dir.c_str(), fileinfo.name, scrmodule, pLuah, pDestMapScrInfo);
		if (ret == -1)
			return FALSE;
		scr_num+=ret;
#if	TRIAL
#if	STAGE_TEST

#else
		if (pDestMapScrInfo->size() > STATE_TEST_COUNT)
			break;
#endif
#endif
	}
	_findclose(handle);

	if (pDestMapScrInfo->empty())
		return FALSE;
	
	return TRUE;
}

int common::scr::LoadStageScript(int num, const char* exedir, const char* modulefile, const char* module,LuaHelper* pLuah, std::map < int, TSTAGE_SCR_INFO >* pDestMapScrInfo)
{
	std::string path;
	std::string str_module;
	clx::md5* md = new clx::md5();
	md->reset();

	LuaFuncParam luaParams;
	LuaFuncParam luaResults;
	WCHAR* wstr = NULL;
	int wstr_len = 0;
	int nID = 0;

	path = exedir;
	path += LUA_STGSCR_DIR;
	path += modulefile;
	str_module = path;
	if (!common::EncodeFileHash(path.c_str(), md))
		return -1;
	TSTAGE_SCR_INFO scrinfo;
	ZeroMemory(&scrinfo, sizeof(TSTAGE_SCR_INFO));

	char modfile[_MAX_PATH*2+1];
	SafePrintfA(modfile, _MAX_PATH*2, "%s%s", LUA_STGSCR_DIR, modulefile);
	int len = strlen(modfile);
	wstr = new WCHAR[len+1]();
	if (wstr)
	{
		if (MultiByteToWideChar(CP_THREAD_ACP, 0, modfile, len, wstr, len) == 0)
		{
			MessageBox(NULL, L"stage script load error", wstr, MB_OK);
			return -1;
		}
	}
	SafePrintf(scrinfo.scr_path, _MAX_PATH*2, L"%s",wstr);
	delete[] wstr;

	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(num).String(module);
	int nScrIndex = num;
	// ステ?ジスクリプトクラス生成
	if (!pLuah->CallFunc("load_StageScript", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "stage loader", MB_OK);
		char log[320];
		SafePrintfA(log, 320, "スクリプトにエラ?が見つかりました(%s)\nスクリプトフ?イルを削除しますか？", path.c_str());
		if ( MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
		{
			DeleteFileA(str_module.c_str());
			return 0;
		}
		else
			return -1;
	}
	luaParams.Clear();
	luaResults.Clear();

	// ステ?ジID取得
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_ID", &luaResults, 2, &luaParams))
	{
		SafeDelete(md);
		MessageBoxA(NULL, "lua func error(getStage_ID)", "loader", MB_OK);
		return -1;
	}
	nID = (int)luaResults.GetNumber(0);
	if (nID > 0xFF || nID < 0)
	{
		SafeDelete(md);
//		WCHAR msg[64];
//		SafePrintf(msg, 64, L"範囲外のIDが設定されているステ?ジスクリプトが見つかりました(ID:%d)", nID);
//		MessageBox(NULL, msg, L"script loader", MB_OK);
		return 0;
	}
	scrinfo.send = (BOOL)luaResults.GetBool(1);
#if STAGE_TEST
	// キャラID取得
	switch (nID)
	{
	case -1:
		{
			luaParams.Number(nScrIndex).Numer(TEST_CODE);
			if (!pLuah->CallFunc("CheckStageDebug", &luaResults, 1, &luaParams))
			{
				SafeDelete(md);
				WCHAR test[64];
				SafePrintf(test, 64, L"スクリプトが不正でした(%d)", nID);
				MessageBox(NULL, test, L"loader", MB_OK);
				return FALSE;
			}
			nID = luaResults.GetNumber(0);
			if (nID == -1)
			{
				WCHAR test[64];
				SafePrintf(test, 64, L"スクリプトが不正です(%d)", nID);
				MessageBox(NULL, test, L"loader", MB_OK);
				return FALSE;
			}
			luaParams.Clear();
			luaResults.Clear();

		}
	default:
		break;
	}
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_ID", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		MessageBoxA(NULL, "lua func error(getStage_ID)", "loader", MB_OK);
		return -1;
	}
	nID = (int)luaResults.GetNumber(0);
	luaParams.Clear();
	luaResults.Clear();
#endif
	if (nID == -1)
	{
		SafeDelete(md);
		WCHAR msg[64];
		SafePrintf(msg, 64, L"不正なステ?ジスクリプトが見つかりました(ID:%d)", nID);
		MessageBox(NULL, msg, L"script loader", MB_OK);
		return -1;
	}
	// 登?済みIDか確認
	std::map < int, TSTAGE_SCR_INFO >::iterator itfind = pDestMapScrInfo->find(nID);
	if (itfind != pDestMapScrInfo->end())
	{
		SafeDelete(md);
		WCHAR msg[64];
		SafePrintf(msg, 64, L"ステ?ジスクリプトに重複IDが見つかりました(ID:%d)", nID);
		MessageBox(NULL, msg, L"script loader", MB_OK);
		return -1;
	}
	scrinfo.ID = nID;

	luaParams.Clear();
	luaResults.Clear();
	// ステ?ジ名取得
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_Name", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}

	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.name, sizeof(scrinfo.name));
	SafeMemCopy(scrinfo.name, wstr, wstr_len*sizeof(WCHAR), MAX_STAGE_NAME*sizeof(WCHAR));
	SafeDeleteArray(wstr);

	luaParams.Clear();
	luaResults.Clear();
	// ステ?ジサ?ネイル情報取得
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_ThumnailInfo", &luaResults, 3, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}

	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.thumnail.path, sizeof(scrinfo.thumnail.path));
	SafeMemCopy(scrinfo.thumnail.path, wstr, wstr_len*sizeof(WCHAR), _MAX_PATH*2*sizeof(WCHAR));
//	wcsncpy_s(scrinfo.thumnail.path, _MAX_PATH*2, wstr, wstr_len);
	SafeDeleteArray(wstr);
	scrinfo.thumnail.size.cx = (int)luaResults.GetNumber(1);
	scrinfo.thumnail.size.cy = (int)luaResults.GetNumber(2);

	path = exedir;
	path += luaResults.GetString(0);
	if (!common::EncodeFileHash(path.c_str(), md))
	{
		SafeDelete(md);
		char log[320];
		SafePrintfA(log, 320, "使用フ?イル(%s)が見つかりませんでした。\nロ?ドをスキップします。");
		MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_OK);
		SafePrintfA(log, 320, "スクリプトフ?イルを削除しますか？\n%s", path.c_str());
		if ( MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
			DeleteFileA(str_module.c_str());
		return -1;
	}

	luaParams.Clear();
	luaResults.Clear();
	// ステ?ジ情報取得
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_StageInfo", &luaResults, 3, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}

	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.stage.path, sizeof(scrinfo.stage.path));
	wcsncpy_s(scrinfo.stage.path, _MAX_PATH*2, wstr, wstr_len);
	SafeDeleteArray(wstr);
	scrinfo.stage.size.cx = (int)luaResults.GetNumber(1);
	scrinfo.stage.size.cy = (int)luaResults.GetNumber(2);

	path = exedir;
	path += luaResults.GetString(0);
	if (!common::EncodeFileHash(path.c_str(), md))
	{
		SafeDelete(md);
		char log[320];
		SafePrintfA(log, 320, "使用フ?イル(%s)が見つかりませんでした。\nロ?ドをスキップします。");
		MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_OK);
		SafePrintfA(log, 320, "スクリプトフ?イルを削除しますか？\n%s", path.c_str());
		if ( MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
			DeleteFileA(str_module.c_str());
		return -1;
	}

	luaParams.Clear();
	luaResults.Clear();
	// ステ?ジ情報取得
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_BGInfo", &luaResults, 3, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}

	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.bg.path, sizeof(scrinfo.bg.path));
	SafeMemCopy(scrinfo.bg.path, wstr, wstr_len*sizeof(WCHAR), _MAX_PATH*2*sizeof(WCHAR));
//	wcsncpy_s(scrinfo.bg.path, _MAX_PATH*2, wstr, wstr_len);
	SafeDeleteArray(wstr);
	scrinfo.bg.size.cx = (int)luaResults.GetNumber(1);
	scrinfo.bg.size.cy = (int)luaResults.GetNumber(2);

	path = exedir;
	path += luaResults.GetString(0);
	if (!common::EncodeFileHash(path.c_str(), md))
	{
		SafeDelete(md);
		char log[320];
		SafePrintfA(log, 320, "使用フ?イル(%s)が見つかりませんでした。\nロ?ドをスキップします。");
		MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_OK);
		SafePrintfA(log, 320, "スクリプトフ?イルを削除しますか？\n%s", path.c_str());
		if ( MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
			DeleteFileA(str_module.c_str());
		return -1;
	}

	// ステ?ジ弾 //
	// 弾数
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_GetBltTypeCount", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.blt_type_count = (int)luaResults.GetNumber(0);
	// 弾情報
	for (int i=0;i<scrinfo.blt_type_count;i++)
	{
		// 弾種取得
		luaParams.Clear();
		luaResults.Clear();
		luaParams.Number(nScrIndex).Number(i);
		if (!pLuah->CallFunc("getStage_GetBltInfo", &luaResults, 8, &luaParams))
		{
			SafeDelete(md);
			const char * pc = pLuah->GetErr();
			MessageBoxA(NULL, pc, "loader", MB_OK);
			return -1;
		}
		scrinfo.blt_info[i].blt_atk = (int)luaResults.GetNumber(0);
		scrinfo.blt_info[i].rec_blt_tex.left = (int)luaResults.GetNumber(1);
		scrinfo.blt_info[i].rec_blt_tex.top = (int)luaResults.GetNumber(2);
		scrinfo.blt_info[i].rec_blt_tex.right = (int)luaResults.GetNumber(3);
		scrinfo.blt_info[i].rec_blt_tex.bottom = (int)luaResults.GetNumber(4);
		scrinfo.blt_info[i].hit_range = (int)luaResults.GetNumber(5);
		scrinfo.blt_info[i].bomb_range = (int)luaResults.GetNumber(6);
		scrinfo.blt_info[i].ephemeris = (BOOL)luaResults.GetBool(7);
	}

	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_SEFilesCount", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.se_count = (int)luaResults.GetNumber(0);
	// 音声のハッシュ値を取得 //
#if 1	// 音声はサ?バ間処理に影響ないのでハッシュ値?ェックしない
	int nSECount = scrinfo.se_count;
	for (int i=0;i<nSECount;i++)
	{
		luaParams.Clear();
		luaResults.Clear();
		luaParams.Number(nScrIndex).Number(i);
		if (!pLuah->CallFunc("getStage_SEFile", &luaResults, 1, &luaParams))
		{
			SafeDelete(md);
			const char * pc = pLuah->GetErr();
			MessageBoxA(NULL, pc, "loader", MB_OK);
			return -1;
		}
		path = exedir;
		path += luaResults.GetString(0);
		if (!common::EncodeFileHash(path.c_str(), md))
		{
			SafeDelete(md);
			char log[320];
			SafePrintfA(log, 320, "使用フ?イル(%s)が見つかりませんでした。\nロ?ドをスキップします。");
			MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_OK);
			SafePrintfA(log, 320, "スクリプトフ?イルを削除しますか？\n%s", path.c_str());
			if ( MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
				DeleteFileA(str_module.c_str());
			return -1;
		}
	}
#endif
	luaParams.Clear();
	luaResults.Clear();
	// ステ?ジBGM情報取得
	luaParams.Number(nScrIndex);
	if (!pLuah->CallFunc("getStage_GetBGMFile", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}

	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.bgm, sizeof(scrinfo.bgm));
	SafeMemCopy(scrinfo.bgm, wstr, wstr_len*sizeof(WCHAR), _MAX_PATH*2*sizeof(WCHAR));
	SafeDeleteArray(wstr);
#if 1	//	BGMハッシュ値は取らない
	path = exedir;
	path += luaResults.GetString(0);
	if (!common::EncodeFileHash(path.c_str(), md))
	{
		SafeDelete(md);
		char log[320];
		SafePrintfA(log, 320, "使用フ?イル(%s)が見つかりませんでした。\nロ?ドをスキップします。");
		MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_OK);
		SafePrintfA(log, 320, "スクリプトフ?イルを削除しますか？\n%s", path.c_str());
		if ( MessageBoxA(NULL, log, "ステ?ジスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
			DeleteFileA(str_module.c_str());
		return FALSE;
	}
#endif
	md->finish();
	ZeroMemory(scrinfo.md5, sizeof(char)*MD5_LENGTH);
	memcpy(scrinfo.md5, (char*)md->to_string().c_str(), sizeof(char)*MD5_LENGTH);
	SafeDelete(md);

#ifdef _DEBUG
	WCHAR wsHash[32+1];
	MultiByteToWideChar(CP_THREAD_ACP, 0, scrinfo.md5, 32+1, wsHash, 32+1);

	WCHAR wsmd[80];
	SafePrintf(wsmd,80, L"#define HASH_STAGE%02d\t\t\"%s\"\n", nID, wsHash);
	OutputDebugStr(wsmd);

#endif
	// スクリプト番号
	scrinfo.scr_index = num;
#if SCR_HASH_CHECK
	scrinfo.flg = FALSE;
#if TRIAL	// ロ?ド時にハッシュ値?ェック
#ifndef _DEBUG
	char *csTriChk = NULL;
	switch (nID)
	{
	case 0:	csTriChk = HASH_STAGE00;		break;
	case 1:	csTriChk = HASH_STAGE01;		break;
	case 2:	csTriChk = HASH_STAGE02;		break;
	case 3:	csTriChk = HASH_STAGE03;		break;
	case 4:	csTriChk = HASH_STAGE04;		break;
	case 5:	csTriChk = HASH_STAGE05;		break;
	case 6:	csTriChk = HASH_STAGE06;		break;
	case 7:	csTriChk = HASH_STAGE07;		break;
#if	STAGE_TEST
	case 8:	csTriChk = HASH_STAGE08;		break;
#endif
	default:
		return 1;	// 他IDは追加しない
	}
	if (strcmp(scrinfo.md5, csTriChk) != 0)
		return -1;

#else	 //_DEBUG
	switch (nID)
	{
	case 0:break;
	case 1:break;
	case 2:break;
	case 3:break;
	case 4:break;
	case 5:break;
	case 6:break;
	case 7:break;
#if	STAGE_TEST
	case 8:break;
		break;
#endif
	default:
		return -1;	// 他IDは追加しない
	}
#endif	// _DEBUG
#endif	// TRIAL	// ロ?ド時にハッシュ値?ェック

#else	//SCR_HASH_CHECK
	scrinfo.flg = TRUE;
#endif	//SCR_HASH_CHECK
	pDestMapScrInfo->insert( std::map<int, TSTAGE_SCR_INFO>::value_type( nID, scrinfo ) );

	return 1;
}

BOOL common::scr::LoadAllCharaScript(lua_State *pL , LuaHelper* pLuah, std::map < int, TCHARA_SCR_INFO >* pDestMapScrInfo)
{
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;
	// Load Lua "loader.lua"
	pDestMapScrInfo->clear();

	//フ?イルの検索
	_finddata_t	fileinfo;
	long handle; 
	char name[_MAX_FNAME*2+1];
	char scrmodule[_MAX_FNAME*2+1];
	char exe_path[_MAX_PATH*2+1];
	std::string exe_dir;

	GetModuleDirA(exe_path, _MAX_PATH*2+1);
	exe_dir = exe_path;
	if (exe_dir[exe_dir.length()-1] != '\\')
		exe_dir += "\\";
	std::string wc = exe_dir;
	wc += LUA_CHRSCR_WC;
	handle = _findfirst(wc.c_str(),&fileinfo);

	_splitpath_s(fileinfo.name, NULL, NULL, NULL, NULL, name, _MAX_FNAME*2, NULL, NULL);
	SafePrintfA(scrmodule, _MAX_FNAME*2+1, "%s%s", LUA_CHRSCR_MOD, name);
	int scr_num = 1;
	int ret = 0;
	// load lua "data.scr.chr.chara.lua"
	ret = LoadCharaScript(scr_num, exe_dir.c_str(), fileinfo.name, scrmodule, pLuah, pDestMapScrInfo);
	if (ret == -1)
		return FALSE;
	scr_num+=ret;

	while(!_findnext(handle, &fileinfo))
	{
		_splitpath_s(fileinfo.name, NULL, NULL, NULL, NULL, name, _MAX_FNAME*2, NULL, NULL);
		SafePrintfA(scrmodule, _MAX_FNAME*2+1, "%s%s", LUA_CHRSCR_MOD, name);
		ret =LoadCharaScript(scr_num, exe_dir.c_str(), fileinfo.name, scrmodule, pLuah, pDestMapScrInfo);
		if (ret == -1)
			return FALSE;
		scr_num+=ret;
#if	TRIAL
#if	CHARA_TEST

#else
		if (pDestMapScrInfo->size() > CHARA_TEST_COUNT)
			break;
#endif
#endif
	}
	_findclose(handle);

	if (pDestMapScrInfo->empty())
		return FALSE;

	return TRUE;
}

BOOL common::scr::LoadCharaScript(int num, const char* exedir, const char* modulefile, const char* module, LuaHelper* pLuah, std::map < int, TCHARA_SCR_INFO >* pDestMapScrInfo)
{
	clx::md5* md = new clx::md5();
	LuaFuncParam luaParams;
	LuaFuncParam luaResults;
	WCHAR* wstr = NULL;
	int wstr_len = 0;
	std::string path;
	std::string str_module;

	path = exedir;
	path += LUA_CHRSCR_DIR;
	path += modulefile;
	str_module = path;
	if (!common::EncodeFileHash(path.c_str(), md))
	{
		SafeDelete(md);
		return -1;
	}

	int nID = 0;
	TCHARA_SCR_INFO scrinfo;
	ZeroMemory(&scrinfo, sizeof(TCHARA_SCR_INFO));

	char modfile[_MAX_PATH*2+1];
	SafePrintfA(modfile, _MAX_PATH*2, "%s%s", LUA_CHRSCR_DIR, modulefile);
	int len = strlen(modfile);
	wstr = new WCHAR[len+1]();
	if (wstr)
	{
		if (MultiByteToWideChar(CP_THREAD_ACP, 0, modfile, len, wstr, len) == 0)
		{
			MessageBox(NULL, L"chara script load error", wstr, MB_OK);
			return -1;
		}
	}
	SafePrintf(scrinfo.scr_path, _MAX_PATH*2, L"%s",wstr);
	delete[] wstr;

	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(num).String(module);
	int nCharaIndex = num;
	if (!pLuah->CallFunc("load_CharaScript", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		char log[320];
		SafePrintfA(log, 320, "スクリプトにエラ?が見つかりました(%s)\nスクリプトフ?イルを削除しますか？", path.c_str());
		if ( MessageBoxA(NULL, log, "キャラスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
		{
			DeleteFileA(str_module.c_str());
			return 0;
		}
		else
			return -1;
	}
	luaParams.Clear();
	luaResults.Clear();
	// キャラID取得
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_ID", &luaResults, 2, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	nID = (int)luaResults.GetNumber(0);
	if (nID > 0xFF || nID < 0)
	{
		SafeDelete(md);
//		WCHAR msg[64];
//		SafePrintf(msg, 64, L"範囲外のIDが設定されているキャラスクリプトが見つかりました(ID:%d)", nID);
//		MessageBox(NULL, msg, L"script loader", MB_OK);
		return 0;
	}
	scrinfo.send = (BOOL)luaResults.GetBool(1);
#if CHARA_TEST
	switch (nID)
	{
	case -1:
		{
			luaParams.Clear();
			luaResults.Clear();
			// キャラID取得
			luaParams.Number(nCharaIndex).Number(TEST_CODE);
			if (!pLuah->CallFunc("CheckCharaDebug", &luaResults, 1, &luaParams))
			{
				SafeDelete(md);
				WCHAR test[64];
				SafePrintf(test, 64, L"スクリプトが不正でした(%d)", nID);
				MessageBox(NULL,test, L"loader", MB_OK);
				return -1;
			}
			nID = (int)luaResults.GetNumber(0);
			if (nID == -1)
			{
				WCHAR test[64];
				SafePrintf(test, 64, L"スクリプトが不正です(%d)", nID);
				MessageBox(NULL,test, L"loader", MB_OK);
				return -1;
			}
		}
		break;
	default:
		break;
	}
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_ID", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	nID = (int)luaResults.GetNumber(0);
	luaParams.Clear();
	luaResults.Clear();
#endif
	if (nID == -1)
	{
		SafeDelete(md);
		WCHAR msg[64];
		SafePrintf(msg, 64, L"不正なキャラスクリプトが見つかりました(ID:%d)", nID);
		MessageBox(NULL, msg, L"script loader", MB_OK);
		return -1;
	}
	// 登?済みIDか確認
	std::map < int, TCHARA_SCR_INFO >::iterator itfind = pDestMapScrInfo->find(nID);
	if (itfind != pDestMapScrInfo->end())
	{
		SafeDelete(md);
		WCHAR msg[64];
		SafePrintf(msg, 64, L"キャラスクリプトに重複IDが見つかりました(ID:%d)", nID);
		MessageBox(NULL, msg, L"script loader", MB_OK);
		return -1;
	}
	scrinfo.ID = nID;

	luaParams.Clear();
	luaResults.Clear();
	// キャラ用テクス?ャフ?イル取得
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_TexFile", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}

	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.tex_path, sizeof(scrinfo.tex_path));
	SafeMemCopy(scrinfo.tex_path, wstr, wstr_len*sizeof(WCHAR), _MAX_PATH*2*sizeof(WCHAR));
//	wcsncpy_s(scrinfo.tex_path, _MAX_PATH*2, wstr, wstr_len);
	SafeDeleteArray(wstr);

	path = exedir;
	path += luaResults.GetString(0);
	if (!common::EncodeFileHash(path.c_str(), md))
	{
		SafeDelete(md);
		char log[320];
		SafePrintfA(log, 320, "使用フ?イル(%s)が見つかりませんでした。\nロ?ドをスキップします。");
		MessageBoxA(NULL, log, "キャラスクリプトロ?ドエラ?", MB_OK);
		SafePrintfA(log, 320, "スクリプトフ?イルを削除しますか？\n%s", path.c_str());
		if ( MessageBoxA(NULL, log, "キャラスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
			DeleteFileA(str_module.c_str());
		return 0;
	}

	luaParams.Clear();
	luaResults.Clear();
	// キャラID取得
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_Name", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.name, sizeof(scrinfo.name));
	SafeMemCopy(scrinfo.name, wstr, wstr_len*sizeof(WCHAR), MAX_CHARACTER_NAME*sizeof(WCHAR));
//	wcsncpy_s(scrinfo.name, MAX_CHARACTER_NAME, wstr, wstr_len);
	SafeDeleteArray(wstr);

	// アイコン情報取得
	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_Icon", &luaResults, 4, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.rec_sel_icon.left = (int)luaResults.GetNumber(0);
	scrinfo.rec_sel_icon.top = (int)luaResults.GetNumber(1);
	scrinfo.rec_sel_icon.right = (int)luaResults.GetNumber(2);
	scrinfo.rec_sel_icon.bottom = (int)luaResults.GetNumber(3);

	// 弾種類数取得
	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_GetBltTypeCount", &luaResults, 2, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.blt_type_count = min(MAX_CHARA_BULLET_TYPE, (int)luaResults.GetNumber(0));
	scrinfo.blt_sel_count = min(MAX_CHARA_BULLET_TYPE, (int)luaResults.GetNumber(1));

	for (int i=0;i<scrinfo.blt_type_count;i++)
	{
		// 弾種取得
		luaParams.Clear();
		luaResults.Clear();
		luaParams.Number(nCharaIndex).Number(i);
		if (!pLuah->CallFunc("getChara_GetBltInfo", &luaResults, 11, &luaParams))
		{
			SafeDelete(md);
			const char * pc = pLuah->GetErr();
			MessageBoxA(NULL, pc, "loader", MB_OK);
			return -1;
		}
		scrinfo.blt_info[i].blt_atk = (int)luaResults.GetNumber(0);
		scrinfo.blt_info[i].blt_delay = (int)luaResults.GetNumber(1);
		scrinfo.blt_info[i].rec_blt_icon.left = (int)luaResults.GetNumber(2);
		scrinfo.blt_info[i].rec_blt_icon.top = (int)luaResults.GetNumber(3);
		scrinfo.blt_info[i].rec_blt_tex.left = (int)luaResults.GetNumber(4);
		scrinfo.blt_info[i].rec_blt_tex.top = (int)luaResults.GetNumber(5);
		scrinfo.blt_info[i].rec_blt_tex.right = (int)luaResults.GetNumber(6);
		scrinfo.blt_info[i].rec_blt_tex.bottom = (int)luaResults.GetNumber(7);
		scrinfo.blt_info[i].hit_range = (int)luaResults.GetNumber(8);
		scrinfo.blt_info[i].bomb_range = (int)luaResults.GetNumber(9);
		scrinfo.blt_info[i].ephemeris = (BOOL)luaResults.GetBool(10);

		scrinfo.blt_info[i].rec_blt_icon.right = BLT_ICON_SIZE_W;
		scrinfo.blt_info[i].rec_blt_icon.bottom = BLT_ICON_SIZE_W;
	}

	// SPEED取得
	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_Move", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.move = min(MAX_CHARACTER_MOVE_PARAM, (int)luaResults.GetNumber(0));

	// Delay取得
	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_Delay", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.delay = max(MIN_GAME_CHARA_DELAY_VALUE, (int)luaResults.GetNumber(0));

	luaParams.Clear();
	luaResults.Clear();
	// キャラテクス?ャ情報取得
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_DrawSize", &luaResults, 2, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.draw_w = (int)luaResults.GetNumber(0);
	scrinfo.draw_h = (int)luaResults.GetNumber(1);

	luaParams.Clear();
	luaResults.Clear();
	// キャラテクス?ャ情報取得
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_TexChr", &luaResults, 5, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.rec_tex_chr.left = (int)luaResults.GetNumber(0);
	scrinfo.rec_tex_chr.top = (int)luaResults.GetNumber(1);
	scrinfo.rec_tex_chr.right = (int)luaResults.GetNumber(2);
	scrinfo.rec_tex_chr.bottom = (int)luaResults.GetNumber(3);
	scrinfo.tex_chr_num = (int)luaResults.GetNumber(4);

	luaParams.Clear();
	luaResults.Clear();
	// キャラテクス?ャ情報(弾発射用)取得
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_TexTrg", &luaResults, 5, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.rec_tex_trg.left = (int)luaResults.GetNumber(0);
	scrinfo.rec_tex_trg.top = (int)luaResults.GetNumber(1);
	scrinfo.rec_tex_trg.right = (int)luaResults.GetNumber(2);
	scrinfo.rec_tex_trg.bottom = (int)luaResults.GetNumber(3);
	scrinfo.tex_trg_num = (int)luaResults.GetNumber(4);

	// MaxHP取得
	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_MaxHP", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.max_hp = max(0, (int)luaResults.GetNumber(0));

	// 発射角度範囲
	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_AngleRange", &luaResults, 3, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.angle_range_min = min(359, REVERSE_ANGLE+(int)luaResults.GetNumber(0));
	scrinfo.angle_range_max = min(359, REVERSE_ANGLE+(int)luaResults.GetNumber(1));
	scrinfo.shot_h = min(127, max(-128, (int)luaResults.GetNumber(2)));

	luaParams.Clear();
	luaResults.Clear();
	// スペルカ?ド情報
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_GetSCBaseInfo", &luaResults, 10, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.sc_info.blt_atk = (int)luaResults.GetNumber(0);
	scrinfo.sc_info.blt_delay = (int)luaResults.GetNumber(1);
	scrinfo.sc_info.rec_blt_icon.left = (int)luaResults.GetNumber(2);
	scrinfo.sc_info.rec_blt_icon.top = (int)luaResults.GetNumber(3);
	scrinfo.sc_info.rec_blt_tex.left = (int)luaResults.GetNumber(4);
	scrinfo.sc_info.rec_blt_tex.top = (int)luaResults.GetNumber(5);
	scrinfo.sc_info.rec_blt_tex.right = (int)luaResults.GetNumber(6);
	scrinfo.sc_info.rec_blt_tex.bottom = (int)luaResults.GetNumber(7);
	scrinfo.sc_info.hit_range = (int)luaResults.GetNumber(8);
	scrinfo.sc_info.bomb_range = (int)luaResults.GetNumber(9);
	scrinfo.sc_info.rec_blt_icon.right = BLT_ICON_SIZE_W;
	scrinfo.sc_info.rec_blt_icon.bottom = BLT_ICON_SIZE_W;

	luaParams.Clear();
	luaResults.Clear();
	// スペルカ?ド情報
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_GetSCPartInfo", &luaResults, 3, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	if ( !luaResults.GetWString(0, &wstr, &wstr_len) )
	{
		SafeDelete(md);
		SafeDeleteArray(wstr);
		MessageBox(NULL, wstr, L"lua", MB_OK);
		return -1;
	}
	ZeroMemory(scrinfo.sc_info.name, sizeof(scrinfo.sc_info.name));
	SafeMemCopy(scrinfo.sc_info.name, wstr, wstr_len*sizeof(WCHAR), MAX_SPELLCARD_NAME*sizeof(WCHAR));
	SafeDeleteArray(wstr);
	scrinfo.sc_info.exp = (int)luaResults.GetNumber(1);
	scrinfo.sc_info.max_exp= (int)min(luaResults.GetNumber(2),5000);

	luaParams.Clear();
	luaResults.Clear();
	luaParams.Number(nCharaIndex);
	if (!pLuah->CallFunc("getChara_SEFilesCount", &luaResults, 1, &luaParams))
	{
		SafeDelete(md);
		const char * pc = pLuah->GetErr();
		MessageBoxA(NULL, pc, "loader", MB_OK);
		return -1;
	}
	scrinfo.se_count = (int)luaResults.GetNumber(0);
	// 音声のハッシュ値を取得 //
#if 1	// 音声はサ?バ間処理に影響ないのでハッシュ値?ェックしない
	int nSECount = scrinfo.se_count;
	for (int i=0;i<nSECount;i++)
	{
		luaParams.Clear();
		luaResults.Clear();
		luaParams.Number(nCharaIndex).Number(i);
		if (!pLuah->CallFunc("getChara_SEFile", &luaResults, 1, &luaParams))
		{
			SafeDelete(md);
			const char * pc = pLuah->GetErr();
			MessageBoxA(NULL, pc, "loader", MB_OK);
			return -1;
		}
		path = exedir;
		path += luaResults.GetString(0);
		if (!common::EncodeFileHash(path.c_str(), md))
		{
			SafeDelete(md);
			char log[321];
			SafePrintfA(log, 320, "使用フ?イル(%s)が見つかりませんでした。\nロ?ドをスキップします。", path.c_str());
			MessageBoxA(NULL, log, "キャラスクリプトロ?ドエラ?", MB_OK);
//			SafePrintfA(log, 320, "スクリプトフ?イルを削除しますか？\n%s", str_module.c_str());
//			if ( MessageBoxA(NULL, log, "キャラスクリプトロ?ドエラ?", MB_YESNO)  == IDYES)
//				DeleteFileA(str_module.c_str());
			return 0;
		}
	}
#endif

	// ハッシュ値
	md->finish();
	ZeroMemory(scrinfo.md5, sizeof(char)*MD5_LENGTH);
	memcpy(scrinfo.md5, (char*)md->to_string().c_str(), sizeof(char)*MD5_LENGTH);
	SafeDelete(md);

#ifdef _DEBUG
	WCHAR wsHash[32+1];
	MultiByteToWideChar(CP_THREAD_ACP, 0, scrinfo.md5, 32+1, wsHash, 32+1);

	WCHAR wsmd[80];
	SafePrintf(wsmd,80, L"#define HASH_CHARA%02d\t\t\"%s\"\n", nID, wsHash);
	OutputDebugStr(wsmd);
	
#endif

	// スクリプト番号
	scrinfo.scr_index = num;
#if SCR_HASH_CHECK
	scrinfo.flg = FALSE;
#if TRIAL	// ロ?ド時にハッシュ値?ェック
#ifndef _DEBUG
	char *csTriChk = NULL;
	switch (nID)
	{
	case 0:	csTriChk = HASH_CHARA00;		break;
	case 1:	csTriChk = HASH_CHARA01;		break;
	case 2:	csTriChk = HASH_CHARA02;		break;
	case 4:	csTriChk = HASH_CHARA04;		break;
#if	CHARA_TEST
	case 6:	csTriChk = HASH_CHARA06;		break;
	case 9:	csTriChk = HASH_CHARA09;		break;
//	case 3:	csTriChk = HASH_CHARA03;		break;
//	case 5:	csTriChk = HASH_CHARA05;		break;
//	case 7:	csTriChk = HASH_CHARA07;		break;
	case 8:	csTriChk = HASH_CHARA08;		break;
#endif
	default:
		return -1;	// 他IDは追加しない
	}
	if (strcmp(scrinfo.md5, csTriChk) != 0)
		return -1;
#else
	switch (nID)
	{
	case 0:break;
	case 1:break;
	case 2:break;
	case 4:break;
#if	CHARA_TEST
//	case 3:break;
//	case 5:break;
	case 6:break;
	case 9:break;
//	case 7:break;
	case 8:break;
#endif
	default:
		return 1;	// 他IDは追加しない
	}
#endif

#else	// TRIAL
#if RTRIAL
	if (nID == 0)
	{
		if (strcmp(scrinfo.md5, HASH_CHARA00) != 0)
			return -1;
	}
#endif
#endif
#else
	scrinfo.flg = TRUE;
#endif
	pDestMapScrInfo->insert( std::map<int, TCHARA_SCR_INFO>::value_type( nID, scrinfo ) );

	luaParams.Clear();
	luaResults.Clear();

	return 1;
}

int common::scr::GetCharaFileHash(LuaHelper* pLuah, int nID, int nFileNo, char* md5, WCHAR* path, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection)
{
	int ret = -1;
	md5[0] = NULL;
	WCHAR* wpath = NULL;
	TCHARA_SCR_INFO* pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(nID, pMapScrInfo);
	if(!pCharaScrInfo)	return -1;
	if (!pCharaScrInfo->send) return -2;
	switch (nFileNo)
	{
	case 0:	wpath = pCharaScrInfo->scr_path;	wcscpy(path, wpath);	break;
	case 1: wpath = pCharaScrInfo->tex_path;	wcscpy(path, wpath);	break;
	default:
		if (pCharaScrInfo->se_count <= nFileNo-2)	return 0;
		return common::scr::GetCharaSEHash(pLuah, pCharaScrInfo->scr_index, nFileNo, md5, path, pCriticalSection);
	}
	int len = 0;
	char* str;
	if (WCharToMB(&str, &len, wpath) )
	{
		char exe_dir[_MAX_PATH*2+1];
		GetModuleDirA(exe_dir, _MAX_PATH*2+1);
		std::string mdstr = exe_dir;
		mdstr += str;
		SafeDeleteArray(str);
		clx::md5* md = new clx::md5();
		md->reset();
		if (EncodeFileHash(mdstr.c_str(), md))
			ret = 1;
		SafeDeleteArray(str);
		if (ret == 1)
		{
			md->finish();
			memcpy(md5, md->to_string().c_str(), MD5_LENGTH);
		}
		SafeDelete(md);
	}
	return ret;
}

BOOL common::scr::GetCharaSEHash(LuaHelper* pLuah, int nCharaIndex, int nFileNo, char* md5, WCHAR* path,CCriticalSection* pCriticalSection)
{
	char exedir[_MAX_PATH*2+1];
	std::string str;

	LuaFuncParam luaParams, luaResults;
	luaParams.Number(nCharaIndex).Number(nFileNo-2);
	if (!common::scr::CallLuaFunc(pLuah, "getChara_SEFile", &luaResults, 1, &luaParams, pCriticalSection))
		return FALSE;
	clx::md5* md = new clx::md5();
	md->reset();
	GetModuleDirA(exedir, _MAX_PATH*2+1);
	str = exedir;
	str += luaResults.GetString(0);
	WCHAR* wpath;
	int	 nPathLen = 0;
	if (luaResults.GetWString(0, &wpath, &nPathLen))
	{
		wcscpy(path, wpath);
		SafeDeleteArray(wpath);
	}

	BOOL ret = common::EncodeFileHash(str.c_str(), md);
	if (ret)
	{
		md->finish();
		memcpy(md5, md->to_string().c_str(), MD5_LENGTH);
	}
	SafeDelete(md);
	return ret;
}

BOOL common::scr::GetStageSEHash(LuaHelper* pLuah, int nStageIndex, int nFileNo, char* md5, WCHAR* path,CCriticalSection* pCriticalSection)
{
	char exedir[_MAX_PATH*2+1];
	std::string str;

	LuaFuncParam luaParams, luaResults;
	luaParams.Number(nStageIndex).Number(nFileNo-4);
	if (!common::scr::CallLuaFunc(pLuah, "getStage_SEFile", &luaResults, 1, &luaParams, pCriticalSection))
		return FALSE;
	clx::md5* md = new clx::md5();
	md->reset();
	GetModuleDirA(exedir, _MAX_PATH*2+1);
	str = exedir;
	str += luaResults.GetString(0);
	WCHAR* wpath;
	int	 nPathLen = 0;
	if (luaResults.GetWString(0, &wpath, &nPathLen))
	{
		wcscpy(path, wpath);
		SafeDeleteArray(wpath);
	}

	BOOL ret = common::EncodeFileHash(str.c_str(), md);
	if (ret)
	{
		md->finish();
		memcpy(md5, md->to_string().c_str(), MD5_LENGTH);
	}
	SafeDelete(md);
	return ret;
}

BOOL common::scr::GetCharaFilePath(WCHAR* path, LuaHelper* pLuah, int nID, int nFileNo, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection)
{
	WCHAR exedir[_MAX_PATH*2+1];
	std::wstring str;
	WCHAR* wpath = NULL;
	TCHARA_SCR_INFO* pCharaScrInfo = common::scr::FindCharaScrInfoFromCharaType(nID, pMapScrInfo);
	if(!pCharaScrInfo)	return FALSE;
	int nCharaIndex = pCharaScrInfo->scr_index;

	switch (nFileNo)
	{
	case 0:	wpath = pCharaScrInfo->scr_path;	break;
	case 1: wpath = pCharaScrInfo->tex_path;	break;
	default:
		if (pCharaScrInfo->se_count <= nFileNo-2)
			return FALSE;
		else
		{
			WCHAR exedir[_MAX_PATH*2+1];
			WCHAR* sefile;
			int nSeFileLen = 0;
			std::wstring str;

			LuaFuncParam luaParams, luaResults;
			luaParams.Clear();
			luaResults.Clear();
			luaParams.Number(nCharaIndex).Number(nFileNo-2);
			if (!common::scr::CallLuaFunc(pLuah, "getChara_SEFile", &luaResults, 1, &luaParams, pCriticalSection))
				return FALSE;
			common::GetExeDir(exedir);
			str = exedir;
			luaResults.GetWString(0,&sefile, &nSeFileLen);
			str += sefile;
			SafeDeleteArray(sefile);
			wcscpy(path, str.c_str());
			return TRUE;
		}
	}
	GetModuleDir(exedir, _MAX_PATH*2+1);
	str = exedir;
	str += wpath;
	wcscpy(path, str.c_str());
	return TRUE;
}

BOOL common::scr::GetStageFilePath(WCHAR* path, LuaHelper* pLuah, int nID, int nFileNo, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection)
{
	WCHAR exedir[_MAX_PATH*2+1];
	std::wstring str;
	WCHAR* wpath = NULL;
	TSTAGE_SCR_INFO* pStageScrInfo = common::scr::FindStageScrInfoFromStageID(nID, pMapScrInfo);
	if(!pStageScrInfo)	return FALSE;
	int nStageIndex = pStageScrInfo->scr_index;

	switch (nFileNo)
	{
	case 0:	wpath = pStageScrInfo->scr_path;		break;
	case 1: wpath = pStageScrInfo->stage.path;		break;
	case 2: wpath = pStageScrInfo->bg.path;			break;
	case 3: wpath = pStageScrInfo->thumnail.path;	break;
	case 4: wpath = pStageScrInfo->bgm;				break;
	default:
		if (pStageScrInfo->se_count <= nFileNo-4)
			return FALSE;
		else
		{
			WCHAR* sefile;
			int nSeFileLen = 0;
			LuaFuncParam luaParams, luaResults;
			luaParams.Clear();
			luaResults.Clear();
			luaParams.Number(nStageIndex).Number(nFileNo-2);
			if (!common::scr::CallLuaFunc(pLuah, "getStage_SEFile", &luaResults, 1, &luaParams, pCriticalSection))
				return FALSE;
			common::GetExeDir(exedir);
			str = exedir;
			luaResults.GetWString(0,&sefile, &nSeFileLen);
			str += sefile;
			SafeDeleteArray(sefile);
			wcscpy(path, str.c_str());
			return TRUE;
		}
	}
	GetModuleDir(exedir, _MAX_PATH*2+1);
	str = exedir;
	str += wpath;
	wcscpy(path, str.c_str());
	return TRUE;
}

int common::scr::GetStageFileHash(LuaHelper* pLuah, int nID, int nFileNo, char* md5, WCHAR* path, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo, CCriticalSection* pCriticalSection)
{
	int ret = -1;
	md5[0] = NULL;
	WCHAR* wpath = NULL;
	TSTAGE_SCR_INFO* pStageScrInfo = common::scr::FindStageScrInfoFromStageID(nID, pMapScrInfo);
	if(!pStageScrInfo)	return -1;
	if (!pStageScrInfo->send)	return -2;
	switch (nFileNo)
	{
	case 0:	wpath = pStageScrInfo->scr_path;		wcscpy(path, wpath);	break;
	case 1: wpath = pStageScrInfo->stage.path;		wcscpy(path, wpath);	break;
	case 2: wpath = pStageScrInfo->bg.path;			wcscpy(path, wpath);	break;
	case 3: wpath = pStageScrInfo->thumnail.path;	wcscpy(path, wpath);	break;
	case 4: wpath = pStageScrInfo->bgm;				wcscpy(path, wpath);	break;
	default:
		if (pStageScrInfo->se_count <= nFileNo-4)	return 0;
		return common::scr::GetStageSEHash(pLuah, pStageScrInfo->scr_index, nFileNo, md5, path, pCriticalSection);
	}
	int len = 0;
	char* str;
	if (WCharToMB(&str, &len, wpath) )
	{
		char exe_dir[_MAX_PATH*2+1];
		GetModuleDirA(exe_dir, _MAX_PATH*2+1);
		std::string mdstr = exe_dir;
		mdstr += str;
		SafeDeleteArray(str);
		clx::md5* md = new clx::md5();
		md->reset();
		if (EncodeFileHash(mdstr.c_str(), md))
			ret = 1;
		if (ret == 1)
		{
			md->finish();
			memcpy(md5, md->to_string().c_str(), MD5_LENGTH);
		}
		SafeDelete(md);
	}
	return ret;
}

TCHARA_SCR_INFO* common::scr::FindCharaScrInfoFromCharaType(int chara_type, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo)
{
	std::map < int, TCHARA_SCR_INFO >::iterator itfind = pMapScrInfo->find(chara_type); 
	if (itfind == pMapScrInfo->end())
	{
#if _DEBUG
		DXTRACE_MSG(L"スクリプト検索エラ?\nFindCharaScrInfo()");
#endif
		return NULL;
	}
	return &(*itfind).second;
}

TSTAGE_SCR_INFO* common::scr::FindStageScrInfoFromStageID(int stage_id, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo)
{
	std::map < int, TSTAGE_SCR_INFO >::iterator itfind = pMapScrInfo->find(stage_id);
	if (itfind == pMapScrInfo->end())
	{
#if _DEBUG
		DXTRACE_MSG(L"スクリプト検索エラ?\nFindStageScrInfo()");
#endif
		return NULL;
	}
	return &(*itfind).second;
}

TSTAGE_SCR_INFO* common::scr::FindStageScrInfoFromStageIndex(int stage_index, std::map < int, TSTAGE_SCR_INFO >* pMapScrInfo)
{
	if (stage_index >= (int)pMapScrInfo->size() && stage_index < 0) return NULL;

	std::map < int, TSTAGE_SCR_INFO >::iterator it = pMapScrInfo->begin();
	for (int i=0;i<stage_index;i++)
	{
		if (it == pMapScrInfo->end())
		{
#if _DEBUG
			DXTRACE_MSG(L"スクリプト取得エラ?\nFindStageScrInfo()");
#endif
			return NULL;
		}
		it++;
	}
	
	return common::scr::FindStageScrInfoFromStageID((*it).first, pMapScrInfo);
}

// Luaの関数呼び出し
BOOL common::scr::CallLuaFunc(LuaHelper* pLuah, char* func_name, LuaFuncParam* pLuaResults, int nResults, LuaFuncParam* pLuaParams, CCriticalSection* pCriticalSection)
{
	pCriticalSection->EnterCriticalSection_Lua(L'1');
	if (!pLuah->CallFunc(func_name, pLuaResults, nResults, pLuaParams))
	{
		const char * pc = pLuah->GetErr();
		pCriticalSection->LeaveCriticalSection_Lua();
		MessageBoxA(NULL, pc, "lua", MB_OK);
		return FALSE;
	}
	pCriticalSection->LeaveCriticalSection_Lua();
	return TRUE;
}

// スクリプトで発射処理
BOOL common::scr::CallShootingFunc(LuaHelper* pLuah, ptype_session shot_sess, int nProcType, int nBltType, int nShotAngle, int nShotPower, TCHARA_SCR_INFO* pCharaScrInfo, int nCharaObjNo, int nFrame, int nIndicatorAngle, int nIndicatorPower, CCriticalSection* pCriticalSection)
{
	BOOL bNextPhase = FALSE;
	POINT pnt;
#ifdef HOGE
	WCHAR msg[32];
	SafePrintf(&msg[0], 32, L"%d ^ %d\n", nShotAngle, nShotPower);
	OutputDebugStr(msg);
#endif
	// 体の中心より高めで発射させる
	short sHeadAngle = 90;
	if (shot_sess->dir != USER_DIRECTION_LEFT)
		sHeadAngle = 270;
	double dRad = D3DXToRadian( (shot_sess->angle+sHeadAngle)%360);

	double dBodyOffsetX = cos(dRad) * (pCharaScrInfo->shot_h);
	double dBodyOffsetY = sin(dRad) * (pCharaScrInfo->shot_h);
	// ラジアンから各方向を取り出す
	dRad = D3DXToRadian(nShotAngle);
	double dx = cos(dRad);
	double dy = sin(dRad);
	pnt.x = (LONG)(dx * (float)nShotPower) * BLT_VEC_FACT_N;
	pnt.y = (LONG)(dy * (float)nShotPower) * BLT_VEC_FACT_N;

	switch (nProcType)
	{
	case BLT_PROC_TYPE_SCR_CHARA:
		{
			// フレ??0なら弾発射によるディレイ値増加
			if (!nFrame)
			{
				// 連射使用時は一回目のディレイ値は計算しない
				if (!shot_sess->chara_state[CHARA_STATE_DOUBLE_INDEX])
					shot_sess->delay += pCharaScrInfo->blt_info[nBltType].blt_delay;
			}
			int nFirstRange = (CHARA_BODY_RANGE+pCharaScrInfo->blt_info[nBltType].hit_range)+1;
			int nFirstX = (int)(shot_sess->ax+(dx*nFirstRange)-dBodyOffsetX); //;(pScrInfo->rec_tex_chr.right/2));
			int nFirstY = (int)(shot_sess->ay+(dy*nFirstRange)-dBodyOffsetY);//*(pScrInfo->rec_tex_chr.bottom/2));

			// Luaで弾作成
			LuaFuncParam luaParams,luaResults;
			luaParams.Number(pCharaScrInfo->scr_index).Number(nCharaObjNo).Number(nBltType).Number(nFirstX).Number(nFirstY).Number(pnt.x).Number(pnt.y).Number(nShotAngle).Number(nShotPower*BLT_VEC_FACT_N).Number(nFrame).Number(nIndicatorAngle).Number(nIndicatorPower*BLT_VEC_FACT_N);
			if (!common::scr::CallLuaFunc(pLuah, "shot_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection))
				return FALSE;
			if (luaResults.GetBool(0))
				bNextPhase = TRUE;
		}
		break;

	case BLT_PROC_TYPE_SCR_SPELL:
		{
			// フレ??0なら弾発射によるディレイ値増加
			if (!nFrame)
				shot_sess->delay += pCharaScrInfo->sc_info.blt_delay;

			int nFirstRange = (CHARA_BODY_RANGE+pCharaScrInfo->sc_info.hit_range)+1;
			int nFirstX = (int)(shot_sess->ax+(dx*nFirstRange)-dBodyOffsetX); //;(pScrInfo->rec_tex_chr.right/2));
			int nFirstY = (int)(shot_sess->ay+(dy*nFirstRange)-dBodyOffsetY);//*(pScrInfo->rec_tex_chr.bottom/2));

			// Luaで弾作成
			LuaFuncParam luaParams,luaResults;
			luaParams.Number(pCharaScrInfo->scr_index).Number(nCharaObjNo).Number(nFirstX).Number(nFirstY).Number(pnt.x).Number(pnt.y).Number(nShotAngle).Number(nShotPower*BLT_VEC_FACT_N).Number(nFrame).Number(nIndicatorAngle).Number(nIndicatorPower*BLT_VEC_FACT_N);
			if (!common::scr::CallLuaFunc(pLuah, "shot_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection))
				return FALSE;
			if (luaResults.GetBool(0))
				bNextPhase = TRUE;
			break;
		}
	}

	return bNextPhase;
}

void common::session::GetSessionName(ptype_session sess, WCHAR name[MAX_USER_NAME+1])
{
	ZeroMemory(name, sizeof(WCHAR)*(MAX_USER_NAME+1));
	SafeMemCopy(name, sess->name, sess->name_len, MAX_USER_NAME*sizeof(WCHAR));
}


///////////////////////////////////////////////////////////
// chr
E_MOVE_STAGE_RESULT common::chr::MoveOnStage(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo)
{
	BOOL bCanMove = TRUE;
	E_MOVE_STAGE_RESULT ret = MOVE_STAGE_RESULT_NONE;
	D3DXVECTOR2 pos = D3DXVECTOR2(sess->ax, sess->ay);
	short sPx = sess->ax;
	short sPy = sess->ay;
	// 足場確認（着地）
	switch (CharaGetDown(pStage, &sPx, &sPy, pMapScrInfo))
	{
	case CHARA_GETDOWN_RESULT_UP_GETDOWN:	// 上に接地
		// 着地位置
		bCanMove = FALSE;
		sess->ay = sPy;
		sess->vy = 0;
		ret = MOVE_STAGE_RESULT_VEC_CHANGE;
		// 着地
		if (!GetDownStage(pStage, sess))
		{
			// 一歩先は地面なし	
		}
		break;
	case CHARA_GETDOWN_RESULT_NOW_GETDOWN:
			ret = MOVE_STAGE_RESULT_VEC_CHANGE;
	case CHARA_GETDOWN_RESULT_ON_GETDOWN:
	{
		// 着地位置
		sess->ay = sPy;
		// いま着地
		if (sess->vy != 0)
		{
			bCanMove = FALSE;
			sess->vy = 0;
			ret = MOVE_STAGE_RESULT_VEC_CHANGE;
			// 着地
			if (!GetDownStage(pStage, sess))
			{
				// 一歩先は地面なし	
//				ret = MOVE_STAGE_RESULT_DROP;	// 着地画面外
			}
		}
		break;
	}
	case CHARA_GETDOWN_RESULT_DOWN:
		{
			// 落下
			ret = MOVE_STAGE_RESULT_MOVED;
			bCanMove = FALSE;
			sess->ay += CHARA_DROP_SPEED;
			PCHARA_SCR_INFO pCharaScrInfo = (PCHARA_SCR_INFO)sess->scrinfo;
			// ステ?ジ外確認
			if (sess->ay >= (pStage->GetStageHeight()+pCharaScrInfo->rec_tex_chr.bottom/2) )
			{
				sess->ay = (short)(pStage->GetStageHeight()+pCharaScrInfo->rec_tex_chr.bottom);
				sess->vy = 0;
				ret = MOVE_STAGE_RESULT_DROP;
			}
			else if (sess->vy == 0)		// 落下はじめ
			{
				sess->vy = CHARA_DROP_SPEED;
				ret = MOVE_STAGE_RESULT_VEC_CHANGE;
			}
			break;
		}
	default:	break;
	}
	// 地面の横移動
	if (bCanMove && sess->vx != 0)
	{
		ret = MOVE_STAGE_RESULT_MOVED;
		MoveGround(pStage, sess, pMapScrInfo);
	}

	return ret;
}

E_CHARA_GETDOWN_RESULT common::chr::CharaGetDown(CMainStage* pStage, short* px, short* py, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo)
{
	E_CHARA_GETDOWN_RESULT ret = CHARA_GETDOWN_RESULT_DOWN;
	D3DXVECTOR2 first_pos = D3DXVECTOR2(*px, *py);
	D3DXVECTOR2 pos = first_pos;
	D3DXVECTOR2 dest_my_pos;
	D3DXVECTOR2 dest_ground_pos;
	// ステ?ジに当たっているか
	BOOL h = pStage->IsSomeHit(&dest_my_pos, &first_pos/*, &hline */,CHARA_BODY_RANGE);	// 初期位置が地中か
	if (h)
	{
		// 地中に埋まっている場合、上方向に地面を探す
		h = pStage->FindSomeGroundUp(&dest_my_pos, &dest_ground_pos, &first_pos, CHARA_BODY_RANGE);
		if (h)
		{
			// 接地位置に
			short dy = (short) (dest_my_pos.y>0)?(short)dest_my_pos.y:((short)(dest_my_pos.y-0.5));
			*py = dy;
			ret = CHARA_GETDOWN_RESULT_UP_GETDOWN;
		}
		else
			OutputDebugStr(L"!NoGroudUp");
	}
	else
	{
		if (pStage->FindGroundDown(&dest_my_pos, &dest_ground_pos, &first_pos, CHARA_BODY_RANGE))
		{
			short dy = (short) (dest_my_pos.y>0)?(short)dest_my_pos.y:((short)(dest_my_pos.y-0.5));
			if (*py == dy)
				ret = CHARA_GETDOWN_RESULT_ON_GETDOWN;
			else if ((*py)+CHARA_DROP_SPEED >= dy)	// いま接地
			{
				*py = dy;
				ret = CHARA_GETDOWN_RESULT_NOW_GETDOWN;
			}
		}
	}
	return ret;
}

E_MOVE_STAGE_RESULT common::chr::MoveStage(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo)
{
//	// 落下?したやつは飛ばす
// ?ェック済み
//	if (sess->obj_state == OBJ_STATE_MAIN_DROP)	return MOVE_STAGE_RESULT_DROP;
	BOOL bCanMove = TRUE;
	E_MOVE_STAGE_RESULT ret = MOVE_STAGE_RESULT_NONE;
	D3DXVECTOR2 pos = D3DXVECTOR2(sess->ax, sess->ay);
	short sPx = sess->ax;
	short sPy = sess->ay;
	// 足場確認（着地）
	switch (GetDown(pStage, &sPx, &sPy, pMapScrInfo))
	{
	case GETDOWN_RESULT_UP:
		// 着地位置
		sess->ay = sPy;
		bCanMove = FALSE;
		sess->vy = 0;
		ret = MOVE_STAGE_RESULT_VEC_CHANGE;
		// 着地
		if (!GetDownStage(pStage, sess))
		{
			// 一歩先は地面なし	
		}
		break;
	case GETDOWN_RESULT_NOW:
	case GETDOWN_RESULT_ON:
	{
		// 着地位置
		sess->ay = sPy;
		// いま着地
		if (sess->vy != 0)
		{
			bCanMove = FALSE;
			sess->vy = 0;
			ret = MOVE_STAGE_RESULT_VEC_CHANGE;
			// 着地
			if (!GetDownStage(pStage, sess))
			{
				// 一歩先は地面なし	
//				ret = MOVE_STAGE_RESULT_DROP;	// 着地画面外
			}
		}
		break;
	}
	case GETDOWN_RESULT_DOWN:
		{
			// 落下
			ret = MOVE_STAGE_RESULT_MOVED;
			bCanMove = FALSE;
			sess->ay += CHARA_DROP_SPEED;
			PCHARA_SCR_INFO pCharaScrInfo = (PCHARA_SCR_INFO)sess->scrinfo;
			// ステ?ジ外確認
			if (sess->ay >= (pStage->GetStageHeight()+pCharaScrInfo->rec_tex_chr.bottom/2) )
			{
				sess->ay = (short)(pStage->GetStageHeight()+pCharaScrInfo->rec_tex_chr.bottom);
				sess->vy = 0;
				ret = MOVE_STAGE_RESULT_DROP;
			}
			else if (sess->vy == 0)		// 落下はじめ
			{
				sess->vy = CHARA_DROP_SPEED;
				ret = MOVE_STAGE_RESULT_VEC_CHANGE;
			}
			break;
		}
	default:	break;
	}
	// 地面の横移動
	if (bCanMove && sess->vx != 0)
	{
		ret = MOVE_STAGE_RESULT_MOVED;
		MoveGround(pStage, sess, pMapScrInfo);
	}

	return ret;
}

// return:TRUE = 接地
E_GETDOWN_RESULT common::chr::GetDown(CMainStage* pStage, short* px, short* py, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo)
{
	E_GETDOWN_RESULT ret = GETDOWN_RESULT_DOWN;
	D3DXVECTOR2 first_pos = D3DXVECTOR2(*px, *py);
	D3DXVECTOR2 pos = first_pos;
	D3DXVECTOR2 dest_my_pos;
	D3DXVECTOR2 dest_ground_pos;
	// 地面が下にあるか
	if ( pStage->FindGround(&dest_my_pos, &dest_ground_pos, &pos, CHARA_BODY_RANGE) )
	{
		short my = *py;
		short dy = (short) (dest_my_pos.y>0)?(short)dest_my_pos.y:((short)(dest_my_pos.y-0.5));
//		short dy = (short) dest_my_pos.y;
		// 既に接地している
		if (my > dy)
		{
			*py = dy+2;//(CHARA_BODY_RANGE/2)+1;
			ret = GETDOWN_RESULT_UP;
		}
		else if (my == dy)
		{
			ret = GETDOWN_RESULT_ON;
		}
		else if (my+CHARA_DROP_SPEED >= dy)	// いま接地
		{
			*py = dy;
			ret = GETDOWN_RESULT_NOW;
		}
	}
	return ret;
}

E_CHECK_GROUND_RESULT common::chr::CheckGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo)
{
	E_CHECK_GROUND_RESULT ret = CHECK_GROUND_RESULT_GROUND;
	D3DXVECTOR2 first_pos = D3DXVECTOR2( (float)sess->ax, (float)sess->ay);
	D3DXVECTOR2 pos = first_pos;
	D3DXVECTOR2 dest_my_pos;
	D3DXVECTOR2 dest_ground_pos;
	// 地面が下にあるか
	if ( pStage->FindGround(&dest_my_pos, &dest_ground_pos,  &pos, CHARA_BODY_RANGE) )
	{
		short y = (short)dest_my_pos.y;
		// 地面
		if (sess->ay == y)
		{
			ret = CHECK_GROUND_RESULT_GROUND;			
		}
		// 1個下より先が地面なら落ちるだけ
		else if (sess->ay+CHARA_DROP_SPEED < y)
		{
			ret = (sess->vy == 0)?CHECK_GROUND_RESULT_FALLDOWN:CHECK_GROUND_RESULT_FALLING;
			// 下?を超える場合、落下?
			TCHARA_SCR_INFO* pScrInfo = common::scr::FindCharaScrInfoFromCharaType(sess->chara_type, pMapScrInfo);
			if (sess->ay >= (pStage->GetStageHeight()+pScrInfo->rec_tex_chr.bottom/2) )
				ret = CHECK_GROUND_RESULT_DROPOUT;
		}
		else if (sess->ay+CHARA_DROP_SPEED >= y) // 地面上に接地
		{
			sess->ay = y;
			ret = CHECK_GROUND_RESULT_GETDOWN;
		}
	}
	else
	{
		ret = (sess->vy == 0)?CHECK_GROUND_RESULT_FALLDOWN:CHECK_GROUND_RESULT_FALLING;
		// 地面が無い、ステ?ジ外確認
		TCHARA_SCR_INFO* pScrInfo = common::scr::FindCharaScrInfoFromCharaType(sess->chara_type, pMapScrInfo);
		if (sess->ay >= (pStage->GetStageHeight()+pScrInfo->rec_tex_chr.bottom/2) )
			ret = CHECK_GROUND_RESULT_DROPOUT;
	}
	return ret;
}

// 着地後の傾きを取得
BOOL common::chr::GetDownStage(CMainStage* pStage, ptype_session sess)
{
	int dirx = sess->vx;
	if (sess->vx == 0)
		dirx = (sess->dir==USER_DIRECTION_LEFT)?-1:1;

	D3DXVECTOR2 first_pos = D3DXVECTOR2( (float)sess->ax, (float)sess->ay);
	D3DXVECTOR2 pos = first_pos;
	D3DXVECTOR2 dest_my_pos;
	D3DXVECTOR2 dest_ground_pos;
	D3DXVECTOR2 move_pos = first_pos;
	int nAngle=sess->angle;

	pos.x += dirx;
	// 地面を探す
	if ( pStage->FindGroundDown(&dest_my_pos, &dest_ground_pos,  &pos, CHARA_BODY_RANGE) )
	{
		// 探した地面と自分の位置の角度を確認
		D3DXVECTOR2 vec;
		vec = dest_my_pos - first_pos;
		// 角度取得
		nAngle = (GetAngle(vec.x, vec.y)+REVERSE_ANGLE)%360;
		if ( dirx > 0)
			nAngle = min( 269, max(90,nAngle));
		else if ( dirx < 0)
		{
			if ( nAngle > 90 && nAngle <= 180)
				nAngle = 90;
			else if ( nAngle > 180 && nAngle < 270)
				nAngle = 270;
		}
	}
	else
		return FALSE;
	sess->angle = (short)nAngle;
	return TRUE;
}

// 0位置は設置、地面移動
E_MOVE_GROUND_RESULT common::chr::MoveGround(CMainStage* pStage, ptype_session sess, std::map < int, TCHARA_SCR_INFO >* pMapCharaScrInfo)
{
	E_MOVE_GROUND_RESULT ret = MOVE_GROUND_RESULT_NONE;
	if (sess->vx == 0)	return ret;

	D3DXVECTOR2 first_pos = D3DXVECTOR2( (float)sess->ax, (float)sess->ay);
	D3DXVECTOR2 pos = first_pos;
	D3DXVECTOR2 dest_my_pos;
	D3DXVECTOR2 dest_ground_pos;
	D3DXVECTOR2 move_pos = first_pos;
	int nCharaAngle = sess->angle;
	int nAngle=0;
	int nTotalAngle = 0;
	BOOL bCanMoveFlg = TRUE;
	short sPx = sess->ax+sess->vx;
	short sPy = sess->ay;
	// 一歩動いたところで接地処理
//	switch (GetDown(pStage, &sPx, &sPy, pMapCharaScrInfo))
	switch (CharaGetDown(pStage, &sPx, &sPy, pMapCharaScrInfo))
	{
//	case GETDOWN_RESULT_DOWN:
	case CHARA_GETDOWN_RESULT_DOWN:
		// 接地できない場合、空中に移動しておく
		sess->ax = sPx;
		return MOVE_GROUND_RESULT_FALL;
	default:		break;
	}
	int nLoopCount = 0;
	for (int i=0;i<CHARA_ACROSS_GROUND_LENGTH;i++)
	{
		pos.x += sess->vx;
		// 移動範囲??ェック
		if (pos.x < 0/*CHARA_BODY_RANGE*/ || pos.x >= pStage->GetStageWidth()/*-CHARA_BODY_RANGE*/)
		{
			bCanMoveFlg = FALSE;
			break;
		}
		// 地面を探す
		if ( pStage->FindGround(&dest_my_pos, &dest_ground_pos,  &pos, CHARA_BODY_RANGE) )
		{
			// 探した地面と自分の位置の角度を確認
			D3DXVECTOR2 vec;
			vec = dest_my_pos - first_pos;
			// 高すぎる
			if (dest_ground_pos.y < first_pos.y)
			{
				bCanMoveFlg = FALSE;
				break;
			}
			// 角度取得
			if (vec.x != 0 || vec.y != 0)
			{
				nAngle = GetAngle(vec.x, vec.y);
				if ( sess->vx > 0)	// →
				{
					nAngle = REVERSE_ANGLE-((nAngle+90)%360);
					if ( nAngle < 0)
					{
						bCanMoveFlg = FALSE;
						break;
					}
					nTotalAngle += /*(nAngle>180)?180:*/nAngle;
				}
				else if ( sess->vx < 0)	// ←
				{
					nAngle = ((nAngle+270)%360);
					if ( nAngle > 180)
					{
						bCanMoveFlg = FALSE;
						break;
					}
					nTotalAngle += /*(nAngle>180)?0:*/nAngle;
				}
			}
			// 移動後の情報を覚えておく
			if (i==0)
			{
				move_pos = dest_my_pos;
				D3DXVECTOR2 vecAngle = dest_my_pos - dest_ground_pos;
				nCharaAngle = nAngle;
			}
		}
		nLoopCount++;
	}
	double dAverageAngle = (double)nCharaAngle;
	if (bCanMoveFlg)
	{
		if (nTotalAngle)
			dAverageAngle = (double)nTotalAngle / (double)nLoopCount;
		// またげない凸や壁がある場合
		if (dAverageAngle > 154)
			bCanMoveFlg = FALSE;
		// 座標変更
		if (bCanMoveFlg)
		{
			ret = MOVE_GROUND_RESULT_MOVED;
			if (sess->MV_c > 0)
			{
				sess->ax = (int)move_pos.x;
				sess->ay = (int)move_pos.y;
			}
		}
		else
			ret = MOVE_GROUND_RESULT_WALL;
	}
	else
		ret = MOVE_GROUND_RESULT_WALL;

	if ( sess->vx > 0)	// →
	{
		sess->dir = USER_DIRECTION_RIGHT;
		if (bCanMoveFlg)
		{
			if (dAverageAngle >= 180)
				sess->angle = 90;
			else if (dAverageAngle <= 0)
				sess->angle = 180;
			else
				sess->angle = min(269, ((short)(180-dAverageAngle)+90)%360);
		}
	}
	else if ( sess->vx < 0)	// ←
	{
		sess->dir = USER_DIRECTION_LEFT;
		if (bCanMoveFlg)
		{
			if (dAverageAngle >= 180)
				sess->angle = 90;
			else if (dAverageAngle <= 0)
				sess->angle = 180;
			else
				sess->angle = (short)(dAverageAngle+270)%360;
		}
	}
	// 移動可?値を減らす
	sess->MV_c = max(sess->MV_c-1, 0);
	return ret;	
}


///////////////////////////////////////////////////////////
// blt
E_CHECK_GROUND_RESULT common::blt::CheckGround(CMainStage* pStage, ptype_blt blt, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo)
{
	E_CHECK_GROUND_RESULT ret = CHECK_GROUND_RESULT_GROUND;
	D3DXVECTOR2 first_pos = D3DXVECTOR2( (float)blt->ax, (float)blt->ay);
	D3DXVECTOR2 pos = first_pos;
	D3DXVECTOR2 dest_my_pos;
	D3DXVECTOR2 dest_ground_pos;

	// 今、宙に浮いているか
	if ( pStage->FindGround(&dest_my_pos, &dest_ground_pos,  &pos, blt->hit_range) )
	{
		// 1個下より先が地面なら落ちるだけ
		if (pos.y+OBJECT_DROP_SPEED < dest_my_pos.y)
		{
			ret = (blt->vy == 0)?CHECK_GROUND_RESULT_FALLING:CHECK_GROUND_RESULT_FALLDOWN;
			// 下?を超える場合、スクリ?ンアウト
			TCHARA_SCR_INFO* pScrInfo = common::scr::FindCharaScrInfoFromCharaType(blt->chara_type, pMapScrInfo);
			if (blt->ay >= (pStage->GetStageHeight()+pScrInfo->blt_info[blt->bullet_type].rec_blt_tex.bottom /2) )
				ret = CHECK_GROUND_RESULT_DROPOUT;
			
		}
		else if (blt->ay != (short)dest_my_pos.y) // 地面上に接地
		{
			blt->ay = (short)(dest_my_pos.y);
			ret = CHECK_GROUND_RESULT_GETDOWN;
		}
	}
	else
	{
		ret = (blt->vy == 0)?CHECK_GROUND_RESULT_FALLDOWN:CHECK_GROUND_RESULT_FALLING;
		// 地面が無い、ステ?ジ外確認
		TCHARA_SCR_INFO* pScrInfo = common::scr::FindCharaScrInfoFromCharaType(blt->chara_type, pMapScrInfo);
		if (blt->ay >= (pStage->GetStageHeight()+pScrInfo->blt_info[blt->bullet_type].rec_blt_tex.bottom/2) )
			ret = CHECK_GROUND_RESULT_DROPOUT;
	}
	return ret;
}

void common::blt::UpdateBulletVec(D3DXVECTOR2* out, ptype_blt blt, int wind, double dAddElapsed)
{
	D3DXVECTOR2 vec = D3DXVECTOR2(
		(float)max(min(32767, (((int)blt->vx+(int)(((int)blt->adx+wind))*dAddElapsed))), -32768),
		(float)max(min(32767, (((int)blt->vy+(int)blt->ady)*dAddElapsed)), -32768));
	*out = vec*(BLT_POS_FACT_F*BLT_VEC_FACT_F);
}
/*
common::E_MOVE_STAGE_RESULT  common::blt::MoveBullet(CMainStage* pStage, ptype_blt blt, std::vector< type_session* > *pVecCharacters, LuaHelper* pLuah, int nWind, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo)
{
	common::E_MOVE_STAGE_RESULT ret = common::MOVE_STAGE_RESULT_NONE;
	switch (blt->obj_state)
	{
	case OBJ_STATE_MAIN_ACTIVE:
		ret = MoveActBullet(pStage, blt, pVecCharacters, pLuah, nWind, pMapScrInfo);
		break;
	case OBJ_STATE_MAIN_WAIT:
		ret = MoveWaitBullet(pStage, blt, pVecCharacters, pLuah, nWind, pMapScrInfo);
		break;
	}
	return ret;
}
*/

BOOL common::blt::GetSlideVec(CMainStage* pStage,int nAngle, D3DXVECTOR2 *out, D3DXVECTOR2 *pos, int hit_range)
{
	D3DXVECTOR2 vecCheckPos,vecTemp;
	int nCheckAngle = nAngle;
	int i=0;
	for (;i<8;i++)
	{
		if (i==4) continue;
		nCheckAngle = (nAngle + (90*i))%360;

		if (nCheckAngle > 337 || nCheckAngle <= 22)
			vecCheckPos = D3DXVECTOR2(-1.0,0.0f)+ (*pos);
		else if (nCheckAngle > 22 && nCheckAngle <= 67)
			vecCheckPos = D3DXVECTOR2(-0.5f,0.5f)+(*pos);
		else if (nCheckAngle > 67 && nCheckAngle <= 112)
			vecCheckPos = D3DXVECTOR2(0.0f,1.0f)+(*pos);
		else if (nCheckAngle > 112 && nCheckAngle <= 157)
			vecCheckPos = D3DXVECTOR2(0.5f,0.5f)+(*pos);
		else if (nCheckAngle > 157 && nCheckAngle <= 202)
			vecCheckPos = D3DXVECTOR2(1.0f,0.0f)+(*pos);
		else if (nCheckAngle > 202 && nCheckAngle <= 247)
			vecCheckPos = D3DXVECTOR2(0.5f,-0.5f)+(*pos);
		else if (nCheckAngle > 247 && nCheckAngle <= 292)
			vecCheckPos = D3DXVECTOR2(0.0f,-1.0f)+(*pos);
		else
			vecCheckPos = D3DXVECTOR2(-0.5f,-0.5f)+(*pos);
		if (!pStage->IsHit(&vecTemp, &vecCheckPos, hit_range)
		&& pStage->IsHit(&vecTemp, &vecCheckPos, hit_range+1))
		{
			*out = vecCheckPos;
			return TRUE;
		}
	}
	return FALSE;
}

common::blt::E_MOVE_ACT_BULLET_RESULT common::blt::MoveActBullet(CMainStage* pStage, ptype_blt blt, std::vector< type_session* > *pVecCharacters, LuaHelper* pLuah, int nWind, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, std::map<int, type_obj*>* pMapObjects, CCriticalSection* pCriticalSection, BOOL bSrv)
{
	common::blt::E_MOVE_ACT_BULLET_RESULT ret = common::blt::MOVE_ACT_BULLET_RESULT_MOVED;

	// 画面外判定
	if (blt->ay > pStage->GetStageHeight()+(MAX_BLT_SIZE_H/2))
		return common::blt::MOVE_ACT_BULLET_RESULT_DROP;

	blt->frame_count++;

	// スクリプト製の弾
	switch (blt->proc_type)
	{
	case BLT_PROC_TYPE_SCR_CHARA:
		{
			LuaFuncParam luaParams;
			LuaFuncParam luaResults;
			// 弾作ったキャラのObjNo,chr_obj_no,obj_no,スクリプト番号,弾?イプ,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
			luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->frame_count).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2);
			if (!common::scr::CallLuaFunc(pLuah, "onFrame_CharaBullet", &luaResults, 0, &luaParams, pCriticalSection))
				return ret;
			break;
		}
	case BLT_PROC_TYPE_SCR_SPELL:
		{
			LuaFuncParam luaParams;
			LuaFuncParam luaResults;
			// 弾作ったキャラのObjNo,chr_obj_no,obj_no,スクリプト番号,弾?イプ,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
			luaParams.Number(blt->scrinfo->scr_index).Number(blt->obj_no).Number(blt->frame_count).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2);
			if (!common::scr::CallLuaFunc(pLuah, "onFrame_CharaSpell", &luaResults, 0, &luaParams, pCriticalSection))
				return ret;
			break;
		}
	case BLT_PROC_TYPE_SCR_STAGE:
		{
			LuaFuncParam luaParams;
			LuaFuncParam luaResults;
			// 弾作ったキャラのObjNo,chr_obj_no,obj_no,スクリプト番号,弾?イプ,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
			luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->frame_count).Number(blt->ax).Number(blt->ay).Number(blt->vx).Number(blt->vy).Number(blt->extdata1).Number(blt->extdata2);
			if (!common::scr::CallLuaFunc(pLuah, "onFrame_StageBullet", &luaResults, 0, &luaParams, pCriticalSection))
				return ret;
			break;
		}
	}

	// フレ??処理でスクリプトによって削除されたか
	if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)
		return common::blt::MOVE_ACT_BULLET_RESULT_REMOVE;

	D3DXVECTOR2 hit;
	D3DXVECTOR2 pos;
	bool bEphemerisVec = FALSE;
	float range = 0.0f;
	int e = 0;
	BOOL bMove = FALSE;
	BOOL bNoVec = FALSE;
	D3DXVECTOR2 vec = D3DXVECTOR2(0,0);
	D3DXVECTOR2 v1 = vec;

	if (blt->scrinfo && ((TCHARA_SCR_INFO*)blt->scrinfo)->blt_info[blt->bullet_type].ephemeris)
	{
		LuaFuncParam luaParams;
		LuaFuncParam luaResults;
		// スクリプト製の弾
		switch (blt->proc_type)
		{
		case BLT_PROC_TYPE_SCR_CHARA:
			{
				// 弾作ったキャラのObjNo,chr_obj_no,obj_no,スクリプト番号,弾?イプ,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->frame_count).Number(blt->bx).Number(blt->by).Number(blt->vx).Number(blt->vy).Number(nWind).Number(blt->extdata1).Number(blt->extdata2);
				if (!common::scr::CallLuaFunc(pLuah, "onGetEphemeris_CharaBullet", &luaResults, 3, &luaParams, pCriticalSection))
					return ret;
				break;
			}
		case BLT_PROC_TYPE_SCR_SPELL:
			{
				// 弾作ったキャラのObjNo,chr_obj_no,obj_no,スクリプト番号,弾?イプ,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->obj_no).Number(blt->frame_count).Number(blt->bx).Number(blt->by).Number(blt->vx).Number(blt->vy).Number(nWind).Number(blt->extdata1).Number(blt->extdata2);
				if (!common::scr::CallLuaFunc(pLuah, "onGetEphemeris_CharaSpell", &luaResults, 3, &luaParams, pCriticalSection))
					return ret;
				break;
			}
		case BLT_PROC_TYPE_SCR_STAGE:
			{
				// 弾作ったキャラのObjNo,chr_obj_no,obj_no,スクリプト番号,弾?イプ,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
				luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number(blt->frame_count).Number(blt->bx).Number(blt->by).Number(blt->vx).Number(blt->vy).Number(nWind).Number(blt->extdata1).Number(blt->extdata2);
				if (!common::scr::CallLuaFunc(pLuah, "onGetEphemeris_StageBullet", &luaResults, 3, &luaParams, pCriticalSection))
					return ret;
				break;
			}
		}
		bEphemerisVec = luaResults.GetBool(2);
		// その位置までを移動したと見なすか
		if (bEphemerisVec)
		{
			float nx = (float)luaResults.GetNumber(0)*BLT_POS_FACT_F;
			float ny = (float)luaResults.GetNumber(1)*BLT_POS_FACT_F;
			pos = D3DXVECTOR2(blt->bx*BLT_POS_FACT_F,blt->by*BLT_POS_FACT_F);
			vec = (D3DXVECTOR2(nx, ny) - pos)*BLT_VEC_FACT_F;
			blt->vx = (short)max(-32878,min(32727, (int)(vec.x*BLT_POS_FACT_N*BLT_VEC_FACT_N)));
			blt->vy = (short)max(-32878,min(32727, (int)(vec.y*BLT_POS_FACT_N*BLT_VEC_FACT_N)));
			// 移動距離
			range = D3DXVec2Length(&vec);
			e = (int)floor(range);	// 切り上げ
			if (e)	
				v1 = vec / (float)e;
			else
			{
				v1 = vec;
				e = 1;
				if (vec.x == 0.0f && vec.y == 0)
					bNoVec = TRUE;
			}
		}
		else
		{
			blt->bx = (short)max(-32878,min(32727, luaResults.GetNumber(0)));
			blt->by = (short)max(-32878,min(32727, luaResults.GetNumber(1)));
			blt->ax = (short)max(-32878,min(32727, (int)(blt->bx*BLT_POS_FACT_F)));
			blt->ay = (short)max(-32878,min(32727, (int)(blt->by*BLT_POS_FACT_F)));
			pos = D3DXVECTOR2(blt->ax,blt->ay);
			bNoVec = TRUE;
		}
	}
	else
	{
		// ?イプがステ?ジの場合、風に影響を受けない
		switch (blt->obj_type)
		{
		case OBJ_TYPE_BLT_STAGE:
		case OBJ_TYPE_ITEM_STAGE:
		case OBJ_TYPE_BLT_LIQUID:
		case OBJ_TYPE_ITEM_LIQUID:
		case OBJ_TYPE_BLT_SLIDE:
		case OBJ_TYPE_ITEM_SLIDE:
			UpdateBulletVec(&vec, blt, 0);
			break;
		default:
			UpdateBulletVec(&vec, blt, nWind);
			break;
		}
		pos = D3DXVECTOR2(	blt->bx*BLT_POS_FACT_F,blt->by*BLT_POS_FACT_F);
		// 移動距離
		range = D3DXVec2Length(&vec);
		e = (int)floor(range);	// 切り上げ
		if (e)	
			v1 = vec / (float)e;
		else
		{
			v1 = vec;
			e = 1;
			if (vec.x == 0.0f && vec.y == 0)
				bNoVec = TRUE;
		}
	}

	LuaFuncParam luaParams;
	LuaFuncParam luaResults;
	int hit_obj_no = 0;
	D3DXVECTOR2 vecSavePos = D3DXVECTOR2(blt->ax, blt->ay);
	BOOL bBreak = FALSE;
	// 距離１ずつ当たり判定
	int i=0;
	if (blt->hit_range<=0)	i=e;
#if 0	// 地面を滑る移動

	if (blt->obj_type&OBJ_TYPE_SLIDE)
	{
		D3DXVECTOR2 vecPrevPos = pos;
		for (;i<e;i++)
		{
			bMove = (i>0);
			D3DXVECTOR2 vecCheckPos;
			vecCheckPos = ((float)(i+1)*v1) + pos;
			BOOL bLuaFuncRes = FALSE;
			D3DXVECTOR2 vec2Temp;

			// 現在地で設置している
			if (pStage->IsHit(&hit, &vecPrevPos, blt->hit_range+1))
			{
				int nAngle = (GetAngle(v1.x,v1.y)+180)%360;

				if (!GetSlideVec(pStage, nAngle, &vec2Temp, &vecPrevPos, blt->hit_range))
				{
					blt->proc_flg |= PROC_FLG_OBJ_REMOVE;
					bBreak = TRUE;
				}
				else
				{
					blt->vx = (vec2Temp-vecPrevPos).x * BLT_VEC_FACT_N;
					blt->vy = (vec2Temp-vecPrevPos).y * BLT_VEC_FACT_N;
					vecSavePos = vec2Temp;
					bBreak = TRUE;
				}
			}
			else
			{
				// 次の位置で設置
				if (pStage->IsHit(&hit, &vecCheckPos, blt->hit_range))
				{
					double dElapsed = 0;
					if (i) dElapsed = (double)i/(double)e;
					vecSavePos = hit-vecCheckPos+pos;
					bBreak = TRUE;
					// 移動しないで当たっている場合、当たった位置から半径分離す
					if (bNoVec)
					{
						D3DXVECTOR2 norm,temp;
						temp = vecCheckPos - hit;
						D3DXVec2Normalize(&norm, &temp);
						temp = ((float)(blt->hit_range+1))*norm + hit;
						vecSavePos.x = ceilf(temp.x);
						vecSavePos.y = ceilf(temp.y);
					}
					else if (!bMove)	// 初期移動位置で埋まっている場合？
					{
						D3DXVECTOR2 norm;
						D3DXVec2Normalize(&norm, &vec);
						// 移動値の逆方向へ弾とキャラの当たり判定分戻す(厳密にはblt->hit_rangeはいらない)
						vecSavePos = vecCheckPos - (norm*((float)(blt->hit_range+1)));
					}
					// スクリプト処理
					if (blt->proc_type != BLT_PROC_TYPE_ITEM)
					{
						luaParams.Clear();		luaResults.Clear();
						BOOL bLuaFuncRes = FALSE;
						switch (blt->proc_type)
						{
						case BLT_PROC_TYPE_SCR_CHARA:
							// script,弾?イプ,弾作ったキャラのObjNo,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(blt->obj_no).Number((int)vecSavePos.x).Number((int)vecSavePos.y).Number((int)hit.x).Number((int)hit.y).Number((int)blt->vx).Number((int)blt->vy).Number(1.0f- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitStage_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						case BLT_PROC_TYPE_SCR_SPELL:
							// script,弾作ったキャラのObjNo,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0f- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitStage_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						case BLT_PROC_TYPE_SCR_STAGE:
							// script,弾?イプ,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0f- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitStage_StageBullet", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						}
						if (!bLuaFuncRes)	return ret;
						// 1ならストップ
						if (luaResults.GetNumber(0) != 0)
						{
							bBreak = TRUE;
							break;
						}
					}
					else
					{
						ret = common::blt::MOVE_ACT_BULLET_RESULT_STAGE_HIT;
						bBreak = TRUE;
						break;
					}
				}
				
				if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)// 削除フラグが立った場合、?める
				{
					ret = common::blt::MOVE_ACT_BULLET_RESULT_REMOVE;
					bBreak = TRUE;
					break;	// 20101121
				}
			}
			if (bBreak)	break;
		}
	}
	else
	{
#endif
		for (;i<e;i++)
		{
			bMove = (i>0);
			D3DXVECTOR2 vecCheckPos;
			vecCheckPos = ((float)(i+1)*v1) + pos;
			BOOL bLuaFuncRes = FALSE;
			for (std::vector< type_session* >::iterator it = pVecCharacters->begin();
				it != pVecCharacters->end();
				it++)
			{
				// スル?
				if (!(*it)->entity																// 不在
				||	(*it)->obj_state & OBJ_STATE_MAIN_NOLIVE_FLG)		// ?亡|落下
					continue;

				type_session* sess = (*it);
				if (common::obj::IsHit(blt->hit_range+CHARA_HIT_RANGE, &vecCheckPos, (*it)))
	//			if (common::obj::IsHit(blt->hit_range, &vecCheckPos, (*it)))
				{
					double dElapsed = (double)i/(double)e;
					hit_obj_no = (*it)->obj_no;								// 当たった番号
					hit =  (v1*(blt->hit_range)) + vecCheckPos;		//	当たった位置
					vecSavePos = ((float)i*v1) + pos;						//	当たった直前の位置
					// 移動しないで当たっている場合、当たった位置から+キャラク?半径分離す
					if (!bNoVec)
					{
//> 20110409
//						D3DXVECTOR2 norm,temp;
//						temp = vecCheckPos - hit;
//						D3DXVec2Normalize(&norm, &temp);
//						//> 20101218
//						temp = /*((float)(blt->hit_range+0.5))*norm + */hit;
//	//					temp = ((float)(blt->hit_range+1))*norm + hit;
//						vecSavePos.x = ceilf(temp.x);
//						vecSavePos.y = ceilf(temp.y);
//< 20110409
						vecSavePos = pos;
					}
					else if (!bMove)	// 初期移動位置で埋まっている場合？
					{
						D3DXVECTOR2 norm;
						D3DXVec2Normalize(&norm, &vec);
						// 移動値の逆方向へ弾とキャラの当たり判定分戻す(厳密にはblt->hit_rangeはいらない)
						vecSavePos = vecCheckPos - (norm*((float)(blt->hit_range+1)));
					}
					
					// スクリプト処理
					BOOL bProcSetBreak = FALSE;
					switch (blt->proc_type)
					{
					case BLT_PROC_TYPE_SCR_CHARA:
						luaParams.Clear();	luaResults.Clear();
						// script,弾?イプ,弾作ったキャラのObjNo,chr_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata1,extdata2
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						if (!common::scr::CallLuaFunc(pLuah, "onHitChara_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection))
							return ret;
						if (luaResults.GetNumber(0))
							bBreak = TRUE;
						break;
					case BLT_PROC_TYPE_SCR_SPELL:
						// script,弾作ったキャラのObjNo,chr_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata1,extdata2
						luaParams.Clear();	luaResults.Clear();
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						if (!common::scr::CallLuaFunc(pLuah, "onHitChara_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection))
							return ret;
						if (luaResults.GetNumber(0))
							bBreak = TRUE;
						break;
					case BLT_PROC_TYPE_SCR_STAGE:
						// script,弾?イプ,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata1,extdata2
						luaParams.Clear();	luaResults.Clear();
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(hit_obj_no).Number(blt->obj_no).Number(blt->ax).Number(blt->ay).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						if (!common::scr::CallLuaFunc(pLuah, "onHitChara_StageBullet", &luaResults, 1, &luaParams, pCriticalSection))
							return ret;
						if (luaResults.GetNumber(0))
							bBreak = TRUE;
						break;
					case BLT_PROC_TYPE_ITEM:
						if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)// 削除フラグが立った場合、?める
						{
							ret = common::blt::MOVE_ACT_BULLET_RESULT_REMOVE;
							bBreak = TRUE;
	//						break;	// 20101121
						}
						else
						{
							ret = common::blt::MOVE_ACT_BULLET_RESULT_CHARA_HIT;
							bBreak = TRUE;
	//						break;
						}
						break;
					}

					if (bBreak)	break;	// ストップ
	//< 20101221
				}
			}

			if (bBreak)	break;
			// 弾同士の当たり判定
			for (std::map<int, type_obj*>::iterator it = pMapObjects->begin();
				it != pMapObjects->end();
				it++)
			{
				BYTE objType = ((ptype_blt)(*it).second)->obj_type;
				// 当たり判定の無いオブジェクト
				if ((BYTE)(objType & OBJ_TYPE_TACTIC)) continue;
				// 自分自身ならスル?
				if ( (DWORD)(*it).second == (DWORD)blt)	continue;
				// アイテ?同士スル?
				if ((blt->obj_type & OBJ_TYPE_ITEM) && (objType & OBJ_TYPE_ITEM)) continue;
				// 削除フラグ持ちの弾ならスル?
				if ((*it).second->proc_flg & PROC_FLG_OBJ_REMOVE)	continue;
				int nObjRange = BLT_DEFAULT_HITRANGE;
				if ((*it).second->obj_type & OBJ_TYPE_BLT)
					nObjRange = ((ptype_blt)(*it).second)->hit_range;
				// 当たり判定
				if (common::obj::IsHit(blt->hit_range+nObjRange, &vecCheckPos, (*it).second ))
				{
					double dElapsed = (double)i/(double)e;
					hit_obj_no = (*it).second->obj_no;					// 当たった番号
					hit =  (v1*(blt->hit_range)) + vecCheckPos;		//	当たった位置
					vecSavePos = ((float)i*v1) + pos;						//	当たった直前の位置
					// 移動しないで当たっている場合、当たった位置から半径分離す
					if (!bNoVec)
					{
//> 20110409
//						D3DXVECTOR2 norm,temp;
//						temp = vecCheckPos - hit;
//						D3DXVec2Normalize(&norm, &temp);
//						temp = /*((float)(blt->hit_range+0.5))*norm + */hit;
//						vecSavePos.x = ceilf(temp.x);
//						vecSavePos.y = ceilf(temp.y);
//< 20110409
						vecSavePos = pos;
					}
					else if (!bMove)	// 初期移動位置で埋まっている場合？
					{
						D3DXVECTOR2 norm;
						D3DXVec2Normalize(&norm, &vec);
						// 移動値の逆方向へ弾とキャラの当たり判定分戻す(厳密にはblt->hit_rangeはいらない)
						vecSavePos = vecCheckPos - (norm*((float)(blt->hit_range+1)));
					}

					BOOL bLuaFuncRes = FALSE;
					int nLuaResult = 0;
					// スクリプト処理
					switch (blt->proc_type)
					{
					case BLT_PROC_TYPE_SCR_CHARA:
						luaParams.Clear();		luaResults.Clear();
						// 動いた側のイベント
						// script,弾?イプ,弾作ったキャラのObjNo,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						if (!common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection))
							return ret;
						nLuaResult = (int)luaResults.GetNumber(0);
						break;
					case BLT_PROC_TYPE_SCR_SPELL:
						{
							luaParams.Clear();		luaResults.Clear();
							// script,弾?イプ,弾作ったキャラのObjNo,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							if (!common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection) )
								return ret;
							nLuaResult = (int)luaResults.GetNumber(0);
							break;
						}
					case BLT_PROC_TYPE_SCR_STAGE:
						{
							luaParams.Clear();		luaResults.Clear();
							// script,弾?イプ,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							if (!common::scr::CallLuaFunc(pLuah, "onHitBullet_StageBullet", &luaResults, 1, &luaParams, pCriticalSection))
								return ret;
							nLuaResult = (int)luaResults.GetNumber(0);
							break;
						}
					case BLT_PROC_TYPE_ITEM:
						break;
					default:
						ret = common::blt::MOVE_ACT_BULLET_RESULT_OBJECT_HIT;
						bBreak = TRUE;
						break;
					}

					// 1ならストップ
					if (nLuaResult)	bBreak = TRUE;

					// 当たられた側が弾ならイベントを知らせる
					if ( (*it).second->obj_type & OBJ_TYPE_BLT)
					{
						ptype_blt hit_blt = (ptype_blt)(*it).second;
						luaParams.Clear();		luaResults.Clear();
						bLuaFuncRes = FALSE;

						switch (hit_blt->proc_type)
						{
						case BLT_PROC_TYPE_SCR_CHARA:
							// script,弾?イプ,弾作ったキャラのObjNo,chr_obj_no,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(hit_blt->scrinfo->scr_index).Number(hit_blt->bullet_type).Number(hit_blt->chr_obj_no).Number(blt->obj_no).Number(hit_obj_no).Number(hit_blt->ax).Number(hit_blt->ay).Number(hit.x).Number(hit.y).Number(hit_blt->vx).Number(hit_blt->vy).Number(0.0).Number(hit_blt->frame_count).Number(hit_blt->extdata1).Number(hit_blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						case BLT_PROC_TYPE_SCR_SPELL:
							// script,弾作ったキャラのObjNo,chr_obj_no,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(hit_blt->scrinfo->scr_index).Number(hit_blt->chr_obj_no).Number(blt->obj_no).Number(hit_obj_no).Number(hit_blt->ax).Number(hit_blt->ay).Number(hit.x).Number(hit.y).Number(hit_blt->vx).Number(hit_blt->vy).Number(0.0).Number(hit_blt->frame_count).Number(hit_blt->extdata1).Number(hit_blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						case BLT_PROC_TYPE_SCR_STAGE:
							// script,弾?イプ,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(hit_blt->scrinfo->scr_index).Number(hit_blt->bullet_type).Number(hit_obj_no).Number(blt->obj_no).Number(hit_blt->ax).Number(hit_blt->ay).Number(hit.x).Number(hit.y).Number(hit_blt->vx).Number(hit_blt->vy).Number(0.0).Number(hit_blt->frame_count).Number(hit_blt->extdata1).Number(hit_blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitBullet_StageBullet", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						}
						if (!bLuaFuncRes)	return ret;
						// スクリプトによって?作されたか
						if (blt->proc_flg & (PROC_FLG_OBJ_REMOVE|PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS))
							bBreak = TRUE;
					}
					if (bBreak)	break;
				}

			}
			if (bBreak)	break;

			// キャラやオブジェクトと当たっていない場合、ステ?ジ当たり判定
			// GASはステ?ジ当たり判定なし
			if ( blt->obj_type & (OBJ_TYPE_SOLID|OBJ_TYPE_LIQUID) )
			{
				if (pStage->IsHit(&hit, &vecCheckPos, blt->hit_range))
				{
					double dElapsed = 0;
					if (i) dElapsed = (double)i/(double)e;
					vecSavePos = ((float)i*v1) + pos;						//	当たった直前の位置
					// 移動しないで当たっている場合、当たった位置から半径分離す
					if (bNoVec)
					{
//> 20110409
//						D3DXVECTOR2 norm,temp;
//						temp = vecCheckPos - hit;
//						D3DXVec2Normalize(&norm, &temp);
//						temp = /*((float)(blt->hit_range+0.5))*norm +*/ hit;
//						vecSavePos.x = ceilf(temp.x);
//						vecSavePos.y = ceilf(temp.y);
//< 20110409
						vecSavePos = pos;
					}
					else if (!bMove)	// 初期移動位置で埋まっている場合？
					{
						D3DXVECTOR2 norm;
						D3DXVec2Normalize(&norm, &vec);
						// 移動値の逆方向へ弾とキャラの当たり判定分戻す(厳密にはblt->hit_rangeはいらない)
						vecSavePos = vecCheckPos - (norm*((float)(blt->hit_range+1)));
					}
					// スクリプト処理
					if (blt->proc_type != BLT_PROC_TYPE_ITEM)
					{
						luaParams.Clear();		luaResults.Clear();
						BOOL bLuaFuncRes = FALSE;
						switch (blt->proc_type)
						{
						case BLT_PROC_TYPE_SCR_CHARA:
							// script,弾?イプ,弾作ったキャラのObjNo,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(blt->obj_no).Number((int)vecSavePos.x).Number((int)vecSavePos.y).Number((int)hit.x).Number((int)hit.y).Number((int)blt->vx).Number((int)blt->vy).Number(1.0f- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitStage_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						case BLT_PROC_TYPE_SCR_SPELL:
							// script,弾作ったキャラのObjNo,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0f- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitStage_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						case BLT_PROC_TYPE_SCR_STAGE:
							// script,弾?イプ,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
							luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0f- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
							bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitStage_StageBullet", &luaResults, 1, &luaParams, pCriticalSection);
							break;
						case BLT_PROC_TYPE_ITEM:
							ret = common::blt::MOVE_ACT_BULLET_RESULT_VEC_CHANGE;
							bBreak = TRUE;
							break;
						}
						if (!bLuaFuncRes)	return ret;
						// 1ならストップ
						if (luaResults.GetNumber(0) != 0)
						{
							bBreak = TRUE;
							break;
						}
					}
					else
					{
						ret = common::blt::MOVE_ACT_BULLET_RESULT_STAGE_HIT;
						bBreak = TRUE;
						break;
					}
					
					if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)// 削除フラグが立った場合、?める
					{
						ret = common::blt::MOVE_ACT_BULLET_RESULT_REMOVE;
						bBreak = TRUE;
						break;	// 20101121
					}
				}
			}
			if (bBreak)	break;
		}
#if 0
	}
#endif
	vec*=(BLT_POS_FACT_N);
	double dElapsed = 0;
	// 当たった位置に修正
	if (bBreak)
	{
		if (!(blt->proc_flg & (PROC_FLG_OBJ_UPDATE_POS|PROC_FLG_OBJ_UPDATE_VEC)))
		{
			dElapsed = (double)i/(double)e;
			common::blt::UpdateBulletVec(&vec, blt, nWind, dElapsed);
			vec*=(BLT_VEC_FACT_N);
//> 20110503
//			blt->vx = (short)(vec.x*(float)BLT_VEC_FACT_N);
//			blt->vy = (short)(vec.y*(float)BLT_VEC_FACT_N);
//< 20110503
			blt->vx = (short)(vec.x*(float)(BLT_VEC_FACT_N*BLT_VEC_FACT_N));
			blt->vy = (short)(vec.y*(float)(BLT_VEC_FACT_N*BLT_VEC_FACT_N));
			//> 20110130 オ?バ?フロ?対策
//			blt->bx = (short)(vecSavePos.x*BLT_VEC_FACT_N);
//			blt->by = (short)(vecSavePos.y*BLT_VEC_FACT_N);
//			blt->ax = (short)(vecSavePos.x);
//			blt->ay = (short)(vecSavePos.y);
			blt->bx = (short)max(min(32767, (int)(vecSavePos.x*BLT_VEC_FACT_N)), -32768);
			blt->by = (short)max(min(32767, (int)(vecSavePos.y*BLT_VEC_FACT_N)), -32768);
			blt->ax = (short)((float)blt->bx*BLT_POS_FACT_F);
			blt->ay = (short)((float)blt->by*BLT_POS_FACT_F);
			//< 20110130 オ?バ?フロ?対策
		}
		else if (blt->proc_flg & PROC_FLG_OBJ_UPDATE_VEC)
		{
			//> 20110130 オ?バ?フロ?対策
//			blt->bx = (short)(vecSavePos.x*BLT_VEC_FACT_N);
//			blt->by = (short)(vecSavePos.y*BLT_VEC_FACT_N);
//			blt->ax = (short)(vecSavePos.x);
//			blt->ay = (short)(vecSavePos.y);
			blt->bx = (short)max(min(32767, (int)(vecSavePos.x*BLT_VEC_FACT_N)), -32768);
			blt->by = (short)max(min(32767, (int)(vecSavePos.y*BLT_VEC_FACT_N)), -32768);
			blt->ax = (short)((float)blt->bx*BLT_POS_FACT_F);
			blt->ay = (short)((float)blt->by*BLT_POS_FACT_F);
			//< 20110130 オ?バ?フロ?対策
		}
	}
	else
	{
		if (!(blt->proc_flg & (PROC_FLG_OBJ_UPDATE_POS|PROC_FLG_OBJ_UPDATE_VEC)))
		{
			blt->vx = (short)(vec.x*(float)BLT_VEC_FACT_N);
			blt->vy = (short)(vec.y*(float)BLT_VEC_FACT_N);
			//> 20110130 オ?バ?フロ?対策
//			blt->bx += (short)vec.x;
//			blt->by += (short)vec.y;
			blt->bx = (short)max(min(32767, (int)vec.x+(int)blt->bx), -32768);
			blt->by = (short)max(min(32767, (int)vec.y+(int)blt->by), -32768);
			//< 20110130 オ?バ?フロ?対策
			blt->ax = (short)((float)blt->bx*BLT_POS_FACT_F);
			blt->ay = (short)((float)blt->by*BLT_POS_FACT_F);
		}
		else if (blt->proc_flg & PROC_FLG_OBJ_UPDATE_VEC)
		{
			//> 20110130 オ?バ?フロ?対策
//			blt->bx += (short)vec.x;
//			blt->by += (short)vec.y;
			blt->bx = (short)max(min(32767, (int)vec.x+(int)blt->bx), -32768);
			blt->by = (short)max(min(32767, (int)vec.y+(int)blt->by), -32768);
			//< 20110130 オ?バ?フロ?対策
			blt->ax = (short)((float)blt->bx*BLT_POS_FACT_F);
			blt->ay = (short)((float)blt->by*BLT_POS_FACT_F);
		}
	}

	// スクリプト処理
	if (blt->proc_type != BLT_PROC_TYPE_ITEM)
	{
		// スクリプトによって削除されたか
		if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)
		{
///			common::obj::SetLuaFlg((type_obj*)blt, LUA_FLG_OBJ_REMOVE, FALSE);
			return common::blt::MOVE_ACT_BULLET_RESULT_REMOVE;
		}
		// スクリプトによって移動値、位置が変えられたか
/*20110103 スクリプト内で情報を送っているので戻り値に必要ない？
		if (blt->proc_flg & (PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS))
		{
			common::obj::SetLuaFlg((type_obj*)blt, (PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS), FALSE);
			return common::blt::MOVE_ACT_BULLET_RESULT_VEC_CHANGE;
		}
*/
		if (bSrv)
			common::obj::SetLuaFlg((type_obj*)blt, (PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS), FALSE);
	}

	return ret;
}

common::blt::E_MOVE_WAIT_BULLET_RESULT  common::blt::MoveWaitBullet(CMainStage* pStage, ptype_blt blt, std::vector< type_session* > *pVecCharacters, LuaHelper* pLuah, int nWind, std::map < int, TCHARA_SCR_INFO >* pMapScrInfo, std::map<int, type_obj*>* pMapObjects, CCriticalSection* pCriticalSection, BOOL bSrv)
{
	common::blt::E_MOVE_WAIT_BULLET_RESULT ret = common::blt::MOVE_WAIT_BULLET_RESULT_NONE;
	BOOL bMove = FALSE;
	D3DXVECTOR2 vec = D3DXVECTOR2(0,0);
	// 画面外判定
	if (blt->ay > pStage->GetStageHeight()+(MAX_BLT_SIZE_H/2))
		return common::blt::MOVE_WAIT_BULLET_RESULT_DROPOUT;

	blt->frame_count++;

	// GASは移動確認なし
	switch (blt->obj_type)
	{
	case OBJ_TYPE_BLT_LIQUID:
	case OBJ_TYPE_ITEM_LIQUID:
		{
			UpdateBulletVec(&vec, blt, 0);

			D3DXVECTOR2 hit;
			D3DXVECTOR2 dest_my_pos;
			D3DXVECTOR2 dest_ground_pos;
			D3DXVECTOR2 pos = D3DXVECTOR2(blt->ax,blt->ay);

			// 地面があるか
			if (pStage->FindGroundDown(&dest_my_pos, &dest_ground_pos, &pos, blt->hit_range))
			{
				int dest_y = (int)dest_my_pos.y;
				// 着地済み
				if (dest_y <= blt->ay)
				{
					bMove = FALSE;
					blt->ay = (short)dest_y;
					blt->vy = 0;
					blt->vx = 0;
					vec = D3DXVECTOR2(0,0);
					ret = common::blt::MOVE_WAIT_BULLET_RESULT_GROUND;
				}
				else if (dest_y <= (blt->ay+OBJECT_DROP_SPEED) )	// 着地
				{
					bMove = TRUE;
					blt->vy = 0;
					blt->vx += nWind;
					ret = common::blt::MOVE_WAIT_BULLET_RESULT_GETDOWN;
					vec = D3DXVECTOR2((float)blt->vx*BLT_POS_FACT_F*BLT_VEC_FACT_F,(float)(dest_y-blt->ay));
				}
				else if (blt->vy != OBJECT_DROP_SPEED*BLT_VEC_FACT_N*BLT_POS_FACT_N)	// 落下はじめ
				{
					bMove = TRUE;
					blt->vx = nWind/2;
					blt->vy = OBJECT_DROP_SPEED;
					ret = common::blt::MOVE_WAIT_BULLET_RESULT_FALLDOWN;
					vec = D3DXVECTOR2((float)nWind*BLT_POS_FACT_F*BLT_VEC_FACT_F,OBJECT_DROP_SPEED);
				}
				else
				{
					bMove = TRUE;
					blt->vx += nWind;
					blt->vy = OBJECT_DROP_SPEED;
					ret = common::blt::MOVE_WAIT_BULLET_RESULT_FALLING;
					vec = D3DXVECTOR2((float)blt->vx*BLT_POS_FACT_F*BLT_VEC_FACT_F,OBJECT_DROP_SPEED);
				}
			}
		}
		break;
	case OBJ_TYPE_BLT_SOLID:
	case OBJ_TYPE_ITEM_SOLID:
	case OBJ_TYPE_BLT_GAS:
	case OBJ_TYPE_ITEM_GAS:
		UpdateBulletVec(&vec, blt, nWind);
		break;
	case OBJ_TYPE_BLT_STAGE:
	case OBJ_TYPE_ITEM_STAGE:
		UpdateBulletVec(&vec, blt, 0);
		break;
	default:
		break;
	}

	D3DXVECTOR2 pos = D3DXVECTOR2(
		blt->bx*BLT_POS_FACT_F,
		blt->by*BLT_POS_FACT_F);
	
	float range = D3DXVec2Length(&vec);
	int e = 1;
	int hit_obj_no = 0;
	D3DXVECTOR2 v1 = vec;
	if (e)	
	{
		bMove = TRUE;
		v1 = vec / (float)e;
	}
	else
	{
		e = 1;
		if (vec.x == 0.0f || vec.y == 0.0f)
			bMove = TRUE;
	}

	D3DXVECTOR2 hit;
	D3DXVECTOR2 vecSavePos;
	BOOL bBreak = FALSE;
	// 距離１ずつキャラと当たり判定
	int i=0;
	for (i=0;i<e;i++)
	{
		D3DXVECTOR2 vecCheckPos;
		vecCheckPos = ((float)(i+1)*v1) + pos;
		// キャラとの当たり判定
		for (std::vector< type_session* >::iterator it = pVecCharacters->begin();
			it != pVecCharacters->end();
			it++)
		{
			// スル?
			if (!(*it)->entity													// 不在
			||	(*it)->obj_state == OBJ_STATE_MAIN_DEAD		// ?亡
			||	(*it)->obj_state == OBJ_STATE_MAIN_DROP)	// 落下
				continue;

			type_session* sess = (*it);
			if (common::obj::IsHit(blt->hit_range+CHARA_HIT_RANGE, &vecCheckPos, (*it)))
//			if (common::obj::IsHit(blt->hit_range, &vecCheckPos, (*it)))
			{
				double dElapsed = (double)i/(double)e;
				hit_obj_no = (*it)->obj_no;								// 当たった番号
				hit =  (v1*(blt->hit_range)) + vecCheckPos;		//	当たった位置
				//	当たった直前の位置
				if (bMove)
					vecSavePos = ((float)i*v1) + pos;
				else
					vecSavePos = pos;

				if (blt->proc_type != BLT_PROC_TYPE_ITEM)
				{
					LuaFuncParam luaParams;
					LuaFuncParam luaResults;
					BOOL bLuaFuncRes = FALSE;
					switch (blt->proc_type)
					{
					case BLT_PROC_TYPE_SCR_CHARA:
						// script,弾?イプ,chr_obj_no,obj_no,スクリプト番号,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitChara_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection);
						break;
					case BLT_PROC_TYPE_SCR_SPELL:
						// script,chr_obj_no,obj_no,スクリプト番号,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitChara_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection);
						break;
					case BLT_PROC_TYPE_SCR_STAGE:
						// script,弾?イプ,obj_no,スクリプト番号,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0- dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitChara_StageBullet", &luaResults, 1, &luaParams, pCriticalSection);
						break;
					}
					if (!bLuaFuncRes)	return ret;
					// 1ならストップ
					if (luaResults.GetNumber(0))
					{
						bBreak = TRUE;
						break;
					}
				}
				else
				{
					bBreak = TRUE;
					break;
				}
			}
		}
		if (bBreak)	break;
//> 20110220
		// 弾同士の当たり判定
		for (std::map<int, type_obj*>::iterator it = pMapObjects->begin();
			it != pMapObjects->end();
			it++)
		{
			BYTE objType = ((ptype_blt)(*it).second)->obj_type;
			// 当たり判定の無いオブジェクト
			if ((BYTE)(objType & OBJ_TYPE_TACTIC)) continue;
			// 自分自身ならスル?
			if ( (DWORD)(*it).second == (DWORD)blt)	continue;
			// アイテ?同士スル?
			if ((blt->obj_type & OBJ_TYPE_ITEM) && (objType & OBJ_TYPE_ITEM)) continue;
			// 削除フラグ持ちの弾ならスル?
			if ((*it).second->proc_flg & PROC_FLG_OBJ_REMOVE)	continue;
			int nObjRange = BLT_DEFAULT_HITRANGE;
			if ((*it).second->obj_type & OBJ_TYPE_BLT)
				nObjRange = ((ptype_blt)(*it).second)->hit_range;
			// 当たり判定
			if (common::obj::IsHit(blt->hit_range+nObjRange, &vecCheckPos, (*it).second ))
			{
				LuaFuncParam luaParams;
				LuaFuncParam luaResults;
				double dElapsed = (double)i/(double)e;
				hit_obj_no = (*it).second->obj_no;					// 当たった番号
				hit =  (v1*(blt->hit_range)) + vecCheckPos;		//	当たった位置
				vecSavePos = ((float)i*v1) + pos;						//	当たった直前の位置
				// 移動しないで当たっている場合、当たった位置から半径分離す
				if (!bMove)	// 初期移動位置で埋まっている場合？
				{
					D3DXVECTOR2 norm;
					D3DXVec2Normalize(&norm, &vec);
					// 移動値の逆方向へ弾とキャラの当たり判定分戻す(厳密にはblt->hit_rangeはいらない)
					vecSavePos = vecCheckPos - (norm*((float)(blt->hit_range+1)));
				}

				BOOL bLuaFuncRes = FALSE;
				int nLuaResult = 0;
				// スクリプト処理
				switch (blt->proc_type)
				{
				case BLT_PROC_TYPE_SCR_CHARA:
					luaParams.Clear();		luaResults.Clear();
					// 動いた側のイベント
					// script,弾?イプ,弾作ったキャラのObjNo,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
					luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
					if (!common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection))
						return ret;
					nLuaResult = (int)luaResults.GetNumber(0);
					break;
				case BLT_PROC_TYPE_SCR_SPELL:
					{
						luaParams.Clear();		luaResults.Clear();
						// script,弾?イプ,弾作ったキャラのObjNo,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->chr_obj_no).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						if (!common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection) )
							return ret;
						nLuaResult = (int)luaResults.GetNumber(0);
						break;
					}
				case BLT_PROC_TYPE_SCR_STAGE:
					{
						luaParams.Clear();		luaResults.Clear();
						// script,弾?イプ,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(blt->scrinfo->scr_index).Number(blt->bullet_type).Number(hit_obj_no).Number(blt->obj_no).Number((double)vecSavePos.x).Number((double)vecSavePos.y).Number(hit.x).Number(hit.y).Number(blt->vx).Number(blt->vy).Number(1.0-dElapsed).Number(blt->frame_count).Number(blt->extdata1).Number(blt->extdata2);
						if (!common::scr::CallLuaFunc(pLuah, "onHitBullet_StageBullet", &luaResults, 1, &luaParams, pCriticalSection))
							return ret;
						nLuaResult = (int)luaResults.GetNumber(0);
						break;
					}
				case BLT_PROC_TYPE_ITEM:
					ret = common::blt::MOVE_WAIT_BULLET_RESULT_VEC_CHANGE;
					bBreak = TRUE;
					break;
				default:
					ret = common::blt::MOVE_WAIT_BULLET_RESULT_VEC_CHANGE;
					bBreak = TRUE;
					break;
				}

				// 1ならストップ
				if (nLuaResult)	bBreak = TRUE;

				// 当たられた側が弾ならイベントを知らせる
				if ( (*it).second->obj_type & OBJ_TYPE_BLT)
				{
					ptype_blt hit_blt = (ptype_blt)(*it).second;
					luaParams.Clear();		luaResults.Clear();
					bLuaFuncRes = FALSE;

					switch (hit_blt->proc_type)
					{
					case BLT_PROC_TYPE_SCR_CHARA:
						// script,弾?イプ,弾作ったキャラのObjNo,chr_obj_no,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(hit_blt->scrinfo->scr_index).Number(hit_blt->bullet_type).Number(hit_blt->chr_obj_no).Number(blt->obj_no).Number(hit_obj_no).Number(hit_blt->ax).Number(hit_blt->ay).Number(hit.x).Number(hit.y).Number(hit_blt->vx).Number(hit_blt->vy).Number(0.0).Number(hit_blt->frame_count).Number(hit_blt->extdata1).Number(hit_blt->extdata2);
						bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaBullet", &luaResults, 1, &luaParams, pCriticalSection);
						break;
					case BLT_PROC_TYPE_SCR_SPELL:
						// script,弾作ったキャラのObjNo,chr_obj_no,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(hit_blt->scrinfo->scr_index).Number(hit_blt->chr_obj_no).Number(blt->obj_no).Number(hit_obj_no).Number(hit_blt->ax).Number(hit_blt->ay).Number(hit.x).Number(hit.y).Number(hit_blt->vx).Number(hit_blt->vy).Number(0.0).Number(hit_blt->frame_count).Number(hit_blt->extdata1).Number(hit_blt->extdata2);
						bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitBullet_CharaSpell", &luaResults, 1, &luaParams, pCriticalSection);
						break;
					case BLT_PROC_TYPE_SCR_STAGE:
						// script,弾?イプ,hit_obj_no,obj_no,当たった位置x,y/移動値x,y/残り移動時間0.0?1.0/extdata
						luaParams.Number(hit_blt->scrinfo->scr_index).Number(hit_blt->bullet_type).Number(hit_obj_no).Number(blt->obj_no).Number(hit_blt->ax).Number(hit_blt->ay).Number(hit.x).Number(hit.y).Number(hit_blt->vx).Number(hit_blt->vy).Number(0.0).Number(hit_blt->frame_count).Number(hit_blt->extdata1).Number(hit_blt->extdata2);
						bLuaFuncRes = common::scr::CallLuaFunc(pLuah, "onHitBullet_StageBullet", &luaResults, 1, &luaParams, pCriticalSection);
						break;
					}
					if (!bLuaFuncRes)	return ret;
					// スクリプトによって?作されたか
					if (blt->proc_flg & (PROC_FLG_OBJ_REMOVE|PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS))
						bBreak = TRUE;
				}
				if (bBreak)	break;
			}
		}
		if (bBreak)	break;
//< 20110220
	}
	double dElapsed = 0.0f;
	// 当たった位置に修正
	if (bBreak)
	{
		dElapsed = (double)i/(double)e;
		common::blt::UpdateBulletVec(&vec, blt, nWind, dElapsed);
		vec*=(BLT_POS_FACT_N);
		blt->vx = (short)(vec.x*(float)BLT_VEC_FACT_N);
		blt->vy = (short)(vec.y*(float)BLT_VEC_FACT_N);
		//> 20110130 オ?バ?フロ?対策
//		blt->bx = (short)(vecSavePos.x*BLT_VEC_FACT_N);
//		blt->by = (short)(vecSavePos.y*BLT_VEC_FACT_N);
//		blt->ax = (short)(vecSavePos.x);
//		blt->ay = (short)(vecSavePos.y);
		blt->bx = (short)max(min(32767, (int)(vecSavePos.x*BLT_VEC_FACT_N)), -32768);
		blt->by = (short)max(min(32767, (int)(vecSavePos.y*BLT_VEC_FACT_N)), -32768);
		blt->ax = (short)((float)blt->bx*BLT_POS_FACT_F);
		blt->ay = (short)((float)blt->by*BLT_POS_FACT_F);
		//< 20110130 オ?バ?フロ?対策
	}
	else if (bMove)
	{
		vec*=(BLT_POS_FACT_N);
		blt->vx = (short)(vec.x*(float)BLT_VEC_FACT_N);
		blt->vy = (short)(vec.y*(float)BLT_VEC_FACT_N);
		//> 20110130 オ?バ?フロ?対策
//		blt->bx += (short)vec.x;
//		blt->by += (short)vec.y;
		blt->bx = (short)max(min(32767, (int)vec.x+(int)blt->bx), -32768);
		blt->by = (short)max(min(32767, (int)vec.y+(int)blt->by), -32768);
		//< 20110130 オ?バ?フロ?対策
		blt->ax = (short)((float)blt->bx*BLT_POS_FACT_F);
		blt->ay = (short)((float)blt->by*BLT_POS_FACT_F);
	}
	// Lua
//	if (blt->proc_type == BLT_PROC_TYPE_SCR_CHARA || blt->proc_type == BLT_PROC_TYPE_SCR_STAGE)
	{
		// スクリプトによる削除がはっせいしたか
		if (blt->proc_flg & PROC_FLG_OBJ_REMOVE)
		{
///			common::obj::SetLuaFlg((type_obj*)blt, LUA_FLG_OBJ_REMOVE, FALSE);
			return common::blt::MOVE_WAIT_BULLET_RESULT_REMOVED;
		}

/*	20110103	スクリプト側で情報を送るからいらない？
		// スクリプトによって移動値、位置が変えられたか
		if (blt->proc_flg & (PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS))
		{
			common::obj::SetLuaFlg((type_obj*)blt, (PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS), FALSE);
			return common::blt::MOVE_WAIT_BULLET_RESULT_VEC_CHANGE;
		}
*/
		common::obj::SetLuaFlg((type_obj*)blt, (PROC_FLG_OBJ_UPDATE_VEC|PROC_FLG_OBJ_UPDATE_POS), FALSE);
	}
	return ret;
}

void common::obj::SetLuaFlg(type_obj* obj, DWORD flg, BOOL on)
{
	if (on)
  		obj->proc_flg |= flg;
	else
		obj->proc_flg &= (flg^0xFFFFFFFF);
}

// 当たり判定
BOOL common::obj::IsHit(int range, D3DXVECTOR2* pos, ptype_obj obj)
{
	D3DXVECTOR2 cc = D3DXVECTOR2(obj->ax, obj->ay)-(*pos);
	return (D3DXVec2LengthSq(&cc) <= range*range);
}

