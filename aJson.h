#pragma once

#ifndef _aJSON_h
#define _aJSON_h


//Static buffer size for incomming messages
const int JsonBuffSize = 2048;

//general keys
const char* const jKeyTarget = "target";// "trg";
const char* const jKeyName = "name";
const char* const jKeyAction = "action";// "act";
const char* const jKeyContent = "content";
const char* const jKeyResult = "result";
const char* const jKeyMessage = "message";//"msg";
const char* const jKeyStatus = "status";
const char* const jKeyEnabled = "enabled";
const char* const jKeySetup = "setup";
const char* const jKeyUpdate = "update";

const char* const jKeyCmdID = "cmdid";
const char* const jKeyConfigPos = "cfgpos";
const char* const jKeyValue = "value";

//boot checker
const char* const jTargetBoot = "boot";

//system
const char* const jTargetSystem = "system";
const char* const jKeySysStatusReadyPauseBoot = "readypauseboot";
const char* const jKeySysStatusBooting = "booting";
const char* const jKeySysStatusBootPaused = "bootpaused";
const char* const jKeySysStatusRunning = "running";
const char* const jKeySysStatusRebootRequired = "rebootreq";
const char* const jKeySystemActionReboot = "reboot";
const char* const jKeySystemActionPauseBoot = "pauseboot";
const char* const jKeySystemActionSystemStatus = "systemstatus";
const char* const jKeySystemActionSnapshot = "snapshot";
const char* const jKeySystemActionSystemReady = "systemready";
const char* const jKeySysStatusFreeMem = "freemem";
const char* const jKeySysStatusUpTime = "uptime";
const char* const jKeySysStatusActionSetTime = "settime";
const char* const jKeySysStatusYear = "y";
const char* const jKeySysStatusMonth = "m";
const char* const jKeySysStatusDay = "d";
const char* const jKeySysStatusHour = "h";
const char* const jKeySysStatusMinute = "mm";
const char* const jKeySysStatusSecond = "s";
//const char* const jKeySysStatus


//Alarm
const char* const jTargetAlarm = "alarm";
const char* const jAlarmTemp = "temp";
const char* const jAlarmHisteresis = "hist";
const char* const jAlarmChannelData = "chdata";
const char* const jAlarmChannel = "ch";
const char* const jAlarmOpen = "open";

//FileManager
const char* const jTargetFileManager = "fileman";
//const char* const jFileManActionRead = "read";
//const char* const jFileManActionWrite = "write";
const char* const jFileManFileName = "fname";
const char* const jFileManData = "data";

//tsensor
const char* const jKeyTargetTSensor = "tsensor";
const char* const jKeyTSensorAddr = "addr";
const char* const jKeyTSensorInterval = "interval";
const char* const jKeyTSensorErrorCount = "errors";

//switch
const char* const jTargetSwitch = "switch";
const char* const jSwitchDependName = "dname";
const char* const jSwitchDependState = "depstate";
const char* const jSwitchForceState = "forcestate";
const char* const jSwitchIsForced = "isforced";
const char* const jSwitchCurrState = "currstate";
const char* const jSwitchClick = "click";

//relaymodules
const char* const jTargetRelay = "relay";
const char* const jRelayActionOpenChannel = "open";
const char* const jRelayActionCloseChannel = "close";
const char* const jRelayActionLockChannel = "lock";
const char* const jRelayActionUnlockChannel = "unlock";
const char* const jRelayActiveLow = "activelow";
const char* const jRelayInverted = "inverted";
const char* const jRelayFlags = "flags";
const char* const jRelayChannelLocked = "locked";
const char* const jRelayChannel = "ch";

//TempTriggers
const char* const jTargetTempTrigger = "trigger";
const char* const jTriggerPieces = "pieces";
const char* const jTriggerCondition = "cndt";
const char* const jTriggerSensorName = "sname";
const char* const jTriggerTemperature = "temp";
const char* const jTriggerHisteresis = "hist";

//CircPumps
const char* const jTargetCircPump = "cpump";
const char* const jCircPumpChannel = "ch";
const char* const jCircPumpMaxSpeed = "maxspd";
const char* const jCircPumpSpeed1Ch = "spd1";
const char* const jCircPumpSpeed2Ch = "spd2";
const char* const jCircPumpSpeed3Ch = "spd3";
const char* const jCircPumpTTriggerName = "trgname";
const char* const jCircPumpCurrentSpeed = "speed";
const char* const jCircPumpValvesOpened = "vopened";
const char* const jCircPumpActionClick = "click";

