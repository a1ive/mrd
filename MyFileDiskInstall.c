//�����ڴ��̵�Э�鰲װ

#include "MyRamDisk.h"






EFI_STATUS
	MyFileDiskInstall(
		IN 	EFI_FILE_HANDLE 	FileDiskFileHandle,
		IN	BOOLEAN				DiskInRam
	)
	{
		EFI_STATUS 							Status=EFI_SUCCESS;
		EFI_DEVICE_PATH_PROTOCOL 			*TempPath1;
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL		*PartitionSFSProtocol=NULL;				//���ڲ����Ƿ��Ѱ�װfatЭ��,û����
		//��ʼ��ȫ��ָ��pridata,����3��˽�����ݽṹ����һ�������������̣��ڶ������ڴ�ͳ��������������������efi��������
		pridata=AllocatePool(3*sizeof(DIDO_DISK_PRIVATE_DATA)+8);
		///��ʼ����ͬ����
		//��ȡ�ļ���С
		Status=FileHandleGetSize(FileDiskFileHandle,&pridata[0].Size);
		//��ȡ������Ϣ����д��Present,StartAddr,Size
		Status=FindPartitionInFile(FileDiskFileHandle,&pridata[1].StartAddr,&pridata[1].Size,&pridata[2].StartAddr,&pridata[2].Size);
		if(EFI_ERROR (Status)) {
			Print(L"No bootable partition on the virtual disk!\n");
			return EFI_NOT_FOUND;
			}		
		if(DiskInRam){				//�Ƿ���Ҫ�����ڴ�
			//�����ڴ�
			Status = gBS->AllocatePool (EfiBootServicesData,pridata[0].Size+8,(VOID**)&pridata[0].StartAddr); 
			if(EFI_ERROR (Status)) {
				Print(L"Allocate Memory failed!\n");
				return EFI_NOT_FOUND;
				}
			Print(L"Creating ramdisk.Please wait.\n");
			//ȫ��iso�ļ�
			FileHandleSetPosition(FileDiskFileHandle,0);	
			Status=FileHandleRead(FileDiskFileHandle,&pridata[0].Size,(VOID*)pridata[0].StartAddr);
			if(EFI_ERROR (Status)){
				Print(L"ReadFile failed.Error=[%r]\n",Status);
				Status=gBS->FreePool((VOID*)pridata[0].StartAddr);
				return EFI_NOT_FOUND;
				}
			//�������ڴ���StartAddr�����ļ�ƫ����
			}else{
				pridata[0].StartAddr=0;
				
				}

		pridata[1].StartAddr=pridata[1].StartAddr+pridata[0].StartAddr;
		pridata[2].StartAddr=pridata[2].StartAddr+pridata[0].StartAddr;		

		

				

		
		
		//��дDP
		//�������̾����DP
		TempPath1=CreateDeviceNode  ( HARDWARE_DEVICE_PATH ,HW_VENDOR_DP ,  sizeof(VENDOR_DEVICE_PATH));
		((VENDOR_DEVICE_PATH*)TempPath1)->Guid=MyGuid;
		pridata[0].VirDiskDevicePath=AppendDevicePathNode(NULL,TempPath1);
		FreePool(TempPath1);
		//��ͳ����ӳ�񣬶�������������
		TempPath1=CreateDeviceNode  ( MEDIA_DEVICE_PATH ,MEDIA_CDROM_DP ,  sizeof(CDROM_DEVICE_PATH));		
		((CDROM_DEVICE_PATH*)TempPath1)->BootEntry=0;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionStart=(pridata[1].StartAddr-pridata[0].StartAddr)/RAM_DISK_BLOCK_SIZE;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionSize=pridata[1].Size/RAM_DISK_BLOCK_SIZE;
		pridata[1].VirDiskDevicePath=AppendDevicePathNode(pridata[0].VirDiskDevicePath,TempPath1);
		FreePool(TempPath1);
		//efi����ӳ��һ����һ������/efi/boot/bootx64.efi������
		TempPath1=CreateDeviceNode  ( MEDIA_DEVICE_PATH ,MEDIA_CDROM_DP ,  sizeof(CDROM_DEVICE_PATH));		
		((CDROM_DEVICE_PATH*)TempPath1)->BootEntry=1;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionStart=(pridata[2].StartAddr-pridata[0].StartAddr)/RAM_DISK_BLOCK_SIZE;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionSize=pridata[2].Size/RAM_DISK_BLOCK_SIZE;
		pridata[2].VirDiskDevicePath=AppendDevicePathNode(pridata[0].VirDiskDevicePath,TempPath1);
		FreePool(TempPath1);

		///��ʼ����ͬ����
		for(INTN	i=0;i<3;i++){
			//pridata[i].Present
			pridata[i].VirDiskHandle=NULL;
			//pridata[i].VirDiskDevicePath	
			pridata[i].VirDiskFileHandle=FileDiskFileHandle;
			pridata[i].InRam=DiskInRam;
			//pridata[i].StartAddr
			//pridata[i].Size
			
			//blockio�ĳ�ʼ��
			CopyMem (&pridata[i].BlockIo, &mFileDiskBlockIoTemplate, sizeof (EFI_BLOCK_IO_PROTOCOL));
			CopyMem (&pridata[i].BlockIo2, &mFileDiskBlockIo2Template, sizeof (EFI_BLOCK_IO2_PROTOCOL));

			pridata[i].BlockIo.Media          		= &pridata[i].Media;
			pridata[i].BlockIo2.Media      	 		= &pridata[i].Media;
			pridata[i].Media.MediaId		  		= 0x1;	
			pridata[i].Media.RemovableMedia   		= TRUE;				
			pridata[i].Media.MediaPresent     		= TRUE;
			pridata[i].Media.LogicalPartition 		= TRUE;					//��ͬ	
			pridata[i].Media.ReadOnly         		= TRUE;								//�����޸�����Ӧ�������ԭ����FALSE
			pridata[i].Media.WriteCaching     		= FALSE;
			pridata[i].Media.IoAlign 		  		= RAM_DISK_BLOCK_SIZE;				//���������������ܱ�diskioʶ��
			pridata[i].Media.BlockSize        		= RAM_DISK_BLOCK_SIZE;
			pridata[i].Media.LastBlock        		= DivU64x32 (
														  pridata[i].Size + RAM_DISK_BLOCK_SIZE - 1,
														  RAM_DISK_BLOCK_SIZE
														  ) - 1;
			

			}
		pridata[0].Media.LogicalPartition 		= FALSE;

				


		//��װ����ӳ��Ŀ�Э��		
		Print(L"installing partition blockio protocol\n");
		Status = gBS->InstallMultipleProtocolInterfaces (
			&pridata[2].VirDiskHandle,
			&gEfiDevicePathProtocolGuid,
			pridata[2].VirDiskDevicePath,
			&gEfiBlockIoProtocolGuid,
			&pridata[2].BlockIo,
	  
			&gEfiBlockIo2ProtocolGuid,
			&pridata[2].BlockIo2,

			NULL
			);
		if(EFI_ERROR(Status)){
			Print(L"%r\n",Status);
			Exit(Status);
			}
		
		Print(L"Virtual partition handle is : %X\n",pridata[2].VirDiskHandle);	

		gBS->ConnectController (pridata[2].VirDiskHandle, NULL, NULL, TRUE);	
		//�����̷ŵ�������������װ����Ϊvirtualbox�Ĺ̼��£����ɵķ�����ʱ���д���
		//��װ���̵Ŀ�Э��		
		Print(L"installing disk blockio protocol\n");
		gBS->InstallMultipleProtocolInterfaces (
			&pridata[0].VirDiskHandle,
			&gEfiDevicePathProtocolGuid,
			pridata[0].VirDiskDevicePath,
			&gEfiBlockIoProtocolGuid,
			&pridata[0].BlockIo,
	  
			&gEfiBlockIo2ProtocolGuid,
			&pridata[0].BlockIo2,

			NULL
			);		
		Print(L"Virtual disk handle is : %X\n",pridata[0].VirDiskHandle);
		gBS->ConnectController (pridata[0].VirDiskHandle, NULL, NULL, TRUE);
				
		//ʵ�����ִ����ConnectController��󲿷ֹ̼����Զ�Ϊpridata[2].VirDiskHandle��װ��fatЭ��		
		//������biosתuefi������http://bbs.wuyou.net/forum.php?mod=viewthread&tid=378197&extra=�Ĺ̼�������
		//��pridata[2].VirDiskHandle�ٰ�װһ��partitionЭ�飬������fatЭ�飬�����ڴ������²�������������

		Status=gBS->HandleProtocol(pridata[2].VirDiskHandle,&gEfiSimpleFileSystemProtocolGuid,(VOID**)&PartitionSFSProtocol);
		if(!EFI_ERROR(Status)){
			return EFI_SUCCESS;
			}
		if(FindBootPartitionHandle()){
			return EFI_ALREADY_STARTED;
			}			
		//����������ϵͳ������fat������Ȼ��Ϊpridata[2].VirDiskHandleǿ�ư�װfatЭ��
		//����̼�û���Զ�Ϊpridata[2].VirDiskHandle��װfatЭ�飬���ҵ�fat���������ǿ�ư�װfatЭ��		
		{

			EFI_HANDLE							FatDriverHandle=NULL;
			EFI_HANDLE							*Buffer;
			UINTN								BufferCount;
			UINTN								BufferIndex;
			EFI_COMPONENT_NAME_PROTOCOL			*DriverNameProtocol;
			CHAR16								*DriverName;
			
			
			//�г����е�֧���������ֵľ��
			Status=gBS->LocateHandleBuffer(ByProtocol,&gEfiComponentNameProtocolGuid,NULL,&BufferCount,&Buffer);
			if(EFI_ERROR (Status)){
				Print(L"ComponentNameProtocol not found.Error=[%r]\n",Status);
				//return Status;
				}
			//ѭ���������е����о��	
			for(BufferIndex=0;BufferIndex<BufferCount;BufferIndex++){
				gBS->HandleProtocol(Buffer[BufferIndex],&gEfiComponentNameProtocolGuid,(VOID**)&DriverNameProtocol);
				DriverNameProtocol->GetDriverName(DriverNameProtocol,"eng",&DriverName);
				//�Ƿ�fat����
				if(NULL!=StrStr(DriverName,L"FAT File System Driver")){
					FatDriverHandle=Buffer[BufferIndex];
					Print(L"FatDriverHandle is:%X\n",FatDriverHandle);
					break;
					}
				}
			//�Ƿ��ҵ���fat����	
			if(FatDriverHandle!=NULL){
				Status=gBS->ConnectController (pridata[2].VirDiskHandle, &FatDriverHandle, NULL, TRUE);
				Print(L"ConnectController Fat %r\n",Status);
				return EFI_SUCCESS;
				}else{
					Print(L"Can't find FAT Driver\n");
					return EFI_NOT_FOUND;
					}
			
		}	

	}