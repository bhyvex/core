|***
	class.dru.inc
	Core Druid Include

***|



|***
 *	DES: Main Druid Routine
 *	USE: 
 *	INI: 
 *  NOTE: 
 ***|
sub core_DRU()
	DEBUG Loop start: ${cbug}core_${Me.Class.ShortName}()\ax
	/doevents
	ISDEAD
	/if (${Bool[${toonControl}]} && ${Bool[${toonControlXTslot}]} && ${toonControlProtect}) /call protect_always_xt
	CHECKTIE

  /if (${timer_set_xtarget} && ${switchHealXTarget}) {
    /call set_xtarget
    /call create_timer timer_set_xtarget ${tXHeal}
  }			
	
	/if (${AutoBot}) {
		/doevents
		/call check_engaged
		/if (!${Macro.Return} && ${usePet}) /call check_pet
	}	


	:killtarget
	CHECKREZ
	/if (${AutoBot} && !${Me.Invis}) {
		/call check1offBuffs
		/if (${Select[${combat},MELEE,RANGE,NUKE]}) {
			/call is_target_valid ${Target2Kill}
			/if (!${isValidTarget} || !${Spawn[${Target2Kill}].ID}) {
				/call clear_combat
				/goto :exit
			} else /if (${isValidTarget} && ${Spawn[${Target2Kill}].ID}) {
				/call is_toast_time
				/if (${Macro.Return}) /call synergy_${Me.Class.ShortName}
			}
		} else /if (${Select[${combat},HEAL,DUAL]}) {
			/call is_toast_time
			/if (${Macro.Return}) /call synergy_${Me.Class.ShortName}
		}
	} else /if (!${AutoBot}) {
		/if (${Target.ID} != ${Target2Kill}) /varset Target2Kill ${Target.ID}
		/if (!${Target.ID} || !${Spawn[${Target2Kill}].ID}) /goto :exit
	}
	
	CLEARCURSOR

	/if (${Select[${combat},MELEE]} && ${isValidTarget}) {
	} else /if (${Select[${combat},RANGE]} && ${isValidTarget}) {
	} else /if (${Select[${combat},NUKE]} && ${isValidTarget}) {
		CHECKREZ
		/call set_combat NUKE
		/call send_pet
		/call cast_AA_ro ${Target2Kill}
		/call disc_${Me.Class.ShortName}
		/if (${DoDoT}) /call cast_DOT
		/if (${DoNuke}) /call cast_nuke
		/call disc_${Me.Class.ShortName}			
		/call click_DPS_${Me.Class.ShortName}
		/if (${doclickItem}) /call click_item
	} else /if (${combat.Equal[HEAL]}) {
		CHECKREZ
		/call set_combat HEAL
		/if (${switchHealGroup}) /call check_heal_group_${Me.Class.ShortName}
		/if (${switchHealXTarget}) /call check_heal_XTarget_${Me.Class.ShortName}
		/if (${switchHealPet}) /call check_pet_heal_${Me.Class.ShortName}
	} else /if (${Select[${combat},DUAL]}) {
		CHECKREZ
		/call set_combat HEAL
		/call send_pet
		/if (${switchHealGroup}) /call check_heal_group_${Me.Class.ShortName}
		/call is_target_valid ${Target2Kill}
		/if (${isValidTarget}) {
			CHECKREZ
			/call set_combat NUKE
			/call cast_AA_ro ${Target2Kill}
			/call disc_${Me.Class.ShortName}
			/if (${DoDoT}) /call cast_DOT
			/if (${DoNuke}) /call cast_nuke
			/call disc_${Me.Class.ShortName}			
			/call click_DPS_${Me.Class.ShortName}
			/if (${doclickItem}) /call click_item
		}
	}
	/call use_mod_rod
	/if (${AutoBot}) {	
		/if (${switchHealCure}) /call cast_cure
		/if (${switchHealSelf}) /call patch_heal_${Me.Class.ShortName}
	}
	/if (${AutoBot} && !${Spawn[${Target2Kill}].ID} || !${AutoBot} && ${Target.ID} && ${Bool[${combat}]}) {
		/call is_target_dead
		/if (!${Macro.Return}) /goto :killtarget
	}
	:exit
