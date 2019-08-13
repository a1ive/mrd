/** ����NTFS����

**/
#include "ntfs.c"
#include "MyRamDisk.h"
EFI_STATUS
	LoadNtfsDriver(
	)
	{
        EFI_STATUS               		Status;
		EFI_DEVICE_PATH_PROTOCOL		*NtfsDriverFileDevicePath=NULL;
		EFI_HANDLE 						BootFileHandleInRamDisk;
		
		///ж�ػ�˶�����ntfs����
		EFI_HANDLE							NtfsDriverHandle=NULL;
		EFI_HANDLE							*Buffer;
		UINTN								BufferCount=0;
		UINTN								BufferIndex;
		EFI_COMPONENT_NAME2_PROTOCOL		*DriverNameProtocol;
		CHAR16								*DriverName=NULL;

		//����������ϵͳ������ntfs������Ȼ����ж�أ���Ϊ��˶�������ntfs���������Բ���		
		//�г����е�֧���������ֵľ��
		Status=gBS->LocateHandleBuffer(ByProtocol,&gEfiComponentName2ProtocolGuid,NULL,&BufferCount,&Buffer);
		if(EFI_ERROR (Status)){
			Print(L"ComponentNameProtocol not found.Error=[%r]\n",Status);
			}
		//ѭ���������е����о��	
		for(BufferIndex=0;BufferIndex<BufferCount;BufferIndex++){
			gBS->HandleProtocol(Buffer[BufferIndex],&gEfiComponentName2ProtocolGuid,(VOID**)&DriverNameProtocol);
			DriverNameProtocol->GetDriverName(DriverNameProtocol,"en-us",&DriverName);
			//�Ƿ�NTFS����
			//Print(L"%s       ",DriverName);
			if(NULL!=DriverName&&
				NULL!=StrStr(DriverName,L"AMI NTFS Driver")){
					
				NtfsDriverHandle=Buffer[BufferIndex];
				Print(L"Found AMI NTFS Driver\n");
				break;
				}
			}
		//�Ƿ��ҵ���NTFS����	
		if(NtfsDriverHandle!=NULL){
			Status=gBS->UnloadImage(NtfsDriverHandle);
			Print(L"Unload AMI NTFS Driver %r\n",Status);
			}else{
				Print(L"Can't find AMI NTFS Driver\n");
				}
				
					
		Print(L"Loading new NTFS driver\n");
		///��ȡ��ǰĿ¼
		NtfsDriverFileDevicePath=GetCurrDirDP(gImageHandle,L"ntfs.efi");
		if(NULL==NtfsDriverFileDevicePath){
			Print(L"Loading 'ntfs.efi' failed!\n");
			return EFI_NOT_FOUND;
			}



				
		Status=gBS->LoadImage(
			FALSE,
			gImageHandle,                   //parent����Ϊ�գ����뱾�ļ���Handle
			NtfsDriverFileDevicePath,						//�ļ���devicepath
			NULL,
			0,
			(VOID**)&BootFileHandleInRamDisk				//����HANDLE��ַ	
			);
		if(EFI_ERROR(Status)){
			Print(L"Loading 'ntfs.efi' failed!\n");
			return EFI_NOT_FOUND;
			}			

		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		if(EFI_ERROR (Status)) {
			Print(L"Start 'ntfs.efi' failed!\n");
			return EFI_NOT_FOUND;
			}
			
		return EFI_SUCCESS;		
	}			