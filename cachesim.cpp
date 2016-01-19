#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<math.h>
#include<vector>
#include<cstdlib>

using namespace std;

struct seti
{
	vector <int>tag;
	vector <int>LRU;
	vector <int>state;
	vector <unsigned long long int>address;
};

struct cachei
{
	int reads;
	int readmiss;int readhits;
	int writeback;
	int writes;
	int writemiss;
	int writehits;
	int intervention;
	int flush;
	int flushopt;
	int mem_read;
	int BusRdX,invalidation,BusUpgr,BusRd,evictions,fills,t;
	vector <seti>set;
};

struct processor
{
	int backinvalidation;
	vector <cachei> cache;
};

vector <processor> p;

int num_proc=4;

int cache_size [2];
int assoc [2];
int blocksize;

int num_sets [2];

int mem_ref,c,copt,flag_bi;
int cachelevel;
int index [2];int offset;

void cacheop(int, int, unsigned long long int, int);
void updateLRU(int, int, int, int);
int space_l1(int proc_num,int cachelevel,int setnum);
int space_l2(int proc_num,int cachelevel,int setnum);


int main(int argc, char *argv[])
{
	if(argc!=7)	{cout<<"Invalid arguments"<<endl;}
	cache_size[0]= atoi(argv[1]);
	cache_size[1]= atoi(argv[2]);
	assoc[0]= atoi(argv[3]);
	assoc[1]= atoi(argv[4]);
	blocksize= atoi(argv[5]);

	num_sets[0]= cache_size[0]/(assoc[0]*blocksize); //cout<<num_sets[0]<<endl;
	num_sets[1]= cache_size[1]/(assoc[1]*blocksize); //cout<<num_sets[1]<<endl;

	index[0]=log2(num_sets[0]); //cout<<index[0]<<endl;
	index[1]=log2(num_sets[1]);	//cout<<index[1]<<endl;
	offset=log2(blocksize);	//cout<<offset<<endl;

	p.resize(4);
	for(int i=0;i<4;i++) p[i].cache.resize(2);
	for(int i=0;i<4;i++) p[i].cache[0].set.resize(num_sets[0]);
	for(int i=0;i<4;i++) p[i].cache[1].set.resize(num_sets[1]);
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<num_sets[0];j++)
		{
			p[i].cache[0].set[j].tag.resize(assoc[0]);
			p[i].cache[0].set[j].LRU.resize(assoc[0]);
			p[i].cache[0].set[j].state.resize(assoc[0]);
			p[i].cache[0].set[j].address.resize(assoc[0]);
		}
		for(int k=0;k<num_sets[1];k++)
		{
			p[i].cache[1].set[k].tag.resize(assoc[1]);
			p[i].cache[1].set[k].LRU.resize(assoc[1]);
			p[i].cache[1].set[k].state.resize(assoc[1]);
			p[i].cache[1].set[k].address.resize(assoc[1]);
		}
	}
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<num_sets[0];j++)
		{
			for(int q=0;q<assoc[0];q++)
			{
				p[i].cache[0].set[j].tag[q]=0;
				p[i].cache[0].set[j].LRU[q]=q;
				p[i].cache[0].set[j].state[q]=0;
				p[i].cache[0].set[j].address[q]=0;
			}
		}
		for(int k=0;k<num_sets[1];k++)
		{
			for(int q=0;q<assoc[1];q++)
			{
				p[i].cache[1].set[k].tag[q]=0;
				p[i].cache[1].set[k].LRU[q]=q;
				p[i].cache[1].set[k].state[q]=0;
				p[i].cache[1].set[k].address[q]=0;
			}
		}
	}
	
