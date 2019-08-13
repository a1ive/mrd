#include "MyRamDisk.h"
///�����ƶ�image�����Ŀ¼��DP�����FileName��Ϊ�գ���FileName��ӵ���ǰ����DP������

EFI_DEVICE_PATH_PROTOCOL* 
	GetCurrDirDP(
		IN	EFI_HANDLE 			FileHandle,
		IN 	const 	CHAR16		*FileName
		)
		{
			EFI_STATUS						Status;
			EFI_DEVICE_PATH_PROTOCOL		*ThisFileDP;
			EFI_DEVICE_PATH_PROTOCOL		*TempDP=NULL;
			EFI_DEVICE_PATH_PROTOCOL		*TempDPToFree=NULL;
			EFI_DEVICE_PATH_PROTOCOL		*TempDPDirPathNode=NULL;
			EFI_DEVICE_PATH_PROTOCOL		*TempDPFilePathNode=NULL;
			EFI_DEVICE_PATH_PROTOCOL		*TempDPNode;
			INTN							StrIndex;
			CHAR16							*NodeFileName;
			CHAR16							*NodeDirName;	
			//���Լ���ӳ��DP��Ϣ
			Status=gBS->HandleProtocol(FileHandle,&gEfiLoadedImageDevicePathProtocolGuid,(VOID**)&ThisFileDP);
			if(EFI_ERROR (Status)){
				Print(L"LoadedImageDevicePathProtocol not found.Error=[%r]\n",Status);
				return NULL;
				}
			//����һ��dp	
			TempDP=DuplicateDevicePath(ThisFileDP);
			TempDPFilePathNode=TempDP;
			TempDPDirPathNode=TempDPFilePathNode;
			//ֻ��һ��endpath�˳�
			if(NULL==TempDP||IsDevicePathEnd(TempDP))return NULL;
			//�ƶ�dir��fileָ���������ڵ�	
			while(!IsDevicePathEnd(NextDevicePathNode(TempDPFilePathNode))){	
				TempDPDirPathNode=TempDPFilePathNode;
				TempDPFilePathNode=NextDevicePathNode(TempDPFilePathNode);
				}
			//path�������������ļ�path�˳�
			if( MEDIA_DEVICE_PATH!=TempDPFilePathNode->Type||
				MEDIA_FILEPATH_DP!=TempDPFilePathNode->SubType)return NULL;
			//���ļ��ڵ㸴��һ��	
			TempDPNode=AppendDevicePathNode(NULL,TempDPFilePathNode);
			//ɾȥԭ�����ļ��ڵ�
			SetDevicePathEndNode(TempDPFilePathNode);
			
			//���յĸ�Ŀ¼���һ��б��
			//Print(L"DirDP len before repair is %d\n",TempDPDirPathNode->Length[0]);	
			//����������ļ��ڵ㲢�ҵ�һ��Ϊ��
			if(MEDIA_DEVICE_PATH==TempDPDirPathNode->Type&&
				MEDIA_FILEPATH_DP==TempDPDirPathNode->SubType&&
				TempDPDirPathNode->Length[0]<8){
				Print(L"Repair first dir node\n");
				SetDevicePathEndNode(TempDPDirPathNode);
				TempDPDirPathNode=CreateDeviceNode(MEDIA_DEVICE_PATH,	MEDIA_FILEPATH_DP,8);
				((FILEPATH_DEVICE_PATH*)TempDPDirPathNode)->PathName[0]=L'\\';
				((FILEPATH_DEVICE_PATH*)TempDPDirPathNode)->PathName[1]=L'\0';
				TempDPToFree=TempDP;
				TempDP=AppendDevicePathNode(TempDP,TempDPDirPathNode);
				FreePool(TempDPToFree);
				}
			//Print(L"DirDP len after repair is %d\n",TempDPDirPathNode->Length[0]);
			
			//���ļ����ַ�������һ��
			NodeFileName=AllocateCopyPool(2*(StrLen(((FILEPATH_DEVICE_PATH*)TempDPNode)->PathName)+1),
				((FILEPATH_DEVICE_PATH*)TempDPNode)->PathName);
			FreePool(TempDPNode);	
			//����ַ����ķָ���
			NodeDirName=AllocateCopyPool(2*(StrLen(FileName)+1),FileName);			
			for(StrIndex=StrLen(NodeFileName)-1;StrIndex>=0;StrIndex--){
				if(NodeFileName[StrIndex]==L'\\'||NodeFileName[StrIndex]==L'/'){
					//��pathnameȥ���ļ�������·�������Ƶ�dirname
					FreePool(NodeDirName);
					NodeDirName=AllocateCopyPool(2*(StrLen(FileName)+1)+2*(StrIndex+2),NodeFileName);
					NodeDirName[StrIndex+1]=L'\0';
					StrCat(NodeDirName,	FileName);	
					break;				
					}
				}
			FreePool(NodeFileName);	
			///����ڲ����������ļ����ӵ�·������	
			//�����ڵ�
			TempDPNode=CreateDeviceNode(MEDIA_DEVICE_PATH,	MEDIA_FILEPATH_DP,(UINT16)(4+2*(StrLen(NodeDirName)+1)));
			//����Ϻ���ļ���д��ȥ
			StrCat(((FILEPATH_DEVICE_PATH*)TempDPNode)->PathName,NodeDirName);
			FreePool(NodeDirName);
			TempDPToFree=TempDP;
			//���ڵ����ӵ�ǰ����������·��
			TempDP=AppendDevicePathNode(TempDP,TempDPNode);
			FreePool(TempDPToFree);
			FreePool(TempDPNode);
			//���ڵ�	
			return TempDP;
		}