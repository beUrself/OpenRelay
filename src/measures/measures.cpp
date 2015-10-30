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
#include "measures.h"
#include <cstdio>

using namespace orelay;
string Measure::FunctionalDescription_get(){
	return FunctionalDescription;
	}

void Measure::FunctionalDescription_set(string Desc){
	FunctionalDescription=Desc;
	}

void Measure::SetClock(Channel<timer> *t){
	MasterClock=t;
	}
bool Measure::Prepare(float Samp){
	RelaySamplingRate=Samp;
	return true;
	}

RMS::RMS(Channel<analogic> *CH_IN, Channel<analogic> *CH_F,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("True RMS Measure"); 
	Input=CH_IN;
	Output=CH_OUT;
	Freq=CH_F;
	Frequency=SYSTEM_FREQUENCY;
	Metodology=DYNAMIC;
	vec=false;
}

RMS::RMS(Channel<analogic> *CH_IN,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("True RMS Measure"); 
	Input=CH_IN;
	Output=CH_OUT;
	Frequency=SYSTEM_FREQUENCY;
	Metodology=STATIC;
	vec=false;
}

/*RMS::RMS(Channel<Vanalogic> *CH_IN,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("True RMS Measure"); 
	VInput=CH_IN;
	Output=CH_OUT;
	Frequency=SYSTEM_FREQUENCY;
	Metodology=STATIC;
	vec=true;
}*/
void RMS::Run(){
	#ifdef DEBUG_ALL
		cout<<"RMS\n";
	#endif
	unsigned  Samples_Cicle;
	double rms;
	if(vec){
		Vanalogic V;
		V=VInput->get_Value();
		rms=0;
		for(unsigned k=0;k<V.size();k++)
			rms+=pow(V[k],2);
		Output->insert_Value(sqrt(rms/(V.size())));
		}
	else{
		if(Metodology==DYNAMIC){
			Frequency=Freq->get_Value();
			if((Frequency<30.00)|| (Frequency>100.00))
				Frequency=SYSTEM_FREQUENCY;
			}
		Samples_Cicle=(int)(RelaySamplingRate/Frequency);
		if(Input->get_Size()>=Samples_Cicle){
			rms=0;
			for(unsigned i=0;i<Samples_Cicle;i++){
				rms+=pow(Input->get_Value(i),2.0);
				}
			Output->insert_Value(sqrt(rms/Samples_Cicle));
			}
		else{
			cout<<"RMS:: Insufficient Number of Samples in Channel"<<Input->get_Name()<<endl;
			exit(0);
			}
	}
}

Frequency::Frequency(Channel<analogic> *CH_IN,Channel<analogic> *CH_OUT,int MET){
	FunctionalDescription_set("Frequency Measure"); 
	Input=CH_IN;
	Output=CH_OUT;
	average=0.0;
	Methodology=MET;
	CrossTime=orelay_gettime();
	CrossTime_old=CrossTime;
	}

void Frequency::Run()
{
//	double average=Input->Average();
	//average = Input->get_Value();
	long long 
		   T1,T2,A1,A2;
	double a,b,c,d;
	
	#ifdef DEBUG_ALL
		cout<<"Frequency\n";
	#endif
	
//	if((Input->get_Value()*Input->get_Value(1))<0){
//	if(abs(Input->get_Value())>=average)
	//if(((Input->get_Value()>=average)&&(Input->get_Value(1)<average))||((Input->get_Value()<=average)&&(Input->get_Value(1)>average)))
	if(((Input->get_Value()>=average)&&(Input->get_Value(1)<average)))
	{
		CrossTime_old=CrossTime;
		/*
		CrossTime=MasterClock->get_Value();
		
		printf("\n\nx1:%ld y1:%lf \n",CrossTime_old,Input->get_Value());
		
		a = (Input->get_Value() - Input->get_Value(1))/(CrossTime - CrossTime_old);
		b = Input->get_Value() - a*CrossTime;
		
		printf("x2:%ld y2:%lf \n",CrossTime,Input->get_Value());
		
		// Obtem-se x para y=0
		CrossTime = -b/a;
		
		printf("CrossTimeDepois:%ld \n",CrossTime);
		*/
		//CrossTime=MasterClock->get_Value(1)+(Input->get_Value(1)*(MasterClock->get_Value(0)-MasterClock->get_Value(1))/(Input->get_Value(0)-Input->get_Value(1)));
		
		/*T1 = MasterClock->get_Value(1);
		T2 = MasterClock->get_Value();
		A1 = Input->get_Value(1);
		A2 = Input->get_Value();
		
		//f(x) = a + bx+ cx^2 + dx^3
		
		a = (1/pow(T1-T2,3))*(A2*pow(T1,3) - 3*A2*T2*pow(T1,2) + 3*A1*pow(T2,2)*T1 - A1*pow(T2,3));
		b = (1/pow(T1-T2,3))*(6*(A2*T1*T2 - A1*T1*T2));
		c = (1/pow(T1-T2,3))*(-3*(-A1*T1 + A2*T1 - A1*T2 + A2*T2));
		d = (1/pow(T1-T2,3))*(2*(A2-A1));
		
		CrossTime = pow(sqrt(pow(-27*a*pow(d,2)+9*b*c*d-2*pow(c,3),2)+4*pow(3*b*d-pow(c,2),3))-27*a*pow(d,2)+9*b*c*d-2*pow(c,3),1.0/3)/(3*pow(2,1.0/3)*d)-(pow(2,1.0/3)*(3*b*d-pow(c,2)))/(3*d*pow(sqrt(pow(-27*a*pow(d,2)+9*b*c*d-2*pow(c,3),2)+4*pow(3*b*d-pow(c,2),3))-27*a*pow(d,2)+9*b*c*d-2*pow(c,3),1.0/3))-c/(3*d);
		
		printf("Diferenca:%ld \n",CrossTime-CrossTime_old);
		*/
		 
		CrossTime=(Input->get_Value(1)*MasterClock->get_Value() - Input->get_Value()*MasterClock->get_Value(1))/(Input->get_Value(1) - Input->get_Value());
	}
	
	//Output->insert_Value(0.5E9/abs(CrossTime-CrossTime_old));
	Output->insert_Value(1E9/abs(CrossTime-CrossTime_old));
}
DFT::DFT(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq, unsigned Harm){	
	FunctionalDescription_set("Discret Fourier Transform"); 
	Input=CH_IN;
	Output=CH_OUT;
	Harmonic=Harm;
	SystemFrequency=Freq;
	}
