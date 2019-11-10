#ifndef H_CLASS_CRITICAL_SECTION___
#define H_CLASS_CRITICAL_SECTION___

#include <windows.h>
#include "TCHAR.h"
#include "util.h"
#include <stack>

//#define DEBUGCS
#ifndef _DEBUG
//#define DEBUGCS
#endif
#ifdef DEBUGCS
#define EnterCriticalSection_Session(x)	EnterCriticalSection_SessionD(x)
#define LeaveCriticalSection_Session		LeaveCriticalSection_SessionD

#define EnterCriticalSection_Object(x)	EnterCriticalSection_ObjectD(x)
#define LeaveCriticalSection_Object		LeaveCriticalSection_ObjectD

#define EnterCriticalSection_Packet(x)	EnterCriticalSection_PacketD(x)
#define LeaveCriticalSection_Packet		LeaveCriticalSection_PacketD

#define EnterCriticalSection_StageTexture(x)	EnterCriticalSection_StageTextureD(x)
#define LeaveCriticalSection_StageTexture		LeaveCriticalSection_StageTextureD

#define EnterCriticalSection_Effect(x)	EnterCriticalSection_EffectD(x)
#define LeaveCriticalSection_Effect		LeaveCriticalSection_EffectD

#define EnterCriticalSection_Sound(x)	EnterCriticalSection_SoundD(x)
#define LeaveCriticalSection_Sound		LeaveCriticalSection_SoundD

#define EnterCriticalSection_Lua(x)	EnterCriticalSection_LuaD(x)
#define LeaveCriticalSection_Lua		LeaveCriticalSection_LuaD

#define EnterCriticalSection_Log(x)	EnterCriticalSection_LogD(x)
#define LeaveCriticalSection_Log		LeaveCriticalSection_LogD

#define EnterCriticalSection_Input(x)	EnterCriticalSection_InputD(x)
#define LeaveCriticalSection_Input		LeaveCriticalSection_InputD

#else
#define EnterCriticalSection_Session(x)	EnterCriticalSection_SessionR()
#define LeaveCriticalSection_Session()	LeaveCriticalSection_SessionR()
#define GetSessoinWord()	0L

#define EnterCriticalSection_Object(x)	EnterCriticalSection_ObjectR()
#define LeaveCriticalSection_Object()	LeaveCriticalSection_ObjectR()
#define GetObjectWord()	0L

#define EnterCriticalSection_Packet(x)	EnterCriticalSection_PacketR()
#define LeaveCriticalSection_Packet		LeaveCriticalSection_PacketR
#define GetObjectPacket()	0L

#define EnterCriticalSection_StageTexture(x)	EnterCriticalSection_StageTextureR()
#define LeaveCriticalSection_StageTexture		LeaveCriticalSection_StageTextureR
#define GetObjectStageTexture()	0L

#define EnterCriticalSection_Effect(x)	EnterCriticalSection_EffectR()
#define LeaveCriticalSection_Effect		LeaveCriticalSection_EffectR
#define GetObjectEffect()	0L

#define EnterCriticalSection_Sound(x)	EnterCriticalSection_SoundR()
#define LeaveCriticalSection_Sound		LeaveCriticalSection_SoundR
#define GetObjectSound()	0L

#define EnterCriticalSection_Lua(x)	EnterCriticalSection_LuaR()
#define LeaveCriticalSection_Lua		LeaveCriticalSection_LuaR
#define GetObjectLua()	0L

#define EnterCriticalSection_Log(x)	EnterCriticalSection_LogR()
#define LeaveCriticalSection_Log		LeaveCriticalSection_LogR
#define GetObjectLog()	0L

#define EnterCriticalSection_Input(x)	EnterCriticalSection_InputR()
#define LeaveCriticalSection_Input		LeaveCriticalSection_InputR
#define GetObjectInput()	0L

#endif

