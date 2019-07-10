#include "MyRamDisk.h"
///������������Ƿ�ɹ���װ

EFI_HANDLE
	FindBootPartitionHandle()
	{
		EFI_STATUS							Status;
		UINTN 								BufferCount=0;
		INTN								BufferIndex=0;
		EFI_HANDLE 							*Buffer=NULL;
		EFI_HANDLE 							FoundDeviceHandle=NULL;
		EFI_DEVICE_PATH_PROTOCOL			*FoundDevicePathToCmp;
		CHAR16								*TextFoundDevicePathToCmp=NULL;
		///����MyGuid���Լ��Ƿ���SFS,�Լ���ȷ�ķ���dp,��ȷ��ǰ�����ɵ�
		
		//�г����еļ��ļ�ϵͳ�豸
		Status=gBS->LocateHandleBuffer(ByProtocol,&gEfiSimpleFileSystemProtocolGuid,NULL,&BufferCount,&Buffer);
		if(EFI_ERROR (Status)){
			Print(L"SimpleFileSystemProtocol not found.Error=[%r]\n",Status);
			return NULL;
			}
		Print(L"Handles found %d\n",BufferCount);	
		for(BufferIndex=BufferCount-1;BufferIndex>=0;BufferIndex--){
			//�����ת����DevicePath
			if(TextFoundDevicePathToCmp)gBS->FreePool(TextFoundDevicePathToCmp);
			FoundDevicePathToCmp = DevicePathFromHandle  ( Buffer[BufferIndex]  );
			TextFoundDevicePathToCmp=ConvertDevicePathToText(FoundDevicePathToCmp,FALSE,FALSE);
			//���Ը��ڵ㣬��ͬ����һ��
			if(((VENDOR_DEVICE_PATH*)FoundDevicePathToCmp)->Header.Type!=HARDWARE_DEVICE_PATH||
				((VENDOR_DEVICE_PATH*)FoundDevicePathToCmp)->Header.SubType!=HW_VENDOR_DP||
				!CompareGuid(&((VENDOR_DEVICE_PATH*)FoundDevicePathToCmp)->Guid,&MyGuid))continue;
			//�����ӽڵ㣬��ͬ����һ��	
			FoundDevicePathToCmp=NextDevicePathNode(FoundDevicePathToCmp);	
			if(((CDROM_DEVICE_PATH*)FoundDevicePathToCmp)->Header.Type!=MEDIA_DEVICE_PATH||
				((CDROM_DEVICE_PATH*)FoundDevicePathToCmp)->Header.SubType!=MEDIA_CDROM_DP||
				((CDROM_DEVICE_PATH*)FoundDevicePathToCmp)->BootEntry!=1)continue;
			FoundDeviceHandle=Buffer[BufferIndex];
			break;
			}
		//û�ҵ����ش���	
		if(NULL==FoundDeviceHandle){
			if(TextFoundDevicePathToCmp)gBS->FreePool(TextFoundDevicePathToCmp);
			Print(L"Handle selected none\n");
			return NULL;
			}
		Print(L"Handle selected %d\n",BufferIndex);
		Print(L"Partition DevicePath=%s\n", TextFoundDevicePathToCmp);
		if(TextFoundDevicePathToCmp)gBS->FreePool(TextFoundDevicePathToCmp);
		return FoundDeviceHandle;
	}