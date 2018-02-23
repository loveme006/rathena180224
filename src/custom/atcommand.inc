// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Atcommands
 * Place the body of custom atcommands in this file.
 * Format:
 *	ACMD_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//ACMD_FUNC(newcommand)
//{
//	clif_displaymessage(fd, "It works!");
//	clif_specialeffect(&sd->bl, EF_HEARTCASTING, AREA);
//	return 0;
//}


/*==========================================
 * 이벤트 명령어 [JaVb]
 *------------------------------------------*/
int atcommand_eevent_timer(int tid, unsigned int tick, int id, intptr_t data)
{
	char output[200];

	switch(data) 
	{
		case 1:
			if(event_eflag==1&&event_dflag==0) {
				event_tick-=1;
				if(event_tick<=0) {
					event_tick=0;
					event_tid=0;
					event_rate=0;
					event_eflag=0;
					// 중단
					sprintf(output,"[ 경험치 이벤트가 종료되었습니다. ]");
				}
				else {
					sprintf(output,"[ 경험치 %.1f배 이벤트가 %d분 남았습니다. ]", ((event_rate / 10.f) + 1.0), event_tick);
					add_timer(gettick()+60000,atcommand_eevent_timer,event_tid,1);
				}
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
				return 1;
			}
			return 1;

		case 2:
			if(event_dflag==1&&event_eflag==0) {
				event_tick-=1;
				if(event_tick<=0) {
					event_tick=0;
					event_tid=0;
					event_rate=0;
					event_dflag=0;
					// 중단
					sprintf(output,"[ 드롭률 이벤트가 종료되었습니다. ]");
				}
				else {
					sprintf(output,"[ 드롭률 %.1f배 이벤트가 %d분 남았습니다. ]", ((event_rate / 10.f) + 1.0), event_tick);
					add_timer(gettick()+60000,atcommand_eevent_timer,event_tid,2);
				}
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
				return 1;
			}
			return 1;
		case 3:
			if(event_eflag==1&&event_dflag==1) {
				event_tick-=1;
				if(event_tick<=0) {
					event_tick=0;
					event_tid=0;
					event_rate=0;
					event_eflag=0;
					event_dflag=0;
					// 중단
					sprintf(output,"[ 경험치획득량과 아이템드롭률 이벤트가 종료되었습니다. ]");
					}
				else {
					sprintf(output,"[ 경험치획득량과 아이템드롭률 %.1f배 이벤트가 %d분 남았습니다. ]", ((event_rate / 10.f) + 1.0), event_tick);
					add_timer(gettick()+60000,atcommand_eevent_timer,event_tid,3);
				}
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
			}
			return 1;
	}
	return 0;
}

/*==========================================
 * @이벤트 command (usage: @이벤트 %s <time(min)> <rate(10:x1)> <type(1(exp):2(item):3(both))> <state(0(only_view):1(start):2(stop):3(add_timer)>)
 *------------------------------------------*/
