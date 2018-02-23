// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Script Commands
 * Place the body of custom script commands in this file.
 * Format:
 *	BUILDIN_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//BUILDIN_FUNC(example)
//{
//	ShowInfo("buildin_example: It works!\n");
//	script_pushint(st,1);
//	return 0;
//}

// [Nerguri] 도작 소스
BUILDIN_FUNC(plagiarism)
{
	int type, lv;
	int skillid = script_getnum(st, 2);
	int idx = 0, cidx = 0;
	int skilllv = script_getnum(st, 3);
	struct map_session_data *sd = map_id2sd(st->rid);

	if (sd == NULL) {
		ShowError("buildin_plagiarism: sd is not found.\n");
		return SCRIPT_CMD_FAILURE;
	}

	if (skillid <= 0 || skilllv <= 0) {
		ShowError("buildin_plagiarism: 스킬id(%d)나 스킬lv(%d)은 1보다 작을 수 없습니다.\n", skillid, skilllv);
		script_reportsrc(st);
		script_pushint(st, -1);
		return SCRIPT_CMD_FAILURE;
	}

	// Copy Referal: dummy skills should point to their source upon copying
	switch (skillid) {
	case AB_DUPLELIGHT_MELEE:
	case AB_DUPLELIGHT_MAGIC:
		skillid = AB_DUPLELIGHT;
		break;
	case WL_CHAINLIGHTNING_ATK:
		skillid = WL_CHAINLIGHTNING;
		break;
	case LG_OVERBRAND_BRANDISH:
	case LG_OVERBRAND_PLUSATK:
		skillid = LG_OVERBRAND;
		break;
	case WM_REVERBERATION_MELEE:
	case WM_REVERBERATION_MAGIC:
		skillid = WM_REVERBERATION;
		break;
	case WM_SEVERE_RAINSTORM_MELEE:
		skillid = WM_SEVERE_RAINSTORM;
		break;
	case GN_CRAZYWEED_ATK:
		skillid = GN_CRAZYWEED;
		break;
	case GN_HELLS_PLANT_ATK:
		skillid = GN_HELLS_PLANT;
		break;
	case GN_SLINGITEM_RANGEMELEEATK:
		skillid = GN_SLINGITEM;
		break;
	}

	//Use skill index, avoiding out-of-bound array [Cydh]
	if (!(idx = skill_get_index(skillid))) {
		ShowError("buildin_plagiarism: skillid(%d) index out of bound array.\n", skillid);
		script_pushint(st, -2);
		return SCRIPT_CMD_SUCCESS;
	}

	if ((skill_db[idx]->copyable.option & 1) && (pc_checkskill(sd, RG_PLAGIARISM)))
		type = 1;
	else if ((skill_db[idx]->copyable.option & 2) && (pc_checkskill(sd, SC_REPRODUCE)))
		type = 2;
	else
		type = 0;

	switch (type) {
	case 1: //Copied by Plagiarism
	{
		if (sd->cloneskill_idx > 0 && sd->status.skill[sd->cloneskill_idx].flag == SKILL_FLAG_PLAGIARIZED) {
			clif_deleteskill(sd, sd->status.skill[sd->cloneskill_idx].id);
			sd->status.skill[sd->cloneskill_idx].id = 0;
			sd->status.skill[sd->cloneskill_idx].lv = 0;
			sd->status.skill[sd->cloneskill_idx].flag = SKILL_FLAG_PERMANENT;
		}

		sd->cloneskill_idx = idx;
		pc_setglobalreg(sd, add_str(SKILL_VAR_PLAGIARISM), skillid);
		pc_setglobalreg(sd, add_str(SKILL_VAR_PLAGIARISM_LV), skilllv);
	}
	break;
	case 2: //Copied by Reproduce
	{
		//Already did SC check
		//Skill level copied depends on Reproduce skill that used
		lv = 1;
		if (sd->reproduceskill_idx > 0 && sd->status.skill[sd->reproduceskill_idx].flag == SKILL_FLAG_PLAGIARIZED) {
			clif_deleteskill(sd, sd->status.skill[sd->reproduceskill_idx].id);
			sd->status.skill[sd->reproduceskill_idx].id = 0;
			sd->status.skill[sd->reproduceskill_idx].lv = 0;
			sd->status.skill[sd->reproduceskill_idx].flag = SKILL_FLAG_PERMANENT;
		}

		sd->reproduceskill_idx = idx;
		pc_setglobalreg(sd, add_str(SKILL_VAR_REPRODUCE), skillid);
		pc_setglobalreg(sd, add_str(SKILL_VAR_REPRODUCE_LV), skilllv);
	}
	break;
	default:
		script_pushint(st, -3);
		return SCRIPT_CMD_SUCCESS;
	}
	sd->status.skill[idx].id = skillid;
	sd->status.skill[idx].lv = skilllv;
	sd->status.skill[idx].flag = SKILL_FLAG_PLAGIARIZED;
	clif_addskill(sd, skillid);

	status_calc_pc_(sd, SCO_NONE);
	script_pushint(st, 0);
	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(get_disidentif_count)
{
	int i;
	int dis_ident_count = 0;

	TBL_PC *sd = NULL;
	script_rid2sd(sd);

	if( sd == NULL )
		return SCRIPT_CMD_SUCCESS;

	for(i=0; i<MAX_INVENTORY; i++) {
		if (sd->inventory.u.items_inventory[i].nameid > 0 && sd->inventory.u.items_inventory[i].identify != 1) {
			++dis_ident_count;
		}
	}

	script_pushint (st, dis_ident_count);

	return SCRIPT_CMD_SUCCESS;
}
BUILDIN_FUNC(identify_item)
{
	int i;
	int ident_count = false;
	int flag = 0;

	TBL_PC *sd = NULL;
	script_rid2sd(sd);

	if( script_hasdata(st,2) )
		flag = script_getnum(st,2);


	if( sd == NULL )
		return SCRIPT_CMD_SUCCESS;

	for(i=0; i<MAX_INVENTORY; i++) {
		if (sd->inventory.u.items_inventory[i].nameid > 0 && sd->inventory.u.items_inventory[i].identify != 1) {
			sd->inventory.u.items_inventory[i].identify = 1;
			clif_item_identified(sd,i,0);
			++ident_count;
			if(0==flag)					continue;
			else if(ident_count>=flag)	break;
		}
	}

	if(ident_count)	script_pushint (st, 1);
	else			script_pushint (st, 0);

	return SCRIPT_CMD_SUCCESS;
}