/*	for(int i=0;i<4;i++)
	{
		cout<<"processor "<<i<<endl;
		cout<<"L1 cache"<<endl;
		for(int j=0;j<num_sets[0];j++)
		{
			for(int q=0;q<assoc[0];q++)
			{
				cout<<p[i].cache[0].set[j].LRU[q]<<"\t";
			}
			cout<<endl;
		}
		cout<<endl;
		cout<<"L2 cache"<<endl;
		for(int k=0;k<num_sets[1];k++)
		{
			for(int q=0;q<assoc[1];q++)
			{
				cout<<p[i].cache[1].set[k].LRU[q]<<"\t";
			}
			cout<<endl;
		}
		cout<<endl;
	}////////////////////////////// Display Initialization*/

	ifstream ifs(argv[6]);
	string inst;string binary;unsigned long long int address;
	cachelevel=0;int command=0;
	unsigned long long int addressi=0;int requesti=0;
	while(!ifs.eof())
	{
		getline(ifs,inst);//cout<<"flag"<<inst<<endl;
		if(inst[0]=='\0') break;
		string instp=inst.substr(0,1);//cout<<"FLAG2"<<instp<<endl;
		string inst1=inst.substr(2,1);//cout<<"FLAG3"<<inst1<<endl;
		string inst2=inst.substr(4,inst.length());
		//inst2=string(shift_fix,'0').append( inst2);
		int proc_num=std::strtoul(instp.c_str(),NULL,10);
		addressi=std::strtoul(inst2.c_str(),NULL,16);
		if(inst1=="r") requesti=0;
		else if(inst1=="w") requesti=1;
		//cout<<endl<<std::dec<<iter<<"  address "<<std::hex<<addressi<<"\t";iter++;
		mem_ref=0;c=0;copt=0;flag_bi=0;
		cachelevel=0;
		cacheop(proc_num,cachelevel,addressi,requesti);
		if(mem_ref==1) p[proc_num].cache[1].mem_read++;
		if(flag_bi==1) p[proc_num].cache[0].t++;
	}
	cout<<"===== Personal Information ====="<<endl<<"Tushar Gupta"<<endl<<"tgupta3"<<endl<<"ECE 506-001"<<endl;
	cout<<"===== 506 SMP Simulator configuration ====="<<endl;
	cout<<"L1_SIZE:\t"<<cache_size[0]<<endl;
	cout<<"L2_SIZE:\t"<<cache_size[1]<<endl;
	cout<<"L1_ASSOC:\t"<<assoc[0]<<endl;
	cout<<"L2_ASSOC:\t"<<assoc[1]<<endl;
	cout<<"BLOCKSIZE:\t"<<blocksize<<endl;
	cout<<"TRACE FILE:\t"<<argv[6]<<endl;
	for(int i=0;i<num_proc;i++)
	{
		for(int j=0;j<2;j++)
		{
		if(j==0) cout<<"============ Simulation results L1 Cache (Processor "<<i<<") ============"<<endl;
		if(j==1) cout<<"============ Simulation results L2 Cache (Processor "<<i<<") ============"<<endl;
		//cout<<"processor"<<i<<"cache"<<j<<endl;
		cout<<"01. number of reads:"<<p[i].cache[j].reads<<endl;
		cout<<"02. number of read misses:"<<p[i].cache[j].readmiss<<endl;
		cout<<"03. number of writes:"<<p[i].cache[j].writes<<endl;
		cout<<"04. number of write misses:"<<p[i].cache[j].writemiss<<endl;
		printf("05. total miss rate: %.2f %% \n",(((float)p[i].cache[j].readmiss+(float)p[i].cache[j].writemiss)/((float)p[i].cache[j].reads+(float)p[i].cache[j].writes))*100);
		if(j==0) cout<<"06. number of back invalidations:"<<p[i].cache[0].t<<endl;
		if(j==0) cout<<"07. number of fills:"<<p[i].cache[j+1].reads<<endl;
		if(j==0) cout<<"08. number of evictions:"<<p[i].cache[j].evictions<<endl;
		if(j==1)
		{
			cout<<"06. number of writebacks:"<<p[i].cache[j].writeback+p[i].cache[j].flush<<endl;
			cout<<"07. number of cache-to-cache transfers:"<<p[i].cache[j].flushopt<<endl;
			cout<<"08. number of memory transactions:"<<p[i].cache[j].mem_read+p[i].cache[j].flush+p[i].cache[j].writeback<<endl;
			cout<<"09. number of interventions:"<<p[i].cache[j].intervention<<endl;
			cout<<"10. number of invalidations:"<<p[i].cache[j].invalidation<<endl;
			cout<<"11. number of flushes:"<<p[i].cache[j].flush<<endl;
			cout<<"12. number of evictions:"<<p[i].cache[j].evictions<<endl;
			
		}
//		cout<<endl;
		}
	}
}

