#include "CMainStage.h"

CMainStage::CMainStage()
{
	m_bStageLoad = FALSE;
	m_nStageWidth = 0;
	m_nStageHeight = 0;
	ZeroMemory(&m_tPngImage, sizeof(TPngImage));
}

CMainStage::~CMainStage()
{
	Clear();
}

void CMainStage::Clear()
{
	SafeFree(m_tPngImage.data);
	SafeFree(m_tPngImage.lines);
	ZeroMemory(&m_tPngImage, sizeof(TPngImage));
	m_nStageWidth = 0;
	m_nStageHeight = 0;
	m_bStageLoad = FALSE;
}

BOOL CMainStage::Init(BYTE* pBuffer, int nSize, const SIZE *sizeStage)
{
	Clear();
	if (!PngLoader::PngLoadFromFileInMemory(&m_tPngImage.data, &m_tPngImage.lines, pBuffer, (UINT)nSize, &m_tPngImage.width, &m_tPngImage.height))
		return FALSE;
	m_nStageWidth = min(m_tPngImage.width, sizeStage->cx);
	m_nStageHeight = min(m_tPngImage.height, sizeStage->cy);

	m_bStageLoad = TRUE;
	return TRUE;
}

BOOL CMainStage::Init(CFiler* pFiler, WCHAR *path, const SIZE *sizeStage)
{
	Clear();
	BYTE* pBuffer = NULL;
	UINT	nBufferSize = 0;
	BOOL ret = FALSE;

	if (!PngLoader::PngLoadFromFileInMemory(&m_tPngImage, path, pFiler))
		return FALSE;
	
	m_nStageWidth = sizeStage->cx;
	m_nStageHeight = sizeStage->cy;

	m_bStageLoad = TRUE;
	return TRUE;
}

// �����蔻��
// hit	_			: out ���������ӏ�(pixel)
// hit_pos		: out ���������ʒu
// pos			: �ʒu
// vec			: �ړ��l
// range		: �����蔼�a
// return		: true=hit
BOOL CMainStage::HitTest(D3DXVECTOR2* hit_spot, D3DXVECTOR2* hit_pos, float* elapsed, D3DXVECTOR2* pos, D3DXVECTOR2* vec, int range)
{
	// ����
	FLOAT fLength = D3DXVec2Length(vec);
	*hit_spot = D3DXVECTOR2(0,0);
	// ����1���}���
	int e = (int)ceil(fLength);	// �؂�グ
	// �ړ����Ȃ�
	if (e < 1)	return FALSE;
	D3DXVECTOR2 v1 = (*vec) / (float)e;	// �ړ�1��
	// �����Ȃ��悤�ɋ���1�������蔻��
	for (int i=0;i<e;i++)
	{
		D3DXVECTOR2 vecChkPos = v1 * ((float)(i+1)) + (*pos);	// �`�F�b�N�ʒu
		if (IsHit(hit_spot, &vecChkPos,/* vec, */range))
		{
			*elapsed = (i>0)?(float)i/(float)e:0.0f;
			*hit_pos = vecChkPos;
			return TRUE;
		}
	}

	return FALSE;
}

// ���ւ̓����蔻��
BOOL CMainStage::LowHitTest(D3DXVECTOR2* hit_spot, D3DXVECTOR2* hit_pos, float* elapsed, D3DXVECTOR2* pos, D3DXVECTOR2* vec, int range)
{	// ����
	FLOAT fLength = D3DXVec2Length(vec);
	*hit_spot = D3DXVECTOR2(0,0);
	// ����1���}���
	int e = (int)ceil(fLength);	// �؂�グ
	// �X�e�[�W�O���肵�Ȃ��悤�␳
	if ((int)pos->y+e > GetStageHeight()-1)
		e = GetStageHeight() - (int)pos->y;
	// �ړ����Ȃ�
	if (e < 1)	return FALSE;
	D3DXVECTOR2 v1 = (*vec) / (float)e;	// �ړ�1��
	// �����Ȃ��悤�ɋ���1�������蔻��
	for (int i=0;i<e;i++)
	{
		D3DXVECTOR2 vecChkPos = v1 * ((float)(i+1)) + (*pos);	// �`�F�b�N�ʒu
		if (IsSomeLowHit(hit_spot, &vecChkPos,/* vec, */range))
		{
			*elapsed = (i>0)?(float)i/(float)e:0.0f;
			*hit_pos = vecChkPos;
			return TRUE;
		}
	}
	return FALSE;
}

