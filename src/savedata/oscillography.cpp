# Copyright (C) 2008-2015 Renato Machado Monaro
# This file is part of OpenRelay.
#
# OpenRelay is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# OpenRelay is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
#include "oscillography.h"

using namespace orelay;
void SaveData::FunctionalDescription_set(string Desc){
	FunctionalDescription=Desc;
	}

string SaveData::FunctionalDescription_get(){
	return FunctionalDescription;
	}
void SaveData::SetClock(Channel<timer> *t){
	MasterClock=t;
	}

void SaveData::SetControl(bool *t){
	Running=t;
	}

void SaveData::Join_Channel( Channel<analogic> *Ch){
	Analogic.push_back(Ch);
	}
void SaveData::Join_Channel( Channel<digital> *Ch){
	Digital.push_back(Ch);
	}
void SaveData::Join_Channel( Channel<timer> *Ch){
	Timer.push_back(Ch);
	}

void SaveData::Join_Channel( Channel<Complex> *Ch){
	Phasors.push_back(Ch);
	}
/*
void SaveData::Join_Channel( Channel<phasor> *Ch){
	Phasor.push_back(Ch);
	}
void SaveData::Join_Channel( Channel<power> *Ch){
	Power.push_back(Ch);
	}*/
Oscillography::Oscillography(Channel<digital> *Trig,int BCicles,int ACicles,string N){
	FunctionalDescription_set("Oscillography"); 
	BeforeCicles=BCicles;
	AfterCicles=ACicles;

	Trigger=Trig;
	Name=N;
	Dynamic=false;
	extension=".osc";
	max_oscillogram = N_MAX_OSCILLOGRAM;
	}
	
Oscillography::Oscillography(Channel<digital> *Trig,int BCicles,int ACicles,string N,unsigned num_max_oscillogram){
	Oscillography(Trig,BCicles,ACicles,N);
	max_oscillogram = num_max_oscillogram;
	}

Oscillography::Oscillography(Channel<digital> *Trig,int BCicles,int ACicles,string N, Channel<string> *Arq){
	FunctionalDescription_set("Oscillography"); 
	BeforeCicles=BCicles;
	AfterCicles=ACicles;
	Trigger=Trig;
	Name=N;
	FileName=Arq;
	Dynamic=true;
	extension=".osc";
	max_oscillogram = N_MAX_OSCILLOGRAM;
	}
	
Oscillography::Oscillography(Channel<digital> *Trig,int BCicles,int ACicles,string N, Channel<string> *Arq,unsigned num_max_oscillogram){
	Oscillography(Trig,BCicles,ACicles,N,Arq);
	max_oscillogram = num_max_oscillogram;
	}

/*bool Oscillography::Prepare(float Samp, unsigned BSize, float Frequency){

	return true;
	}
*/



Evaluation::Evaluation(Channel<digital> *Trig,int BCicles,int ACicles,string N){
	FunctionalDescription_set("Evaluation"); 
	BeforeCicles=BCicles;
	AfterCicles=ACicles;
	Trigger=Trig;
	Trigged=0;
	Name=N;
	NOsc_criadas=0;
	Dynamic=false;
	extension=".eval";
	max_oscillogram = N_MAX_OSCILLOGRAM;
	}
	
Evaluation::Evaluation(Channel<digital> *Trig,int BCicles,int ACicles,string N,unsigned num_max_oscillogram){
	Evaluation(Trig,BCicles,ACicles,N);
	max_oscillogram = num_max_oscillogram;
	}

/*Oscillography::Oscillography(Channel<digital> *Trig,int BeforePoints,int AfterPoints,string N){
	PostFaultPoints=(unsigned)AfterPoints;
	PreFaultPoints=(unsigned)BeforePoints;
	Trigger=Trig;
	Trigged=0;
	Name=N;
	NOsc_criadas=0;
	}*/


/*
unsigned Oscillography::RequiredBufferSize(void){
	return (PreFaultPoints+PostFaultPoints);
	}*/