ACMD_FUNC(eevent)
{
	char output[200];
	
	int spl,flag=0,rate,tick,itimer;
	double i=0;

	if(!message||!*message) {
		clif_displaymessage(fd,"[이벤트]: 사용방법을 확인해 주십시오.");
		sprintf(output,"<사용법: %s 시간(분) 배율(10:1배) 분류(1(경험치):2(아이템):3(둘다)) 동작(0(상태보기):1(시작):2(정지):3(시간추가))>",command);
		clif_displaymessage(fd,output);
		return 3;
	}

	// Message 분할
	if((sscanf(message,"%d %d %d %d",&tick,&rate,&spl,&flag))<3) {
		clif_displaymessage(fd,"[이벤트]: 사용방법을 확인해 주십시오.");
		sprintf(output,"<사용법: %s 시간(분) 배율(10:1배) 분류(1(경험치):2(아이템):3(둘다)) 동작(0(상태보기):1(시작):2(정지):3(시간추가))>",command);
		clif_displaymessage(fd,output);
		return 3;
	}
	
	// 각 입력된 데이터 확인
	if(flag<0||flag>3) {
		clif_displaymessage(fd,"[이벤트]: 동작를 확인해 주십시오. ");
		sprintf(output,"<사용법: %s 시간(분) 배율(10:1배) 분류(1(경험치):2(아이템):3(둘다)) 동작(0(상태보기):1(시작):2(정지):3(시간추가))>",command);
		clif_displaymessage(fd,output);
		return 3;
	}
	
	if(spl!=1&&spl!=2&&spl!=3) {
		clif_displaymessage(fd,"[이벤트]: 분류 플래그를 확인해 주십시오. ");
		sprintf(output,"<사용법: %s 시간(분) 배율(10:1배) 분류(1(경험치):2(아이템):3(둘다)) 동작(0(상태보기):1(시작):2(정지):3(시간추가))>",command);
		clif_displaymessage(fd,output);
		return 3;
	}

	switch(spl) {
		case 1: // 경험치

			switch(flag)
			{

			case 0:
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_dflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
					return 1;
				}
				// 상태 표시
				if(event_eflag==0) {
					sprintf(output,"[이벤트]: 현재 경험치 이벤트가 진행중이 아닙니다.");
				}
				else if(event_eflag==1){
						sprintf(output,"[이벤트]: 현재 경험치 이벤트가 진행중입니다.(남은 시간 %d분)",event_tick);
				}
				else {
					sprintf(output,"[이벤트]: 현재 플래그로 작동중인 이벤트가 아닙니다.");
				}
				clif_displaymessage(fd,output);
				return 2;

			case 1:
				// 이미 flag 작동중인게 있나 확인
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_dflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
					return 1;
				}
				
				
				// 이미 시작인지 채크
				if(event_eflag==1) {
					sprintf(output,"[이벤트]: 이미 경험치 이벤트가 시행중입니다. 남은시간: %d분",event_tick);
					clif_displaymessage(fd,output);
					return 1;
				}

				// tick 채크 
				if(tick<=0) {
					clif_displaymessage(fd,"[이벤트]: 시간 데이터가 잘못되었습니다.");;
					return 1;
				}

				// rate 채크
				if(rate<=0) {
					clif_displaymessage(fd,"[이벤트]: 배율 데이터가 잘못되었습니다.");
					return 1;
				}

				
				// 확률 설정
				event_rate=rate;

				// 플래그 설정
				event_eflag=1;
				
				// 시간 설정
				event_tick=tick;

				// 설정 재확인
				if(event_eflag==0) {
					// 플래그 재설정
					clif_displaymessage(fd,"[이벤트]: 플래그 재설정 오류");
					ShowError("atcommand_eevent: Flag Error!\n");
					return 1;
				}

				// Tick 설정
				if(event_tick!=tick) {
					// Tick
					ShowError("atcommand_eevent: Timer Tick 오류 \n");
					clif_displaymessage(fd,"[이벤트]: Timer Tick Error!");
					return 1;
				}

				// 확률
				if(event_rate!=rate) {
					// 오류
					clif_displaymessage(fd,"[이벤트]: EXP bonus Rate Error!");
					ShowError("atcommand_eevent: EXP bonus Rate Error!\n");
					return 1;
				}
				
				// 본격적인 Timer 설정 시작
				itimer = add_timer(gettick()+60000,atcommand_eevent_timer,event_tid,1);

				event_tid = itimer;

				// 방송 시작
				i=((double)event_rate/10)+1;

				sprintf(output,"[ %d분간 경험치 획득량이 %.1f배가 됩니다. ]",event_tick,i);
				clif_displaymessage(fd,output);
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
				clif_displaymessage(fd,"[이벤트]: 경험치 이벤트가 시작되었습니다.");
				return 0;

			case 2:
				// 중단
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_dflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
					return 1;
				}

				if(event_eflag==0) {
					clif_displaymessage(fd,"[이벤트]: 경험치 이벤트가 진행중이 아닙니다.");
					return 1;
				}
				
				// 먼저 Timer 삭제
				delete_timer(event_tid,atcommand_eevent_timer);
				
				// 각 데이터
				event_eflag=0;
				event_rate=0;
				event_tick=0;
				event_tid=0;

				if(event_eflag!=0) {
					ShowError("atcommand_eevent: Flag 해체 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: Flag 해체 실패");
					return 1;
				}
				
				if(event_rate!=0) {
					ShowError("atcommand_eevent: 확률 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 확률 리셋 실패");
					return 1;
				}

				if(event_tick!=0) {
					ShowError("atcommand_eevent: 틱 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 틱 리셋 실패");
					return 1;
				}

				if(event_tid!=0) {
					ShowError("atcommand_eevent: 타이머 아이디 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 타이머 아이디 리셋 실패");
					return 1;
				}
				
				// 끝... 이제 방송
				intif_broadcast("[ 경험치 이벤트가 종료되었습니다. ]", strlen("[ 경험치 이벤트가 종료되었습니다. ]") + 1, BC_DEFAULT);
				clif_displaymessage(fd,"[이벤트]: 경험치 이벤트가 종료 되었습니다.");
				return 0;

			case 3:
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_dflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
					return 1;
				}
				// 타임 추가.
				if(event_eflag==1) {
					// 추가 시작
					if(tick<0) {
						if(tick<(event_tick*-1)) {
							tick=(event_tick*-1);
							event_tick=1;
						}
						else {
							event_tick+=tick;
						}
					} else {
						event_tick+=tick;
					}


					// 결과 공지.
					sprintf(output,"[ 경험치 이벤트가 %d분 연장되어 %d분 남았습니다. ]",tick,event_tick);
					intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
					clif_displaymessage(fd,"[이벤트]: 경험치 이벤트가 정상적으로 연장되었습니다.");
					return 1;
				}
				else {
					clif_displaymessage(fd,"[이벤트]: 경험치 이벤트가 시행중이 아닙니다.");
					return 1;
				}
					
			default:
				return false;
			}
		
		case 2:
			// 아이템
			switch(flag)
			{

			case 0:
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
					return 1;
				}
				if(event_dflag==0) {
					sprintf(output,"[이벤트]: 현재 드롭률 이벤트가 진행중이 아닙니다.");
				}
				else if(event_dflag==1){
						sprintf(output,"[이벤트]: 현재 드롭률 이벤트가 진행중입니다.(남은 시간 %d분)",event_tick);
				}
				else {
					sprintf(output,"[이벤트]: 현재 플래그로 작동중인 이벤트가 아닙니다.");
				}
				clif_displaymessage(fd,output);
				return 2;

			case 1:
				// 이미 flag 작동중인게 있나 확인
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
					return 1;
				}
				
				
				// 이미 시작인지 채크
				if(event_dflag==1) {
					sprintf(output,"[이벤트]: 이미 드롭률 이벤트가 시행중입니다. 남은시간: %d분",event_tick);
					clif_displaymessage(fd,output);
					return 1;
				}

				// tick 채크 
				if(tick<=0) {
					clif_displaymessage(fd,"[이벤트]: 시간 데이터가 잘못되었습니다.");;
					return 1;
				}

				// rate 채크
				if(rate<=0) {
					clif_displaymessage(fd,"[이벤트]: 배율 데이터가 잘못되었습니다.");
					return 1;
				}

				
				// 확률 설정
				event_rate=rate;

				// 플래그 설정
				event_dflag=1;
				
				// 시간 설정
				event_tick=tick;

				// 설정 재확인
				if(event_dflag==0) {
					// 플래그 재설정
					clif_displaymessage(fd,"[이벤트]: 플래그 재설정 오류");
					ShowError("atcommand_eevent: Flag Error!\n");
					return 1;
				}

				// Tick 설정
				if(event_tick!=tick) {
					// Tick
					ShowError("atcommand_eevent: Timer Tick 오류 \n");
					clif_displaymessage(fd,"[이벤트]: Timer Tick Error!");
					return 1;
				}

				// 확률
				if(event_rate!=rate) {
					// 오류
					clif_displaymessage(fd,"[이벤트]: Drop rate bonus Rate Error!");
					ShowError("atcommand_eevent: Drop rate bonus Rate Error!\n");
					return 1;
				}
				
					// 본격적인 Timer 설정 시작
				itimer = add_timer(gettick()+60000,atcommand_eevent_timer,event_tid,2);
				event_tid = itimer;

				// 방송 시작
				i=((double)event_rate/10)+1;

				sprintf(output,"[ %d분간 아이템 드롭률이 %.1f배가 됩니다. ]",event_tick,i);
				clif_displaymessage(fd,output);
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
				clif_displaymessage(fd,"[이벤트]: 드롭률 이벤트가 시작되었습니다.");
				return 0;

			case 2:
				// 중단
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
					return 1;
				}
				
				if(event_dflag==0) {
					clif_displaymessage(fd,"[이벤트]: 드롭률 이벤트가 진행중이 아닙니다.");
					return 2;
				}
				
				// 먼저 Timer 삭제
				delete_timer(event_tid,atcommand_eevent_timer);
				
				// 각 데이터
				event_dflag=0;
				event_rate=0;
				event_tick=0;
				event_tid=0;

				if(event_dflag!=0) {
					ShowError("atcommand_eevent: Flag 해체 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: Flag 해체 실패");
					return 1;
				}
				
				if(event_rate!=0) {
					ShowError("atcommand_eevent: 확률 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 확률 리셋 실패");
					return 1;
				}

				if(event_tick!=0) {
					ShowError("atcommand_eevent: 틱 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 틱 리셋 실패");
					return 1;
				}

				if(event_tid!=0) {
					ShowError("atcommand_eevent: 타이머 아이디 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 타이머 아이디 리셋 실패");
					return 1;
				}
				
				// 끝... 이제 방송
				sprintf(output,"[ 드롭률 이벤트가 종료되었습니다. ]");
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
				clif_displaymessage(fd,"[이벤트]: 드롭률 이벤트가 종료 되었습니다.");
				return 0;

			case 3:
				// 타임 추가.
				if(event_dflag==1 && event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 + 드롭률 이벤트)");
					return 1;

				} else if(event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
					return 1;
				}
				if(event_dflag==1) {
					// 추가 시작
					if(tick<0) {
						if(tick<(event_tick*-1)) {
							tick=(event_tick*-1);
							event_tick=1;
						}
						else {
							event_tick+=tick;
						}
					} else {
						event_tick+=tick;
					}


					// 결과 공지.
					sprintf(output,"[ 드롭률 이벤트가 %d분 연장되어 %d분 남았습니다. ]",tick,event_tick);
					intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
					clif_displaymessage(fd,"[이벤트]: 드롭률 이벤트가 정상적으로 연장되었습니다.");
					return 0;
				}
				else {
					clif_displaymessage(fd,"[이벤트]: 드롭률 이벤트가 시행중이 아닙니다.");
					return 1;
				}
					
			default:
				return 1;
			}

		case 3: // 통합
			switch(flag)
			{

			case 0:
				if(event_eflag!=event_dflag) {
					if(event_eflag==1) {
						clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
						return 1;

					} else if(event_dflag==1) {
						clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
						return 1;
					}
				}
				// 상태 표시
				if(event_eflag==0 && event_dflag==0) {
					sprintf(output,"[이벤트]: 현재 통합 이벤트 진행중이 아닙니다.");
				}
				else if(event_eflag==1 && event_dflag==1){
						sprintf(output,"[이벤트]: 현재 통합 이벤트가 진행중입니다.(남은 시간 %d분)",event_tick);
				}
				else {
					sprintf(output,"[이벤트]: 현재 플래그로 작동중인 이벤트가 아닙니다.");
				}
				clif_displaymessage(fd,output);
				return 1;

			case 1:
				// 이미 flag 작동중인게 있나 확인
				if(event_eflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
					return 1;

				} else if(event_dflag==1) {
					clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
					return 1;
				}
				
				
				// 이미 시작인지 채크
				if(event_eflag==1&&event_dflag==1) {
					sprintf(output,"[이벤트]: 이미 통합 이벤트가 시행중입니다. 남은시간: %d분",event_tick);
					clif_displaymessage(fd,output);
					return 1;
				}

				// tick 채크 
				if(tick<=0) {
					clif_displaymessage(fd,"[이벤트]: 시간 데이터가 잘못되었습니다.");;
					return 1;
				}

				// rate 채크
				if(rate<=0) {
					clif_displaymessage(fd,"[이벤트]: 배율 데이터가 잘못되었습니다.");
					return 1;
				}

				
				// 확률 설정
				event_rate=rate;

				// 플래그 설정
				event_eflag=1;
				event_dflag=1;

				// 시간 설정
				event_tick=tick;

				// 설정 재확인
				if(event_eflag==0||event_dflag==0) {
					// 플래그 재설정
					clif_displaymessage(fd,"[이벤트]: 플래그 재설정 오류");
					ShowError("atcommand_eevent: Flag Error!\n");
					return 1;
				}

				// Tick 설정
				if(event_tick!=tick) {
					// Tick
					ShowError("atcommand_eevent: Timer Tick 오류 \n");
					clif_displaymessage(fd,"[이벤트]: Timer Tick Error!");
					return 1;
				}

				// 확률
				if(event_rate!=rate) {
					// 오류
					clif_displaymessage(fd,"[이벤트]: Total bonus Rate Error!");
					ShowError("atcommand_eevent: Total bonus Rate Error!\n");
					return 1;
				}
		
				itimer = add_timer(gettick()+60000,atcommand_eevent_timer,event_tid,3);

				event_tid = itimer;

				// 방송 시작
				i=((double)event_rate/10)+1;

				sprintf(output,"[ %d분간 드롭률과 경험치량이 %.1f배가 됩니다. ]",event_tick,i);
				clif_displaymessage(fd,output);
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
				clif_displaymessage(fd,"[이벤트]: 통합 이벤트가 시작되었습니다.");
				return 0;

			case 2:
				// 중단
				if(event_eflag!=event_dflag) {
					if(event_eflag==1) {
						clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
						return 1;

					} else if(event_dflag==1) {
						clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
						return 1;
					}
				}

				if(event_dflag==0&&event_eflag==0) {
					clif_displaymessage(fd,"[이벤트]: 통합 이벤트가 진행중이 아닙니다.");
					return 1;
				}

				delete_timer(event_tid,atcommand_eevent_timer);

				
				// 각 데이터
				event_eflag=0;
				event_dflag=0;
				event_rate=0;
				event_tick=0;
				event_tid=0;

				if(event_eflag!=0||event_dflag!=0) {
					ShowError("atcommand_eevent: Flag 해체 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: Flag 해체 실패");
					return 1;
				}
				
				if(event_rate!=0) {
					ShowError("atcommand_eevent: 확률 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 확률 리셋 실패");
					return 1;
				}

				if(event_tick!=0) {
					ShowError("atcommand_eevent: 틱 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 틱 리셋 실패");
					return 1;
				}

				if(event_tid!=0) {
					ShowError("atcommand_eevent: 타이머 아이디 리셋 실패\n");
					clif_displaymessage(fd,"atcommand_eevent: 타이머 아이디 리셋 실패");
					return 1;
				}
				
				// 끝... 이제 방송
				sprintf(output,"[ 통합 이벤트가 종료되었습니다. ]");
				intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
				clif_displaymessage(fd,"[이벤트]: 통합 이벤트가 종료 되었습니다.");
				return 0;

			case 3:
				if(event_eflag!=event_dflag) {
					if(event_eflag==1) {
						clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (경험치 이벤트)");
						return 1;

					} else if(event_dflag==1) {
						clif_displaymessage(fd,"[이벤트]: 현재 다른 옵션으로 이벤트가 진행중입니다. (드롭률 이벤트)");
						return 1;
					}
				}
				// 타임 추가.
				if(event_eflag==1&&event_dflag==1) {
					// 추가 시작
					if(tick<0) {
						if(tick<(event_tick*-1)) {
							tick=(event_tick*-1);
							event_tick=1;
						}
						else {
							event_tick+=tick;
						}
					} else {
						event_tick+=tick;
					}


					// 결과 공지.
					sprintf(output,"[ 통합 이벤트가 %d분 연장되어 %d분 남았습니다. ]",tick,event_tick);
					intif_broadcast(output, strlen(output) + 1, BC_DEFAULT);
					clif_displaymessage(fd,"[이벤트]: 통합 이벤트가 정상적으로 연장되었습니다.");
					return 0;
				}
				else {
					clif_displaymessage(fd,"[이벤트]: 통합 이벤트가 시행중이 아닙니다.");
					return 1;
				}
					
			default:
				return 1;
			}

		default:
			return 1;

	}

	return 1;
}