DFT::DFT(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq){	
	Input=CH_IN;
	Output=CH_OUT;
	Harmonic=1;
	SystemFrequency=Freq;
	}
bool DFT::Prepare(float Samp){
	RelaySamplingRate=Samp;
	N=(unsigned)(RelaySamplingRate/SystemFrequency);
	cout<<"Fourier N"<<N<<endl;
	return true;
	}
void DFT::Run(){
	Complex Sum,i(0.0,1.0);
	Sum= Complex(0.0,0.0);
	for(unsigned j=0;j<N;j++)
		Sum+=Input->get_Value(j)*exp(((analogic)(2*M_PI/N)*i)*(analogic)(j*Harmonic));
	Sum/=(analogic)N;
	Sum*=sqrt(2);
	Output->insert_Value(Sum);
	}

Power::Power(Channel<analogic> *CH_I1,Channel<analogic> *CH_V1,Channel<analogic> *CH_S,Channel<analogic> *CH_P,Channel<analogic> *CH_Q, float Freq){
	FunctionalDescription_set("Monophasic Power Measure"); 
	Current.push_back(CH_I1);
	Voltage.push_back(CH_V1);
	Active=CH_P;
	Reactive=CH_Q;
	Apparent=CH_S;
	SystemFrequency=Freq;
	}
Power::Power(Channel<analogic> *CH_V1,Channel<analogic> *CH_V2,Channel<analogic> *CH_V3,Channel<analogic> *CH_I1,Channel<analogic> *CH_I2,Channel<analogic> *CH_I3,Channel<analogic> *CH_S,Channel<analogic> *CH_P,Channel<analogic> *CH_Q, float Freq){	
	FunctionalDescription_set("Triphasic Power Measure"); 
	Current.push_back(CH_I1);
	Voltage.push_back(CH_V1);
	Current.push_back(CH_I2);
	Voltage.push_back(CH_V2);
	Current.push_back(CH_I3);
	Voltage.push_back(CH_V3);
	Active=CH_P;
	Reactive=CH_Q;
	Apparent=CH_S;
	SystemFrequency=Freq;
	}
bool Power::Prepare(float Samp){
	RelaySamplingRate=Samp;
	N=(int)(RelaySamplingRate/SystemFrequency);
	return true;
	}