class CCriticalSection
{
public:
	CCriticalSection()
	{
		InitializeCriticalSection(&m_CriticalSectionSession);
		InitializeCriticalSection(&m_CriticalSectionPacket);
		m_CriticalSectionObject = m_CriticalSectionSession;
//		InitializeCriticalSection(&m_CriticalSectionObject);
		InitializeCriticalSection(&m_CriticalSectionStageTexture);
		InitializeCriticalSection(&m_CriticalSectionEffect);
		InitializeCriticalSection(&m_CriticalSectionSound);
		InitializeCriticalSection(&m_CriticalSectionLua);
		InitializeCriticalSection(&m_CriticalSectionLog);
#if !USE_DINPUT
		InitializeCriticalSection(&m_CriticalSectionInput);
#endif
	};
	~CCriticalSection()
	{
#ifdef DEBUGCS
		if (!m_stackSession.empty())
			OutputDebugStr(L"!CCriticalSection::Session\n");
		if (!m_stackPacket.empty())
			OutputDebugStr(L"!CCriticalSection::Packet\n");
//		if (m_stackObject.size())
//			OutputDebugStr(L"!CCriticalSection::Object");
		if (!m_stackStageTexture.empty())
			OutputDebugStr(L"!CCriticalSection::StageTexture\n");
		if (!m_stackEffect.empty())
			OutputDebugStr(L"!CCriticalSection::Effect\n");
		if (!m_stackSound.empty())
			OutputDebugStr(L"!CCriticalSection::Sound\n");
		if (!m_stackLua.empty())
			OutputDebugStr(L"!CCriticalSection::Lua\n");
		if (!m_stackLog.empty())
			OutputDebugStr(L"!CCriticalSection::Log\n");
#if !USE_DINPUT
		if (!m_stackInput.empty())
			OutputDebugStr(L"!CCriticalSection::m_bInputFlg\n");
#endif
#endif
		OutputDebugStr(L"m_CriticalSectionPacket\n");
		DelCS(&m_CriticalSectionPacket);
		OutputDebugStr(L"m_CriticalSectionSession\n");
		DelCS(&m_CriticalSectionSession);
//		OutputDebugStr(L"m_CriticalSectionObject");
//		DelCS(&m_CriticalSectionObject);
		OutputDebugStr(L"m_CriticalSectionStageTexture\n");
		DelCS(&m_CriticalSectionStageTexture);
		OutputDebugStr(L"m_CriticalSectionEffect\n");
		DelCS(&m_CriticalSectionEffect);
		OutputDebugStr(L"m_CriticalSectionSound\n");
		DelCS(&m_CriticalSectionSound);
		OutputDebugStr(L"m_CriticalSectionLua\n");
		DelCS(&m_CriticalSectionLua);
		OutputDebugStr(L"m_CriticalSectionLog\n");
		DelCS(&m_CriticalSectionLog);
#if !USE_DINPUT
		DelCS(&m_CriticalSectionInput);
#endif
	};

// inline処理
#ifdef DEBUGCS
	inline void DelCS(CRITICAL_SECTION* cs)
	{
		if (!TryEnterCriticalSection(cs))
		{
			OutputDebugStr(L"TryEnterCriticalSection:true\n");
			LeaveCriticalSection(cs);
		}
		DeleteCriticalSection(cs);
	};
#else
	inline void DelCS(CRITICAL_SECTION* cs)
	{
		if (!TryEnterCriticalSection(cs))
			LeaveCriticalSection(cs);
		DeleteCriticalSection(cs);
	};
#endif
	// Session
#ifdef DEBUGCS
	inline void EnterCriticalSection_SessionD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionSession))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackSession.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Session", MB_OK);
			}
		}
		m_stackSession.push(w);
	};
	inline void LeaveCriticalSection_SessionD()
	{
		m_stackSession.pop();
		LeaveCriticalSection(&m_CriticalSectionSession);
	};
	inline std::stack<WCHAR>& GetSessionWord()
	{	return m_stackSession;	};
#else
	inline void EnterCriticalSection_SessionR()
	{
		EnterCriticalSection(&m_CriticalSectionSession);
	};
	inline void LeaveCriticalSection_SessionR()
	{
		LeaveCriticalSection(&m_CriticalSectionSession);
	};