bool SaveData::Prepare(float Samp, unsigned BSize, float Frequency){
	RelaySamplingRate=Samp;
	PostFaultPoints=(unsigned)AfterCicles*(RelaySamplingRate/Frequency);
	PreFaultPoints=(unsigned)BeforeCicles*(RelaySamplingRate/Frequency);
	//cout<<"PreFault:"<<PreFaultPoints<<" PostFault:"<<PostFaultPoints<<" Max:"<<UINT_MAX<<endl;
	Trigged=0;
	NOsc_criadas = 0;
	NOsc_salvas = 0;
	//posOscillogramStack = 0;
	if(PreFaultPoints>=UINT_MAX){
		//cout<<"Warning: Max Number of the Points holded by Evalution is "<<UINT_MAX<<endl;
		return false;
		}
	if((PreFaultPoints)>=BSize){
//	if((PreFaultPoints+PostFaultPoints)>=BSize){
		//cout<<"Oscillography:: Number of Points ("<<(PreFaultPoints)<<") Requested to Oscillography is Bigger than Buffer Size("<<BSize<<")\n";
		return false;
		}
	names.push_back("Time");
	for(unsigned j=0;j<Timer.size();j++){
		names.push_back(Timer[j]->get_Name());
		}
	for(unsigned j=0;j<Analogic.size();j++){
		names.push_back(Analogic[j]->get_Name());
		}
	for(unsigned j=0;j<Phasors.size();j++){
		names.push_back(Phasors[j]->get_Name()+"(Module)");
		names.push_back(Phasors[j]->get_Name()+"(Angle)");
		}
	for(unsigned j=0;j<Digital.size();j++){
		names.push_back(Digital[j]->get_Name());
		}
		
	for(unsigned i=0; i < max_oscillogram; i++)
	{
		Oscillogram osc = Oscillogram(names, Name, extension, Timer.size()+1, Analogic.size(), Digital.size(), Phasors.size(), PostFaultPoints+PreFaultPoints);
		OscillogramStack.push_back(osc);
	}

	return true;
}

void SaveData::Run(){
	if(Trigged>0){ //Wait for the buffer completition
		unsigned pos_criar = NOsc_criadas % max_oscillogram;
		Trigged--;
		
		if(!OscillogramStack[pos_criar].get_ready())
		{
			unsigned pos_canal = PostFaultPoints + PreFaultPoints - Trigged - 1;
			//cout << "Oscillogram vai preencher 1" << endl;
			OscillogramStack[pos_criar].insert_time(0, pos_canal, MasterClock->get_Value(PreFaultPoints));
			//cout << "Oscillogram preenchendo" << endl;
			for(unsigned j=0;j<Timer.size();j++){
				OscillogramStack[pos_criar].insert_time(j+1, pos_canal, Timer[j]->get_Value(PreFaultPoints));
			}
			//cout << "Oscillogram vai preencher 2" << endl;
			for(unsigned j=0;j<Analogic.size();j++){
				OscillogramStack[pos_criar].insert_analogic(j, pos_canal, Analogic[j]->get_Value(PreFaultPoints));
			}
			//cout << "Oscillogram vai preencher 3" << endl;
			for(unsigned j=0;j<Phasors.size();j++){
				OscillogramStack[pos_criar].insert_complex(j, pos_canal, Phasors[j]->get_Value(PreFaultPoints));
			}
			//cout << "Oscillogram vai preencher 4" << endl;
			for(unsigned j=0;j<Digital.size();j++){
				OscillogramStack[pos_criar].insert_digital(j, pos_canal, Digital[j]->get_Value(PreFaultPoints));
			}
			if(Trigged==0){ //buffer completed
				cout<<"Trigged End"<<endl;
				////cout<<"Name File:"<<File<<endl;
				//OscillogramStack.push_back(Oscillogram(t,a,d,ph,names,File));
				NOsc_criadas++;
				OscillogramStack[pos_criar].set_ready(true);
				//cout << "Oscillogram terminado" << endl;
			}
			
		}
		else
			cout << "OscillogramStack full" << endl;
	}
	if((Trigger->get_Value(0))&&(!Trigger->get_Value(1))&&(Trigged==0)){ //Wait for Trigger
		cout << "Trigged" << endl;
		if(Dynamic)
			OscillogramStack[NOsc_criadas % max_oscillogram].set_file_name(FileName->get_Value());
		else
			OscillogramStack[NOsc_criadas % max_oscillogram].set_file_name(NOsc_criadas+1);
		
		////cout<<"Name File:"<<File<<endl;
		Trigged=PostFaultPoints+PreFaultPoints;
		//Oscillogram osc = Oscillogram(names,File, Timer.size()+1, Analogic.size(), Digital.size(), Phasors.size(), PostFaultPoints+PreFaultPoints);
		//OscillogramList.push_back(osc);
		//cout << "Oscillogram criado" << endl;
	}
		
}

