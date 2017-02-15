#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	char *s1,*s2,*c,*c2,*p;
	unsigned char *a,*t,*f;
	int n,i,ind,sz,vl,size,alloc,fblk,obts=4,dblk,dbts;
	int frag,eff;
	s1=calloc(17,sizeof(unsigned char));
	s2=calloc(17,sizeof(unsigned char));
	c=calloc(15,sizeof(unsigned char));
	c2=calloc(15,sizeof(unsigned char));
	do{
		fgets(s1,20,stdin);
		strcpy(s2,s1);
		p=strtok(s1," ");
		if(strcmp(p,"INITIALIZE")==0){				//INITIALIZE
			sscanf(s2,"%s %d",c,&n);
			size=n;
			a=calloc(n,sizeof(unsigned char));
		}
		else if(strcmp(p,"FINALIZE\n")==0){			//FINALIZE
			free(a);
		}
		else if(strcmp(p,"DUMP\n")==0){				//DUMP
			n=size;
			for(i=0;i<n;i++){
				if(i%16==0){
					printf("%08X\t",i);
				}
				printf("%02X ",*(unsigned char*)(a+i));
				if(i%8==7){printf(" ");}
				if(i%16==15){
					printf("\n");
				}
			}
			if(n%16!=0){printf("\n");}
			printf("%08X\n",n);
		}
		else if(strcmp(p,"ALLOC")==0){				//ALLOC
			sscanf(s2,"%s %d",c,&n);
			alloc=0;
			t=a;
			f=t;	
			if((*(int*)a==0)&&(n<=size-16)){
				t=a+sizeof(int);
				*(int*)t=0;
				*(int*)(t+1*sizeof(int))=0;
				*(int*)(t+2*sizeof(int))=n+12;
				*(int*)a=4;
				alloc=1;
				printf("%d\n",16);
			}else if(n+12<=(*(int*)a)-4){
				*(int*)(t+1*sizeof(int))=*(int*)a;
				*(int*)(t+2*sizeof(int))=0;
				*(int*)(t+3*sizeof(int))=n+12;
				*(int*)(a+(*(int*)a+4)*sizeof(char))=4;
				*(int*)a=4;
				alloc=1;
				printf("%d\n",16);
			}else{
				t=a+(*(int*)t)*sizeof(char);
				while(*(int*)t!=0){
					if(n+12<=(*(int*)t-*(int*)f-(*(int*)(t+2*sizeof(int))))){
						*(int*)(t+(*(int*)(t+2*sizeof(int)))*sizeof(char))=*(int*)t;
						*(int*)(t+(*(int*)(t+2*sizeof(int)))*sizeof(char)+1*sizeof(int))=*(int*)f;
						*(int*)(t+(*(int*)(t+2*sizeof(int)))*sizeof(char)+2*sizeof(int))=n+12;
						*(int*)(a+(*(int*)t)*sizeof(char)+sizeof(int))=*(int*)f+*(int*)(t+2*sizeof(int));
						*(int*)t=*(int*)f+*(int*)(t+2*sizeof(int));
						alloc=1;
						printf("%d\n",*(int*)t+12);
						break;
					}
					f=t;
					t=a+(*(int*)t)*sizeof(char);
				}
				if(alloc==0){
					if(n+12<=size-*(int*)f-*(int*)(t+2*sizeof(int))){
						*(int*)(t+(*(int*)(t+2*sizeof(int)))*sizeof(char))=0;
						*(int*)(t+(*(int*)(t+2*sizeof(int)))*sizeof(char)+1*sizeof(int))=*(int*)f;
						*(int*)(t+(*(int*)(t+2*sizeof(int)))*sizeof(char)+2*sizeof(int))=n+12;
						*(int*)t=*(int*)f+*(int*)(t+2*sizeof(int));
						alloc=1;
						printf("%d\n",*(int*)t+12);
					}
				}
			}
			if(alloc==0){
				printf("0\n");
			}
			if(alloc==1){
				obts=obts+n+12;
			}
		}
		else if(strcmp(p,"FREE")==0){					//FREE
			sscanf(s2,"%s %d",c,&n);
			t=a+(n-12)*sizeof(char);
			f=a+*(int*)(t+sizeof(int))*sizeof(char);
			if(*(int*)t!=0){
				*(int*)(a+*(int*)t*sizeof(char)+sizeof(int))=*(int*)(t+sizeof(int));
				*(int*)f=*(int*)t;
			}
			else{
				*(int*)f=0;
			}
			obts=obts-*(int*)(t+2*sizeof(int));
		}
		else if(strcmp(p,"FILL")==0){					//FILL
			sscanf(s2,"%s %d %d %d",c,&ind,&sz,&vl);
			t=a+ind*sizeof(char);
			i=0;
			while(i<sz){
				*(unsigned char*)t=vl;
				i++;
				t=t+sizeof(char);
			}
		}
		else if(strcmp(p,"SHOW")==0){					//SHOW
			sscanf(s2,"%s %s",c,c2);
			t=a;
			f=t;
			fblk=0;
			dbts=0;
			dblk=0;
			if(strcmp(c2,"FREE")==0){					//FREE
				if(*(int*)a==0){
					printf("1 blocks (%d bytes) free\n",size-4);
				}
				else{
					t=a+*(int*)t;
					if(*(int*)a>4){
						fblk++;
					}
					while(*(int*)t!=0){
						if(*(int*)f+*(int*)(t+2*sizeof(int))<*(int*)t){
							fblk++;
						}
						f=t;
						t=a+*(int*)t*sizeof(char);
					}
					if(size-*(int*)f-*(int*)(t+2*sizeof(int))!=0){
						fblk++;
					}
					printf("%d blocks (%d bytes) free\n",fblk,size-obts);
				}
			}
			else if(strcmp(c2,"USAGE")==0){			//USAGE
				if(*(int*)a==0){
					printf("0 blocks (0 bytes) used\n0%% efficiency\n0%% fragmentation\n");
				}
				else{
					t=a+*(int*)t;
					if(*(int*)a>4){
						fblk++;
					}
					while(*(int*)t!=0){
						if(*(int*)f+*(int*)(t+2*sizeof(int))<*(int*)t){
							fblk++;
						}
						f=t;
						t=a+*(int*)t;
					}
					if(size-*(int*)f-*(int*)(t+2*sizeof(int))!=0){
						fblk++;
					}
					t=a;
					f=t;
					
					do{
						t=a+*(int*)t*sizeof(char);
						dbts=dbts+*(int*)(t+2*sizeof(int))-12;
						dblk++;
					}while(*(int*)t!=0);
					eff=dbts*100/(obts);
					frag=((fblk-1)*100/dblk);
					printf("%d blocks (%d bytes) used\n%d%% efficiency\n%d%% fragmentation\n",dblk,dbts,eff,frag);
				}
			}
			else if(strcmp(c2,"ALLOCATIONS")==0){	//ALLOCATIONS
				if(*(int*)a==0){
					printf("OCCUPIED 4 bytes\nFREE %d bytes",size-4);
				}
				else{
					t=a+*(int*)t;
					printf("OCCUPIED 4 bytes\n");
					if(*(int*)f>4){
						printf("FREE %d bytes\n",*(int*)f-4);
					}
					while(*(int*)t!=0){
						printf("OCCUPIED %d bytes\n",*(int*)(t+2*sizeof(int)));
						if(*(int*)f+*(int*)(t+2*sizeof(int))<*(int*)t){
							printf("FREE %d bytes\n",*(int*)t-*(int*)f-*(int*)(t+2*sizeof(int)));
						}
						f=t;
						t=a+*(int*)t;
					}
					printf("OCCUPIED %d bytes\n",*(int*)(t+2*sizeof(int)));
					if(*(int*)f+*(int*)(t+2*sizeof(int))<size){
						printf("FREE %d bytes\n",size-*(int*)f-*(int*)(t+2*sizeof(int)));
					}
				}
			}
		}
	}while(strcmp(p,"FINALIZE\n")!=0);
	
	free(s1);
	free(s2);
	free(c);
	free(c2);
	
	return 0;
}