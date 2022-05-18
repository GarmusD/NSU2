#include "FileManager.h"
#include "base64.hpp"
#include "aJson.h"
#include "JsonPrinter.h"

const int InputLength = 128;

CFileManager::CFileManager(const int pin, const uint8_t speed)
{
	sdpin = pin;
	sdspeed = speed;	
	sdOk = false;
	isWriting = false;
}


CFileManager::~CFileManager()
{
}

void CFileManager::InitHW()
{
	pinMode(sdpin, OUTPUT);
	digitalWrite(sdpin, HIGH);
}

bool CFileManager::Begin()
{
	if (!sd.begin(sdpin)) 
	{
		Log.error("SD access error!");
		sdOk = false;
		return false;
	}
	
	
	//sd.remove("switches.cfg");
	//sd.remove("tempsens.cfg");
	//sd.remove("relays.cfg");
	//sd.remove("triggers.cfg");
	//sd.remove("crcpumps.cfg");
	//sd.remove("colltrs.cfg");
	//sd.remove("cmfzones.cfg");
	//sd.remove("ktype.cfg");
	//sd.remove("wtrboilr.cfg");
	//sd.remove("woodblr.cfg");
	//sd.remove("sysfan.cfg");
	//sd.remove("alarm.cfg");

	sdOk = true;
	return true;
}

bool CFileManager::SDCardOk()
{
	return sdOk;
}

SdFat & CFileManager::SDFat()
{
	return sd;
}

bool CFileManager::FileExists(const char * fname)
{
	if (file.isOpen())
	{
		file.close();
	}
	return sd.exists(fname);
}

bool CFileManager::FileOpen(const char * fname)
{
	if (file.isOpen())
		file.close();
	return file.open(fname, O_RDWR);
}

bool CFileManager::FileCreate(const char * fname)
{
	if (file.isOpen())
		file.close();
	return file.open(fname, O_RDWR | O_TRUNC | O_CREAT);
}

bool CFileManager::FileTruncate()
{
	return file.truncate(0);
}

bool CFileManager::FileClose()
{
	if (file.isOpen())
	{
		return file.close();
	}
	return false;
}

bool CFileManager::FileSeek(uint32_t pos)
{
	return file.seekSet(pos);
}

uint8_t CFileManager::FileReadByte()
{
	return file.read();
}

int32_t CFileManager::FileReadInt()
{
	int32_t res;
	file.read(&res, sizeof(int32_t));
	return res;
}

bool CFileManager::FileReadBuffer(void * buff, size_t length)
{
	return file.read(buff, length) != -1;
}

bool CFileManager::FileWriteByte(uint8_t val)
{
	file.write(val);
	file.sync();
	return !file.getWriteError();
}

bool CFileManager::FileWriteInt(int32_t val)
{
	file.write(&val, sizeof(val));
	file.sync();
	return !file.getWriteError();
}

bool CFileManager::FileWriteBuff(const void * buff, size_t length)
{
	file.write(buff, length);
	file.sync();
	return !file.getWriteError();
}

FileStatus CFileManager::OpenConfigFile(const char * fname, const VersionInfo & vi)
{
	if (!SDCardOk())
		return FileStatus_Inaccessible;

	if (!FileExists(fname))
	{
		char s[128];
		sprintf(s, "Config file '%s' does not exists. Creating new one...", fname);
		Log.info(s);
		if (FileCreate(fname))
		{
			goto RecreateFile;
		}
		return FileStatus_Inaccessible;
	}
	if (FileOpen(fname) && file.fileSize() >= sizeof(VersionInfo))
	{
		VersionInfo cvi;
		FileReadBuffer(&cvi, sizeof(VersionInfo));
		//Compare versions
		if (cvi.vstring[0] == vi.vstring[0] && cvi.vstring[1] == vi.vstring[1] && cvi.vstring[2] == vi.vstring[2] && cvi.version == vi.version)
		{
			return FileStatus_OK;
		}
		else
		{
			char s[128];
			sprintf(s, "Config file '%s' version mismatch.", fname);
			Log.info(s);
			goto RecreateFile;
		}
	}
	else
	{
		char s[128];
		sprintf(s, "Config file '%s' is corrupted. Recreating...", fname);
		Log.info(s);
		goto RecreateFile;
	}
	return FileStatus_Inaccessible;

RecreateFile:
	Log.info("Rewriting config file header...");
	if (!FileTruncate())
	{
		Log.error("FileTruncate() error.");
		FileClose();
		return FileStatus_RWError;
	}
	if (!FileWriteBuff(&vi, sizeof(VersionInfo)))
	{
		Log.error("FileWriteBuff() error while writing VersionInfo header.");
		FileClose();
		return FileStatus_RWError;
	}
	return FileStatus_ReCreate;
}