void Power::Run(){
	#ifdef DEBUG_ALL
		cout<<"Power\n";
	#endif	
	Complex i(0.0,1.0),I,V;
	analogic p=0,q=0,s=0;
	for(unsigned k=0;k<Current.size();k++){
		I=Complex(0.0,0.0);
		for(unsigned j=0;j<N;j++)
			I+=Current[k]->get_Value(j)*exp(((analogic)(2*M_PI/N)*i)*(analogic)j);
		I=I/(analogic)N;
		V=Complex(0.0,0.0);
		for(unsigned j=0;j<N;j++)
			V+=Voltage[k]->get_Value(j)*exp(((analogic)(2*M_PI/N)*i)*(analogic)j);;
		V=V/(analogic)N;
	p+=2*abs(V)*abs(I)*cos(arg(V)-arg(I));
	q+=2*abs(V)*abs(I)*sin(arg(V)-arg(I));
	}
	s=sqrt(pow(p,2)+pow(q,2));
	Active->insert_Value(p);
	Reactive->insert_Value(q);
	Apparent->insert_Value(s);


/*	phasor V,I;
	double p=0,q=0,s=0,h=0;
	for(unsigned k=0;k<Input_1.size();k++){
		for(int i=0;i<N;i++)
			fftw_in[i]=Input_1[k]->get_Value(i);
		fftw_execute(fftw_p);
		for(int i=0;i<((N/2)+1);i++)
			V.insert_Value(Complex(fftw_out[i][0],fftw_out[i][1])*sqrt(2)/(double)N,i);
		for(int i=0;i<N;i++)
			fftw_in[i]=Input_2[k]->get_Value(i);
		fftw_execute(fftw_p);
		for(int i=0;i<((N/2)+1);i++)
			I.insert_Value(Complex(fftw_out[i][0],fftw_out[i][1])*sqrt(2)/(double)N,i);
		for(unsigned i=0;i<V.size();i++){
			p+=abs(V[i])*abs(I[i])*cos(arg(V[i])-arg(I[i]));
			q+=abs(V[i])*abs(I[i])*sin(arg(V[i])-arg(I[i]));
			}
		s+=rms(V)*rms(I);
		if(s>(pow(p,2)+pow(q,2)))
			h+=sqrt(pow(s,2)-pow(p,2)-pow(q,2));
		}
	Output->insert_Value(power(s,p,q,h));*/
	}
	
	
PowerDQ::PowerDQ(Channel<analogic> *CH_VD,Channel<analogic> *CH_ID,Channel<analogic> *CH_VQ,Channel<analogic> *CH_IQ,Channel<analogic> *CH_P, Channel<analogic> *CH_Q){
	Vd=CH_VD;
	Vq=CH_VQ;
	Id=CH_ID;
	Iq=CH_IQ;
	P=CH_P;
	Q=CH_Q;
	}
void PowerDQ::Run(){
	P->insert_Value((Vd->get_Value()*Id->get_Value()+Vq->get_Value()*Iq->get_Value()));
	Q->insert_Value((-Vq->get_Value()*Id->get_Value()+Vd->get_Value()*Iq->get_Value()));
	}
bool PowerDQ::Prepare(float){
	return true;
	}

Power2::Power2(Channel<analogic> *CH_VA,Channel<analogic> *CH_VB,Channel<analogic> *CH_VC,Channel<analogic> *CH_IA, Channel<analogic> *CH_IB, Channel<analogic> *CH_IC,Channel<analogic> *CH_P, Channel<analogic> *CH_Q){
	Va=CH_VA;
	Vb=CH_VB;
	Vc=CH_VC;
	Ia=CH_IA;
	Ib=CH_IB;
	Ic=CH_IC;
	P=CH_P;
	Q=CH_Q;
	}
void Power2::Run(){
	P->insert_Value(Va->get_Value()*Ia->get_Value()+Vb->get_Value()*Ib->get_Value()+Vc->get_Value()*Ic->get_Value());
	Q->insert_Value((Va->get_Value()*(Ib->get_Value()-Ic->get_Value())+Vb->get_Value()*(Ic->get_Value()-Ia->get_Value())+Vc->get_Value()*(Ia->get_Value()-Ib->get_Value()))/(sqrt(3)));
	}
bool Power2::Prepare(float){
	return true;
	}

void Sum::Join_Channel(Channel<analogic> *In){
	FunctionalDescription_set("Sum Operator"); 
	Input.push_back(In);
	Scalar.push_back(1.0);	
	}
void Sum::Join_Channel(Channel<analogic> *In, double S){
	FunctionalDescription_set("Sum Operator"); 
	Input.push_back(In);
	Scalar.push_back(S);
	}
Sum::Sum(Channel<analogic>*CH_OUT){
	Output=CH_OUT;
	}

bool Sum::Prepare(float Samp){
	RelaySamplingRate=Samp;
	if(Input.size()==0){
		cout<<"Must Have more then 1 channel for the operation.\n";
		return false;
		}
	return true;
	}

void Sum::Run(){
	analogic temp=0;
	for(unsigned k=0;k<Input.size();k++)
		temp+=Input[k]->get_Value()*Scalar[k];
	Output->insert_Value(temp);
	}


/*Diff::Diff(Channel<analogic>*CH_OUT){
	Output=CH_OUT;
	}*/

