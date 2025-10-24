#include "CSoundManager.h"
#include "MyMacro.h"

CSoundManager::CSoundManager()
	: m_pSound	()
{
	//インスタンス生成.
	for( int i = 0; i < enList::max; i++ )
	{
		m_pSound[i] = new CSound();
	}
}

CSoundManager::~CSoundManager()
{
	Release();

	//インスタンス破棄.
	for( int i = enList::max - 1; i >= 0; i-- )
	{
		SAFE_DELETE( m_pSound[i] );
	}
}

//サウンドデータ読込関数.
bool CSoundManager::Load( HWND hWnd )
{
	struct SoundList
	{
		int listNo;				//enList列挙型を設定.
		const TCHAR path[256];	//ファイルの名前(パス付き).
		const TCHAR alias[32];	//エイリアス名.
	};
	SoundList SList[] =
	{
		{ enList::SE_Jump,		_T("Data\\Sound\\SE\\Jump.wav"),			_T("SE_Jump")	},
		{ enList::BGM_Bonus,	_T("Data\\Sound\\BGM\\BonusGameHouse.mp3"),	_T("BGM_Bonus")	},
		{ enList::BGM_Battle,	_T("Data\\Sound\\BGM\\battle.mp3"),			_T("BGM_Battle")},
		{ enList::BGM_Menu,		_T("Data\\Sound\\BGM\\menu.mp3"),			_T("BGM_Menu")	},
		{ enList::SE_Clear,		_T("Data\\Sound\\SE\\Clear.wav"),			_T("SE_Clear")	},
		{ enList::SE_PlayerHit,	_T("Data\\Sound\\SE\\Malfunction.wav"),	_T("SE_PlayerHit")	},
		{ enList::SE_EnemyHit,	_T("Data\\Sound\\SE\\DAMAGED.wav"),			_T("SE_Damage")	},
		{ enList::SE_PlayerShot,_T("Data\\Sound\\SE\\Shot.wav"),	_T("SE_Shot")	},
		{ enList::SE_BossShot,	_T("Data\\Sound\\SE\\BossShot.wav"),		_T("SE_BossShot")	},
		{ enList::SE_BossRotShot,_T("Data\\Sound\\SE\\BossRotShot.wav"),	_T("SE_BossRotShot")	},
		{ enList::SE_Boss,		_T("Data\\Sound\\SE\\Boss.wav"),			_T("SE_Boss")	},
	};
	//配列の最大要素数を算出 (配列全体のサイズ/配列1つ分のサイズ).
	int list_max = sizeof( SList ) / sizeof( SList[0] );
	for( int i = 0; i < list_max; i++ )
	{
		if( m_pSound[SList[i].listNo]->Open(
			SList[i].path,
			SList[i].alias,
			hWnd ) == false )
		{
			return false;
		}
		lstrcpy(m_SoundInfo[SList[i].listNo].path, SList[i].path);
		lstrcpy(m_SoundInfo[SList[i].listNo].alias, SList[i].alias);
	}

	return true;
}

//サウンドデータ解放関数.
void CSoundManager::Release()
{
	//開いた時と逆順で閉じる.
	for( int i = enList::max - 1; i >= 0; i-- )
	{
		if( m_pSound[i] != nullptr )
		{
			m_pSound[i]->Close();
		}
	}
	for (auto& kv : m_voicePools)
	{
		for (auto* p : kv.second.voices)
		{
			if (p) { p->Close(); }
			SAFE_DELETE(p);
		}
	}
	m_voicePools.clear();
}

bool CSoundManager::CreateVoicePool(enList list, int count, HWND hWnd)
{
	if (count <= 0) return false;

	// 既存プールがあれば破棄
	auto it = m_voicePools.find(list);
	if (it != m_voicePools.end())
	{
		for (auto* p : it->second.voices)
		{
			if (p) { p->Close(); }
			SAFE_DELETE(p);
		}
		m_voicePools.erase(it);
	}

	const SoundInfo& info = m_SoundInfo[list];
	if (info.path[0] == _T('\0') || info.alias[0] == _T('\0'))
	{
		// Load前
		return false;
	}

	VoicePool pool;
	pool.voices.reserve(static_cast<size_t>(count));
	for (int i = 0; i < count; ++i)
	{
		CSound* s = new CSound();
		TCHAR alias[64] = _T("");
		wsprintf(alias, _T("%s_%d"), info.alias, i); // 例: SE_Shot_0

		if (!s->Open(info.path, alias, hWnd))
		{
			s->Close();
			SAFE_DELETE(s);
			for (auto* p : pool.voices)
			{
				if (p) { p->Close(); }
				SAFE_DELETE(p);
			}
			return false;
		}
		pool.voices.push_back(s);
	}
	pool.index = 0;
	m_voicePools[list] = std::move(pool);
	return true;
}

void CSoundManager::PlayFromPool(enList list)
{
	auto it = m_voicePools.find(list);
	if (it == m_voicePools.end() || it->second.voices.empty())
	{
		m_pSound[list]->PlaySE();
		return;
	}

	VoicePool& pool = it->second;
	CSound* voice = pool.voices[pool.index];
	if (voice)
	{
		voice->PlaySE();
	}
	pool.index = (pool.index + 1) % pool.voices.size();
}