void CFileManager::ReadFile(JsonObject jo)
{
	Log.debug("CFileManager::ReadFile()");
	char fname[MAX_NAME_LENGTH];
	fname[0] = 0;

	if(jo.containsKey(jFileManFileName))
		strlcpy(fname, jo[jFileManFileName], MAX_NAME_LENGTH);
	const char * cmdid = PrintJson.getCmdID();

	if (file.open(fname, O_READ))
	{
		
		unsigned char input[InputLength];
		unsigned char base64out[InputLength*2];

		int res = file.read(input, InputLength);
		while (res > 0)
		{
			int l = encode_base64(input, res, base64out);
			base64out[l] = 0;
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetFileManager;
			root[jKeyAction] = jValueGet;// jFileManActionRead;
			if (cmdid[0] != 0)
				root[jKeyCmdID] = cmdid;
			root[jKeyResult] = jValueResultOk;
			root[jFileManData] = base64out;

			PrintJson.Print(root);

			res = file.read(input, InputLength);
		}
		if (res == 0)
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetFileManager;
			root[jKeyAction] = jValueGet;// jFileManActionRead;
			if (cmdid[0] != 0)
				root[jKeyCmdID] = cmdid;
			root[jKeyResult] = jValueResultDone;

			PrintJson.Print(root);
		}
		else if (res < 0)//error
		{
			PrintJson.PrintResultError(jTargetFileManager, "file_read_error");
		}
		//delete base64out;
		//delete input;
		file.close();
		return;
	}
	PrintJson.PrintResultError(jTargetFileManager, "file_open_error");
}

void CFileManager::WriteFile(JsonObject jo)
{
	if (!isWriting)
	{
		Log.debug("CFileManager::WriteFile()");
		char fname[MAX_NAME_LENGTH];
		fname[0] = 0;
		if (jo.containsKey(jFileManFileName))
		{
			strlcpy(fname, jo[jFileManFileName], MAX_NAME_LENGTH);
			if (file.open(fname, O_RDWR | O_CREAT))
			{
				PrintJson.PrintResultOk(jTargetFileManager, jValueSet);
				isWriting = true;
			}
			else
			{
				PrintJson.PrintResultError(jTargetFileManager, jErrorFileOpenError);
				isWriting = false;
			}
		}
		else 
		{
			PrintJson.PrintResultError(jTargetFileManager, jErrorInvalidDataSet);
			isWriting = false;
		}
	}
	else
	{
		if (jo.containsKey(jKeyContent))
		{
			if (strcmp(jo[jKeyContent], jFileManData) == 0)
			{
				unsigned char output[InputLength];
				unsigned char base64in[InputLength * 2];
				strlcpy((char*)base64in, jo[jKeyValue], InputLength * 2);
				int res = decode_base64(base64in, output);
				if (file.isOpen())
				{
					int wres = file.write(output, res);
					if (wres == res)
					{
						PrintJson.PrintResultOk(jTargetFileManager, jValueSet);
					}
					else
					{
						PrintJson.PrintResultError(jTargetFileManager, jErrorFileWriteError);
					}
				}
			}
			else if (strcmp(jo[jKeyContent], jValueResultDone) == 0)
			{
				if (file.close())
				{
					isWriting = false;
					PrintJson.PrintResultOk(jTargetFileManager, jValueSet);
				}
				else
				{
					PrintJson.PrintResultError(jTargetFileManager, "err_close_file");
				}
			}
		}
		else
		{
			PrintJson.PrintResultError(jTargetFileManager, jErrorInvalidDataSet);
			if (file.isOpen())
			{
				file.close();
			}
		}
	}
}

void CFileManager::ParseJson(JsonObject jo)
{
	Log.debug("CFileManager::ParseJson()");
	const char * action = jo[jKeyAction];
	if (strcmp(action, jValueGet) == 0)
	{
		ReadFile(jo);
		return;
	}
	if (strcmp(action, jValueSet) == 0)
	{
		WriteFile(jo);
		return;
	}
}

CFileManager FileManager(SD_PIN);