void Diff::Run(){
	analogic temp=0;
	temp=Input[0]->get_Value();
	for(unsigned k=1;k<Input.size();k++)
		temp-=Input[k]->get_Value()*Scalar[k];
	Output->insert_Value(temp);
	}

/*Max::Max(Channel<analogic>*CH_OUT){
	Output=CH_OUT;
	}*/

void Max::Run(){
	analogic temp=Input[0]->get_Value();
	for(unsigned k=1;k<Input.size();k++)
		if(temp<Input[k]->get_Value())
			temp=Input[k]->get_Value();
	Output->insert_Value(temp);
	}


ZeroCrossDetection::ZeroCrossDetection(Channel<analogic>* CH_IN, Channel<digital>* CH_OUT){
	FunctionalDescription_set("Zero Cross Detection"); 
	Input=CH_IN;
	Output=CH_OUT;
}

void ZeroCrossDetection::Run(){
	if((Input->get_Value()>=0)&&(Input->get_Value(1)<=0))
		Output->insert_Value(true);
	else
		Output->insert_Value(false);
}



Mean::Mean(Channel<analogic> *CH_IN,Channel<analogic> *CH_OUT, unsigned S){
	FunctionalDescription_set("Mean Measure"); 
	Input=CH_IN;
	Output=CH_OUT;
		Scale=1.0;
	if(S<Input->get_Size())
		N=S;
	else
		N=Input->get_Size();
}

Mean::Mean(Channel<analogic> *CH_IN,Channel<analogic> *CH_OUT, unsigned S, double Sc){
	FunctionalDescription_set("Mean Measure"); 
	Input=CH_IN;
	Output=CH_OUT;
	Scale=Sc;
	if(S<Input->get_Size())
		N=S;
	else
		N=Input->get_Size();
}

Mean::Mean(Channel<analogic> *CH_IN,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("Mean Measure"); 
	Input=CH_IN;
	Output=CH_OUT;
			Scale=1.0;
	N=Input->get_Size();
}

void Mean::Run(){
	analogic tmp;
	tmp=0;
	for(unsigned i=0;i<N;i++)
		tmp+=Input->get_Value(i)/Scale;
	Output->insert_Value(tmp/(analogic)N);
}

Abs::Abs(Channel<analogic> *CH_IN,Channel<analogic> *CH_OUT){
	Input=CH_IN;
	Output=CH_OUT;
	Type=ANALOG;
}

Abs::Abs(Channel<Complex> *CH_IN,Channel<analogic> *CH_OUT){
	Input_C=CH_IN;
	Output=CH_OUT;
	Type=COMPLEX;
}
void Abs::Run(){
	if(Type==COMPLEX)
		Output->insert_Value(abs(Input_C->get_Value()));
	else
		Output->insert_Value(abs(Input->get_Value()));
}


Scale::Scale(Channel<analogic> *CH_IN,Channel<analogic> *CH_OUT, double G, double Off){
	Input=CH_IN;
	Output=CH_OUT;
	Gain=G;
	Offset=Off;
	}
void Scale::Run(){
	Output->insert_Value(Gain*Input->get_Value()+Offset);
	}



StringComposer::StringComposer(Channel<string> *Str, string Sep){
	Output=Str;
	Separator=Sep;
	Position=0;
	Out.setf(ios::fixed);
	Out.fill('0');
	}
void StringComposer::Join(Channel<analogic> *ChAna, unsigned Dec,unsigned Width, string Prefix){
	AnalogicList.push_back(ChAna);
	AnalogicPrefix.push_back(Prefix);
	AnalogicDecimal.push_back(Dec);
	AnalogicWidth.push_back(Width);
	AnalogicPosition.push_back(Position);
	Position++;
}
void StringComposer::Join(Channel<string> *ChStr, string Prefix){
	StringList.push_back(ChStr);
	StringPrefix.push_back(Prefix);
	StringPosition.push_back(Position);
	Position++;
}

void StringComposer::Run(){
	Out.str("");
	for(unsigned k=0;k<Position;k++){
		for(unsigned l=0;l<AnalogicPosition.size();l++)
			if(AnalogicPosition[l]==k){
				Out<<AnalogicPrefix[l]<<setw(AnalogicWidth[l])<<setprecision(AnalogicDecimal[l])<<AnalogicList[l]->get_Value();
				if(k<(Position-1))
					Out<<Separator;
				}
		for(unsigned l=0;l<StringPosition.size();l++)
			if(StringPosition[l]==k){
				Out<<StringPrefix[l]<<StringList[l]->get_Value();
				if(k<(Position-1))
					Out<<Separator;
				}
		}
	Output->insert_Value(Out.str());
	}

