#ifndef __WRPGCREATURE_H
#define __WRPGCREATURE_H

#include "../WObj_AI/AI_Auxiliary.h"	// TSimpleDynamicList<int16> m_lHitList;

/*��������������������������������������������������������������������������������������������*\
	File:			WRPGCreature
					
	Author:			Magnus Runesson
					
	Copyright:		
					
	Contents:		
					
	Comments:		
					
	History:		
		030225:		Created file, fick ingen l�n, det �r kallt, Bush ska enligt
					l�psedeln d�da Saddam och alla hans barn inom 48 timmar. F�r
					se hur det blir med den saken.
		030425:		Det �r varmt, har f�tt l�n och Saddam och hans s�ner lever
					fortfarande. Bush �r en d�lig m�nniska. Saddam ocks�.
\*____________________________________________________________________________________________*/

#include "WRPGiTEM.h"


/*��������������������������������������������������������������������*\
	Class:			CRPG_Object_CreatureAttack
					
	Comments:		
\*____________________________________________________________________*/

class CRPG_Object_CreatureAttack : public CRPG_Object_Item
{
private:
	static const char* ms_pszDirectionFlags[];

	MRTC_DECLARE;

private:
	int16 m_DelayTicks;
	int16 m_ActiveTick;
	int16 m_Damage;
	uint32 m_DamageType;
	int16 m_StartTick;
	int16 m_StopTick;
	enum {
		DIRECTION_FWD			= 0x1,	//Attack selection is made in activate direction
		DIRECTION_BWD			= 0x2,	//Attack selection is made in reversed activate direction
		DIRECTION_MOVE			= 0x4,
		DIRECTION_ATTACKSELECTIONMASK = DIRECTION_FWD | DIRECTION_BWD | DIRECTION_MOVE,

		DIRECTION_USEBASEPOS	= 0x8,	//Use character base position instead of activate position
		DIRECTION_NOWAIT		= 0x10, //Don't apply wait when using weapon. This REALLY shouldn't be a direction flag :)
		DIRECTION_CHECKLOS		= 0x20, //Need LOS to give damage. This REALLY shouldn't be a direction flag :)
	};
	uint8 m_DirectionFlags;
	fp32 m_ForwardAttackDistance;
	fp32 m_BackwardAttackDistance;
	uint8 m_HalfAttackWidth;
	uint8 m_HalfAttackDepth;
	uint8 m_MinAttackHeight;
	uint8 m_AttackForce;
	TSimpleDynamicList<int16> m_lHitList;

	void ParseDirectionFlag(const CRegistry* _pKey);
	void ParseActiveTime(const CRegistry* _pKey);

protected:
	virtual int FindAttackTargets(const CMat4Dfp32& _Mat, int _iObject, const int16** _ppRetList);
	virtual bool CanAttack();
	virtual void Attack(const CMat4Dfp32 &_Mat, CRPG_Object *_pRoot, int _iObject, int _Input);

public:
	virtual bool OnEvalKey(uint32 _KeyHash, const CRegistry* _pKey);
	virtual void OnCreate();
	bool OnProcess(CRPG_Object* _pRoot, int _iObject);
	virtual bool IsEquippable(CRPG_Object* _pRoot);
	virtual bool Activate(const CMat4Dfp32 &_Mat, CRPG_Object *_pRoot, int _iObject, int _Input);
	virtual bool Deactivate(const CMat4Dfp32 &_Mat, CRPG_Object *_pRoot, int _iObject, int _Input);
	virtual bool IsMeleeWeapon();
};

#endif
