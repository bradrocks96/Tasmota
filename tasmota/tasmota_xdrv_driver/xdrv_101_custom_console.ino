#ifdef USE_CUSTOM_CONSOLE

#warning **** custom console is included... ****

#define XDRV_101 101

const char CustomConsoleCommands[] PROGMEM = "|"
  "RtcSetLocalTime|"
  "RtcGetUtcTime";

void (* const CustomConsoleCommand[])(void) PROGMEM = {
  &CnmdRtcSetLocalTime, &CnmdRtcGetUtcTime};

void CnmdRtcSetLocalTime(void){
    //output format {"Time":"2023-06-21T11:54:51"}
    //input RtcSetLocalTime 2023-06-21T11:54:51
    //rtc hold UTC time

    TIME_T tm;
    char buf[5]; 

    if(XdrvMailbox.data_len != 19){
      AddLog(LOG_LEVEL_INFO, PSTR("19 characters expected entered : %d"), XdrvMailbox.data_len);
      AddLog(LOG_LEVEL_INFO, PSTR("Example : %s"),"2023-06-21T11:54:51");
      ResponseCmndDone();
      return;
    }

    if((XdrvMailbox.data[4] != '-') || (XdrvMailbox.data[7] != '-') || (XdrvMailbox.data[10] != 'T') || (XdrvMailbox.data[13] != ':') || (XdrvMailbox.data[16] != ':')){
      AddLog(LOG_LEVEL_INFO, PSTR("Invalid format. Example : %s"), "2023-06-21T11:54:51");
      ResponseCmndDone();
      return;
    }

    AddLog(LOG_LEVEL_INFO, PSTR("Check the time zone setting, if it is incorrect, the UTC time in the RTC will be incorrect."));
    AddLog(LOG_LEVEL_INFO, PSTR("Current time zone : %s"), GetTimeZone().c_str());

    memcpy(buf,XdrvMailbox.data,4);
    tm.year = (uint16_t)(atoi(buf) - 1970);
    memset(buf,0,sizeof(buf));
    
    memcpy(buf,&XdrvMailbox.data[5],2);
    tm.month = (uint8_t)atoi(buf);
    memset(buf,0,sizeof(buf));
    
    memcpy(buf,&XdrvMailbox.data[8],2);
    tm.day_of_month = (uint8_t)atoi(buf);
    memset(buf,0,sizeof(buf));
    
    memcpy(buf,&XdrvMailbox.data[11],2);
    tm.hour = (uint8_t)atoi(buf);
    memset(buf,0,sizeof(buf));

    memcpy(buf,&XdrvMailbox.data[14],2);
    tm.minute = (uint8_t)atoi(buf);
    memset(buf,0,sizeof(buf));
     
    memcpy(buf,&XdrvMailbox.data[17],2);
    tm.second = (uint8_t)atoi(buf);
    memset(buf,0,sizeof(buf));

  uint32_t local_time = MakeTime(tm);
  uint32_t utc_time = local_time - RtcTimeZoneOffset(local_time);
  DS3231SetTime(utc_time);

  uint32_t epoch = DS3231ReadTime();
  AddLog(LOG_LEVEL_INFO, PSTR("Time RTC UTC : %s"), GetDT(epoch).c_str());
  AddLog(LOG_LEVEL_INFO, PSTR("Restart to apply changes"));
  ResponseCmndDone();
}

void CnmdRtcGetUtcTime(void){
  
  uint32_t epoch = DS3231ReadTime();
  AddLog(LOG_LEVEL_INFO, PSTR("Time RTC UTC : %s"), GetDT(epoch).c_str());

ResponseCmndDone();
}

bool Xdrv101(uint32_t function)
{

  bool result = false;

  switch (function) {
    case FUNC_COMMAND:
      result = DecodeCommand(CustomConsoleCommands, CustomConsoleCommand);
      break;

    case FUNC_PRE_INIT:
    break;

    case FUNC_INIT:
      break;

    case FUNC_EVERY_SECOND:
      break;
  }

  return result;
}


#endif //USE_CUSTOM_CONSOLE