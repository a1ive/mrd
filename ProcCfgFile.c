#include "MyRamDisk.h"

///���������ļ�����iso�ļ����
EFI_STATUS
	ProcCfgFile(
		DIDO_OPTION_STATUS						*OptionStatus,	
		EFI_FILE_HANDLE							CurrDirHandle,
		IN 				 CHAR16					*ConfigFileName			//�����ļ���
		)
		{
			EFI_STATUS							Status;
			EFI_FILE_HANDLE						CfgFileHandle;
			CHAR8								*ConfigFileLine;
			UINTN 								ConfigFileSize;
			//�������ļ�
			Print(L"Process config file\n");
			Status=CurrDirHandle->Open(CurrDirHandle,&CfgFileHandle,ConfigFileName,EFI_FILE_MODE_READ,EFI_FILE_DIRECTORY);
			if(EFI_ERROR (Status)){
				Print(L"Can't find cfg file 'imgboot.cfg' in current dir\n");
				return EFI_NOT_FOUND;
				}
			//�������ļ�	
			FileHandleGetSize(CfgFileHandle	,&ConfigFileSize);
			ConfigFileLine=AllocateZeroPool(ConfigFileSize+1);
			FileHandleRead(CfgFileHandle,&ConfigFileSize,ConfigFileLine);
			
			//Ҫ����ASCII��UNICODE��ת��

			OptionStatus->OptionStringSizeInByte=ConfigFileSize*2;
			OptionStatus->OptionString=AllocateZeroPool(OptionStatus->OptionStringSizeInByte+2);
			AsciiStrToUnicodeStrS(ConfigFileLine,OptionStatus->OptionString,ConfigFileSize+1);
			//���ַ����ֽ⵽����Ա
			DispatchOptions(OptionStatus);
			FreePool(OptionStatus->OptionString);
			if(OptionStatus->ImageFileName==NULL){
				Print(L"Config file didn't have a file name\n");
				return EFI_NOT_FOUND;
				}
			Print(L"Image file name is:%s\n",OptionStatus->ImageFileName);	
			return 	EFI_SUCCESS;
			
	

		}