/*
WaveletDetector::WaveletDetector(Channel<digital>* CH_OUT,unsigned k, unsigned N, unsigned SplSize, double thr){
	FunctionalDescription_set("Wavelet Detector"); 
	Order=k;
	Level=N;
	SampleSize=SplSize;
	w=gsl_wavelet_alloc(gsl_wavelet_daubechies,Order);
	work=gsl_wavelet_workspace_alloc(SampleSize);
	data=(double*)malloc(SampleSize*sizeof(double));
	Threshold=thr;
	Output=CH_OUT;
	}

WaveletDetector::~WaveletDetector(){
	gsl_wavelet_free(w);
	gsl_wavelet_workspace_free(work);
	}

void WaveletDetector::Join_Channel(Channel<analogic> *In){
	Input.push_back(In);
	}

void WaveletDetector::Run(){
	detect=false;
	for(unsigned k=0;k<Input.size();k++){
		for(unsigned l=0;l<SampleSize;l++){
			data[l]=Input[k]->get_Value(l);
			}
		gsl_wavelet_transform_forward(w,data,Level,SampleSize,work);
		Energy=0;
		for(unsigned l=SampleSize/2;l<SampleSize;l++){
			Energy+=pow(data[l],2);
			}
		if(Energy>=Threshold)
			detect=true;
		}
	Output->insert_Value(detect);
	}
*/
SumN::SumN(Channel<analogic>*CH_OUT,Channel<analogic>*CH_IN,unsigned beg,unsigned end){
	FunctionalDescription_set("SumN"); 
	Output=CH_OUT;
	Input=CH_IN;
	Begin=beg;
	End=end;
	}
SumN::SumN(Channel<analogic>*CH_OUT,Channel<analogic>*CH_IN,unsigned end){
	FunctionalDescription_set("SumN"); 
	Output=CH_OUT;
	Input=CH_IN;
	Begin=0;
	End=end;
	}
void SumN::Run(){
	analogic temp=0;
	for(unsigned k=Begin;k<=End;k++)
		temp+=Input->get_Value(k);
	Output->insert_Value(temp);
	}

MeanN::MeanN(Channel<analogic>*CH_OUT,Channel<analogic>*CH_IN,unsigned beg,unsigned end){
	FunctionalDescription_set("MeanN"); 
	Output=CH_OUT;
	Input=CH_IN;
	Begin=beg;
	End=end;
	}
MeanN::MeanN(Channel<analogic>*CH_OUT,Channel<analogic>*CH_IN,unsigned end){
	FunctionalDescription_set("MeanN"); 
	Output=CH_OUT;
	Input=CH_IN;
	Begin=0;
	End=end;
	}
void MeanN::Run(){
	analogic temp=0;
	for(unsigned k=Begin;k<=End;k++)
		temp+=Input->get_Value(k);
	Output->insert_Value(temp/(End-Begin+1));
	}

Entropy::Entropy(Channel<analogic>*CH_OUT,Channel<analogic>*CH_IN,unsigned beg,unsigned end){
	FunctionalDescription_set("Entropy"); 
	Output=CH_OUT;
	Input=CH_IN;
	Begin=beg;
	End=end;
	}
Entropy::Entropy(Channel<analogic>*CH_OUT,Channel<analogic>*CH_IN,unsigned end){
	FunctionalDescription_set("Entropy"); 
	Output=CH_OUT;
	Input=CH_IN;
	Begin=0;
	End=end;
	}
void Entropy::Run(){
	analogic temp=0;
	for(unsigned k=Begin;k<=End;k++)
		temp+=fabs(Input->get_Value(k))*log(fabs(Input->get_Value(k)));
	Output->insert_Value(temp/(End-Begin+1));
	}

Derivative::Derivative(Channel<analogic> *CH_OUT, Channel<analogic> *CH_IN){
	FunctionalDescription_set("Derivative"); 
	Output=CH_OUT;
	Input=CH_IN;
	N=1;
	}

Derivative::Derivative(Channel<analogic> *CH_OUT,Channel<analogic> *CH_IN, unsigned n){
	Output=CH_OUT;
	Input=CH_IN;
	N=n;
	if(!(N>0))
		N=1;
	if(N>CH_OUT->get_Size())
		N=CH_OUT->get_Size()-1;
	}
void Derivative::Run(){
	Output->insert_Value(1E9*Input->Sub(0,N)/MasterClock->Sub(0,N));
	}

Integrate::Integrate(Channel<analogic> *CH_OUT,Channel<analogic> *CH_IN){
	FunctionalDescription_set("Integrate"); 
	Output=CH_OUT;
	Input=CH_IN;
	N=1;
	Acc=0;
	}

Integrate::Integrate(Channel<analogic> *CH_OUT,Channel<analogic> *CH_IN,unsigned n){
	Output=CH_OUT;
	Input=CH_IN;
	N=n;
	if(!(N>0))
		N=1;
	if(N>CH_OUT->get_Size())
		N=CH_OUT->get_Size()-1;
	Acc=0;
	}