// 20101118�œK�� 
BOOL CMainStage::IsSomeLowHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range)
{
	int is	=	(int)floor(pos->y+0.5);
	is = max(0, min(GetStageHeight(), is));
	int ie	=	range+(int)floor(pos->y+0.5);
	ie = max(0, min(GetStageHeight(), ie));
	int js	=	-range+(int)floor(pos->x+0.5);
	js = max(0, min(GetStageWidth(), js));
	int je	=	range+(int)floor(pos->x+0.5);
	je = max(0, min(GetStageWidth(), je));

	double dRangeLen = (double)range*(double)range;
	for (int i=is;i != ie;i++)
	{
		DWORD* p = m_tPngImage.lines[i];
		p += js;
		for (int j=js;j != je;j++)
		{
//			DWORD px = GetStagePixel(j,i);	// �F�擾
			DWORD px = *p;

			if (/*	px ^ MAIN_STAGE_GETPIXEL_OUTOFSTAGE	// �X�e�[�W�O
			&&*/	px&MAIN_STAGE_ALPHA_MASK						// �s�N�Z���L��
			)
			{
				// ���蒆�S�ʒu����s�N�Z���ʒu�̋��������߂�
				D3DXVECTOR2 p = D3DXVECTOR2((float)j, (float)i) - (*pos);
				float fPxLen = D3DXVec2LengthSq(&p);
				if (fPxLen <= dRangeLen)						// ����������
				{
					// �������L��
					hit->x = (float)j;
					hit->y = (float)i;
					return TRUE;
				}
			}
			p++;
		}
	}
	return FALSE;
}

// 20101118�œK��
// ���������邩�`�F�b�N
BOOL CMainStage::IsSomeHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range)
{
	int is	= -range+(int)floor(pos->y+0.5);
	is = max(0, min(GetStageHeight(), is));
	int ie	=	range+(int)floor(pos->y+0.5);
	ie = max(0, min(GetStageHeight(), ie));
	int js	=	-range+(int)floor(pos->x+0.5);
	js = max(0, min(GetStageWidth(), js));
	int je	=	range+(int)floor(pos->x+0.5);
	je = max(0, min(GetStageWidth(), je));

	double dRangeLen = (double)range*(double)range;
	for (int i=is;i != ie;i++)
	{
		DWORD* p = m_tPngImage.lines[i];
		p += js;
		for (int j=js;j != je;j++)
		{
//			DWORD px = GetStagePixel(j,i);	// �F�擾
			DWORD px = *p;

			if (/*	px ^ MAIN_STAGE_GETPIXEL_OUTOFSTAGE	// �X�e�[�W�O
			&&*/	px&MAIN_STAGE_ALPHA_MASK						// �s�N�Z���L��
			)
			{
				// ���蒆�S�ʒu����s�N�Z���ʒu�̋��������߂�
				D3DXVECTOR2 p = D3DXVECTOR2((float)j, (float)i) - (*pos);
				float fPxLen = D3DXVec2LengthSq(&p);
				if (fPxLen <= dRangeLen)						// ����������
				{
					// �������L��
					hit->x = (float)j;
					hit->y = (float)i;
					return TRUE;
				}
			}
			p++;
		}
	}
	return FALSE;
}

