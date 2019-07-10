#include "MyRamDisk.h"

///���������ļ�����iso�ļ����
EFI_FILE_HANDLE
	ProcCfgFile(
		EFI_DEVICE_PATH_PROTOCOL				*CurrDirDP,
		EFI_FILE_HANDLE							CurrDirHandle,
		IN 				 CHAR16					*ConfigFileName			//�����ļ���
		)
		{
			EFI_STATUS							Status;
			EFI_HANDLE 							CurrDeviceHandle;		//��ǰ�豸�ľ��
			EFI_FILE_HANDLE						CfgFileHandle;
			EFI_FILE_HANDLE						OutPutIsoFileHandle;
			EFI_SIMPLE_FILE_SYSTEM_PROTOCOL 	*DidoTempProtocol;
			EFI_FILE_PROTOCOL 					*DidoVolumeHandle=NULL,*DidoFileHandle;
			CHAR16								*MyTempFileName=NULL;
			BOOLEAN 							IsAscii=FALSE;
			CHAR16								*ConfigFileLine;
			//�������ļ�
			Status=CurrDirHandle->Open(CurrDirHandle,&CfgFileHandle,ConfigFileName,EFI_FILE_MODE_READ,EFI_FILE_DIRECTORY);
			if(EFI_ERROR (Status)){
				Print(L"Can't find cfg file 'isoboot.cfg' in current dir\n");
				return NULL;
				}
			//�������ļ�	
			ConfigFileLine=FileHandleReturnLine(CfgFileHandle	,&IsAscii);
			//����Ҫ����ASCII��UNICODE��ת��
			MyTempFileName=ConfigFileLine;
			Print(L"Iso file in cfg is:%s\n",MyTempFileName);
			//��б��L'/'�滻Ϊ��б��L'\\'
			for(UINTN i=0;i<StrLen(MyTempFileName);i++){
				if(MyTempFileName[i]==L'/')MyTempFileName[i]=L'\\';
				}				
			if(MyTempFileName[0]!=L'\\'){
				Status=CurrDirHandle->Open(CurrDirHandle,&OutPutIsoFileHandle,MyTempFileName,EFI_FILE_MODE_READ,EFI_FILE_DIRECTORY);
				if(EFI_ERROR (Status)){
					return NULL;
					}else{
						return OutPutIsoFileHandle;
						}
				}
			//�������·��	
			Status=gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid,&CurrDirDP,&CurrDeviceHandle);
			//ͨ���豸������ҵ����ļ�ϵͳЭ��
			Status=gBS->HandleProtocol(CurrDeviceHandle,&gEfiSimpleFileSystemProtocolGuid,(VOID**)&DidoTempProtocol);
			if(EFI_ERROR (Status)){
				Print(L"SimpleFileSystemProtocol not found.Error=[%r]\n",Status);
				return NULL;
				}					
			//�򿪸���
			Status=DidoTempProtocol->OpenVolume(DidoTempProtocol,&DidoVolumeHandle);
			if(EFI_ERROR (Status)){
				Print(L"OpenCurrVolume failed.Error=[%r]\n",Status);
				return NULL;
				}
			
			
			//�򿪾���·��ָ����iso�ļ�
			
			Status=DidoVolumeHandle->Open(DidoVolumeHandle,&DidoFileHandle,MyTempFileName,EFI_FILE_MODE_READ,EFI_FILE_READ_ONLY);
			if(EFI_ERROR (Status)){
				Print(L"Config File Setting Is Wrong.Error=[%r]\n",Status);
				//return EFI_SUCCESS;
				//continue;
				return NULL;
				}
			//����ļ��ǲ���Ŀ¼
			Status=FileHandleIsDirectory(DidoFileHandle);
			if(EFI_SUCCESS==Status){
				Print(L"Not a file.Error=[%r]\n",Status);
				FileHandleClose(DidoFileHandle);
				return NULL;
				}	
			return 	DidoFileHandle;
			
	

		}