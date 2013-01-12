/*
 * dcf77.c
 *
 * Created: 25-9-2012 11:47:48
 *  Author: Koen Beckers
 */ 
#include "main.h"
time times[5];

void addMinute(time *theTime){
	theTime->minutes++;
	if(theTime->minutes == 60){
		theTime->minutes = 0;
		theTime->hours++;
	}
	if(theTime->hours == 24){
		theTime->hours = 0;
	}
}

uint8_t compareTime(time *time1, time *time2, uint8_t offset){
	for(uint8_t of = 0; of <= offset; of++){
		addMinute(time1);
	}
	
	if(time1->hours == time2->hours && time1->hours == time2->hours){
		return 1;
	}else{
		return 0;
	}
}

uint8_t checkTime(time currentTime){
	uint8_t correctCount = 0;
	for(uint8_t i = 0; i <= 3; i++){
		correctCount += compareTime(&times[i],&currentTime,i);
	}
	
	for(uint8_t i = 1; i<= 3; i++){
		times[i-1] = times[1];
	}
	times[3] = currentTime;
	
	if(correctCount > 2){
		return 1;
	}else{
		return 0;
	}
}

uint8_t handleDCF(uint8_t bit, uint8_t second){
	uint8_t ret = 0;
	//if(CHECKFLAG(FLAG_SYNCED)){
		switch(second){
			case 0: break;
			case 1: break;
			case 15: break; //If backup antenna is being used
			case 16: break; //Announcement for summer or wintertime
			case 17: break; //0 for wintertime, 1 for summertime
			case 18: break; //1 for wintertime, 0 for summertime
			case 20: if(bit != 1){ ret = 1; }; break; //Start of time, should always be 1!
			case 21: dcfTime.minutes  = (bit << 0); break;
			case 22: dcfTime.minutes |= (bit << 1); break;
			case 23: dcfTime.minutes |= (bit << 2); break;
			case 24: dcfTime.minutes |= (bit << 3); break;
			case 25: dcfTime.minutes += ((bit << 0) * 10); break;
			case 26: dcfTime.minutes += ((bit << 1) * 10); break;
			case 27: dcfTime.minutes += ((bit << 2) * 10); break;
			case 28: if( ( (dcfTime.minutes + bit) & 1) != 1){ ret = 1;} break; //Checksum of minutes
			case 29: dcfTime.hours  = (bit << 0); break;
			case 30: dcfTime.hours |= (bit << 1); break;
			case 31: dcfTime.hours |= (bit << 2); break;
			case 32: dcfTime.hours |= (bit << 3); break;
			case 33: dcfTime.hours += (bit << 0) * 10; break;
			case 34: dcfTime.hours += (bit << 1) * 10; break;
			case 35: if( ( (dcfTime.hours + bit) & 1) != 1){ ret = 1;} break; //Checksum of hours
		}
	//}	
	return ret;
}

uint8_t errorInHandle = 0;
void handleBit(){
	
	if(onCounter > 7 && onCounter < 14){
		errorInHandle = handleDCF(0, secondCounter);
	}else if(onCounter > 17 && onCounter < 24){
		errorInHandle = handleDCF(1, secondCounter);
	}else if(onCounter != 0){
		//Error bit!
		//SETFLAG(FLAG_ERROR);
		if(secondCounter > 20 && secondCounter < 36){
			errorInHandle = 1;
		} //The rest isn't needed		
	}
	
	secondCounter++;
	if(offCounter >= 93 && secondCounter < 59 && secondCounter > 2){
		//It CAN read the clock, just not handle it yet
		SETFLAG(FLAG_SYNCED);
		RESETFLAG(FLAG_STARTUP);
		RESETFLAG(FLAG_ERROR);
		secondCounter = 0;
		//This probably means it just started up..
	}else if(offCounter >= 93 && secondCounter >= 59){
		if(errorInHandle){
			SETFLAG(FLAG_ERROR);
			errorInHandle = 0;
		}else{
			RESETFLAG(FLAG_ERROR);
		}
		//Oh, new minute! :O
		secondCounter = 0;
		SETFLAG(FLAG_SYNCED);
		//Display the new time
		if(dcfTime.hours > 24 || dcfTime.minutes > 59){		
			//ERROR!
			SETFLAG(FLAG_ERROR);
			dcfTime.hours = 0;
			dcfTime.minutes = 0;
		}
		//if(!checkTime(dcfTime)){
		//	SETFLAG(FLAG_ERROR);
		//}
		/*if(dcfHours > currentHour && dcfHours - currentHour >= 2 && currentHour != 0 && currentHour != 23){
			SETFLAG(FLAG_ERROR);
			addMinute();
		}else if(dcfHours < currentHour && currentHour - dcfHours >= 2 && currentHour != 0 && currentHour != 23){
			SETFLAG(FLAG_ERROR);
			addMinute();
		}
		
		if(dcfMinutes > currentMinutes && dcfMinutes - currentMinutes >= 9 && currentMinutes != 0 && currentMinutes != 59){
			SETFLAG(FLAG_ERROR);
			addMinute();
		}else if(dcfMinutes < currentMinutes && currentMinutes - dcfMinutes >= 9 && currentMinutes != 0 && currentMinutes != 59){
			SETFLAG(FLAG_ERROR);
			addMinute();
		}*/
		if(!CHECKFLAG(FLAG_ERROR)){
			currentTime.hours = dcfTime.hours;
			currentTime.minutes = dcfTime.minutes;
			RESETFLAG(FLAG_STARTUP);
		}else{
			//RESETFLAG(FLAG_ERROR);
			RESETFLAG(FLAG_SYNCED);
			if(currentTime.hours == 0 && currentTime.minutes == 0){
				currentTime.hours = dcfTime.hours;
				currentTime.minutes = dcfTime.minutes;
			}else{
				addMinute(&currentTime);
			}
		}
	}
	onCounter = 0;
	offCounter = 0;
}