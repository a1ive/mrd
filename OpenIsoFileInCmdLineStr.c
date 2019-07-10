#include "MyRamDisk.h"
///��������ָ����iso�ļ������ؾ��
EFI_FILE_HANDLE
	OpenIsoFileInCmdLineStr(
//		IN	CHAR16*								CmdLine,
		EFI_FILE_HANDLE							CurrDirHandle
	)
	{
		EFI_STATUS							Status;
		EFI_LOADED_IMAGE_PROTOCOL			*ThisFileLIP;
		CHAR16								**Argv;
		UINTN								Argc=0;
		UINTN								StrStartPos=0;
		CHAR16								*IsoFileName;
		CHAR16 								*DidoFoundStr=NULL;
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
		Argv=AllocatePool(ThisFileLIP->LoadOptionsSize/2);
		for(UINTN	i=0;i<ThisFileLIP->LoadOptionsSize/2;i++){
//			Print(L"%c   %X\n",((CHAR16*)ThisFileLIP->LoadOptions)[i],((CHAR16*)ThisFileLIP->LoadOptions)[i]);
			if((((CHAR16*)ThisFileLIP->LoadOptions)[i]==L' '&&((CHAR16*)ThisFileLIP->LoadOptions)[i+1]!=L'\0')||(i+1)*2==ThisFileLIP->LoadOptionsSize){
				Argv[Argc]=AllocatePool((i+1-StrStartPos)*2);
				for(UINTN j=0;j<=i;j++){
					Argv[Argc][j]=((CHAR16*)ThisFileLIP->LoadOptions)[j+StrStartPos];
					if(j+StrStartPos==i){
						Argv[Argc][j]=L'\0';
						
						}
					}
				Print(L"cmdline is %s\n",Argv[Argc]);
				Argc=Argc+1;
				StrStartPos=i+1;
				Print(L"Argc is %d\n",Argc);
				}
			
			}
		for(UINTN i=0;i<Argc;i++){
			
			//�򵥵��жϣ���.iso��������Ϊ��iso�ļ�	
			DidoFoundStr=StrStr(Argv[i],L".iso");
			if(NULL!=DidoFoundStr){
				FoundIsoFile=TRUE;
				IsoFileName=Argv[i];
				break;
				}	
			}
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
		return 	DidoFileHandle;
		
		
	}