void cacheop(int proc_num, int cachelevel, unsigned long long int addressp, int request)
{
	//cout<<proc_num<<" "<<cachelevel<<" "<<std::hex<<addressp<<" "<<request<<endl;
	int tagreq=addressp/pow(2,(index[cachelevel]+offset));
	unsigned long long int store=addressp;
	//cout<<std::hex<<"address "<<addressp<<"tag "<<tagreq<<"\t";
	int setnum=0;
	if(request==0) p[proc_num].cache[cachelevel].reads++;
	if(request==1) p[proc_num].cache[cachelevel].writes++;
	int k=0;
	for(int i=0;i<(index[cachelevel]+offset);i++)
	{
		if((i>=offset)&&i<(index[cachelevel]+offset))
		{
			setnum=((addressp%2)*pow(2,k))+setnum;
			k++;
			addressp=addressp/2;
		}
		else
			addressp=addressp/2;
	}
	addressp=store;
	//cout<<"flag "<<addressp<<" "<<setnum<<" "<<tagreq<<endl;
	int found=0;
	for(int i=0;i<assoc[cachelevel];i++)
	{
		if(p[proc_num].cache[cachelevel].set[setnum].tag[i]==tagreq)
		{
			found=1;
			if(cachelevel==0)	//0-Invalid, 1- Valid // 0-read, 1-write
			{
				if(request==0)
				{
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==1) updateLRU(proc_num,cachelevel,setnum,tagreq);
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==0)
					{
						p[proc_num].cache[cachelevel].readmiss++;
						p[proc_num].cache[cachelevel].set[setnum].state[i]=1;
						updateLRU(proc_num,cachelevel,setnum,tagreq);
						cacheop(proc_num,1,addressp,0);
					}
				}
				if(request==1)
				{
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==1)
					{
						updateLRU(proc_num,cachelevel,setnum,tagreq);
						
						////////////////////////////	MID L2
						p[proc_num].cache[1].writes++;
						int tagreq1=addressp/pow(2,(index[1]+offset));
						unsigned long long int store1=addressp;
						int setnum1=0;
						int k2=0;
						for(int k1=0;k1<(index[1]+offset);k1++)
						{
							//cout<<setnum1<<'\t';
							if((k1>=offset)&&k1<(index[1]+offset))
							{
								//cout<<setnum1<<"med"<<k1<<'\t';
								setnum1=((addressp%2)*pow(2,k2))+setnum1;
								k2++;
								addressp=addressp/2;
							}
							else
								addressp=addressp/2;
						}
						addressp=store1; //cout<<endl<<setnum1<<endl;
						//cout<<"flag "<<addressp<<" "<<index[1]<<" "<<offset<<" "<<setnum1<<" "<<tagreq1<<endl;
						for(int j=0;j<assoc[1];j++)
						{
							if(p[proc_num].cache[1].set[setnum1].tag[j]==tagreq1)	//0,1,2,3=invalid,shared,modified,exclusive
							{
								if(p[proc_num].cache[1].set[setnum1].state[j]==2) updateLRU(proc_num,1,setnum1,tagreq1);
								if(p[proc_num].cache[1].set[setnum1].state[j]==3)
								{
									p[proc_num].cache[1].set[setnum1].state[j]=2;
									updateLRU(proc_num,1,setnum1,tagreq1);
								}
								if(p[proc_num].cache[1].set[setnum1].state[j]==1)
								{
									p[proc_num].cache[1].set[setnum1].state[j]=2;
									updateLRU(proc_num,1,setnum1,tagreq1);
									p[proc_num].cache[1].BusUpgr++;
									for(int w=0;w<num_proc;w++)
									{
										if(w!=proc_num) cacheop(w,1,addressp,4);	//4 is BusUpgr
									}
								}
							}
						}
						///////////////////////////////////
					}
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==0)
					{
						p[proc_num].cache[cachelevel].writemiss++;
						cacheop(proc_num,1,addressp,1);				//1 is ProWr
					}
				}
			}
			if(cachelevel==1)
			{
				if(request==0)
				{
					p[proc_num].cache[0].fills++;
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]!=0) updateLRU(proc_num,cachelevel,setnum,tagreq);
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==0)
					{
						p[proc_num].cache[cachelevel].readmiss++;
						p[proc_num].cache[cachelevel].BusRd++;
						for(int j=0;j<num_proc;j++)
						{
							if(j!=proc_num)	cacheop(j,1,addressp,2);		//2 is BusRd, 3--> BusRdX, 4--> BusUpgr
						}
						if(c==0) {p[proc_num].cache[cachelevel].set[setnum].state[i]=3; mem_ref=1;}
						if(c==1) p[proc_num].cache[cachelevel].set[setnum].state[i]=1;
						if(copt==1) p[proc_num].cache[cachelevel].flushopt++;
						updateLRU(proc_num,cachelevel,setnum,tagreq);
					}
				}
				if(request==1)
				{

					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==2) updateLRU(proc_num,cachelevel,setnum,tagreq);
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==3)
					{
						p[proc_num].cache[cachelevel].set[setnum].state[i]=2;
						updateLRU(proc_num,cachelevel,setnum,tagreq);
					}
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==1)
					{
						p[proc_num].cache[cachelevel].set[setnum].state[i]=2;
						p[proc_num].cache[cachelevel].BusUpgr++;
						for(int j=0;j<num_proc;j++)
						{
							if(j!=proc_num) cacheop(j,1,addressp,4);
						}
						updateLRU(proc_num,cachelevel,setnum,tagreq);
					}
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==0)
					{
						p[proc_num].cache[cachelevel].writemiss++;
						p[proc_num].cache[cachelevel].BusRdX++;
						p[proc_num].cache[cachelevel].set[setnum].state[i]=2;
						//mem_ref=1;
						for(int j=0;j<num_proc;j++)
						{
							if(j!=proc_num) cacheop(j,1,addressp,3);
						}
						updateLRU(proc_num,cachelevel,setnum,tagreq);
						if(copt==1) 
							p[proc_num].cache[cachelevel].flushopt++;
						if(copt==0) mem_ref=1;
					}

				}
				if(request==2)
				{
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==2)
					{
						p[proc_num].cache[cachelevel].set[setnum].state[i]=1;
						p[proc_num].cache[cachelevel].flush++;
						p[proc_num].cache[cachelevel].intervention++;
						c=1;copt=1;
					}
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==3)
					{
						p[proc_num].cache[cachelevel].set[setnum].state[i]=1;
						c=1;p[proc_num].cache[cachelevel].intervention++;
						copt=1;
					}
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==1)
					{
						c=1;
						copt=1;
					}
				}

				if(request==3)
				{
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==2)
					{
						p[proc_num].cache[cachelevel].flush++;copt=1;
						p[proc_num].cache[cachelevel].set[setnum].state[i]=0;
						//cache[proc_num].writeback++; Flush is writing back to memroy.
						int initialLRU=p[proc_num].cache[cachelevel].set[setnum].LRU[i];
						for(int j=0;j<assoc[cachelevel];j++)
						{
							if(p[proc_num].cache[cachelevel].set[setnum].LRU[j]>initialLRU && p[proc_num].cache[cachelevel].set[setnum].state[j]!=0) p[proc_num].cache[cachelevel].set[setnum].LRU[j]--;
						}
						p[proc_num].cache[cachelevel].set[setnum].LRU[i]=assoc[cachelevel]-1;
						p[proc_num].cache[cachelevel].invalidation++;

						//// L1 Invalidation
							p[proc_num].backinvalidation++;
							int tagreq1=addressp/pow(2,(index[0]+offset));
							unsigned long long int store1=addressp;
							int setnum1=0;
							int k2=0;
							for(int k1=0;k1<(index[1]+offset);k1++)
							{
								if((k1>=offset)&&k1<(index[1]+offset))
								{
									setnum1=((addressp%2)*pow(2,k2))+setnum1;
									k2++;
									addressp=addressp/2;
								}
								else
									addressp=addressp/2;
							}
							addressp=store1;
							int initialLRU1,a;
							for(int j=0;j<assoc[0];j++)
							{
								if(p[proc_num].cache[0].set[setnum1].tag[j]==tagreq1)	//0,1,2,3=invalid,shared,modified,exclusive
								{
									if(p[proc_num].cache[0].set[setnum1].state[j]!=0) 
									{p[proc_num].cache[0].t++;
									p[proc_num].cache[0].set[setnum1].state[j]=0;a=j;
									initialLRU1=p[proc_num].cache[0].set[setnum1].LRU[j];
									
									for(int l=0;l<assoc[0];l++)
									{
										if(p[proc_num].cache[0].set[setnum1].LRU[l]>initialLRU1 && p[proc_num].cache[0].set[setnum1].state[l]!=0) p[proc_num].cache[0].set[setnum1].LRU[l]--;
									}
									p[proc_num].cache[0].set[setnum1].LRU[a]=assoc[0]-1;
									p[proc_num].cache[0].invalidation++;	}

									break;
								}
							}
											
						////////////
					}
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==3)
					{
						p[proc_num].cache[cachelevel].set[setnum].state[i]=0;
						copt=1;
						int initialLRU=p[proc_num].cache[cachelevel].set[setnum].LRU[i];
						for(int j=0;j<assoc[cachelevel];j++)
						{
							if(p[proc_num].cache[cachelevel].set[setnum].LRU[j]>initialLRU && p[proc_num].cache[cachelevel].set[setnum].state[j]!=0) p[proc_num].cache[cachelevel].set[setnum].LRU[j]--;
						}
						p[proc_num].cache[cachelevel].set[setnum].LRU[i]=assoc[cachelevel]-1;
						p[proc_num].cache[cachelevel].invalidation++;

							//// L1 Invalidation
							p[proc_num].backinvalidation++;
							int tagreq1=addressp/pow(2,(index[0]+offset));
							unsigned long long int store1=addressp;
							int setnum1=0;
							int k2=0;
							for(int k1=0;k1<(index[1]+offset);k1++)
							{
								if((k1>=offset)&&k1<(index[1]+offset))
								{
									setnum1=((addressp%2)*pow(2,k2))+setnum1;
									k2++;
									addressp=addressp/2;
								}
								else
									addressp=addressp/2;
							}
							addressp=store1;
							int initialLRU1,a;
							for(int j=0;j<assoc[0];j++)
							{
								if(p[proc_num].cache[0].set[setnum1].tag[j]==tagreq1)	//0,1,2,3=invalid,shared,modified,exclusive
								{
									if(p[proc_num].cache[0].set[setnum1].state[j]!=0) 
									{p[proc_num].cache[0].t++;
									p[proc_num].cache[0].set[setnum1].state[j]=0;a=j;
									initialLRU1=p[proc_num].cache[0].set[setnum1].LRU[j];
									
									for(int l=0;l<assoc[0];l++)
									{
										if(p[proc_num].cache[0].set[setnum1].LRU[l]>initialLRU1 && p[proc_num].cache[0].set[setnum1].state[l]!=0) p[proc_num].cache[0].set[setnum1].LRU[l]--;
									}
									p[proc_num].cache[0].set[setnum1].LRU[a]=assoc[0]-1;
									p[proc_num].cache[0].invalidation++;}

									break;
								}
							}
							
												
						////////////


					}
					else if(p[proc_num].cache[cachelevel].set[setnum].state[i]==1)
					{
						p[proc_num].cache[cachelevel].set[setnum].state[i]=0;
						copt=1;
						int initialLRU=p[proc_num].cache[cachelevel].set[setnum].LRU[i];
						for(int j=0;j<assoc[cachelevel];j++)
						{
							if(p[proc_num].cache[cachelevel].set[setnum].LRU[j]>initialLRU && p[proc_num].cache[cachelevel].set[setnum].state[j]!=0) p[proc_num].cache[cachelevel].set[setnum].LRU[j]--;
						}
						p[proc_num].cache[cachelevel].set[setnum].LRU[i]=assoc[cachelevel]-1;
						p[proc_num].cache[cachelevel].invalidation++;	

							//// L1 Invalidation
							p[proc_num].backinvalidation++;
							int tagreq1=addressp/pow(2,(index[0]+offset));
							unsigned long long int store1=addressp;
							int setnum1=0;
							int k2=0;
							for(int k1=0;k1<(index[1]+offset);k1++)
							{
								if((k1>=offset)&&k1<(index[1]+offset))
								{
									setnum1=((addressp%2)*pow(2,k2))+setnum1;
									k2++;
									addressp=addressp/2;
								}
								else
									addressp=addressp/2;
							}
							addressp=store1;
							int initialLRU1,a;
							for(int j=0;j<assoc[0];j++)
							{
								if(p[proc_num].cache[0].set[setnum1].tag[j]==tagreq1)	//0,1,2,3=invalid,shared,modified,exclusive
								{
									if(p[proc_num].cache[0].set[setnum1].state[j]!=0) 
									{p[proc_num].cache[0].t++;
									p[proc_num].cache[0].set[setnum1].state[j]=0;a=j;
									initialLRU1=p[proc_num].cache[0].set[setnum1].LRU[j];

									for(int l=0;l<assoc[0];l++)
									{
										if(p[proc_num].cache[0].set[setnum1].LRU[l]>initialLRU1 && p[proc_num].cache[0].set[setnum1].state[l]!=0) p[proc_num].cache[0].set[setnum1].LRU[l]--;
									}
									p[proc_num].cache[0].set[setnum1].LRU[a]=assoc[0]-1;
									p[proc_num].cache[0].invalidation++;				
									}
									break;
								}
							}
						////////////
					}
				}
				if(request==4)
				{
					if(p[proc_num].cache[cachelevel].set[setnum].state[i]==1)
					{
						p[proc_num].cache[cachelevel].set[setnum].state[i]=0;
						int initialLRU=p[proc_num].cache[cachelevel].set[setnum].LRU[i];
						for(int j=0;j<assoc[cachelevel];j++)
						{
							if(p[proc_num].cache[cachelevel].set[setnum].LRU[j]>initialLRU && p[proc_num].cache[cachelevel].set[setnum].state[j]!=0) p[proc_num].cache[cachelevel].set[setnum].LRU[j]--;
						}
						p[proc_num].cache[cachelevel].set[setnum].LRU[i]=assoc[cachelevel]-1;
						p[proc_num].cache[cachelevel].invalidation++;

							//// L1 Invalidation
							p[proc_num].backinvalidation++;
							int tagreq1=addressp/pow(2,(index[0]+offset));
							unsigned long long int store1=addressp;
							int setnum1=0;
							int k2=0;
							for(int k1=0;k1<(index[1]+offset);k1++)
							{
								if((k1>=offset)&&k1<(index[1]+offset))
								{
									setnum1=((addressp%2)*pow(2,k2))+setnum1;
									k2++;
									addressp=addressp/2;
								}
								else
									addressp=addressp/2;
							}
							addressp=store1;
							int initialLRU1,a;
							for(int j=0;j<assoc[0];j++)
							{
								if(p[proc_num].cache[0].set[setnum1].tag[j]==tagreq1)	//0,1,2,3=invalid,shared,modified,exclusive
								{

									if(p[proc_num].cache[0].set[setnum1].state[j]!=0) 
									{p[proc_num].cache[0].t++;
									p[proc_num].cache[0].set[setnum1].state[j]=0;a=j;
									initialLRU1=p[proc_num].cache[0].set[setnum1].LRU[j];

									for(int l=0;l<assoc[0];l++)
									{
										if(p[proc_num].cache[0].set[setnum1].LRU[l]>initialLRU1 && p[proc_num].cache[0].set[setnum1].state[l]!=0) p[proc_num].cache[0].set[setnum1].LRU[l]--;
									}
									p[proc_num].cache[0].set[setnum1].LRU[a]=assoc[0]-1;
									p[proc_num].cache[0].invalidation++;}

									break;
								}
							}
							
												
						////////////
					}
				}
			//	break;
			}
		}
	}
	if(found==0)
	{
		if(cachelevel==0)
		{
			if(request==0)
			{
				p[proc_num].cache[cachelevel].readmiss++;
				int place=space_l1(proc_num,cachelevel,setnum);
				p[proc_num].cache[cachelevel].set[setnum].tag[place]=tagreq;
				p[proc_num].cache[cachelevel].set[setnum].address[place]=addressp;
				p[proc_num].cache[cachelevel].set[setnum].state[place]=1;
				updateLRU(proc_num,cachelevel,setnum,tagreq);
				cacheop(proc_num,1,addressp,0);
			}
			if(request==1)
			{
				p[proc_num].cache[cachelevel].writemiss++;
				cacheop(proc_num,1,addressp,1);
			}
		}
		if(cachelevel==1)
		{
			if(request==0)
			{
				int place = space_l2(proc_num,cachelevel,setnum);
				p[proc_num].cache[cachelevel].set[setnum].tag[place]=tagreq;
				p[proc_num].cache[cachelevel].set[setnum].address[place]=addressp;

				p[proc_num].cache[cachelevel].readmiss++;
				p[proc_num].cache[cachelevel].BusRd++;
				for(int j=0;j<num_proc;j++)
				{
					if(j!=proc_num)	cacheop(j,1,addressp,2);		//2 is BusRd, 3--> BusRdX, 4--> BusUpgr
				}
				if(c==0) {p[proc_num].cache[cachelevel].set[setnum].state[place]=3; mem_ref=1;}
				if(c==1) p[proc_num].cache[cachelevel].set[setnum].state[place]=1;
				if(copt==1) p[proc_num].cache[cachelevel].flushopt++;
				updateLRU(proc_num,cachelevel,setnum,tagreq);
			}
			if(request==1)
			{
				int place = space_l2(proc_num,cachelevel,setnum);
				p[proc_num].cache[cachelevel].set[setnum].tag[place]=tagreq;
				p[proc_num].cache[cachelevel].set[setnum].address[place]=addressp;
				p[proc_num].cache[cachelevel].writemiss++;
				p[proc_num].cache[cachelevel].BusRdX++;
				p[proc_num].cache[cachelevel].set[setnum].state[place]=2;
				//mem_ref=1;
				for(int j=0;j<num_proc;j++)
				{
					if(j!=proc_num) cacheop(j,1,addressp,3);
				}
				updateLRU(proc_num,cachelevel,setnum,tagreq);
				if(copt==1) 
					p[proc_num].cache[cachelevel].flushopt++;
				if(copt==0) mem_ref=1;
			}
		}
	}
}