//Collectors
const char* const jTargetCollector = "collector";
const char* const jCollectorValves = "valves";
const char* const jCollectorValve = "valve";
const char* const jCollectorCPumpName = "cpname";
const char* const jCollectorValveCount = "vcnt";
const char* const jCollectorValveType = "type";
const char* const jCollectorValveChannel = "ch";

//ComfortZones
const char* const jTargetComfortZone = "czone";
const char* const jCZoneTitle = "title";
const char* const jCZoneCollectorName = "clname";
const char* const jCZoneActuator = "act";
const char* const jCZoneHisterezis = "hist";
const char* const jCZoneRoomSensorName = "rsname";
const char* const jCZoneRoomTempHigh = "rth";
const char* const jCZoneRoomTempLow = "rtl";
const char* const jCZoneFloorSensorName = "fsname";
const char* const jCZoneFloorTempHigh = "fth";
const char* const jCZoneFloorTempLow = "ftl";
const char* const jCZoneCurrRoomTemp = "crt";
const char* const jCZoneCurrFloorTemp = "cft";
const char* const jCZoneActuatorOpened = "actopened";
const char* const jCZoneLowTempMode = "lowmode";

//KType
const char* const jTargetKType = "ktype";
const char* const jKTypeInterval = "interval";

//WaterBoilers
const char* const jTargetWaterBoiler = "waterboiler";
const char* const jWaterBoilerTempSensorName = "tsname";
const char* const jWaterBoilerCPumpName = "cpname";
const char* const jWaterBoilerTTriggerName = "trgname";
const char* const jWaterBoilerElHeatingEnabled = "elhenabled";
const char* const jWaterBoilerElPowerChannel = "powerch";
const char* const jWaterBoilerElHeatingData = "powerdata";
const char* const jWaterBoilerEHStartHour = "starth";
const char* const jWaterBoilerEHStartMin = "startm";
const char* const jWaterBoilerEHStopHour = "stoph";
const char* const jWaterBoilerEHStopMin = "stopm";

//WoodBoiler
const char* const jTargetWoodBoiler = "woodboiler";
const char* const jWoodBoilerWorkingTemp = "worktemp";
const char* const jWoodBoilerHisteresis = "hist";
const char* const jWoodBoilerTempSensorName = "tsname";
const char* const jWoodBoilerKTypeName = "ktpname";
const char* const jWoodBoilerExhaustFanChannel = "smokech";
const char* const jWoodBoilerLadomatChannel = "lch";
const char* const jWoodBoilerLadomatTriggerName = "ltrgname";
const char* const jWoodBoilerLadomatWorkingTemp = "ltemp";
const char* const jWoodBoilerCurrentTemp = "currtemp";
const char* const jWoodBoilerLadomatStatus = "lstatus";
const char* const jWoodBoilerSmokeFanStatus = "exhstatus";
const char* const jWoodBoilerTempStatus = "tstatus";
const char* const jWoodBoilerWBName = "wbname";

const char* const jWoodBoilerActionIkurimas = "ikurimas";
const char* const jWoodBoilerActionSwitch = "switch";
const char* const jWoodBoilerTargetExhaust = "exhaust";
const char* const jWoodBoilerTargetLadomat = "ladomat";
//const char* const jWoodBoiler

//SystemFan
const char* const jTargetSystemFan = "sysfan";
const char* const jSysFanTSensorName = "tsname";
const char* const jSysFanMinTemp = "mint";
const char* const jSysFanMaxTemp = "maxt";
//Action values
//const char* const jActionClick = "click";

//general values
const char* const jValueError = "error";

const char* const jValueGet = "get";
const char* const jValueSet = "set";
const char* const jValueInfo = "info";

const char* const jValueResultOk = "ok";
const char* const jValueResultError = "error";
const char* const jValueResultNull = "null";
const char* const jValueResultDone = "done";
const char* const jValueResultUnknownAction = "act_unknown";

const char* const jValueConfig = "config";
const char* const jValueConfigPlus = "configplus";

//error codes
const char* const jErrorInvalidDataSet = "inv_data_set";
const char* const jErrorUnsupportedCommand = "unsuppcmd";
const char* const jErrorFileOpenError = "file_open_error";
const char* const jErrorFileCloseError = "file_close_error";
const char* const jErrorFileWriteError = "file_write_error";
const char* const jErrorItemByNameNotFound = "item_by_name_not_found";



#endif

