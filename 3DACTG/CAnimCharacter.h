#pragma once
#include "CCharacter.h"
#include "CSkinMesh.h"

class CAnimCharacter
	: public CCharacter
{

public:

	CAnimCharacter();
	virtual ~CAnimCharacter() override;
	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void InitAnimation();
	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog) override;
	
	//メッシュを接続する.
	void AttachSkinMesh(CSkinMesh& pMesh);
	void DetachSkinMesh() {
		m_pSkinMesh = nullptr;
	}
	void GetSkinMesh(CSkinMesh** ppMesh) {
		*ppMesh = m_pSkinMesh;
	}

	void SetAnimSpeed(double speed) { m_AnimSpeed = speed; }
	bool SetAnimNo(int no);


private:

	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	//アニメーションコントローラ
	CSkinMesh* m_pSkinMesh;
	int m_AnimNo;
	float m_AnimTime;
	float m_AnimSpeed;
	D3DXVECTOR3 m_vBonePos;
	
};