#endif
	// Packet
#ifdef DEBUGCS
	inline void EnterCriticalSection_PacketD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionPacket))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackPacket.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Packet", MB_OK);
			}
		}
		m_stackPacket.push(w);
	};
	inline void LeaveCriticalSection_PacketD()
	{
		m_stackPacket.pop();
		LeaveCriticalSection(&m_CriticalSectionPacket);
	};
	inline std::stack<WCHAR>& GetPacketWord()
	{	return m_stackPacket;	};
#else
	inline void EnterCriticalSection_PacketR()
	{
		EnterCriticalSection(&m_CriticalSectionPacket);
	};
	inline void LeaveCriticalSection_PacketR()
	{
		LeaveCriticalSection(&m_CriticalSectionPacket);
	};
#endif
	// Object

#ifdef DEBUGCS
	inline void EnterCriticalSection_ObjectD(WCHAR w)
	{
//		EnterCriticalSection(&m_CriticalSectionObject);
//		m_stackObject.push(w);
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionSession))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackSession.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Session", MB_OK);
			}
		}
		m_stackSession.push(w);
	};
	inline void LeaveCriticalSection_ObjectD()
	{
//		m_stackObject.pop();
//		LeaveCriticalSection(&m_CriticalSectionObject);
		m_stackSession.pop();
		LeaveCriticalSection(&m_CriticalSectionSession);
	};
	inline std::stack<WCHAR>& GetObjectWord()
	{	return m_stackObject;	};
#else
	inline void EnterCriticalSection_ObjectR()
	{
		EnterCriticalSection(&m_CriticalSectionSession);
	};
	inline void LeaveCriticalSection_ObjectR()
	{
		LeaveCriticalSection(&m_CriticalSectionSession);
	};
#endif
	// StageTexture
#ifdef DEBUGCS
	inline void EnterCriticalSection_StageTextureD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionStageTexture))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackStageTexture.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section StageTexture", MB_OK);
			}
		}
		m_stackStageTexture.push(w);
	};
	inline void LeaveCriticalSection_StageTextureD()
	{
		m_stackStageTexture.pop();
		LeaveCriticalSection(&m_CriticalSectionStageTexture);
	};
	inline std::stack<WCHAR>& GetStageTextureWord()
	{	return m_stackStageTexture;	};
#else
	inline void EnterCriticalSection_StageTextureR()
	{
		EnterCriticalSection(&m_CriticalSectionStageTexture);
	};
	inline void LeaveCriticalSection_StageTextureR()
	{
		LeaveCriticalSection(&m_CriticalSectionStageTexture);
	};
#endif
	// Effect
#ifdef DEBUGCS
	inline void EnterCriticalSection_EffectD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionEffect))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackEffect.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Effect", MB_OK);
			}
		}
		m_stackEffect.push(w);
	};
	inline void LeaveCriticalSection_EffectD()
	{
		m_stackEffect.pop();
		LeaveCriticalSection(&m_CriticalSectionEffect);
	};
	inline std::stack<WCHAR>& GetEffectWord()
	{	return m_stackEffect;	};
#else
	inline void EnterCriticalSection_EffectR()
	{
		EnterCriticalSection(&m_CriticalSectionEffect);
	};
	inline void LeaveCriticalSection_EffectR()
	{
		LeaveCriticalSection(&m_CriticalSectionEffect);
	};
#endif
	// Sound
#ifdef DEBUGCS
	inline void EnterCriticalSection_SoundD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionSound))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackSound.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Sound", MB_OK);
			}
		}
		m_stackSound.push(w);
	};
	inline void LeaveCriticalSection_SoundD()
	{
		m_stackSound.pop();
		LeaveCriticalSection(&m_CriticalSectionSound);
	};
	inline std::stack<WCHAR>& GetSoundWord()
	{	return m_stackSound;	};
#else
	inline void EnterCriticalSection_SoundR()
	{
		EnterCriticalSection(&m_CriticalSectionSound);
	};
	inline void LeaveCriticalSection_SoundR()
	{
		LeaveCriticalSection(&m_CriticalSectionSound);
	};