int space_l2(int proc_num,int cachelevel,int setnum)
{

	for(int i=0;i<assoc[cachelevel];i++)
	{	
		if(p[proc_num].cache[cachelevel].set[setnum].state[i]==0) {return i;break;}
	}
	for(int i=0;i<assoc[cachelevel];i++)
	{
		if(p[proc_num].cache[cachelevel].set[setnum].LRU[i]==(assoc[cachelevel]-1)) 
		{
			p[proc_num].cache[cachelevel].evictions++;
			if(p[proc_num].cache[cachelevel].set[setnum].state[i]==2) p[proc_num].cache[cachelevel].writeback++;// cout<<"evicted pos "<<i<<endl;

			unsigned long long int addrs=p[proc_num].cache[cachelevel].set[setnum].address[i];
										//// L1 Invalidation
							//p[proc_num].backinvalidation++;
							int tagreq1=addrs/pow(2,(index[0]+offset));//cout<<"Flag "<<addrs<<endl;
							unsigned long long int store1=addrs;
							int setnum1=0;
							int k2=0;
							for(int k1=0;k1<(index[1]+offset);k1++)
							{
								if((k1>=offset)&&k1<(index[1]+offset))
								{
									setnum1=((addrs%2)*pow(2,k2))+setnum1;
									k2++;
									addrs=addrs/2;
								}
								else
									addrs=addrs/2;
							}
							addrs=store1;
							int initialLRU1=0,a=0;
							for(int j=0;j<assoc[0];j++)
							{
								if(p[proc_num].cache[0].set[setnum1].tag[j]==tagreq1)	//0,1,2,3=invalid,shared,modified,exclusive
								{
									if(p[proc_num].cache[0].set[setnum1].state[j]!=0) {p[proc_num].cache[0].t++;
									p[proc_num].cache[0].set[setnum1].state[j]=0;a=j;
									initialLRU1=p[proc_num].cache[0].set[setnum1].LRU[j];

									for(int l=0;l<assoc[0];l++)
									{
										if(p[proc_num].cache[0].set[setnum1].LRU[l]>initialLRU1 && p[proc_num].cache[0].set[setnum1].state[l]!=0) {p[proc_num].cache[0].set[setnum1].LRU[l]--;}
									}
									p[proc_num].cache[0].set[setnum1].LRU[a]=assoc[0]-1;
									p[proc_num].cache[0].invalidation++;}

									break;
								}
							}
							//cout<<a<<" flag"<<endl;					
						////////////*/

			return i;
		}
	}
}


