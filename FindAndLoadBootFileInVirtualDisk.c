#include "MyRamDisk.h"
///������������Ƿ�ɹ���װ

EFI_HANDLE
	FindAndLoadBootFileInVirtualDisk()
	{
		EFI_STATUS							Status;
		UINTN 								BufferCount=0;
		UINTN								BufferIndex=0;
		EFI_HANDLE 							*Buffer=NULL;
		EFI_DEVICE_PATH_PROTOCOL			*FoundDevicePathToCmp;
		EFI_DEVICE_PATH_PROTOCOL			*BootFileDevicePath2;
		EFI_HANDLE							BootImageHandle=NULL;		
		CHAR16								*TextFoundDevicePathToCmp=NULL;
		///����MyGuid���Լ��Ƿ���efi/boot/bootx64.efi,��ȷ��
		
		//�г����еļ��ļ�ϵͳ�豸
		Status=gBS->LocateHandleBuffer(ByProtocol,&gEfiSimpleFileSystemProtocolGuid,NULL,&BufferCount,&Buffer);
		if(EFI_ERROR (Status)){
			Print(L"SimpleFileSystemProtocol not found.Error=[%r]\n",Status);
			return NULL;
			}
		Print(L"Handles found %d\n",BufferCount);	
		for(BufferIndex=0;BufferIndex<BufferCount;BufferIndex++){
			//�����ת����DevicePath
			
			FoundDevicePathToCmp = DevicePathFromHandle  ( Buffer[BufferIndex]  );
			
			//���Ը��ڵ㣬��ͬ����һ��
			if(((VENDOR_DEVICE_PATH*)FoundDevicePathToCmp)->Header.Type!=HARDWARE_DEVICE_PATH||
				((VENDOR_DEVICE_PATH*)FoundDevicePathToCmp)->Header.SubType!=HW_VENDOR_DP||
				!CompareGuid(&((VENDOR_DEVICE_PATH*)FoundDevicePathToCmp)->Guid,&MyGuid))continue;
			/*������Ҫ�����ӽڵ�
			//�����ӽڵ㣬��ͬ����һ��	
			FoundDevicePathToCmp=NextDevicePathNode(FoundDevicePathToCmp);	
			if(
				(((CDROM_DEVICE_PATH*)FoundDevicePathToCmp)->Header.Type!=MEDIA_DEVICE_PATH||
				((CDROM_DEVICE_PATH*)FoundDevicePathToCmp)->Header.SubType!=MEDIA_CDROM_DP||
				((CDROM_DEVICE_PATH*)FoundDevicePathToCmp)->BootEntry!=1)
				&&
				(((HARDDRIVE_DEVICE_PATH*)FoundDevicePathToCmp)->Header.Type!=MEDIA_DEVICE_PATH||
				((HARDDRIVE_DEVICE_PATH*)FoundDevicePathToCmp)->Header.SubType!=MEDIA_HARDDRIVE_DP)
				)
				continue;
			*/	
			
			///�����ڴ澵���е�bootx64.efi
			//��װ�����ļ�/EFI/BOOT/BOOTX64.EFI��devicepath
			BootFileDevicePath2=FileDevicePath(Buffer[BufferIndex],EFI_REMOVABLE_MEDIA_FILE_NAME);

			Status=gBS->LoadImage(
				TRUE,
				gImageHandle,                   //parent����Ϊ�գ����뱾�ļ���Handle
				BootFileDevicePath2,			//�ļ���devicepath
				NULL,
				0,
				(VOID**)&BootImageHandle				//����HANDLE��ַ	
				);							
			if(EFI_ERROR(Status)){
				continue;
				}
			
			break;
			}
			
		//û�ҵ����ش���	
		if(NULL==BootImageHandle){
			Print(L"Handle selected none\n");
			return NULL;
			}
		//��ʾ�ҵ�����Ϣ	
		Print(L"Handle selected %d\n",BufferIndex);
		TextFoundDevicePathToCmp=ConvertDevicePathToText(BootFileDevicePath2, FALSE, FALSE);
		Print(L"Boot file path is:%s\n",TextFoundDevicePathToCmp);
		if(NULL!=TextFoundDevicePathToCmp)FreePool(TextFoundDevicePathToCmp);
	
		return BootImageHandle;
		
		
		
	}