#endif
	// Lua
#ifdef DEBUGCS
	inline void EnterCriticalSection_LuaD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionLua))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackLua.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Lua", MB_OK);
			}
		}
		m_stackLua.push(w);
	};
	inline void LeaveCriticalSection_LuaD()
	{
		m_stackLua.pop();
		LeaveCriticalSection(&m_CriticalSectionLua);
	};
	inline std::stack<WCHAR>& GetLuaWord()
	{	return m_stackLua;	};
#else
	inline void EnterCriticalSection_LuaR()
	{
		EnterCriticalSection(&m_CriticalSectionLua);
	};
	inline void LeaveCriticalSection_LuaR()
	{
		LeaveCriticalSection(&m_CriticalSectionLua);
	};
#endif
	// Log
#ifdef DEBUGCS
	inline void EnterCriticalSection_LogD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionLog))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackLog.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Log", MB_OK);
			}
		}
		m_stackLog.push(w);
	};
	inline void LeaveCriticalSection_LogD()
	{
		m_stackLog.pop();
		LeaveCriticalSection(&m_CriticalSectionLog);
	};
	inline std::stack<WCHAR>& GetLogWord()
	{	return m_stackLog;	};
#else
	inline void EnterCriticalSection_LogR()
	{
		EnterCriticalSection(&m_CriticalSectionLog);
	};
	inline void LeaveCriticalSection_LogR()
	{
		LeaveCriticalSection(&m_CriticalSectionLog);
	};
#endif
#if !USE_DINPUT
	// Input
#ifdef DEBUGCS
	inline void EnterCriticalSection_InputD(WCHAR w)
	{
		int i=0;
		while (!TryEnterCriticalSection(&m_CriticalSectionInput))
		{
			Sleep(1);
			if (++i == 3000)
			{
				WCHAR ws[3] = {m_stackInput.top(), w, L'\0'};
				MessageBox(NULL, ws, L"critical section Input", MB_OK);
			}
		}
		m_stackInput.push(w);
	};
	inline void LeaveCriticalSection_InputD()
	{
		m_stackInput.pop();
		LeaveCriticalSection(&m_CriticalSectionInput);
	};
	inline std::stack<WCHAR>& GetInputWord()
	{	return m_stackInput;	};
#else
	inline void EnterCriticalSection_InputR()
	{
		EnterCriticalSection(&m_CriticalSectionInput);
	};
	inline void LeaveCriticalSection_InputR()
	{
		LeaveCriticalSection(&m_CriticalSectionInput);
	};
#endif
#endif
private:

// 変数
	CRITICAL_SECTION m_CriticalSectionPacket;				// パケットキュ?保護
	CRITICAL_SECTION m_CriticalSectionSession;			// ユ?ザ?情報保護
	CRITICAL_SECTION m_CriticalSectionObject;				// オブジェクト情報保護
	CRITICAL_SECTION m_CriticalSectionStageTexture;	// テクス?ャ情報保護
	CRITICAL_SECTION m_CriticalSectionEffect;				// エフェクト情報保護
	CRITICAL_SECTION m_CriticalSectionSound;				// サウンド情報保護
	CRITICAL_SECTION m_CriticalSectionLua;					// Lua情報保護
	CRITICAL_SECTION m_CriticalSectionLog;					// Log情報保護
#if !USE_DINPUT
	CRITICAL_SECTION m_CriticalSectionInput;				// 入力情報保護
#endif
#ifdef DEBUGCS
	std::stack<WCHAR> m_stackSession;
	std::stack<WCHAR> m_stackObject;
	std::stack<WCHAR> m_stackPacket;
	std::stack<WCHAR> m_stackStageTexture;
	std::stack<WCHAR> m_stackEffect;
	std::stack<WCHAR> m_stackSound;
	std::stack<WCHAR> m_stackLua;
	std::stack<WCHAR> m_stackLog;
#if !USE_DINPUT
	std::stack<WCHAR> m_stackInput;
#endif
#endif
};

#endif