void SaveData::Poll(){
	int pos_salvar;
	cout<<"Poll"<<endl;
	while(*Running){
		//cout<<"Polling"<<endl;
		pos_salvar = NOsc_salvas % max_oscillogram;
		if(OscillogramStack[pos_salvar].get_ready()){
				cout<<"Ready"<<endl;
				OscillogramStack[pos_salvar].Save();
				OscillogramStack[pos_salvar].set_ready(false);
				NOsc_salvas++;
		}
	}
	cout<<"Poll End"<<endl;
}

/*
void Oscillography::CreateOscillogram(){
//	oscillogram Osc(FaultInstant,"Evento_1");
//	Osc.Add(MasterClock,PreFaultPoints+PostFaultPoints);
	vector< vector<timer> > t;
	vector< vector<analogic> > a;
	vector< vector<digital> > d;
	vector< vector<Complex> > ph;
	vector<string> names;
	t.push_back(MasterClock->get_Vector(PreFaultPoints+PostFaultPoints));
	names.push_back("Time");
	for(unsigned j=0;j<Timer.size();j++){
		t.push_back(Timer[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Timer[j]->get_Name());
		}
	for(unsigned j=0;j<Analogic.size();j++){
		a.push_back(Analogic[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Analogic[j]->get_Name());
		}
	for(unsigned j=0;j<Phasors.size();j++){
		ph.push_back(Phasors[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Phasors[j]->get_Name()+"(Module)");
		names.push_back(Phasors[j]->get_Name()+"(Angle)");
		}
	for(unsigned j=0;j<Digital.size();j++){
		d.push_back(Digital[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Digital[j]->get_Name());
		}
	std::stringstream out;
	if(Dynamic)
		out<<FileName->get_Value();
	else
		out << NOsc+1;
	string File=Name+"/"+out.str()+".osc";
	OscillogramStack.push_back(Oscillogram(t,a,d,ph,names,File));
	//SaveOscillogramThreadList.create_thread(boost::bind(&SaveData::Save, this, FileName,t,a,d,ph,names));
//	Osc.Save(Name.c_str());
	}
*/

//Oscillogram::Oscillogram(vector<vector<timer> > time, vector<vector<analogic> > analog, vector<vector<digital> > dig, vector<vector<Complex> > cp, vector<string> n, string FName){
Oscillogram::Oscillogram(vector<string> n, string path, string extension, unsigned sizeTimer, unsigned sizeAnalogic, unsigned sizeDigital, unsigned sizeComplex, unsigned numPoints){
	names=n;
	caminho = path;
	extensionArq = extension;
	
	
	t.resize(sizeTimer);
	a.resize(sizeAnalogic);
	ph.resize(sizeComplex);
	d.resize(sizeDigital);
	
	
	for(unsigned int i=0;i<sizeTimer;i++)
		t[i].resize(numPoints);
	for(unsigned int i=0;i<sizeAnalogic;i++)
		a[i].resize(numPoints);
	for(unsigned int i=0;i<sizeComplex;i++)
		ph[i].resize(numPoints);
	for(unsigned int i=0;i<sizeDigital;i++)
		d[i].resize(numPoints);
		
	set_ready(false);
}
	
