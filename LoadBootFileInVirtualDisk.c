#include "MyRamDisk.h"
///��ȡ���ָ����ISO�ļ�����ע���ramdisk��Ȼ������ramdisk�����bootx64.efi
EFI_STATUS
LoadBootFileInVirtualDisk(
	IN		EFI_DEVICE_PATH_PROTOCOL			*RegedRamDiskDevicePath,
	OUT		EFI_FILE_HANDLE						*BootMyFileHandle
	)
	{
		EFI_STATUS 							Status;

		UINTN 								BufferCount=0;
		INTN								BufferIndex=0;
		EFI_HANDLE 							*Buffer=NULL;
		EFI_HANDLE 							FoundDeviceHandle=NULL;
		EFI_DEVICE_PATH_PROTOCOL			*FoundDevicePathToCmp;
		CHAR16	                           	*TextRamDiskDevicePathToBoot=NULL;
		CHAR16								*TextFoundDevicePathToCmp=NULL;
		EFI_DEVICE_PATH_PROTOCOL			*BootFileDevicePath2;
		
		//��ʾ�����̵�DP
		TextRamDiskDevicePathToBoot=ConvertDevicePathToText(RegedRamDiskDevicePath, FALSE, FALSE); 
		Print(L"RamDisk DevicePath=%s\n", TextRamDiskDevicePathToBoot);

		
		
		//�г����еļ��ļ�ϵͳ�豸
		Status=gBS->LocateHandleBuffer(ByProtocol,&gEfiSimpleFileSystemProtocolGuid,NULL,&BufferCount,&Buffer);
		if(EFI_ERROR (Status)){
			Print(L"SimpleFileSystemProtocol not found.Error=[%r]\n",Status);
			return Status;
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
			
			return EFI_LOAD_ERROR;
			}

		Print(L"Handle selected %d\n",BufferIndex);
		Print(L"Partition DevicePath=%s\n", TextFoundDevicePathToCmp);
		if(TextFoundDevicePathToCmp)gBS->FreePool(TextFoundDevicePathToCmp);
			
		///�����ڴ澵���е�bootx64.efi
		//��װ�����ļ�/EFI/BOOT/BOOTX64.EFI��devicepath
		BootFileDevicePath2=FileDevicePath(FoundDeviceHandle,EFI_REMOVABLE_MEDIA_FILE_NAME);
		TextFoundDevicePathToCmp=ConvertDevicePathToText(BootFileDevicePath2, FALSE, FALSE);
		Print(L"Boot file path is:%s\n",TextFoundDevicePathToCmp);
		Status=gBS->LoadImage(
			FALSE,
			gImageHandle,                   //parent����Ϊ�գ����뱾�ļ���Handle
			BootFileDevicePath2,			//�ļ���devicepath
			NULL,
			0,
			(VOID**)BootMyFileHandle				//����HANDLE��ַ	
			);				
		if(EFI_ERROR (Status)) {
			Print(L"Loadimage failed! Error=[%r]\n",Status);
			return Status;
			}
	return EFI_SUCCESS;
	}