/** ����NTFS����

**/
#include "ntfs.c"
#include "MyRamDisk.h"
EFI_STATUS
	LoadNtfsDriver(
	)
	{
        EFI_STATUS               		Status;
//		EFI_FILE_HANDLE					NtfsDriverHandle;
		EFI_HANDLE 						BootFileHandleInRamDisk;
		
		
		Print(L"Loading NTFS driver\n");
				
		Status=gBS->LoadImage(
			FALSE,
			gImageHandle,                   //parent����Ϊ�գ����뱾�ļ���Handle
			NULL,						//�ļ���devicepath
			(VOID*)acntfs,
			(UINTN)&endofntfsdriver-(UINTN)acntfs,
			(VOID**)&BootFileHandleInRamDisk				//����HANDLE��ַ	
			);
		if(EFI_ERROR(Status)){
			Print(L"Loading NTFS driver failed!\n");
			return EFI_NOT_FOUND;
			}			

		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		if(EFI_ERROR (Status)) {
			Print(L"Start NTFS driver failed!\n");
			return EFI_NOT_FOUND;
			}
			
		return EFI_SUCCESS;		
	}			