BOOL CMainStage::IsHit(D3DXVECTOR2 *hit, D3DXVECTOR2* pos, int range)
{
	std::multimap<float, POINTS, greater<float>> mapHits;
	POINTS psHit;

	int is	=	max(0,-range+(int)floor(pos->y+0.5));
//	int is	=	max(-range,-range+(int)floor(pos->y+0.5));
	int ie	=	min(GetStageHeight(), max(0, range+(int)floor(pos->y+0.5)));
//	int ie	=	min(GetStageHeight(), max(1, range+(int)floor(pos->y+0.5)));
//	nt js	=	max(-range,-range+(int)floor(pos->x+0.5));
	int js	=	max(0,-range+(int)floor(pos->x+0.5));
	int je	=	min(GetStageWidth(), range+(int)floor(pos->x+0.5));

	double dRangeLen = (double)range*(double)range;
	BOOL bHit = FALSE;
	for (int i=is;i < ie;i++)
	{
		DWORD* p = m_tPngImage.lines[i];
		p += js;
		for (int j=js;j < je;j++)
		{
//			DWORD px = GetStagePixel(j,i);	// �F�擾
			DWORD px = *p;

			if (/*	px ^ MAIN_STAGE_GETPIXEL_OUTOFSTAGE	// �X�e�[�W�O
			&&*/	px&MAIN_STAGE_ALPHA_MASK						// �s�N�Z���L��
			)
			{
				// ���蒆�S�ʒu����s�N�Z���ʒu�̋��������߂�
				D3DXVECTOR2 p = D3DXVECTOR2((float)j, (float)i) - (*pos);
				float fPxLen = D3DXVec2LengthSq(&p);
				if (fPxLen <= dRangeLen)						// ����������
				{
					// �������L��
					psHit.x = j;		psHit.y = i;
					mapHits.insert( std::multimap<float, POINTS, greater<float>>::value_type( fPxLen, psHit ) );
					bHit = TRUE;
				}
			}
			p++;
		}
	}
	// multimap�̎����\�[�g�ŋߋ����𓖂������ʒu�ɐݒ�
	if (bHit)
	{
		std::multimap<float, POINTS, greater<float>>::iterator itFirst = mapHits.begin();
		hit->x = (float)(*itFirst).second.x;
		hit->y = (float)(*itFirst).second.y;
	}
//	hit->x = (float)j;
//	hit->y = (float)i;
	mapHits.clear();
	return bHit;
}

// �e��������������
// pos			: �ʒu
// range		: ���͈�
void CMainStage::HitBullet(D3DXVECTOR2* pos, int range)
{
	// �͈�0�Ȃ珈�����Ȃ�
	if (!range)	return;
	// �X�e�[�W��̒e�ō��ʒu
	int x = max((int)pos->x-range, 0);											// �͈�left
	int y = max((int)pos->y-range, 0);											// �͈�top
	int ex = min(m_nStageWidth, (int)pos->x+range);	// �͈�right
	int ey = min(m_nStageHeight, (int)pos->y+range);	// �͈�bottom
//	int ex = min(m_tPngImage.width, (int)pos->x+range);	// �͈�right
//	int ey = min(m_tPngImage.height, (int)pos->y+range);	// �͈�bottom
	double dRangeLen = (double)range*(double)range;			// ���a�Q��
	for (;y<ey;y++)
	{
		for (;x<ex;x++)
		{
			// ���蒆�S�ʒu����s�N�Z���ʒu�̋��������߂�
//			D3DXVECTOR2 p = D3DXVECTOR2((float)x, (float)y)-(*pos);
			float fPxLen = D3DXVec2LengthSq(&(D3DXVECTOR2((float)x, (float)y)-(*pos)));
			if (fPxLen <= dRangeLen)						// ����������
				SetStagePixel(x,y, 0x00000000);					// 0�Ŗ��߂�
		}
	}
}

// �n�ʂ�T��
// dest_my_pos					: �������������̈ʒu
// dest_ground_pos			: ���������ڒn�ʒu
// first_pos						: �T���n�߂̈ʒu
// range
// return							: ����������TRUE
BOOL CMainStage::FindGround(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range)
{
//	D3DXVECTOR2 hline = D3DXVECTOR2(0, 0, 0);
	BOOL ret = IsSomeHit(dest_my_pos, first_pos/*, &hline */,range);	// �����ʒu���n����
	if (ret)	ret = FindGroundUp(dest_my_pos, dest_ground_pos, first_pos, range);
	else		ret = FindGroundDown(dest_my_pos, dest_ground_pos, first_pos, range);
	return ret;
}

