/*
# Copyright (C) 2008-2015 Renato Machado Monaro, Vinicius de Cillo Moro
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
*/
#ifndef PROTECTION_H
#define PROTECTION_H

//#include <boost/thread/barrier.hpp>
//#include <boost/thread/thread.hpp>

#include "channel.h"
#include "parameters.h"
#include "types.h"
namespace orelay{
enum {IEEE_EI=0,IEEE_VI,IEEE_MI,IEC_A,IEC_B,IEC_C,IEC_SI,IAC_EI,IAC_VI,IAC_I,IAC_SI,I2T};
enum {OVER_MAGNITUDE=0,UNDER_MAGNITUDE};
//! Classe Protection.
/** Esta é uma classe abstrata que serve como base para as outras classes empregadas nos algoritmos de proteção.*/

class Protection
	{
	public:
		/*! Método que retorna uma string com a descrição funcional da proteção.*/
		/** @return Retorna a descrição funcional da proteção.*/
		string FunctionalDescription_get(void);
		/*! Método que retorna uma string com o Código ANSI da proteção.*/               
		/** @return Retorna o código ANSI da proteção.*/		
		string ANSICode_get(void);              
//		virtual void Parametrization_set(void){};
//		void Parametrization_print()(void){};
		void SetClock(Channel<timer> *);    /*!< Definição do ponteiro de tempo para a proteção.*/       
		virtual void Run()=0;              /*!< Método virtual pura necessária para a execução das funções de proteção.*/
		virtual bool Prepare(float)=0;              /*!< Método virtual pura necessária para a execução das funções de proteção.*/      
	protected:
		void FunctionalDescription_set(string);/*!< Método que atribui a descrição funcional à proteção.*/
		void ANSICode_set(string);             /*!< Método que atribui o código ANSI à proteção.*/ 
		string FunctionalDescription;          /*!< Atributo que armazena a descrição funcional.*/
		string ANSICode;                       /*!< Atributo que armazena o código ANSI.*/
		Channel<timer> *MasterClock;      /*!< Atributo que armazena o ponteiro de tempo usado no relé.*/
	};
//! Classe Magnitude.
/** Esta é uma classe abstrata derivada da classe Protection.*/
class Magnitude: public Protection
	{
	protected:
		unsigned Comparation; /*!< Atributo que armazena se é OverMagnitude ou UnderMagnitude.*/ 
		analogic PickUp; /*!< Atributo que armazena o valor de disparo (Pick Up).*/ 
		Channel<analogic> *Input_ANAL; /*!< Canal de entrada Analógico, o qual será monitorado pela proteção.*/
		Channel<Complex> *Input_CPLX; /*!< Canal de entrada Complexo, o qual será monitorado pela proteção.*/
		analogic Input;/*!< Armazena o Módulo da entrada.*/
		Channel<digital> *Output; /*!< Canal de saída, o qual define o status atual da proteção.*/
		bool Input_Type; /*Contém a informação referente à natureza do sinal de entrada (Complex=true,Analogic=False).*/
		float dt;
	};

//! Classe Magnitude.
/** Esta é uma classe abstrata derivada da classe Protection.*/
class InstantaneousMagnitude: public Magnitude{
	protected:
		void Run(); /*!< Método para executar a função de proteção.*/
		bool Prepare(float); /*!< Método para preparar a função de proteção.*/		
};
//! Classe Magnitude.
/** Esta é uma classe abstrata derivada da classe Protection.*/
class DefiniteTimeMagnitude: public Magnitude{
	protected:
		double DefiniteTime;  /*!< Atributo que armazena o tempo de espera do relé.*/
		double DefiniteIntegralTime; /*!< Atributo que armazena o tempo decorrido após o PickUp.*/
		void Run(); /*!< Método para executar a função de proteção.*/	
		bool Prepare(float); /*!< Método para preparar a função de proteção.*/		
};

//! Classe InstantaneousOverVoltage.
/** Esta é uma classe derivada da classe Magnitude. */
class InverseTimeMagnitude: public Magnitude{
	protected:
		double TDM; /*!< Atributo que armazena o dial (multiplicador) de tempo.*/
		unsigned CurveType; /*!< Atributo que armazena tipo da curva.*/
		bool Resetable; /*!< Atributo que armazena se será usado o retorno do disco.*/
		double A,B,P,Tr,K,E,C,D; /*!< Atributos que são utilizados na fórmula para o cálculo do tempo de atuação do relé.*/ 
		double InverseIntegralTime; /*!< Atributo que armazena o somatório do inverso do tempo durante cada execução.*/
		void Run(); /*!< Método para executar a função de proteção.*/	
		bool Prepare(float); /*!< Método para preparar a função de proteção.*/		
		void MakeCurve(); /*!< Método que cria a curva a ser utilizada na proteção.*/	
};

//! Classe InstantaneousOverVoltage.
/** Esta é uma classe derivada da classe InstantaneousMagnitude. */
class InstantaneousOverVoltage: public InstantaneousMagnitude
	{
	public:
		/*!Construtor da classe InstantaneousOverVoltage, dedicada à proteção de Sobre-Tensão instantânea.*/
		/** @param In é a entrada (analógica) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousOverVoltage(Channel<analogic>* In, Channel<digital>* Out, analogic Pick);
		/*!Construtor da classe InstantaneousOverVoltage, dedicada à proteção de Sobre-Tensão instantânea.*/
		/** @param In é a entrada (fasor) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousOverVoltage(Channel<Complex>* In, Channel<digital>* Out, analogic Pick);
	};

//! Classe InstantaneousUnderVoltage.
/** Esta é uma classe derivada da classe InstantaneousMagnitude, dedicada à proteção de Sub-Tensão instantânea.*/
class InstantaneousUnderVoltage: public InstantaneousMagnitude
	{
	public:
		/*!Construtor da classe UnderVolage.*/
		/** @param In é a entrada (analógica) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousUnderVoltage(Channel<analogic>* In, Channel<digital>* Out, analogic Pick);
		/*!Construtor da classe UnderVolage.*/
		/** @param In é a entrada (fasor) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousUnderVoltage(Channel<Complex>* In, Channel<digital>* Out, analogic Pick);
	};
//! Classe  InstantaneousOverCurrent.
/** Esta é uma classe derivada da classe InstantaneousMagnitude, dedicada à proteção de Sobre-Corrente instantânea.*/
class InstantaneousOverCurrent: public InstantaneousMagnitude
	{
	public:
		/*!Construtor da classe OverCurrent.*/
		/** @param In é a entrada (analógica) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousOverCurrent(Channel<analogic>* In, Channel<digital>* Out, analogic Pick);
		/*!Construtor da classe OverCurrent.*/
		/** @param In é a entrada (fasor) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousOverCurrent(Channel<Complex>* In, Channel<digital>* Out, analogic Pick);
	};
//! Classe InstantaneousUnderCurrent.
/** Esta é uma classe derivada da classe InstantaneousMagnitude, dedicada à proteção de Sub-Corrente instantânea.*/
class InstantaneousUnderCurrent: public InstantaneousMagnitude
	{
	public:
		/*!Construtor da classe UnderCurrent.*/
		/** @param In é a entrada (analógica) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousUnderCurrent(Channel<analogic>* In, Channel<digital>* Out, analogic Pick);
		/*!Construtor da classe UnderCurrent.*/
		/** @param In é a entrada (fasor) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param Pick é o valor de pick up usado para comparação e atuação da proteção.*/
		InstantaneousUnderCurrent(Channel<Complex>* In, Channel<digital>* Out, analogic Pick);
	};

//! Classe InverseTimeOverCurrent.
/** Esta é uma classe derivada da classe InverseTimeMagnitude, dedicada à proteção de Sobre-Corrente com tempo de atuação inverso.*/
class InverseTimeOverCurrent: public InverseTimeMagnitude 
	{
	public:	
	/*!Construtor da classe InverseTimeOverCurrent.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeOverCurrent(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
	/*!Construtor da classe InverseTimeOverCurrent.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).
	@param Reset define se será usado ou não o tempo de retorno do disco (reset time).*/
	InverseTimeOverCurrent(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type, bool Reset);
	/*!Construtor da classe InverseTimeOverCurrent.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeOverCurrent(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
	/*!Construtor da classe InverseTimeOverCurrent.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).
	@param Reset define se será usado ou não o tempo de retorno do disco (reset time).*/
	InverseTimeOverCurrent(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type, bool Reset);

};


//! Classe InverseTimeUnderCurrent.
/** Esta é uma classe derivada da classe InverseTimeMagnitude, dedicada à proteção de Sub-Corrente com tempo de atuação inverso.*/
class InverseTimeUnderCurrent: public InverseTimeMagnitude 
	{
	public:	
	/*!Construtor da classe InverseTimeUnderCurrent.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeUnderCurrent(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
	/*!Construtor da classe InverseTimeUnderCurrent.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeUnderCurrent(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
};


//! Classe InverseTimeOverVoltage.
/** Esta é uma classe derivada da classe InverseTimeMagnitude, dedicada à proteção de Sobre-Tensão com tempo de atuação inverso.*/
class InverseTimeOverVoltage: public InverseTimeMagnitude 
	{
	public:	
	/*!Construtor da classe InverseTimeOverVoltage.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeOverVoltage(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
	/*!Construtor da classe InverseTimeOverVoltage.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeOverVoltage(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
};


//! Classe InverseTimeUnderVoltage.
/** Esta é uma classe derivada da classe InverseTimeMagnitude, dedicada à proteção de Sub-Voltage com tempo de atuação inverso.*/
class InverseTimeUnderVoltage: public InverseTimeMagnitude 
	{
	public:	
	/*!Construtor da classe InverseTimeUnderVoltage.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeUnderVoltage(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
	/*!Construtor da classe InverseTimeUnderVoltage.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o multiplicador de tempo.
	@param Type é o tipo de curva (um dos três tipos da curva IEEE ou um dos quatro da IEC ou ainda um dos quatro da IAC).*/
	InverseTimeUnderVoltage(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial, unsigned Type);
};


//! Classe DefiniteTimeUnderVoltage.
/** Esta é uma classe derivada da classe DefiniteTimeMaginitude, dedicada à proteção de Sub-Tensão com tempo de atuação definido e fixo.*/
class DefiniteTimeUnderVoltage: public DefiniteTimeMagnitude 
	{
	public:	
	/*!Construtor da classe DefiniteTimeUnderVoltage.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeUnderVoltage(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial);
	/*!Construtor da classe DefiniteTimeUnderVoltage.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeUnderVoltage(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial);
};

//! Classe DefiniteTimeOverVoltage.
/** Esta é uma classe derivada da classe DefiniteTimeMaginitude, dedicada à proteção de Sobre-Tensão com tempo de atuação definido e fixo.*/
class DefiniteTimeOverVoltage: public DefiniteTimeMagnitude 
	{
	public:	
	/*!Construtor da classe DefiniteTimeOverVoltage.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeOverVoltage(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial);
	/*!Construtor da classe DefiniteTimeOverVoltage.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de tensão usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeOverVoltage(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial);
};

//! Classe DefiniteTimeUnderCurrent.
/** Esta é uma classe derivada da classe DefiniteTimeMaginitude, dedicada à proteção de Sub-Corrente com tempo de atuação definido e fixo.*/
class DefiniteTimeUnderCurrent: public DefiniteTimeMagnitude 
	{
	public:	
	/*!Construtor da classe DefiniteTimeUnderCurrent.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeUnderCurrent(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial);
	/*!Construtor da classe DefiniteTimeUnderCurrent.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeUnderCurrent(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial);
};

//! Classe DefiniteTimeOverCurrent.
/** Esta é uma classe derivada da classe DefiniteTimeMaginitude, dedicada à proteção de Sobre-Corrente com tempo de atuação definido e fixo.*/
class DefiniteTimeOverCurrent: public DefiniteTimeMagnitude 
	{
	public:	
	/*!Construtor da classe DefiniteTimeOverCurrent.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeOverCurrent(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial);
	/*!Construtor da classe DefiniteTimeOverCurrent.*/
	/** @param In é a entrada (fasor) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeOverCurrent(Channel<Complex>* In, Channel<digital>* Out, analogic Pick, double Dial);
};


//! Classe Differencial.
/** Esta é uma classe derivada da classe Protection, dedicada à proteção diferencial.*/
class Differential: public Protection
	{
	public:
		/*!Construtor da classe Differencial.*/
		/** @param In1 é a entrada 1 (analógica) a ser monitorada pela proteção.
		@param In2é a entrada 2 (analógica) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param In é a corrente nominal.
		@param PickUp1 é o valor de pick-up baixo (pu).
		@param BasePoint1 Ponto base da primeira declividade (pu).
		@param Slope1 Primeira declividade (%).
		@param BasePoint2 Ponto base da segunda declividade (pu).
		@param Slope2 Segunda declividade (%).
		@param PickUp2 é o valor de pick-up alto (pu).*/
		Differential(Channel<Complex>* In1, Channel<Complex>* In2, Channel<digital>* Out, double In, double PickUp1, double BasePoint1, double Slope1, double BasePoint2, double Slope2,double PickUp2);
		void Run(); /*!< Método para executar a função de proteção.*/	
		bool Prepare(float); /*!< Método para preparar a função de proteção.*/	
		void PrintCurve(); /*!< Método que imprime a curva de proteção.*/	
	protected:
		double PickUp1; /*!< Pick-up baixo (pu)*/
		double PickUp2; /*!< Pick-up alto (pu)*/
		double BasePoint1; /*!< Ponto base onde a primeira declividade  cruza o eixo ordenado em pu da curva de proteção.*/
		double Slope1; /*!< Primeira declividade da curva de proteção dada em porcentagem (0-100%).*/
		double BasePoint2; /*!< Ponto base onde a segunda declividade  cruza o eixo ordenado em pu da curva de proteção.*/
		double Slope2; /*!< Segunda declividade da curva de proteção dada em porcentagem (0-100%).*/
		Channel<Complex> *Input1; /*!< Canal de entrada, corrente de entrada do elemento protegido.*/
		Channel<Complex> *Input2; /*!< Canal de entrada, corrente de saída do elemento protegido.*/
		Channel<digital> *Output; /*!< Canal de saída, o qual define o status atual da proteção.*/
	private:
		double Break1, Break2, Break3, Ir,Io;
		double a1,b1,a2,b2,INorm;
	
};

//! Classe DifferencialSpline.
/** Esta é uma classe derivada da classe Protection, dedicada à proteção diferencial.*/
class DifferentialSpline: public Protection
	{
	public:
		/*!Construtor da classe DifferentialSpline.*/
		/** @param In1 é a entrada 1 (analógica) a ser monitorada pela proteção.
		@param In2é a entrada 2 (analógica) a ser monitorada pela proteção.
		@param Out é a saída (digital) que indica o status atual da proteção.
		@param In é a corrente nominal.
		@param PickUp1 é o valor de pick-up baixo (pu).
		@param BreakPoint1 Ponto final da atuação da declividade 1.
		@param Slope1 Primeira declividade (%).
		@param BreakPoint2 Ponto inicial da atuação da declividade 2.
		@param Slope2 Segunda declividade (%).*/
		DifferentialSpline(Channel<Complex>* In1, Channel<Complex>* In2, Channel<digital>* Out, double In, double PickUp, double BreaKPoint1, double Slope1, double BreakPoint2, double Slope2);
		void Run(); /*!< Método para executar a função de proteção.*/	
		bool Prepare(float); /*!< Método para preparar a função de proteção.*/		
	protected:
		double PickUp1; /*!< Pick-up baixo (pu)*/
		double PickUp2; /*!< Pick-up alto (pu)*/
		double BreakPoint1; /*!< Ponto base onde a primeira declividade  cruza o eixo ordenado em pu da curva de proteção.*/
		double Slope1; /*!< Primeira declividade da curva de proteção dada em porcentagem (0-100%).*/
		double BreakPoint2; /*!< Ponto base onde a segunda declividade  cruza o eixo ordenado em pu da curva de proteção.*/
		double Slope2; /*!< Segunda declividade da curva de proteção dada em porcentagem (0-100%).*/
		Channel<Complex> *Input1; /*!< Canal de entrada, corrente de entrada do elemento protegido.*/
		Channel<Complex> *Input2; /*!< Canal de entrada, corrente de saída do elemento protegido.*/
		Channel<digital> *Output; /*!< Canal de saída, o qual define o status atual da proteção.*/
		double Spline(); /*!< Método que determina a atuação do relé na região de transição entre as duas inclinações.*/
		void PrintCurve(); /*!< Método que imprime a curva de proteção.*/
	private:
		double Break1, Break2, Break3, Ir,Io;
		double a1,b1,a2,b2,INorm,t,p0,m0,p1,m1;
	
};

//! Classe DefiniteTimeUnderFrequency.
/** Esta é uma classe derivada da classe DefiniteTimeMaginitude, dedicada à proteção de Sub-Corrente com tempo de atuação definido e fixo.*/
class DefiniteTimeUnderFrequency: public DefiniteTimeMagnitude 
	{
	public:	
	/*!Construtor da classe DefiniteTimeUnderFrequency.*/
	/** @param In é a entrada (analógica) a ser monitorada pela proteção.
	@param Out é a saída (digital) que indica o status atual da proteção.
	@param Pick é o valor de pick up de corrente usado nesta função de proteção.
	@param Dial é o dial de tempo, ou seja, o atraso de tempo(s).*/
	DefiniteTimeUnderFrequency(Channel<analogic>* In, Channel<digital>* Out, analogic Pick, double Dial);
};
}
#endif /* !PROTECTION_H */
