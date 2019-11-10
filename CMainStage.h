#ifndef H_CLASS_MAIN_STAGE___
#define H_CLASS_MAIN_STAGE___

#include <windows.h>
#include "TCHAR.h"
#include <math.h>
#include "util.h"
#include <png.h>
#include <map>
#include <vector>
#include "PngLoader.h"

#ifdef _DEBUG
#pragma comment(lib, "PngLoader_d.lib")
#else
#pragma comment(lib, "PngLoader.lib")
#endif

#define MAIN_STAGE_ALPHA_MASK	(0xFF000000)
#define MAIN_STAGE_GETPIXEL_OUTOFSTAGE	(0x00000000)

class CMainStage
{
public:
	CMainStage();
	virtual ~CMainStage();

	BOOL Init(CFiler* pFiler, WCHAR *path, const SIZE* sizeStage);
	BOOL Init(BYTE* pBuffer, int nSize, const SIZE *sizeStage);
	void Clear();
	
	BOOL IsStageLoad() { return m_bStageLoad;	};

	// 当たり判定
	// hit				: out 当たった箇所(pixel)
	// hit_pos		: out 当たった位置
	// pos			: 位置
	// vec			: 移動値
	// range		: 当たり半径
	// return		: true=hit
	BOOL HitTest(D3DXVECTOR2* hit_spot, D3DXVECTOR2* hit_pos, float* elapsed, D3DXVECTOR2* pos, D3DXVECTOR2* vec, int range);

	// 下への当たり判定
	// hit				: out 当たった箇所(pixel)
	// hit_pos		: out 当たった位置
	// pos			: 位置
	// vec			: 移動値
	// range		: 当たり半径
	// return		: true=hit
	BOOL LowHitTest(D3DXVECTOR2* hit_spot, D3DXVECTOR2* hit_pos, float* elapsed, D3DXVECTOR2* pos, D3DXVECTOR2* vec, int range);

	// 弾が当たった処理
	// pos			: 位置
	// range		: 削る範囲
	void HitBullet(D3DXVECTOR2* pos, int range);

	BOOL IsHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range);
	BOOL IsSomeLowHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range);
	BOOL IsSomeHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range);

	// 地面を探す
	// dest_my_pos					: 見つかった自分の位置
	// dest_ground_pos			: 見つかった接地位置
	// first_pos						: 探し始めの位置
	// range							: 半径
	// return							: 見つかったらTRUE
	BOOL FindGround(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);

	// 地面を探す
	// dest_my_pos					: 見つかった自分の位置
	// dest_ground_pos			: 見つかった接地位置
	// first_pos						: 探し始めの位置
	// range							: 半径
	// return							: 見つかったらTRUE
	BOOL FindGroundDown(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);
	// 地?を探す
	// dest_my_pos					: 見つかった自分の位置
	// dest_ground_pos			: 見つかった接地位置
	// first_pos						: 探し始めの位置
	// range							: 半径
	// return							: 見つかったらTRUE
	BOOL FindGroundUp(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);
	// 地?を探す
	// dest_my_pos					: 見つかった自分の位置
	// dest_ground_pos			: 見つかった接地位置
	// first_pos						: 探し始めの位置
	// range							: 半径
	// return							: 見つかったらTRUE
	BOOL FindSomeGroundUp(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);

	// 地面を消す
	// pos		: 消す位置
	// range	: 消す半径
	// return	: 1ピクセル以上削除されたか
	int EraseStage(D3DXVECTOR2* pos, int range);

	// ステ?ジへ画像?り付け
	BOOL PasteImage(CFiler* pFiler, WCHAR *path, RECT* pImageRect, int nStageOrignX, int nStageOrignY);
	// ステ?ジへ画像?り付け
	BOOL PasteImage(LPDIRECT3DDEVICE9 pDev, RECT* pImageRect, int nStageOrignX, int nStageOrignY, LPDIRECT3DTEXTURE9 pCopyTexture);

	// 読み込んであるPNGイメ?ジからテクス?ャ作成
	BOOL CreateTexture(LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 *ppCreatedTexture);

	inline int GetWidth()	{	return m_tPngImage.width;	};
	inline int GetHeight()	{	return m_tPngImage.height;	};
	inline int GetStageWidth() { return m_nStageWidth;	};
	inline int GetStageHeight() { return m_nStageHeight;	};
	const TPngImage* GetImage() const { return &m_tPngImage;	};

	BOOL GetGroundsXPos(std::vector<int>* vecXPos);

protected:
	DWORD GetStagePixel(int x,int y);
	void SetStagePixel(int x,int y, DWORD px);

	int m_nStageWidth;
	int m_nStageHeight;

	TPngImage m_tPngImage;
	BOOL m_bStageLoad;
};

#endif
