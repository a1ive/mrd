#include "MyRamDisk.h"

//һ��ͨ�õĴ��ļ��ĺ���
EFI_FILE_HANDLE
	OpenFileInDevice(
		EFI_HANDLE			DeviceHandle,
		CHAR16				*AbsFileName
	)
	{
		EFI_STATUS							Status;
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL 	*DidoTempProtocol;
		EFI_FILE_PROTOCOL	 				*DidoVolumeHandle=NULL;
		EFI_FILE_PROTOCOL					*DidoFileHandle=NULL;
	
		Status=gBS->HandleProtocol(DeviceHandle,&gEfiSimpleFileSystemProtocolGuid,(VOID**)&DidoTempProtocol);
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
		//�򿪾���·��ָ���ľ����ļ�
		Status=DidoVolumeHandle->Open(DidoVolumeHandle,&DidoFileHandle,AbsFileName,EFI_FILE_MODE_READ,EFI_FILE_READ_ONLY);
		if(EFI_ERROR (Status)){
			Print(L"Open image file failed.Error=[%r]\n",Status);
			return NULL;
			}
			
		//����ļ��ǲ���Ŀ¼
		Status=FileHandleIsDirectory(DidoFileHandle);
		if(EFI_SUCCESS==Status){
			Print(L"Not a file.Error=[%r]\n",Status);
			FileHandleClose(DidoFileHandle);
			return NULL;
			}
		return DidoFileHandle;
	
	}



///��������ָ����iso�ļ������ؾ��,�Լ���ʱ���Ƿ������ڴ�Ĳ���
EFI_FILE_HANDLE
	OpenFileInOptionStatus(
		DIDO_OPTION_STATUS					*OptionStatus,
		EFI_FILE_HANDLE						CurrDirHandle
	)
	{
		EFI_STATUS							Status;
		EFI_LOADED_IMAGE_PROTOCOL			*ThisFileLIP;
		CHAR16								*IsoFileName=NULL;
		EFI_FILE_PROTOCOL	 				*DidoFileHandle=NULL;
		UINTN i;	
		EFI_DEVICE_PATH_PROTOCOL			*DevDevicePath;
		EFI_HANDLE							DevHandle;
		CHAR16 								*AbsFileName=NULL;

		//���������ļ�����	
		if(NULL==OptionStatus->ImageFileName){
			Print(L"OptionStatus didn't have a file name\n");	
			return NULL;
			}
		Print(L"File name in OptionStatus is:%s\n",OptionStatus->ImageFileName);
		//���ļ����ַ������Ƶ���ʱ����
		IsoFileName=AllocateCopyPool(StrnSizeS(OptionStatus->ImageFileName,MAX_FILE_NAME_STRING_SIZE),OptionStatus->ImageFileName);
		//��б��L'/'�滻Ϊ��б��L'\\'
		for(i=0;i<StrLen(IsoFileName);i++){
			if(IsoFileName[i]==L'/')IsoFileName[i]=L'\\';
			}				
		//�õ�����·��
		AbsFileName=AllocateZeroPool(StrnSizeS(OptionStatus->ImageFileName,MAX_FILE_NAME_STRING_SIZE)+2);
		if(IsoFileName[0]!=L'\\')
			AbsFileName[0]=L'\\';
		StrCatS(AbsFileName,StrnSizeS(OptionStatus->ImageFileName,MAX_FILE_NAME_STRING_SIZE)+2,IsoFileName);
		
	
		///δָ���豸
		if(NULL==OptionStatus->DevicePathToFindImage){
			///���·��
			if(IsoFileName[0]!=L'\\'){
				//�򿪾����ļ�
				Status=CurrDirHandle->Open(CurrDirHandle,&DidoFileHandle,IsoFileName,EFI_FILE_MODE_READ,EFI_FILE_DIRECTORY);
				if(EFI_ERROR (Status)){
					Print(L"Open image file fail\n");
					return NULL;
					}
				Print(L"Open image file success\n");
				return DidoFileHandle;
				}
			
			///�������·��				
			if(IsoFileName[0]==L'\\'){
				//���Լ���ӳ��DP��Ϣ
				Status=gBS->HandleProtocol(gImageHandle,&gEfiLoadedImageProtocolGuid,(VOID**)&ThisFileLIP);
				if(EFI_ERROR (Status)){
					Print(L"LoadedImageProtocol not found.Error=[%r]\n",Status);
					return NULL;
					}
				//�򿪾����ļ�	
				DidoFileHandle=OpenFileInDevice(ThisFileLIP->DeviceHandle,IsoFileName);
				if(DidoFileHandle==NULL)
					Print(L"Open image file fail\n");
				if(DidoFileHandle!=NULL)
					Print(L"Open image file success\n");				
				return 	DidoFileHandle;
				}			
			
			}
		
					
		///�������ļ�ϵͳ�в���ָ���ľ���
		if(0==StrCmp(OptionStatus->DevicePathToFindImage,L"auto")||0==StrCmp(OptionStatus->DevicePathToFindImage,L"AUTO")){
			///�Զ���������
			UINTN							BufferIndex;
			UINTN							BufferCount;
			EFI_HANDLE 						*Buffer=NULL;
			
			//�г�����disk�豸
			Status=gBS->LocateHandleBuffer(ByProtocol,&gEfiDiskIoProtocolGuid,NULL,&BufferCount,&Buffer);
			if(EFI_ERROR (Status)){
				Print(L"DiskIo Protocol not found.Error=[%r]\n",Status);
				return NULL;
				}
			for(BufferIndex=0;BufferIndex<BufferCount;BufferIndex++){
				//Ϊÿ�������豸��װ����
				gBS->ConnectController (Buffer[BufferIndex], NULL, NULL, TRUE);				
				}
				
			//�г����еļ��ļ�ϵͳ�豸
			Status=gBS->LocateHandleBuffer(ByProtocol,&gEfiSimpleFileSystemProtocolGuid,NULL,&BufferCount,&Buffer);
			if(EFI_ERROR (Status)){
				Print(L"SimpleFileSystem Protocol not found.Error=[%r]\n",Status);
				return NULL;
				}
			Print(L"Device handles found %d\n",BufferCount);	
			for(BufferIndex=0;BufferIndex<BufferCount;BufferIndex++){
				//�򿪾����ļ�
				DidoFileHandle=OpenFileInDevice(Buffer[BufferIndex],AbsFileName);
				if(DidoFileHandle!=NULL){
					Print(L"Device handles selected %d\n",BufferIndex+1);
					break;
					}
				}
			if(DidoFileHandle==NULL)
				Print(L"Handle selected none\n");
//			if(DidoFileHandle!=NULL)
//				Print(L"Open image file success\n");				
			return DidoFileHandle;	
			}
		
		///ָ���豸�������
		Print(L"DevDevicePath is:%s\n",OptionStatus->DevicePathToFindImage);
		DevDevicePath=ConvertTextToDevicePath(OptionStatus->DevicePathToFindImage);
		Status=gBS->LocateDevicePath(&gEfiDiskIoProtocolGuid,&DevDevicePath,&DevHandle);
		gBS->ConnectController (DevHandle, NULL, NULL, TRUE);
		//�򿪾����ļ�
		DidoFileHandle=OpenFileInDevice(DevHandle,AbsFileName);
		//����ļ����Ƿ�����
		if(DidoFileHandle==NULL)
			Print(L"Open image file fail\n");
		if(DidoFileHandle!=NULL)
			Print(L"Open image file success\n");		
		return DidoFileHandle;
	}
	
	