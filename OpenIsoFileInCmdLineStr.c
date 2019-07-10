#include "MyRamDisk.h"
///��������ָ����iso�ļ������ؾ��,�Լ���ʱ���Ƿ������ڴ�Ĳ���
EFI_FILE_HANDLE
	OpenIsoFileInCmdLineStr(
		DIDO_CMDLINE_STATUS					*CmdLineStatus,
		EFI_FILE_HANDLE						CurrDirHandle
	)
	{
		EFI_STATUS							Status;
		EFI_LOADED_IMAGE_PROTOCOL			*ThisFileLIP;
		CHAR16								**Argv;
		UINTN								Argc=0;
		UINTN								StrStartPos=0;
		CHAR16								*IsoFileName;
		BOOLEAN 							FoundIsoFile=FALSE;
		EFI_FILE_PROTOCOL	 				*DidoVolumeHandle=NULL,*DidoFileHandle=NULL;
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL 	*DidoTempProtocol;		
		//���Լ���ӳ��DP��Ϣ
		Status=gBS->HandleProtocol(gImageHandle,&gEfiLoadedImageProtocolGuid,(VOID**)&ThisFileLIP);
		if(EFI_ERROR (Status)){
			Print(L"LoadedImageProtocol not found.Error=[%r]\n",Status);
			return NULL;
			}
		//�ֽ������У���ɺ������main����һ��ʹ��������
		Argv=AllocatePool(ThisFileLIP->LoadOptionsSize);
		for(UINTN	i=0;i<ThisFileLIP->LoadOptionsSize/2;i++){
			
//			Print(L"%c   %X\n",((CHAR16*)ThisFileLIP->LoadOptions)[i],((CHAR16*)ThisFileLIP->LoadOptions)[i]);
			//�ҵ��ָ���ǰһ��
			if(i==ThisFileLIP->LoadOptionsSize/2-1||
				((CHAR16*)ThisFileLIP->LoadOptions)[i+1]==L' '||
				((CHAR16*)ThisFileLIP->LoadOptions)[i+1]==L'\0'
				){
				Argv[Argc]=AllocatePool((i+2-StrStartPos)*2);
				for(UINTN j=0;j<i+1-StrStartPos;j++){
					Argv[Argc][j]=((CHAR16*)ThisFileLIP->LoadOptions)[j+StrStartPos];
					if(j==i+1-StrStartPos-1){
						Argv[Argc][j+1]=L'\0';
						}
					}
				//�ų��������ķָ���	
				while(i<ThisFileLIP->LoadOptionsSize/2-1&&
					(((CHAR16*)ThisFileLIP->LoadOptions)[i+1]==L' '||
					 ((CHAR16*)ThisFileLIP->LoadOptions)[i+1]==L'\0')
					){
					i=i+1;
					}						
				
				Print(L"cmdline is %s\n",Argv[Argc]);
				Argc=Argc+1;
				StrStartPos=i+1;
				Print(L"Argc is %d\n",Argc);
				}
			
			}
		//���������е�-mem -wait���ļ�������

		Print(L"Check cmdline\n");	
		for(UINTN i=0;i<Argc;i++){
			UINTN StringLenth=StrnLenS(Argv[i],ThisFileLIP->LoadOptionsSize/2);
			if(	StringLenth>=5&&
				(Argv[i][StringLenth-4]==L'.'||Argv[i][StringLenth-4]==L'.')&&
				(Argv[i][StringLenth-3]==L'i'||Argv[i][StringLenth-3]==L'I')&&
				(Argv[i][StringLenth-2]==L's'||Argv[i][StringLenth-2]==L'S')&&
				(Argv[i][StringLenth-1]==L'o'||Argv[i][StringLenth-1]==L'O')){
				FoundIsoFile=TRUE;
				IsoFileName=Argv[i];
				}
			if(	StringLenth>=4&&
				(NULL!=StrStr(Argv[i],L"-mem")||NULL!=StrStr(Argv[i],L"-MEM"))){
				CmdLineStatus->LoadIsoInMemory=TRUE;
				}
			if(	StringLenth>=5&&
				(NULL!=StrStr(Argv[i],L"-wait")||NULL!=StrStr(Argv[i],L"-WAIT"))){
				CHAR16					*TempStr;
				TempStr=AllocateZeroPool(StrSize(Argv[i+1])+2);
				TempStr[0]=L' ';
				StrCatS(TempStr, StrSize(Argv[i+1])+2,Argv[i+1]);	
				CmdLineStatus->WaitTimeSec=StrDecimalToUintn(TempStr);
				FreePool(TempStr);
				}
			if(	StringLenth>=6&&
				(NULL!=StrStr(Argv[i],L"-debug")||NULL!=StrStr(Argv[i],L"-DEBUG"))){
				CmdLineStatus->DebugDropToShell=TRUE;
				}
				
			}

		//��������iso�ļ�����	
		if(FALSE==FoundIsoFile){
			Print(L"CmdLine is wrong\n");	
			return NULL;
			}
		Print(L"Iso file in cmdline is:%s\n",IsoFileName);
		//��б��L'/'�滻Ϊ��б��L'\\'
		for(UINTN i=0;i<StrLen(IsoFileName);i++){
			if(IsoFileName[i]==L'/')IsoFileName[i]=L'\\';
			}			
		
		if(IsoFileName[0]!=L'\\'){
			Status=CurrDirHandle->Open(CurrDirHandle,&DidoFileHandle,IsoFileName,EFI_FILE_MODE_READ,EFI_FILE_DIRECTORY);
			if(EFI_ERROR (Status)){
				Print(L"Open isofile fail\n");
				return NULL;
				}else{
					Print(L"Open isofile success\n");
					return DidoFileHandle;
					}
			}
		//�������·��	

		Status=gBS->HandleProtocol(ThisFileLIP->DeviceHandle,&gEfiSimpleFileSystemProtocolGuid,(VOID**)&DidoTempProtocol);
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
		
		
		//�򿪾���·��ָ����iso�ļ�
		
		Status=DidoVolumeHandle->Open(DidoVolumeHandle,&DidoFileHandle,IsoFileName,EFI_FILE_MODE_READ,EFI_FILE_READ_ONLY);
		if(EFI_ERROR (Status)){
			Print(L"CmdLine is wrong.Error=[%r]\n",Status);
			//return EFI_SUCCESS;
			//continue;
			return NULL;
			}
		//����ļ��ǲ���Ŀ¼
		Status=FileHandleIsDirectory(DidoFileHandle);
		if(EFI_SUCCESS==Status){
			Print(L"Not a file.Error=[%r]\n",Status);
			FileHandleClose(DidoFileHandle);
			return NULL;
			}
		///���Ӵ���ȴ�ʱ����Ƿ������ڴ�Ĵ���
		
			
		return 	DidoFileHandle;
		
		
	}