void Integrate::Run(){
	Acc+=(MasterClock->Sub(N,0)/1E9)*(Input->Add(0,N)/2.0);
	Output->insert_Value(Acc);
	}

IRestOper::IRestOper(Channel<Complex> *CH_IN_A,Channel<Complex> *CH_IN_B, Channel<analogic> *CH_REST, Channel<analogic> *CH_OPER){
	FunctionalDescription_set("IRestOper"); 
	Input_A=CH_IN_A;
	Input_B=CH_IN_B;
	Output_REST=CH_REST;
	Output_OPER=CH_OPER;
	Base=1.0;
	}
IRestOper::IRestOper(Channel<Complex> *CH_IN_A,Channel<Complex> *CH_IN_B, Channel<analogic> *CH_REST, Channel<analogic> *CH_OPER, double No){
	Input_A=CH_IN_A;
	Input_B=CH_IN_B;
	Output_REST=CH_REST;
	Output_OPER=CH_OPER;
	Base=No;
	}
void IRestOper::Run(){
	Output_REST->insert_Value((abs(Input_A->get_Value())+abs(Input_B->get_Value()))/Base);
	Output_OPER->insert_Value(abs(Input_A->get_Value()+Input_B->get_Value())/Base);
	}
	
Compose::Compose(Channel<Vanalogic> *CH_OUT){
	FunctionalDescription_set("Compose"); 
	Output=CH_OUT;
	}
void Compose::Join_Channel(Channel<Vanalogic> *CH){
	VInputs.push_back(CH);
	}
void Compose::Run(){
	Vanalogic V,Vout;
	for(unsigned k=0;k<VInputs.size();k++){
		V=VInputs[k]->get_Value();
		for(unsigned l=0;l<V.size();l++)
			Vout.push_back(V[l]);
		}
	Output->insert_Value(Vout);
	}


Division::Division(Channel<analogic> *CH_1,Channel<analogic> *CH_2,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("Div/Prod"); 
	Output=CH_OUT;
	Input_1=CH_1;
	Input_2=CH_2;
	comp=false;
	}
Division::Division(Channel<Complex> *CH_1,Channel<Complex> *CH_2,Channel<Complex> *CH_OUT){
	FunctionalDescription_set("Div/Prod"); 
	Output_C=CH_OUT;
	Input_1_C=CH_1;
	Input_2_C=CH_2;
	comp=true;
	}
void Division::Run(){
	if(comp)
		Output_C->insert_Value(Input_1_C->get_Value()/Input_2_C->get_Value());
	else
		Output->insert_Value(Input_1->get_Value()/Input_2->get_Value());
	}
	
Product::Product(Channel<analogic> *CH_1,Channel<analogic> *CH_2,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("Div/Prod"); 
	Output=CH_OUT;
	Input_1=CH_1;
	Input_2=CH_2;
	comp=false;
	}
Product::Product(Channel<Complex> *CH_1,Channel<Complex> *CH_2,Channel<Complex> *CH_OUT){
	FunctionalDescription_set("Div/Prod"); 
	Output_C=CH_OUT;
	Input_1_C=CH_1;
	Input_2_C=CH_2;
	comp=true;
	}

void Product::Run(){
	if(comp)
		Output_C->insert_Value(Input_1_C->get_Value()*Input_2_C->get_Value());
	else
		Output->insert_Value(Input_1->get_Value()*Input_2->get_Value());
	}


ZeroSequence::ZeroSequence(Channel<Complex> *CH_1,Channel<Complex> *CH_2,Channel<Complex> *CH_3,Channel<Complex> *CH_OUT){
	FunctionalDescription_set("SequenceCalc"); 
	Output=CH_OUT;
	Input_1=CH_1;
	Input_2=CH_2;
	Input_3=CH_3;
	alpha=polar(1.0, 2.0*M_PI/3.0);
	}
void ZeroSequence::Run(){
	Output->insert_Value((Input_1->get_Value()+Input_2->get_Value()+Input_3->get_Value())/Complex(3,0));
	}
void NegativeSequence::Run(){
	Output->insert_Value((Input_1->get_Value()+pow(alpha,2)*Input_2->get_Value()+alpha*Input_3->get_Value())/Complex(3,0));
	}

void PositiveSequence::Run(){
	Output->insert_Value((Input_1->get_Value()+alpha*Input_2->get_Value()+pow(alpha,2)*Input_3->get_Value())/Complex(3,0));
	}