// �n�ʂ�T��
// dest_my_pos					: �������������̈ʒu
// dest_ground_pos			: ���������ڒn�ʒu
// first_pos						: �T���n�߂̈ʒu
// range							: ���a
// return							: ����������TRUE
BOOL CMainStage::FindGroundDown(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range)
{
	D3DXVECTOR2 hline = D3DXVECTOR2(0, (float)m_nStageHeight - first_pos->y);
	float f;
//	D3DXVECTOR2 hline = D3DXVECTOR2(0, (float)m_tPngImage.height - first_pos->y+1, 0);
	if (LowHitTest(dest_ground_pos, dest_my_pos, &f, first_pos, &hline, range))
	{
		dest_my_pos->y -= 1;	// ���������ʒu��1��-���a
		return TRUE;
	}
	return FALSE;
}

// �n�\��T��
// dest_my_pos					: �������������̈ʒu
// dest_ground_pos			: ���������ڒn�ʒu
// first_pos						: �T���n�߂̈ʒu
// range							: ���a
// return							: ����������TRUE
BOOL CMainStage::FindSomeGroundUp(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range)
{
//	if (first_pos->y<= 0)	return FALSE;	// 20101122
	
//	D3DXVECTOR2 vec = D3DXVECTOR2(0,-1.0f,0);
	// �����Ȃ��悤�ɋ���1�������蔻��
//	for (int m_nPosY = (int)first_pos->y-1; m_nPosY>=0; --m_nPosY)
	for (int m_nPosY = (int)first_pos->y; m_nPosY>=(-range-1);--m_nPosY)
	{
		D3DXVECTOR2 vecChkPos = D3DXVECTOR2(first_pos->x, (float)m_nPosY);
		if (!IsSomeHit(dest_ground_pos, &vecChkPos/*, &vec*/, range))
		{
			*dest_my_pos = vecChkPos;
			*dest_ground_pos = vecChkPos;
			dest_ground_pos->y += (float)range;
			return TRUE;
		}
	}
	return FALSE;
}

// �n�\��T��
// dest_my_pos					: �������������̈ʒu
// dest_ground_pos			: ���������ڒn�ʒu
// first_pos						: �T���n�߂̈ʒu
// range							: ���a
// return							: ����������TRUE
BOOL CMainStage::FindGroundUp(D3DXVECTOR2* dest_my_pos, D3DXVECTOR2* dest_ground_pos, D3DXVECTOR2* first_pos, int range)
{
//	if (first_pos->y<= 0)	return FALSE;	// 20101122
	
//	D3DXVECTOR2 vec = D3DXVECTOR2(0,-1.0f,0);
	// �����Ȃ��悤�ɋ���1�������蔻��
//	for (int m_nPosY = (int)first_pos->y-1; m_nPosY>=0; --m_nPosY)
	for (int m_nPosY = (int)first_pos->y-1; m_nPosY>=(-range-1);--m_nPosY)
	{
		D3DXVECTOR2 vecChkPos = D3DXVECTOR2(first_pos->x, (float)m_nPosY);
		if (!IsHit(dest_ground_pos, &vecChkPos/*, &vec*/, range))
		{
			*dest_my_pos = vecChkPos;
			*dest_ground_pos = vecChkPos;
			dest_ground_pos->y += (float)range;
			return TRUE;
		}
	}
	return FALSE;
}

DWORD CMainStage::GetStagePixel(int x,int y)
{
	// �C���[�W�͈͊O���m�F
	if (x<0
	|| y<0
	|| (int)m_tPngImage.width <= x
	|| (int)m_tPngImage.height <= y
	)
		return MAIN_STAGE_GETPIXEL_OUTOFSTAGE;
	
	DWORD* line = m_tPngImage.lines[y];
	return *(line+=x);
}

