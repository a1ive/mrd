/** @file 
	��UEFI�����°���iso�ļ�ֱ��������Ҳ֧��ixpe����

**/
#include "MyRamDisk.h"

///ȫ��˽������ָ��,���������ַ���
DIDO_DISK_PRIVATE_DATA 			*pridata;

///ȫ�������в���ָ��
DIDO_OPTION_STATUS				*OptionStatus;

///������ڣ���Ϊ��������ò�Ʋ�����main��Ϊ���
EFI_STATUS EFIAPI UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
		)
	{
        EFI_STATUS               		Status;
		EFI_FILE_HANDLE					BootFileHandleInRamDisk=NULL;
		EFI_DEVICE_PATH_PROTOCOL		*CurrDirDP;
		EFI_FILE_HANDLE					IsoFileHandle;
		EFI_FILE_HANDLE					CurrDirHandle;
		
		///��ʾ�汾��
		Print(L"imgboot version 47\n");
		///��ʼ�������в���״̬
		OptionStatus=AllocateZeroPool(sizeof(DIDO_OPTION_STATUS));
		OptionStatus->LoadInMemory=FALSE;
		OptionStatus->DebugDropToShell=FALSE;
		OptionStatus->ImageFileType=UNKNOWNTYPE;
		OptionStatus->ImageFileName=NULL;  
		OptionStatus->WaitTimeSec=0;
		OptionStatus->DevicePathToFindImage=NULL;
		OptionStatus->UseBuildInNtfsDriver=FALSE;

		///��ʼ��ȫ��ָ��pridata,����3��˽�����ݽṹ����һ�����������̣��ڶ���δʹ�ã�������������������
		pridata=AllocateZeroPool(3*sizeof(DIDO_DISK_PRIVATE_DATA)+8);

/*//����4g�߽�
{///��ȡ��ǰĿ¼
		CHAR8 *buffer;
		UINTN   sss=512;
		UINTN i;
		CurrDirDP=GetCurrDirDP(gImageHandle,L"ggg.vhd");
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
		buffer=AllocateZeroPool(512);	
		FileHandleSetPosition(CurrDirHandle,0x100000000);
		FileHandleRead(CurrDirHandle,&sss,buffer);
		for(i=0;i<505;i+=8)
			Print(L"%016llX   ",*(UINTN *)(buffer+i));
		if(NULL!=CurrDirHandle)return EFI_SUCCESS;
}
//���Խ���
*/

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
		Status=FileDiskInstall(IsoFileHandle);
		if(Status==EFI_NOT_FOUND) {
			Print(L"Sorry!Can't boot this file.\n");
			goto errordroptoshell;
			}
		
		///���������ϵ������ļ�
		if(pridata[2].VirDiskHandle!=NULL)
			BootFileHandleInRamDisk=LoadBootFileInVirtualDisk(pridata[2].VirDiskHandle);
		///���������ļ�
		if(BootFileHandleInRamDisk==NULL) {
			BootFileHandleInRamDisk=FindAndLoadBootFileInVirtualDisk(); 
			}
		if(BootFileHandleInRamDisk==NULL) {	
			Print(L"Sorry!Can't boot this file.\n");
			goto errordroptoshell;
			}
			
		///�����Կ���
		if(OptionStatus->DebugDropToShell==TRUE){
			Print(L"Debug drop to shell\n");
			goto errordroptoshell;
			}
		
		///���뽵�����м�����ܵȴ������ұ���ʵ������pe��logo����������
		gBS->RestoreTPL(TPL_APPLICATION);		
			
		///�ȴ�WaitTimeSec�룬���������м���application��������ʵ����������
		DidoWaitSec(OptionStatus->WaitTimeSec);
		
		///����
		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		Print(L"Image returned\n");
		return EFI_SUCCESS;
			
errordroptoshell:			
		///ʧ������һ��shellx64.efi
		Print(L"Loading shellx64.efi\n");
		CurrDirDP=GetCurrDirDP(gImageHandle,L"shellx64.efi");
		if(NULL==CurrDirDP){
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
			Print(L"Load shellx64.efi failed!\n");
			return EFI_SUCCESS;
			}
			
		///���뽵�����м�����ܵȴ������ұ���ʵ������pe��logo����������
		gBS->RestoreTPL(TPL_APPLICATION);
	
		///�ȴ�30��
		DidoWaitSec(OptionStatus->WaitTimeSec>30?OptionStatus->WaitTimeSec:30);	
		
		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		if(EFI_ERROR (Status)) {
			Print(L"Start shellx64.efi failed!\n");
			return EFI_SUCCESS;
			}			
/**/	return EFI_SUCCESS;
	}


