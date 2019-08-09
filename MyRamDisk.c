/** @file 
	��UEFI�����°���iso�ļ�ֱ��������Ҳ֧��ixpe����

**/
#include "MyRamDisk.h"
///ȫ��˽������ָ��,���������ַ���
DIDO_DISK_PRIVATE_DATA *pridata;

///������ڣ���Ϊ��������ò�Ʋ�����main��Ϊ���
EFI_STATUS EFIAPI UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
		)
	{
        EFI_STATUS               		Status;
		DIDO_OPTION_STATUS				*OptionStatus;
		EFI_FILE_HANDLE					BootFileHandleInRamDisk;
		EFI_DEVICE_PATH_PROTOCOL		*CurrDirDP;
		EFI_FILE_HANDLE					IsoFileHandle;
		EFI_FILE_HANDLE					CurrDirHandle;
		
	
		///��ʼ�������в���״̬
		OptionStatus=AllocateZeroPool(sizeof(DIDO_OPTION_STATUS));
		OptionStatus->LoadInMemory=FALSE;
		OptionStatus->DebugDropToShell=FALSE;
		OptionStatus->ImageFileType=HARDDISKFILE;
		OptionStatus->ImageFileName=NULL;  
		OptionStatus->WaitTimeSec=0;
		OptionStatus->DevicePathToFindImage=NULL;
		OptionStatus->UseBuildInNtfsDriver=FALSE;
		///��ȡ��ǰĿ¼
		CurrDirDP=GetCurrDirDP(gImageHandle,L"");
		if(NULL==CurrDirDP){
			Print(L"GetCurrDirDP Error\n");
			return EFI_SUCCESS;
			}
			
		///�򿪵�ǰĿ¼	
		CurrDirHandle=OpenFileByDevicePath(CurrDirDP);
		if(NULL==CurrDirHandle){
			Print(L"Open CurrDir Error\n");
			return EFI_SUCCESS;
			}
			
		///���������в���
		Status=ProcCmdLine(OptionStatus);
		if(NULL==OptionStatus->ImageFileName){	
			///���������ļ�
			Status=ProcCfgFile(OptionStatus,CurrDirHandle,L"imgboot.cfg");
			}
			
		///����ntfs����
		if(OptionStatus->UseBuildInNtfsDriver)
			Status=LoadNtfsDriver();
			
		///��ָ�������ļ�		
		IsoFileHandle=OpenFileInOptionStatus(OptionStatus,CurrDirHandle);	
		if(NULL==IsoFileHandle){
			//�ڵ�ǰĿ¼������һ��iso�ļ�����
			OptionStatus->ImageFileType=ISOFILE;
			IsoFileHandle=OpenFirstIsoFileInDir(CurrDirHandle);
			if(NULL==IsoFileHandle){
				Print(L"Open ISO file Error\n");
				goto errordroptoshell;
				}
			}
			
		///��װ������
		if(OptionStatus->ImageFileType==ISOFILE)	
			Status=IsoFileDiskInstall(IsoFileHandle,OptionStatus->LoadInMemory);
		if(OptionStatus->ImageFileType==HARDDISKFILE)	
			Status=HdFileDiskInstall(IsoFileHandle,OptionStatus->LoadInMemory);	
		if(Status==EFI_NOT_FOUND) {
			Print(L"Sorry!Can't boot this file.\n");
			goto errordroptoshell;
			}

		///���������ϵ������ļ�
		Status=LoadBootFileInVirtualDisk(pridata[2].VirDiskHandle,&BootFileHandleInRamDisk);
		if(EFI_ERROR (Status)) {
			Status=LoadBootFileInVirtualDisk(NULL,&BootFileHandleInRamDisk); //����
			}
		if(EFI_ERROR (Status)) {	
			Print(L"Sorry!Can't boot this iso file.\n");
			goto errordroptoshell;
			}
			
		///�����Կ���
		if(OptionStatus->DebugDropToShell==TRUE)goto errordroptoshell;
		///�ȴ�WaitTimeSec�룬���������м���application��������ʵ����������
		DidoWaitSec(OptionStatus->WaitTimeSec);
		
		///����
		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		if(EFI_ERROR (Status)) {
			Print(L"Startimage failed!\n");
			goto errordroptoshell;
			}
			
errordroptoshell:			
		///ʧ������һ��shellx64.efi
		Print(L"Debug drop to shell\n");
		CurrDirDP=GetCurrDirDP(gImageHandle,L"shellx64.efi");
		if(NULL==CurrDirDP){
			Print(L"Start shellx64.efi failed!\n");
			return EFI_SUCCESS;
			}		
		Status=gBS->LoadImage(
			FALSE,
			gImageHandle,                   //parent����Ϊ�գ����뱾�ļ���Handle
			CurrDirDP,			//�ļ���devicepath
			NULL,
			0,
			(VOID**)&BootFileHandleInRamDisk				//����HANDLE��ַ	
			);
		if(EFI_ERROR(Status)){
			Print(L"Start shellx64.efi failed!\n");
			return EFI_SUCCESS;
			}			
		///�ȴ�30��
//		DidoWaitSec(OptionStatus->WaitTimeSec>30?OptionStatus->WaitTimeSec:30);	
		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		if(EFI_ERROR (Status)) {
			Print(L"Start shellx64.efi failed!\n");
			return EFI_SUCCESS;
			}			
/**/	return EFI_SUCCESS;
	}