/return



|***
 *	DES: Combat skills, Disciplines and Alternate abilities.
 *	USE: Disc
 *	INI: 
 *  NOTE: 
 ***|
sub disc_DRU()
	DEBUG	${cbug}disc.${Me.Class.ShortName}()\ax
	ISDEAD
	/doevents
	/call check_exit TRUE
	/if (${Macro.Return}) /goto :exit
	/call is_target_dead
	/if (${Macro.Return} || !${Target.ID}) /goto :exit
	/if (${switchBanestrike}) /call cast_aa_banestrike ${Target2Kill}
	/if (${switchdeBuffSnare}) /call cast_aa_snare ${Target2Kill}
	/if (${switchdeBuffEradicate}) /call cast_eradicate_magic ${Target2Kill}
	/if (${Me.AltAbilityReady[Storm Strike]}) /call core_cast "Storm Strike" alt ${Target2Kill}
	/if (${Me.AltAbilityReady[Season's Wrath]}) /call core_cast "Season's Wrath" alt ${Target2Kill}
	:exit
/return



|***
 *	DES: emergency healing
 *	USE: auto
 *	INI: 
 *  NOTE: 
 ***|
sub patch_heal_DRU()
	DEBUG ${cbug}patch_heal_${Me.Class.ShortName}()\ax
	/call check_exit
	/if (${Macro.Return}) /goto :exit
	/if (${Me.PctHPs} <= 50*.${tmpHealOverride} && ${Me.AltAbilityReady[Convergence of Spirits]}) /call core_cast "Convergence of Spirits" alt ${Me.ID}
	/if (${Me.PctHPs} <= 80*.${tmpHealOverride} && ${Me.ItemReady[Prayers of Life]}) /call core_cast "Prayers of Life" item 0
	/if (${Me.PctHPs} <= 60*.${tmpHealOverride} && ${Me.ItemReady[Overflowing Urn of Life]}) /call core_cast "Overflowing Urn of Life" item 0
	/if (${SpawnCount[npc ${If[${setIgnoreLOS},,los]} radius ${MobAgro} playerstate 4 targetable]} >= 4 && ${Me.ItemReady[${CircleofLife}]} && !${Me.Song[${FindItem[${CircleofLife}].Spell}].ID} && ${Spell[${FindItem[${CircleofLife}].Spell}].Stacks}) /call core_cast "${CircleofLife}" item 0
	:exit
/return



|***
 *	DES: 
 *	USE: 
 *	INI: 
 *  NOTE: 
 ***|
sub click_DPS_DRU()
	DEBUG ${cbug}click_DPS_${Me.Class.ShortName}()\ax 
	ISDEAD
	/doevents
	/call check_exit
	/if (${Macro.Return}) /goto :exit
	/call is_target_dead
	/if (${Macro.Return} || !${Target.ID}) /goto :exit
	/if (${Bool[${Epic}]} && ${Me.ItemReady[${Epic}]} && ${Spell[${FindItem[${Epic}].Spell}].Stacks} && !${Me.Buff[${FindItem[${Epic}].Spell}].ID} && !${Me.Song[${FindItem[${Epic}].Spell}].ID})  /call core_cast "${Epic}" item ${Target2Kill}
	/call click_horn_of_unity 	
	/if (${Me.PctMana} < 60*.${tmpHealOverride} && ${Me.ItemReady[${CircleofMana}]} && !${Me.Song[${FindItem[${CircleofMana}].Spell}].ID} && ${Spell[${FindItem[${CircleofMana}].Spell}].Stacks}) /call core_cast "${CircleofMana}" item 0
	:exit
/return



|***
 *	DES: casts druid Blessing of Ro AA deBuff
 *	USE: automatic
 *	INI: 
 *  NOTE: 
 ***|
sub cast_AA_ro(int _tmpID)
	DEBUG ${cbug}cast_AA_ro()\ax
	ISDEAD
	/doevents
	/call check_exit
	/if (${Macro.Return} || ${timer_ro_${_tmpID}} || ${Target.Buff[Blessing of Ro].ID} || ${Target.Buff[Fixation of Ro].ID}) /goto :exit
	/call is_target_dead
	/if (${Macro.Return}) /goto :exit
	
	| /if ((${Target.BuffDuration[]} <= 2 && ${Spawn[${Target.Buff[].Caster}].ID} == ${Me.ID}) || !${Target.Buff[].ID}) /goto :exit{
	/if (${Bool[${debuffRo}]} && ${Me.AltAbilityReady[${debuffRo}]}) {
		/call core_cast "${debuffRo}" alt ${_tmpID}
		/if (${Select[${castReturn},CAST_SUCCESS,CAST_IMMUNE,CAST_RESISTED,CAST_RESIST,CAST_NOHOLD,CAST_TAKEHOLD]}) {
			/call create_timer timer_ro_${_tmpID} ${Spell[${debuffRo}].Duration.TotalSeconds}s
		}		
	}
	:exit
/return



|***
 *	DES: Check Group and heal the fucks if the need it.
 *	USE: /healgroup
 *	INI: 
 *  NOTE: 
 ***|
sub check_heal_group_DRU()
	DEBUG ${cbug}check_heal_group_DRU()\ax
	/if (${Me.State.Equal[FEIGN]}) /goto :exit
	/declare _count						int	local

	:loopheal
	ISDEAD
	/doevents
	CHECKTIE
	/varset WorstHurt 				${Me.ID}
	/varset WorstHP 					${If[${HealOverride},${Float[${HealOverride}].Centi},99.99]}
	/varset WorstClass 				DRU
	/varset MinorHurt 				0
	/varset MajorHurt					0
	
	| Does the control toon need healing?
	/if (!${Bool[${Group.Member[${toonControl}]}]} && ${Bool[${toonControl}]} && ${toonControlProtect} && !${Spawn[${toonControl}].Dead} && ${Bool[${Spawn[${toonControl}]}]} && ${Spawn[${toonControl}].ID} && ${Spawn[${toonControl}].PctHPs} <= ${WorstHp} && (${Spawn[${toonControl}].Distance} <= ${Spell[${healRemedy1}].MyRange} && ${Spawn[${toonControl}].PctHPs} <= ${${Spawn[${toonControl}].Class.ShortName}Point}*.${tmpHealOverride} || ${Spawn[${toonControl}].Distance} <= ${Spell[${healRenewal1}].MyRange} && ${Spawn[${toonControl}].PctHPs} <= ${${Spawn[${toonControl}].Class.ShortName}Point}*.${tmpHealOverride}*.7)) {
		/varset WorstHurt ${Spawn[${toonControl}].ID}
		/varset WorstHp ${Spawn[${toonControl}].PctHPs}
		/varset WorstClass ${Spawn[${toonControl}].Class.ShortName}
	} 

	/for _count 0 to ${Group.Members}
		/if (!${Group.Member[${_count}].Dead} && (${Spawn[pc id ${Group.Member[${_count}].ID}].ID} || ${Spawn[mercenary id ${Group.Member[${_count}].ID}].ID}) && ${Group.Member[${_count}].PctHPs} <= ${WorstHp} && (${Group.Member[${_count}].Distance} <= ${Spell[${healRemedy1}].MyRange} && ${Group.Member[${_count}].PctHPs} <= ${${Group.Member[${_count}].Class.ShortName}Point}*.${tmpHealOverride} || ${Group.Member[${_count}].Distance} <= ${Spell[${healRenewal1}].MyRange} && ${Group.Member[${_count}].PctHPs} <= ${${Group.Member[${_count}].Class.ShortName}Point}*.${tmpHealOverride})) {
			/varset WorstHurt ${Group.Member[${_count}].ID}
			/varset WorstHp ${Group.Member[${_count}].PctHPs}
			/varset WorstClass ${Group.Member[${_count}].Class.ShortName}
		} 
		
    /if (!${Spawn[pc id ${Group.Member[${_count}].ID}].Dead} && (${Group.Member[${_count}].PctHPs} < 85*.${tmpHealOverride} || ${Group.Member[${_count}].PctHPs} < ${${Group.Member[${_count}].Class.ShortName}Point}*.${tmpHealOverride}) && ${Group.Member[${_count}].PctHPs} > ${${Group.Member[${_count}].Class.ShortName}Point}*.${tmpHealOverride}*.7 && ${Group.Member[${_count}].Distance} <= ${Spell[${healGroup}].AERange}) {
      /varcalc MinorHurt ${MinorHurt}+1
			UPDATEHUD "HEALING"
    } else /if (!${Spawn[pc id ${Group.Member[${_count}].ID}].Dead} && ${Group.Member[${_count}].PctHPs} < ${${Group.Member[${_count}].Class.ShortName}Point}*.${tmpHealOverride}*.7 && ${Group.Member[${_count}].Type.NotEqual[Corpse]} && ${Group.Member[${_count}].Distance} <= ${Spell[${healGroup}].AERange}) {
      /varcalc MajorHurt ${MajorHurt}+1
      UPDATEHUD "HEAL"
    }
		ISDEAD
		CHECKEXIT
	 	/doevents
  /next _count

  /if (${Bool[${Me.Buff["Divine Aura"]}]} || ${Bool[${Me.Buff["Divine Barrier"]}]} || ${Bool[${Me.Buff["Sacred Barrier"]}]} ) /goto :exit
  UPDATEHUD "HEAL"
  CHECKREZ

 	/call check_no_cast_buffs

	| Panic Heals 
	/if (${Spawn[${WorstHurt}].PctHPs} <= 45*.${tmpHealOverride} && ${Spawn[${WorstHurt}].Distance} <= ${Spell[${healPanic}].MyRange} && ${Me.SpellReady[${healPanic}]}) {
		/call core_cast "${healPanic}" 0 ${WorstHurt}
		/if (${castReturn.NotEqual[CAST_SUCCESS]} && ${Macro.Return.NotEqual[CAST_OUTOFMANA]}) /goto :loopheal
	}
	/if (!${Me.SpellReady[${PatchSpell}]} && !${Me.SpellReady[${HealSpell}]} && ${Spawn[${WorstHurt}].PctHPs} < ${${WorstClass}Point}*.6*.${tmpHealOverride} && ${Me.AltAbilityReady[Peacful Convergence of Spirits]} && ${Spawn[${WorstHurt}].Distance} <= ${Spell[${healPanic}].MyRange}) {
		/call core_cast "Convergence of Spirits" alt ${WorstHurt}
	}
	/if (!${Me.SpellReady[${PatchSpell}]} && !${Me.SpellReady[${HealSpell}]} && ${Spawn[${WorstHurt}].PctHPs} < ${${WorstClass}Point}*.5*.${tmpHealOverride} && ${Me.AltAbilityReady[Convergence of Spirits]} && ${Spawn[${WorstHurt}].Distance} <= ${Spell[${healPanic}].MyRange}) {
		/call core_cast "Peacful Convergence of Spirits" alt ${WorstHurt}
	}
	/if (!${Me.SpellReady[${PatchSpell}]} && !${Me.SpellReady[${HealSpell}]} && ${Spawn[${WorstHurt}].PctHPs} < 35*.${tmpHealOverride} && ${Me.AltAbilityReady[Swarm of Fireflies]} && ${Spawn[${WorstHurt}].Distance} <= ${Spell[Swarm of Fireflies].MyRange}) {
		/call core_cast "Swarm of Fireflies" alt ${WorstHurt}
	}
  
	| Group Heal
	/doevents
	/if (${useGroupHeal}) {
	  /if (${MajorHurt}+${MinorHurt} > 1 && ${Me.SpellReady[${healWholeGroup}]}) {
	    /call core_cast "${healWholeGroup}" 0 ${Me.ID} 
	    /if (${Select[${castReturn},CAST_SUCCESS]} && !${Select[${castReturn},CAST_OUTOFMANA]})  /goto :loopheal
	  } 	
	 	/if (${MajorHurt}+${MinorHurt} > 2 && ${Me.AltAbilityReady[Peacful Spirit of the Wood]}) {
			/call core_cast "Peacful Spirit of the Wood" alt ${Me.ID} 
		}
	 	/if (${MajorHurt}+${MinorHurt} > 3 && ${Me.AltAbilityReady[Blessing of Tunare]}) {
			/call core_cast "Blessing of Tunare" alt ${Me.ID} 
		}

	}
	
	ISDEAD
	CHECKEXIT
	/doevents

	| Rejuvilation Heal
	/if (${Spawn[${WorstHurt}].PctHPs} <= ${${WorstClass}Point}*.${tmpHealOverride} && ${Me.SpellReady[${healRejuvilation}]}) {
		/if (${Me.SpellReady[${healRejuvilation}]}) {
			/call core_cast "${healRejuvilation}" 0 ${WorstHurt}
		} 
		/goto :loopheal
	}
	ISDEAD
	CHECKEXIT
	/doevents
 	
	| Adrenaline / Sanavida Heal 
	/if (${Spawn[${WorstHurt}].PctHPs} <= ${${WorstClass}Point}*.${tmpHealOverride} && (${Me.SpellReady[${healAdrenaline}]} || ${Me.SpellReady[${healSanavida}]})) {
		/if (${Me.SpellReady[${healAdrenaline}]}) {
			/call core_cast "${healAdrenaline}" 0 ${WorstHurt} 
		} else /if (${Me.SpellReady[${healSanavida}]}) {
			/call core_cast "${healSanavida}" 0 ${WorstHurt}
		} 
		/goto :loopheal
	}  
  
 	:exit
 	UPDATEHUD
/return



|***
 *	DES: Check XTargets and heal the bastards if the need it.
 *	USE: /healxtarget
 *	INI: 
 *  NOTE: 
 ***|
sub check_healXTarget_DRU()
	DEBUG ${cbug}check_HealXTarget_${Me.Class.ShortName}()\ax
	ISDEAD
	/if (${Me.State.Equal[FEIGN]}) /goto :exit
  /declare _count int local
  /for _count 1 to 13
  	/doevents
  	UPDATEHUD "HEAL XTarget"

    /if (${Spawn[${Me.XTarget[${_count}].ID}].ID} && (${Spawn[${Me.XTarget[${_count}].ID}].Type.Equal[PC]} || ${ProtectNPCList.Find[${Spawn[${Me.XTarget[${_count}].ID}].CleanName}]}) && ${Spawn[${Me.XTarget[${_count}].ID}].PctHPs} <= ${${Spawn[id ${Me.XTarget[${_count}].ID}].Class.ShortName}Point}*.${tmpHealOverride}) {
      /if (${Spawn[${Me.XTarget[${_count}].ID}].PctHPs} <= 45 && ${Spawn[${Me.XTarget[${_count}].ID}].Distance} <= ${Spell[${healPanic}].MyRange} && ${Me.SpellReady[${healPanic}]}) {
        /call core_cast "${healPanic}" 0 ${Me.XTarget[${_count}].ID}
      } else /if (${Spawn[${Me.XTarget[${_count}].ID}].Distance} <= ${Spell[${healRenewal1}].MyRange} && ${Spawn[${Me.XTarget[${_count}].ID}].PctHPs} <= ${${Spawn[id ${Me.XTarget[${_count}].ID}].Class.ShortName}Point}*.${tmpHealOverride} && (${Me.SpellReady[${healRenewal1}]} || ${Me.SpellReady[${healRenewal2}]} && || ${Me.SpellReady[${healRenewal3}]})) {
        /if (${Me.SpellReady[${healRenewal1}]}) {
          /call core_cast "${healRenewal1}" 0 ${Me.XTarget[${_count}].ID}
        } else /if (${Me.SpellReady[${healRenewal2}]}) {
          /call core_cast "${healRenewal2}" 0 ${Me.XTarget[${_count}].ID}
        } 
      } else /if (${Spawn[${Me.XTarget[${_count}].ID}].Distance} <= ${Spell[${healIntervention1}].MyRange} && ${Spawn[${Me.XTarget[${_count}].ID}].PctHPs} <= ${${Spawn[id ${Me.XTarget[${_count}].ID}].Class.ShortName}Point}*.${tmpHealOverride} && (${Me.SpellReady[${healIntervention1}]} || ${Me.SpellReady[${healIntervention2}]} || ${Me.SpellReady[${healIntervention3}]})) {
        /if (${Me.SpellReady[${InterventionSpell1}]}) {
          /call core_cast "${healIntervention1}" 0 ${Me.XTarget[${_count}].ID}
        } else /if (${Me.SpellReady[${healIntervention2}]}) {
          /call core_cast "${healIntervention2}" 0 ${Me.XTarget[${_count}].ID}
        } 
      } 
    }
  /next _count
	:exit
	UPDATEHUD
/return



|***
 *	DES: Checks and Heals Pets. Group ONLY
 *	USE: /healpet [toggle]
 *	INI: 
 *  NOTE: 
 ***|
sub check_pet_heal_DRU()
	DEBUG ${cbug}checkPetHeal.${Me.Class.ShortName}()\ax
	ISDEAD
	/if (${Me.State.Equal[FEIGN]} || ${Me.Moving} || ${Target.ID} == ${Me.Pet.ID}) /goto :exit
	/declare _count int local
	/varset pWorstHurt ${Me.CleanName}
	/varset pWorstHP 100
	/varset pWorstHurtID ${Me.ID}
	/varset pWorstClass NULL
	/varset pMajorHurt 0

	/for _count 0 to ${Group.Members}
		/doevents
		/if (${Group.Member[${_count}].Pet.ID}) {
			/if (${Group.Member[${_count}].Pet.Distance} <= ${Spell[${healRenewal1}].MyRange}) {
				/if ((${Group.Member[${_count}].Pet.PctHPs} <= ${pWorstHP}) && (${Group.Member[${_count}].Pet.PctHPs} <= ${${Group.Member[${_count}].Class.ShortName}Point}*.${tmpHealOverride})) {
					/varset pWorstClass ${Group.Member[${_count}].Pet.Class.ShortName}
					/varset pWorstHurt ${Group.Member[${_count}].Pet.Name}
					/varset pWorstHurtID ${Group.Member[${_count}].Pet.ID}
					/varset pWorstHP ${Group.Member[${_count}].Pet.PctHPs}
				}
			}
		}
	/next _count

	/if (${pWorstHurtID} == ${Me.ID}) /goto :exit
	/if (${Spawn[${pWorstHurtID}].PctHPs} < ${${pWorstClass}Point}*.${tmpHealOverride} && ${Spawn[${pWorstHurtID}].Distance} <= ${Spell[${healRenewal1}].MyRange} && ${Me.SpellReady[${healRenewal1}]}) {
		/call core_cast "${healRenewal1}" 0 ${pWorstHurtID} 
	} else /if (${Spawn[${pWorstHurtID}].PctHPs} < ${${pWorstClass}Point}*.${tmpHealOverride} && ${Spawn[${pWorstHurtID}].Distance} <= ${Spell[${healRenewal2}].MyRange} && ${Me.SpellReady[${healRenewal2}]}) {
		/call core_cast "${healRenewal2}" 0 ${pWorstHurtID} 
	}

	:exit
/return



|***
 *	DES: MGB druid SoWood if both AAs are ready
 *	USE: /bc wood, /wood
 *	INI: 
 *  NOTE: Will not cast wood if the buff is present.
 ***|
sub cast_AA_wood()
	DEBUG ${cbug}cast_AA_wood()\ax ${If[${Me.AltAbilityReady[Mass Group Buff]},${cgood}MGB\ax,${cbad}MGB\ax]} ${If[${Me.AltAbilityReady[Spirit of the Wood]},${cgood}Wood\ax,${cbad}Wood\ax]}
	ISDEAD
	/doevents
	/if (${AutoBot}) /goto :exit
	/if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[Spirit of the Wood]} && !${Bool[${Me.Song[Spirit of the Wood]}]}) {
		/call core_cast "Mass Group Buff" alt 0
		/call core_cast "Spirit of the Wood" alt 0
	} else /if (${Bool[${Me.Song[Spirit of the Wood]}]}) {
		NOUT ${owarning} ${sep} trying to cast Spirit of the Wood while its running.	
	}		
	:exit
/return