void Oscillogram::insert_time(unsigned int i, unsigned j, timer time){
	//cout << "Insert Time inicio" << endl;
	t[i][j] = time;
	//cout << "Insert Time fim" << endl;
}

void Oscillogram::insert_analogic(unsigned int i, unsigned j, analogic anal){
	a[i][j] = anal;
}

void Oscillogram::insert_digital(unsigned int i, unsigned j, digital dig){
	d[i][j] = dig;
}

void Oscillogram::insert_complex(unsigned int i, unsigned j, Complex comp){
	ph[i][j] = comp;
}

void Oscillogram::set_ready(bool b){
	ready = b;
}

bool Oscillogram::get_ready(){
	return ready;
}

void Oscillogram::set_file_name(string file_name){
	FileName = file_name;
}

void Oscillogram::set_file_name(int num_file){
	num_arq = num_file;
}

string Oscillogram::get_file_name(){
	return FileName;
}



bool Oscillogram::Save(){
	ofstream File;
	
	if(FileName == "")
	{
		stringstream nomeArqStream;
		nomeArqStream << caminho << num_arq << extensionArq;
		FileName = nomeArqStream.str();
	}
	
	File.open(FileName.c_str());
	if(!File){
		cerr<<"Oscillograph:: Error Openning File: "<<FileName<<endl;
		return false;
		}
	File.setf(ios::showpoint|ios::fixed);
	File<<"#";
	for(unsigned j=0;j<names.size();j++){
		File<<"\""<<j+1<<"-"<<names[j]<<"\" ";
		}
	File<<endl;
	unsigned Size=t[0].size()-1;
	//for(unsigned i=Size;i>0;i--){
	for(unsigned i=0;i<t[0].size();i++){
		for(unsigned j=0;j<t.size();j++){
			File.precision(6);
			File.width(10);
			File.fill(' ');
			File<<(t[j][i]-t[j][0])/1E9<<" ";
			}
		for(unsigned j=0;j<a.size();j++){
			File.precision(6);
			File.width(10);
			File.fill(' ');
			File<<a[j][i]<<" ";
			}
		for(unsigned j=0;j<ph.size();j++){
			File.precision(6);
			File.width(10);
			File.fill(' ');
			File<<abs(ph[j][i])<<" "<<arg(ph[j][i])<<" ";
			}
		for(unsigned j=0;j<d.size();j++){
			File.width(1);
			File<<d[j][i]<<" ";
			}
		File<<endl;
		}
	File.close();
	FileName = "";
		cout<<"Saved"<<endl;
	return true;
	}

/*
void Evaluation::CreateOscillogram(){
//	oscillogram Osc(FaultInstant,"Evento_1");
//	Osc.Add(MasterClock,PreFaultPoints+PostFaultPoints);
	vector< vector<timer> > t;
	vector< vector<analogic> > a;
	vector< vector<digital> > d;
	vector< vector<Complex> > ph;
	vector<string> names;
	t.push_back(MasterClock->get_Vector(PreFaultPoints+PostFaultPoints));
	names.push_back("Time");
	for(unsigned j=0;j<Timer.size();j++){
		t.push_back(Timer[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Timer[j]->get_Name());
		}
	for(unsigned j=0;j<Analogic.size();j++){
		a.push_back(Analogic[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Analogic[j]->get_Name());
		}
	for(unsigned j=0;j<Phasors.size();j++){
		ph.push_back(Phasors[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Phasors[j]->get_Name());
		}

	for(unsigned j=0;j<Digital.size();j++){
		d.push_back(Digital[j]->get_Vector(PreFaultPoints+PostFaultPoints));
		names.push_back(Digital[j]->get_Name());
		}
	std::stringstream out;
	out << NOsc+1;
	string FileName=Name+"/"+out.str()+extension;
	OscillogramStack.push_back(Oscillogram(t,a,d,ph,names,FileName));
	//SaveOscillogramThreadList.create_thread(boost::bind(&SaveData::Save, this, FileName,t,a,d,ph,names));
//	Osc.Save(Name.c_str());
	}*/