// 20101118�œK��
void CMainStage::SetStagePixel(int x,int y, DWORD px)
{
	// �C���[�W�͈͊O���m�F
//	if (x<0 || y<0 || m_tPngImage.width <= x || m_tPngImage.height <= y) return;
	
	DWORD* line = m_tPngImage.lines[y];
	line+=x;
	*line = px;
}

// 20101118�œK��
// �n�ʂ�����
// pos		: �����ʒu
// range	: �������a
int CMainStage::EraseStage(D3DXVECTOR2* pos, int range)
{
	if (!range) return 0;
	int ret = 0;
	int is	=	-range+(int)floor(pos->y+0.5);
	is = max(0, min(GetStageHeight(), is));
	int ie	=	range+(int)floor(pos->y+0.5);
	ie = max(0, min(GetStageHeight(), ie));
	int js	=	-range+(int)floor(pos->x+0.5);
	js = max(0, min(GetStageWidth(), js));
	int je	=	range+(int)floor(pos->x+0.5);
	je = max(0, min(GetStageWidth(), je));

	double dRangeLen = (double)range*(double)range;
	for (int i=is;i != ie;i++)
	{
		DWORD* p = m_tPngImage.lines[i];
		p += js;
		for (int j=js;j != je;j++)
		{
//			DWORD px = GetStagePixel(j,i);	// �F�擾
			DWORD alpha = (*p)&MAIN_STAGE_ALPHA_MASK;
			// �A���t�@�l�L���0xFF�ȊO
			if (alpha && (alpha^MAIN_STAGE_ALPHA_MASK))
			{
				// ���蒆�S�ʒu����s�N�Z���ʒu�̋��������߂�
//				D3DXVECTOR2 pxpos = D3DXVECTOR2((float)j, (float)i) - (*pos);
				if (D3DXVec2LengthSq(&(D3DXVECTOR2((float)j, (float)i) - (*pos))) <= dRangeLen)		// ����������
				{
					SetStagePixel(j,i, 0x00000000);								// �s�N�Z���폜
					ret++;
				}
			}
			p++;
		}
	}
	return ret;
}

// �X�e�[�W�։摜�\��t��
BOOL CMainStage::PasteImage(CFiler* pFiler, WCHAR *path, RECT* pImageRect, int nStageOrignX, int nStageOrignY)
{
	TPngImage tPasteImage;
	if (!PngLoader::PngLoadFromFileInMemory(&tPasteImage, path, pFiler))
		return FALSE;

	BOOL ret = PngLoader::PasteFromImage(pImageRect, nStageOrignX, nStageOrignY, &m_tPngImage, &tPasteImage);
	// ���
	SafeFree(tPasteImage.data);
	SafeFree(tPasteImage.lines);
	ZeroMemory(&tPasteImage, sizeof(TPngImage));
	return ret;
/*
	// �\��t����͈�
	RECT rcPaste;
	rcPaste.left = max(0,nStageOrignX);
	rcPaste.right = nStageOrignX+pImageRect->right;
	if ( (nStageOrignX+pImageRect->right) >= m_nStageWidth)
		rcPaste.right = (nStageOrignX+pImageRect->right) - m_nStageWidth;
	if (rcPaste.left > rcPaste.right)
		return TRUE;

	rcPaste.top = max(0, nStageOrignY);
	rcPaste.bottom = nStageOrignY+pImageRect->bottom;
	if ( (nStageOrignY+pImageRect->bottom)  >= m_nStageHeight)
		rcPaste.bottom = (nStageOrignY+pImageRect->bottom)-m_nStageHeight;
	if (rcPaste.top > rcPaste.bottom)
		return TRUE;

	TPngImage tPasteImage;
	if (!PngLoader::PngLoadFromFileInMemory(&tPasteImage, path, pFiler))
		return FALSE;

	int nWidth = rcPaste.right-rcPaste.left;
//	size_t sizePasteLine = (size_t)((rcPaste.right-rcPaste.left)*sizeof(DWORD));

	// 1�s������
	for (int y=0;y<pImageRect->bottom;y++)
	{
		DWORD* dest_line = m_tPngImage.lines[y+rcPaste.top];
		dest_line+=rcPaste.left;
		DWORD* src_line = tPasteImage.lines[y+pImageRect->top];
		for (int x=0;x<nWidth;x++)
		{
			if (*src_line & 0xFF000000)
				*dest_line = (*dest_line&0xFF000000)|(*src_line);
			dest_line++;
			src_line++;
		}
	}

	// ���
	SafeFree(tPasteImage.data);
	SafeFree(tPasteImage.lines);
	ZeroMemory(&tPasteImage, sizeof(TPngImage));
	return TRUE;
*/
}