AlphaBeta::AlphaBeta(Channel<analogic> *CH_1,Channel<analogic> *CH_2,Channel<analogic> *CH_3,Channel<analogic> *CH_4,Channel<analogic> *CH_5,Channel<analogic> *CH_6, bool Dir){
	WZero=true;
	Direction=Dir;
		Input_1=CH_1;
		Input_2=CH_2;
		Input_3=CH_3;
		Output_1=CH_4;
		Output_2=CH_5;
		Output_3=CH_6;

	}

AlphaBeta::AlphaBeta(Channel<analogic> *CH_1,Channel<analogic> *CH_2,Channel<analogic> *CH_3,Channel<analogic> *CH_4,Channel<analogic> *CH_5, bool Dir){
	WZero=false;
	Direction=Dir;
		Input_1=CH_1;
		Input_2=CH_2;
		Input_3=CH_3;
		Output_1=CH_4;
		Output_2=CH_5;

	}

bool AlphaBeta::Prepare(float t){
	return true;
}

void AlphaBeta::Run(){
	if(Direction){//ABC TO Alpha-Beta
		a=Input_1->get_Value();
		b=Input_2->get_Value();
		c=Input_3->get_Value();
		Output_1->insert_Value((2*a-b-c)/3.0);
		Output_2->insert_Value((b-c)/sqrt(3));
		if(WZero){ //Gamma
			Output_3->insert_Value((a+b+c)/3.0);
			}
		}

	else{//Alpha-Beta TO ABC
		alpha=Input_1->get_Value();
		beta=Input_2->get_Value();
		gamma=0;
		if(WZero){
			gamma=Input_3->get_Value();
			}
		Output_1->insert_Value(alpha+gamma);
		Output_2->insert_Value((-alpha+sqrt(3)*beta)/2.0+gamma);
		Output_3->insert_Value((-alpha-sqrt(3)*beta)/2.0+gamma);
		}
}


DQ0::DQ0(Channel<analogic> *CH_1,Channel<analogic> *CH_2,Channel<analogic> *CH_3,Channel<analogic> *CH_4,Channel<analogic> *CH_5,Channel<analogic> *CH_6,Channel<analogic> *CH_T, bool Dir){
	WZero=true;
	Direction=Dir;
	Theta=CH_T;
	Input_1=CH_1;
	Input_2=CH_2;
	Input_3=CH_3;
	Output_1=CH_4;
	Output_2=CH_5;
	Output_3=CH_6;

	}

DQ0::DQ0(Channel<analogic> *CH_1,Channel<analogic> *CH_2,Channel<analogic> *CH_3,Channel<analogic> *CH_4,Channel<analogic> *CH_5, Channel<analogic> *CH_T, bool Dir){
	WZero=false;
	Direction=Dir;
	Theta=CH_T;
	if(Dir){
		Input_1=CH_1;
		Input_2=CH_2;
		Input_3=CH_3;
		Output_1=CH_4;
		Output_2=CH_5;
		}
		else{
		Input_1=CH_1;
		Input_2=CH_2;
		Output_1=CH_3;
		Output_2=CH_4;
		Output_3=CH_5;
		}
	}

bool DQ0::Prepare(float t){
	return true;
}

void DQ0::Run(){
	ang=Theta->get_Value();
	if(Direction){//ABC TO D-Q
		a=Input_1->get_Value();
		b=Input_2->get_Value();
		c=Input_3->get_Value();
		Output_1->insert_Value(sqrt(2.0/3.0)*(cos(ang)*a+cos(ang-2.0*M_PI/3.0)*b+cos(ang+2.0*M_PI/3.0)*c));
		Output_2->insert_Value(sqrt(2.0/3.0)*(-sin(ang)*a-sin(ang-2.0*M_PI/3.0)*b-sin(ang+2.0*M_PI/3.0)*c));
		if(WZero){ //Zero
			Output_3->insert_Value(sqrt(2)*(a+b+c)/2.0);
			}
		}

	else{//D-Q TO ABC
		d=Input_1->get_Value();
		q=Input_2->get_Value();
		z=0;
		if(WZero){
			z=Input_3->get_Value();
			}
		Output_1->insert_Value(sqrt(2.0/3.0)*(cos(ang)*d-sin(ang)*q+sqrt(2)*z/2.0));
		Output_2->insert_Value(sqrt(2.0/3.0)*(cos(ang-2*M_PI/3)*d-sin(ang-2*M_PI/3)*q+sqrt(2)*z/2.0));
		Output_3->insert_Value(sqrt(2.0/3.0)*(cos(ang+2*M_PI/3)*d-sin(ang+2*M_PI/3)*q+sqrt(2)*z/2.0));
		}
}


Washout::Washout(Channel<analogic> *CH_1,Channel<analogic> *CH_OUT,double Tc){
	FunctionalDescription_set("Washout"); 
	Input_1=CH_1;
	Output=CH_OUT;
	d=Tc;
	}