static void atcommand_buf_sub(const int fd,struct map_session_data* sd,const int mode)
{
	if (sd && sd->state.active) 
	{
		int time;

		switch(mode)
		{
			case 1:	time=10*60;	break;
			case 2:	time=15*60;	break;
			case 3:	time=20*60;	break;
			default:time=10*60;	break;
		}

		switch(mode)
		{
			case 3:
				if(pc_isdead(sd)) 
				{
					clif_skill_nodamage(&sd->bl,&sd->bl,ALL_RESURRECTION,4,1);
					status_revive(&sd->bl, 100, 100);
				}
			case 2:
				clif_skill_nodamage(&sd->bl,&sd->bl,PR_MAGNIFICAT,10,sc_start(&sd->bl,&sd->bl,SC_MAGNIFICAT,100,10,time*1000));
				clif_skill_nodamage(&sd->bl,&sd->bl,PR_GLORIA,10,sc_start(&sd->bl,&sd->bl,SC_GLORIA,100,10,time*1000));
			case 1:
				clif_skill_nodamage(&sd->bl,&sd->bl,HP_ASSUMPTIO,10,sc_start(&sd->bl,&sd->bl,SC_ASSUMPTIO,100,10,time*1000));
				clif_skill_nodamage(&sd->bl,&sd->bl,PR_IMPOSITIO,10,sc_start(&sd->bl,&sd->bl,SC_IMPOSITIO,100,10,time*1000));
			default:
				clif_skill_nodamage(&sd->bl,&sd->bl,AL_BLESSING,10,sc_start(&sd->bl,&sd->bl,SC_BLESSING,100,10,time*1000));
				clif_skill_nodamage(&sd->bl,&sd->bl,AL_INCAGI,10,sc_start(&sd->bl,&sd->bl,SC_INCREASEAGI,100,10,time*1000));
		}

		if (battle_config.pc_invincible_time > 0)
			pc_setinvincibletimer(sd, battle_config.pc_invincible_time);
	}
}

