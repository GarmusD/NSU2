#include "FileManager.h"
#include "Base64.h"
#include "aJson.h"
#include "JsonPrinter.h"

const int InputLength = 64;

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

void CFileManager::Init()
{
	pinMode(sdpin, OUTPUT);
	digitalWrite(sdpin, HIGH);
}

bool CFileManager::Begin()
{
	if (!sd.begin(sdpin, sdspeed)) {
		Log.error("SD access error!");
		sdOk = false;
		return false;
	}
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

void CFileManager::ReadFile(JsonObject& jo)
{
	Log.debug("CFileManager::ReadFile()");
	char fname[MAX_NAME_LENGTH];
	fname[0] = 0;

	if(jo.containsKey(jFileManFileName))
		strlcpy(fname, jo[jFileManFileName], MAX_NAME_LENGTH);
	String cmdid = String(PrintJson.getCmdID());

	if (file.open(fname, O_READ))
	{
		
		char input[InputLength];
		char base64out[InputLength*2];

		int res = file.read(input, InputLength);
		while (res > 0)
		{
			int l = base64_encode(base64out, input, res);
			base64out[l] = 0;
			StaticJsonBuffer<512> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetFileManager;
			root[jKeyAction] = jValueGet;// jFileManActionRead;
			if (cmdid.length())
				root[jKeyCmdID] = cmdid;
			root[jKeyResult] = jValueResultOk;
			root[jFileManData] = base64out;

			PrintJson.Print(root);

			res = file.read(input, InputLength);
		}
		if (res == 0)
		{
			StaticJsonBuffer<512> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetFileManager;
			root[jKeyAction] = jValueGet;// jFileManActionRead;
			if (cmdid.length())
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

void CFileManager::WriteFile(JsonObject& jo)
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
				char output[InputLength];
				char base64in[InputLength * 2];
				strlcpy(base64in, jo[jFileManData], InputLength * 2);
				int res = base64_decode(output, base64in, strlen(base64in));
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

void CFileManager::ParseJson(JsonObject& jo)
{
	Log.debug("CFileManager::ParseJson()");
	String action = jo[jKeyAction];
	if (action.equals(jValueGet))
	{
		ReadFile(jo);
		return;
	}
	if (action.equals(jValueSet))
	{
		WriteFile(jo);
		return;
	}
}

CFileManager FileManager(SD_PIN, SPI_HALF_SPEED);