void Washout::Run(){
	if(first_run){
		Output->insert_Value(-Input_1->get_Value());
		first_run=false;
		}
	Output->insert_Value((Input_1->get_Value(0)-Input_1->get_Value(1)-b*Output->get_Value(0))/a);
	}
bool Washout::Prepare(float fs){
	double T;
	T=1/fs;
	b=d*T/2-1;
	a=d*T/2+1;
	cout<<"a:"<<a<<" b:"<<b<<" T:"<<T<<endl;
	first_run=true;
	return true;
	}


Arg::Arg(Channel<Complex> *CH_IN,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("Arg"); 
	Input_C=CH_IN;
	Output=CH_OUT;
	}
void Arg::Run(){
	Output->insert_Value(arg(Input_C->get_Value()));
	}

Conj::Conj(Channel<Complex> *CH_IN,Channel<Complex> *CH_OUT){
	FunctionalDescription_set("Conj/Real/Imag"); 
	Input_C=CH_IN;
	Output=CH_OUT;
	}
void Conj::Run(){
	Output->insert_Value(conj(Input_C->get_Value()));
	}


void Real::Run(){
	Output->insert_Value(real(Input_C->get_Value()));
	}
void Imag::Run(){
	Output->insert_Value(imag(Input_C->get_Value()));
	}

DiffArg::DiffArg(Channel<Complex> *CH_IN1,Channel<Complex> *CH_IN2,Channel<analogic> *CH_OUT){
	FunctionalDescription_set("DiffArg"); 
	Input_C1=CH_IN1;
	Input_C2=CH_IN2;
	Output=CH_OUT;
	}
void DiffArg::Run(){
	double tmp;
	tmp=arg(Input_C1->get_Value())-arg(Input_C2->get_Value());
	if(tmp<0)
		tmp+=2*M_PI;
	Output->insert_Value(tmp);
	}

PLLdq::PLLdq(Channel<analogic> *CH_1,Channel<analogic> *CH_2,Channel<analogic> *CH_3,Channel<analogic> *CH_T){
	Input_1=CH_1;
	Input_2=CH_2;
	Input_3=CH_3;
	Theta=CH_T;
	Inital_Value=0;
	}
bool PLLdq::Prepare(float fs){
	Vd=new Channel<analogic>("Vd",5);
	Vq=new Channel<analogic>("Vq",5);
	Ws=new Channel<analogic>("Ws",5);

	transformada=new DQ0(Input_1,Input_2,Input_3,Vd,Vq,Theta, true);
	control= new PI(Vq, Ws,0.0, 10000, 10,-fs/4, fs/4, true,1.0/fs);
	integral=new Integrate(Theta, Ws);

	transformada->SetClock(MasterClock);
	control->SetClock(MasterClock);
	integral->SetClock(MasterClock);
	return true;
	}

void PLLdq::Run(){
	transformada->Run();
	control->Run();
	integral->Run();
	}

PLLdq::~PLLdq(){
	delete Vd;
	delete Vq;
	delete Ws;
	delete control;
	delete transformada;
	delete integral;
	}

ButterFilter::ButterFilter(Channel<analogic> *In,Channel<analogic> *Out, double F, long O, double A, double S){
	Order=O;
	Cut_Freq=F;
	Atennuation=A;
	Input=In;
	Output=Out;
	Scale=S;
	}

ButterFilter::ButterFilter(Channel<analogic> *In,Channel<analogic> *Out, double F, long O, double A){
	Order=O;
	Cut_Freq=F;
	Atennuation=A;
	Input=In;
	Output=Out;
	Scale=1.0;
	}
bool ButterFilter::Prepare(float Freq){
	cout<<"Famos:"<<Freq<<" Fcut:"<<Cut_Freq<<" DigFreq:"<<Cut_Freq/Freq<<endl;
	Filter= new butterworth<analogic>(Cut_Freq/Freq, Order, Atennuation);
	Filter->reset();
	return true;
	}
void ButterFilter::Run(){
	Output->insert_Value(Filter->clock(Input->get_Value()*Scale));
	}
ButterFilter::~ButterFilter(){
	delete Filter;
}
	
Limiter::Limiter(Channel<analogic> *CH_1,Channel<analogic> *CH_2, double lim_inf,double lim_sup){
	Input=CH_1;
	Output=CH_2;
	Limit_Superior=lim_sup;
	Limit_Inferior=lim_inf;
	}
void Limiter::Run(){
	analogic in,out;
	in=Input->get_Value();
	out=in;
	if(in>Limit_Superior){
		out=Limit_Superior;
		}
	if(in<Limit_Inferior){
		out=Limit_Inferior;
		}
	Output->insert_Value(out);
	}
bool Limiter::Prepare(float){
	return true;
	}


