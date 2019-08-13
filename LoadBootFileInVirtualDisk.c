#include "MyRamDisk.h"
///������ָ�����豸(�ɹ����ϵ�bootimage����)�е�/efi/boot/bootx64.efi�������������ļ��ľ��
EFI_HANDLE
	LoadBootFileInVirtualDisk(
		IN		EFI_HANDLE							*BootImagePartitionHandle 
		)
		{
			EFI_STATUS 							Status;
			EFI_HANDLE							BootImageHandle;
			
			CHAR16								*TextFoundDevicePathToCmp=NULL;
			EFI_DEVICE_PATH_PROTOCOL			*BootFileDevicePath2;
			

			if(BootImagePartitionHandle==NULL){
				return NULL;
				}
					
			///�����ڴ澵���е�bootx64.efi
			//��װ�����ļ�/EFI/BOOT/BOOTX64.EFI��devicepath
			BootFileDevicePath2=FileDevicePath(BootImagePartitionHandle,EFI_REMOVABLE_MEDIA_FILE_NAME);

			Status=gBS->LoadImage(
				TRUE,
				gImageHandle,                   //parent����Ϊ�գ����뱾�ļ���Handle
				BootFileDevicePath2,			//�ļ���devicepath
				NULL,
				0,
				(VOID**)&BootImageHandle				//����HANDLE��ַ	
				);				
			TextFoundDevicePathToCmp=ConvertDevicePathToText(BootFileDevicePath2, FALSE, FALSE);
			Print(L"Boot file path is:%s\n",TextFoundDevicePathToCmp);
			if(NULL!=TextFoundDevicePathToCmp)FreePool(TextFoundDevicePathToCmp);
			if(EFI_ERROR (Status)) {
				Print(L"Loadimage failed! Error=[%r]\n",Status);
				return NULL;
				}
				
			return BootImageHandle;
		}