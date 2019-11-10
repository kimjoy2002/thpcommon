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

	// �����蔻��
	// hit				: out ���������ӏ�(pixel)
	// hit_pos		: out ���������ʒu
	// pos			: �ʒu
	// vec			: �ړ��l
	// range		: �����蔼�a
	// return		: true=hit
	BOOL HitTest(D3DXVECTOR2* hit_spot, D3DXVECTOR2* hit_pos, float* elapsed, D3DXVECTOR2* pos, D3DXVECTOR2* vec, int range);

	// ���ւ̓����蔻��
	// hit				: out ���������ӏ�(pixel)
	// hit_pos		: out ���������ʒu
	// pos			: �ʒu
	// vec			: �ړ��l
	// range		: �����蔼�a
	// return		: true=hit
	BOOL LowHitTest(D3DXVECTOR2* hit_spot, D3DXVECTOR2* hit_pos, float* elapsed, D3DXVECTOR2* pos, D3DXVECTOR2* vec, int range);

	// �e��������������
	// pos			: �ʒu
	// range		: ���͈�
	void HitBullet(D3DXVECTOR2* pos, int range);

	BOOL IsHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range);
	BOOL IsSomeLowHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range);
	BOOL IsSomeHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range);

	// �n�ʂ�T��
	// dest_my_pos					: �������������̈ʒu
	// dest_ground_pos			: ���������ڒn�ʒu
	// first_pos						: �T���n�߂̈ʒu
	// range							: ���a
	// return							: ����������TRUE
	BOOL FindGround(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);

	// �n�ʂ�T��
	// dest_my_pos					: �������������̈ʒu
	// dest_ground_pos			: ���������ڒn�ʒu
	// first_pos						: �T���n�߂̈ʒu
	// range							: ���a
	// return							: ����������TRUE
	BOOL FindGroundDown(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);
	// �n?��T��
	// dest_my_pos					: �������������̈ʒu
	// dest_ground_pos			: ���������ڒn�ʒu
	// first_pos						: �T���n�߂̈ʒu
	// range							: ���a
	// return							: ����������TRUE
	BOOL FindGroundUp(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);
	// �n?��T��
	// dest_my_pos					: �������������̈ʒu
	// dest_ground_pos			: ���������ڒn�ʒu
	// first_pos						: �T���n�߂̈ʒu
	// range							: ���a
	// return							: ����������TRUE
	BOOL FindSomeGroundUp(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range);

	// �n�ʂ�����
	// pos		: �����ʒu
	// range	: �������a
	// return	: 1�s�N�Z���ȏ�폜���ꂽ��
	int EraseStage(D3DXVECTOR2* pos, int range);

	// �X�e?�W�։摜?��t��
	BOOL PasteImage(CFiler* pFiler, WCHAR *path, RECT* pImageRect, int nStageOrignX, int nStageOrignY);
	// �X�e?�W�։摜?��t��
	BOOL PasteImage(LPDIRECT3DDEVICE9 pDev, RECT* pImageRect, int nStageOrignX, int nStageOrignY, LPDIRECT3DTEXTURE9 pCopyTexture);

	// �ǂݍ���ł���PNG�C��?�W����e�N�X?���쐬
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
