#include "MyRamDisk.h"
///������ָ�����豸(�ɹ����ϵ�bootimage����)�е�/efi/boot/bootx64.efi�������������ļ��ľ��
EFI_STATUS
LoadBootFileInVirtualDisk(
	IN		EFI_HANDLE							*BootImagePartitionHandle, 
	OUT		EFI_FILE_HANDLE						*BootMyFileHandle
	)
	{
		EFI_STATUS 							Status;


		CHAR16								*TextFoundDevicePathToCmp=NULL;
		EFI_DEVICE_PATH_PROTOCOL			*BootFileDevicePath2;
		
		if(BootImagePartitionHandle==NULL){
			BootImagePartitionHandle=FindBootPartitionHandle();
			}
		
				
		///�����ڴ澵���е�bootx64.efi
		//��װ�����ļ�/EFI/BOOT/BOOTX64.EFI��devicepath
		BootFileDevicePath2=FileDevicePath(BootImagePartitionHandle,EFI_REMOVABLE_MEDIA_FILE_NAME);
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