int space_l1(int proc_num,int cachelevel,int setnum)
{
	for(int i=0;i<assoc[cachelevel];i++)
	{
		if(p[proc_num].cache[cachelevel].set[setnum].state[i]==0) {return i;break;}
	}
	for(int i=0;i<assoc[cachelevel];i++)
	{
		if(p[proc_num].cache[cachelevel].set[setnum].LRU[i]==(assoc[cachelevel]-1)) 
		{
			p[proc_num].cache[cachelevel].evictions++;
			//if(cache[proc_num].set[setnum].state[i]==2) cache[proc_num].writeback++;// cout<<"evicted pos "<<i<<endl;
			return i;
		}
	}
}

void updateLRU(int proc_num, int cachelevel, int setnum, int tagreq)
{
	int currentLRU;int k=0;
	for(int i=0;i<assoc[cachelevel];i++)
	{
		if(p[proc_num].cache[cachelevel].set[setnum].tag[i]==tagreq && p[proc_num].cache[cachelevel].set[setnum].state[i]!=0)
		{
			currentLRU=p[proc_num].cache[cachelevel].set[setnum].LRU[i];
			k=i;
		}
	}
	for(int i=0;i<assoc[cachelevel];i++)
	{
		if(p[proc_num].cache[cachelevel].set[setnum].state[i]!=0){
		if((p[proc_num].cache[cachelevel].set[setnum].LRU[i])<currentLRU) {p[proc_num].cache[cachelevel].set[setnum].LRU[i]++;}}	
	}
	p[proc_num].cache[cachelevel].set[setnum].LRU[k]=0;
}
