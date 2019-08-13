#include "MyRamDisk.h"
///�򿪲���ָ�����ļ���Ŀ¼�����ؾ��	
EFI_FILE_HANDLE
	OpenFileByDevicePath(
		IN 		EFI_DEVICE_PATH_PROTOCOL 			*CurrDevicePath			//��ǰ�豸���豸·��
		)
		{
			EFI_STATUS								Status;
			EFI_DEVICE_PATH_PROTOCOL				*NextNodeDP;	
			EFI_HANDLE 								CurrDeviceHandle;		//��ǰ�豸�ľ��			
			EFI_SIMPLE_FILE_SYSTEM_PROTOCOL 		*DidoTempProtocol;
			EFI_FILE_PROTOCOL 						*DidoVolumeHandle=NULL,*DidoDirHandle,*DidoFileHandle=NULL;

			//�ֽ�DP
			Status=gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid,&CurrDevicePath,&CurrDeviceHandle);
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
			//�򿪵�һ�ڵ�Ŀ¼
			
			Status=DidoVolumeHandle->Open(DidoVolumeHandle,&DidoDirHandle,((FILEPATH_DEVICE_PATH*)CurrDevicePath)->PathName,EFI_FILE_MODE_READ,EFI_FILE_DIRECTORY);
			if(EFI_ERROR (Status)){
				Print(L"Open CurrDir1 failed.Error=[%r]\n",Status);
				return DidoVolumeHandle;
				}
			//���Եڶ��ڵ�	
			NextNodeDP=NextDevicePathNode(CurrDevicePath);	
			if(MEDIA_DEVICE_PATH!=NextNodeDP->Type||
				MEDIA_FILEPATH_DP!=NextNodeDP->SubType||
				0==StrLen(((FILEPATH_DEVICE_PATH*)NextNodeDP)->PathName)){
				Print(L"Node2 not open\n");	
				return 	DidoDirHandle;
				}
			//�򿪵ڶ��ڵ�Ŀ¼�����ļ�
			Status=DidoDirHandle->Open(DidoDirHandle,&DidoFileHandle,((FILEPATH_DEVICE_PATH*)NextNodeDP)->PathName,EFI_FILE_MODE_READ,EFI_FILE_DIRECTORY);
			if(EFI_ERROR (Status)){
				Print(L"Open Node2 failed.Error=[%r]\n",Status);
				return DidoDirHandle;
				}
			return	DidoFileHandle;
	
			
		}