// �X�e�[�W�։摜�\��t��
BOOL CMainStage::PasteImage(LPDIRECT3DDEVICE9 pDev, RECT* pImageRect, int nStageOrignX, int nStageOrignY, LPDIRECT3DTEXTURE9 pCopyTexture)
{
	return PngLoader::PasteFromTexture(pDev, pImageRect, nStageOrignX, nStageOrignY, &m_tPngImage, pCopyTexture);
}

// �ǂݍ���ł���PNG�C���[�W����e�N�X�`���쐬
BOOL CMainStage::CreateTexture(LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 *ppCreatedTexture)
{
	if (!m_bStageLoad)	return FALSE;

	int nStageWidth = m_tPngImage.width;
	int nStageHeight = m_tPngImage.height;
	HRESULT hr = pDev->CreateTexture(nStageWidth, nStageHeight, 1, 0/*D3DUSAGE_DYNAMIC*/, D3DDEFAULT_FORMAT, D3DPOOL_MANAGED, ppCreatedTexture, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"�e�N�X�`�����쐬�ł��܂���", L"CreateTexture error", MB_OK);
		return FALSE;
	}
	LPDIRECT3DTEXTURE9 pCreatedTexture = *ppCreatedTexture;

	// �T�[�t�F�[�X���b�N
	D3DLOCKED_RECT LockedRect;
	hr = pCreatedTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD);
	if (hr != D3D_OK)
	{
		MessageBox(NULL, L"�X�e�[�W�摜�̑���Ɏ��s���܂���", L"error", MB_OK);
		return FALSE;
	}
	DWORD* p = (DWORD*)LockedRect.pBits;
	const TPngImage* pStagePngImage = GetImage();

	size_t sizePasteLine = (size_t)(nStageWidth*sizeof(DWORD));
	/*
	for (int y=0;y<nStageHeight;y++)
	{
		// �]����
		DWORD* p1 = (DWORD*)((char*)LockedRect.pBits
							+ ((LockedRect.Pitch) * (y)));
		// �]����
		DWORD* src_line = pStagePngImage->lines[y];
		memcpy(p1, src_line, sizePasteLine);
	}
	*/
	memcpy(LockedRect.pBits, pStagePngImage->lines[0], sizePasteLine*nStageHeight);

	hr = pCreatedTexture->UnlockRect(0);
	if (hr != D3D_OK)
	{
		MessageBox(NULL, L"�X�e�[�W�摜�̑���Ɏ��s���܂���", L"error", MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CMainStage::GetGroundsXPos(std::vector<int>* vecXPos)
{
	int step = CHARA_BODY_RANGE*2;
	int e= m_nStageWidth - CHARA_BODY_RANGE;

	D3DXVECTOR2	vec = D3DXVECTOR2(0,0);

	// �L���Ȓn�ʂ��
	for (int i=CHARA_BODY_RANGE;i<e;i+= step)
	{
		vec.x = (float)i;
		D3DXVECTOR2 dest_my_pos,dest_ground_pos;
		if (FindGroundDown(&dest_my_pos, &dest_ground_pos, &vec, CHARA_BODY_RANGE))
			vecXPos->push_back(i);
	}
	return !(vecXPos->empty());
}