#include "MyRamDisk.h"
///���������д������������OptionStatus�ĳ�Ա
EFI_STATUS
	DispatchOptions(
		DIDO_OPTION_STATUS					*OptionStatus
	)
	{
		EFI_STATUS							Status=EFI_SUCCESS;
		CHAR16								**Argv;
		UINTN								Argc=0;
		UINTN								StrStartPos=0;
		UINTN i,j;	

		//�ֽ������У���ɺ������main����һ��ʹ��������
		Argv=AllocatePool(OptionStatus->OptionStringSizeInByte/2);
		for(i=0;i<OptionStatus->OptionStringSizeInByte/2;i++){
			//�ҵ��ָ���ǰһ��
			if(i==OptionStatus->OptionStringSizeInByte/2-1||
				((CHAR16*)OptionStatus->OptionString)[i+1]==L' '||
				((CHAR16*)OptionStatus->OptionString)[i+1]==L'\x09'||
				((CHAR16*)OptionStatus->OptionString)[i+1]==L'\x0a'||
				((CHAR16*)OptionStatus->OptionString)[i+1]==L'\x0d'||
				((CHAR16*)OptionStatus->OptionString)[i+1]==L'\0'
				){
				Argv[Argc]=AllocatePool((i+2-StrStartPos)*2);
				for(j=0;j<i+1-StrStartPos;j++){
					Argv[Argc][j]=((CHAR16*)OptionStatus->OptionString)[j+StrStartPos];
					if(j==i+1-StrStartPos-1){
						Argv[Argc][j+1]=L'\0';
						}
					}
				//�ų��������ķָ���	
				while(i<OptionStatus->OptionStringSizeInByte/2-1&&
					(((CHAR16*)OptionStatus->OptionString)[i+1]==L' '||
					((CHAR16*)OptionStatus->OptionString)[i+1]==L'\x09'||
					((CHAR16*)OptionStatus->OptionString)[i+1]==L'\x0a'||
					((CHAR16*)OptionStatus->OptionString)[i+1]==L'\x0d'||
					 ((CHAR16*)OptionStatus->OptionString)[i+1]==L'\0')
					){
					i=i+1;
					}						
				
//				Print(L"cmdline is %s\n",Argv[Argc]);
				Argc=Argc+1;
				StrStartPos=i+1;
//				Print(L"Argc is %d\n",Argc);
				}
			
			}


			
		///���������е�-mem -wait -type��-file����
		for(i=0;i<Argc;i++){
			//��ֹgrub2������߽�׳��
			UINTN StringLenth;
			StringLenth=StrnLenS(Argv[i],OptionStatus->OptionStringSizeInByte/2);
			//������-mem����	
			if(	StringLenth>=4&&
				(0==StrCmp(Argv[i],L"-mem")||0==StrCmp(Argv[i],L"-MEM"))){
				OptionStatus->LoadInMemory=TRUE;
				continue;
				}

			//������-debug����		
			if(	StringLenth>=6&&
				(0==StrCmp(Argv[i],L"-debug")||0==StrCmp(Argv[i],L"-DEBUG"))){
				OptionStatus->DebugDropToShell=TRUE;
				continue;
				}
			//������-wait����		
			if(	StringLenth>=5&&i<Argc-1&&
				(0==StrCmp(Argv[i],L"-wait")||0==StrCmp(Argv[i],L"-WAIT"))){
				CHAR16					*TempStr;
				TempStr=AllocateZeroPool(StrSize(Argv[i+1])+2);
				TempStr[0]=L' ';
				StrCatS(TempStr, StrSize(Argv[i+1])+2,Argv[i+1]);	
				OptionStatus->WaitTimeSec=StrDecimalToUintn(TempStr);
				if(TempStr!=NULL)FreePool(TempStr);
				continue;
				}	
				
			//������-altsign����		
			if(	StringLenth>=8&&i<Argc-1&&
				(0==StrCmp(Argv[i],L"-altsign")||0==StrCmp(Argv[i],L"-altsign"))){
				CHAR16					*TempStr;
				TempStr=AllocateZeroPool(StrSize(Argv[i+1])+2);
				TempStr[0]=L' ';
				StrCatS(TempStr, StrSize(Argv[i+1])+2,Argv[i+1]);	
				OptionStatus->AltDiskSign=(UINT32)StrDecimalToUintn(TempStr);
				if(TempStr!=NULL)FreePool(TempStr);
				continue;
				}					
				
			//������-file����		
			if(	StringLenth>=5&&i<Argc-1&&
				(0==StrCmp(Argv[i],L"-file")||0==StrCmp(Argv[i],L"-FILE"))){
				OptionStatus->ImageFileName=AllocateCopyPool(StrSize(Argv[i+1])+2,Argv[i+1]);
				continue;
				}					
			//������-type����		
			if(	StringLenth>=5&&i<Argc-1&&
				(0==StrCmp(Argv[i],L"-type")||0==StrCmp(Argv[i],L"-TYPE"))){
				CHAR16					*TempStr;
				TempStr=AllocateCopyPool(StrSize(Argv[i+1])+2,Argv[i+1]);
				if(0==StrCmp(TempStr,L"cd")||0==StrCmp(TempStr,L"CD")){
					OptionStatus->ImageFileType=ISOFILE;
					}
				if(0==StrCmp(TempStr,L"hd")||0==StrCmp(TempStr,L"HD")){
					OptionStatus->ImageFileType=HARDDISKFILE;
					}
				if(0==StrCmp(TempStr,L"fd")||0==StrCmp(TempStr,L"FD")){
					OptionStatus->ImageFileType=FLOPPYFILE;
					}	
						
				continue;
				}
			//������-dev����
			if(	StringLenth>=4&&i<Argc-1&&
				(0==StrCmp(Argv[i],L"-dev")||0==StrCmp(Argv[i],L"-DEV"))){
				OptionStatus->DevicePathToFindImage=AllocateCopyPool(StrSize(Argv[i+1])+2,Argv[i+1]);
				continue;
				}
			//������-ntfs����
			if(	StringLenth>=5&&
				(0==StrCmp(Argv[i],L"-ntfs")||0==StrCmp(Argv[i],L"-NTFS"))){
				OptionStatus->UseBuildInNtfsDriver=TRUE;
				continue;
				}				
								
			}
/*		
		for(i=0;i<Argc;i++){
			FreePool(argv[i]);
			}
		FreePool(argv);	
*/
		return Status;	
	}