ACMD_FUNC(bbuf)
{
	int mode = 0;
	struct s_mapiterator* iter;
	struct map_session_data* pl_sd;

	char temp[1024];
	char msg[50];

	nullpo_retr(-1, sd);

	if(!message || !*message || sscanf(message, "%d", &mode) < 1) 
	{
		clif_displaymessage(fd, "버프 번호를 선택해 주십시오. (사용법: @버프 <번호, 메시지>)");
		clif_displaymessage(fd, " 0 : 블레싱,민첩성증가(10분)");
		clif_displaymessage(fd, " 1 : 블레싱,민첩성증가,아숨프티오,임포시티오(10분)");
		clif_displaymessage(fd, " 2 : 블레싱,민첩성증가,아숨프티오,임포시티오,마니피캇,글로리아(15분)");
		clif_displaymessage(fd, " 3 : 리저렉션,블레싱,민/증,아숨,임포,마니피캇,글로리아(20분)");
		return -1;
	}

	if (mode > 3) mode = 3;

	iter = mapit_getallusers();

	for( pl_sd = (TBL_PC*)mapit_first(iter); mapit_exists(iter); pl_sd = (TBL_PC*)mapit_next(iter) )
		atcommand_buf_sub(fd,pl_sd,mode);

	mapit_free(iter);

	switch(mode)
	{
		case 0: strcpy(msg,"약간의");	break;
		case 1:	strcpy(msg,"대충");		break;
		case 2:	strcpy(msg,"쓸만한");	break;
		case 3:	strcpy(msg,"막강한");	break;
	}

	sprintf(temp, "<%s>님께서 %s 파워를 보여줍니다.", sd->status.name, msg);

	intif_broadcast(temp,strlen(temp)+1,0x10);

	return 0;
}
