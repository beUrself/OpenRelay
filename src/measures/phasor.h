/*
# Copyright (C) 2008-2016 Rodolfo Varraschim Rocha
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

#include <relay.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <iostream>
#include <fstream>

using namespace orelay;

//! Classe RDFT.
/** Esta � uma classe que � derivada da classe Measure.*/
class RDFT: public Measure{
	public:
	    /*! Construtor da classe RDFT que calcula o fasor da Harm�nica especificada. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param Freq � a frequ�ncia fundamental do sinal de entrada.
		@param Harm � o harm�nico desejado.*/
		RDFT(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq, unsigned Harm);
		/*! Construtor da classe RDFT que calcula o fasor da Primeria Harm�nica. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param Freq � a frequ�ncia fundamental do sinal de entrada.*/
		RDFT(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq);
		bool Prepare(float);
		virtual void Run();  /*!< M�todo que executa a fun��o de medida.*/
	protected:
		Channel<analogic>* Input; /*!< Canal de entrada, o qual ser� utilizado para realizar a medi��o.*/
		Channel<Complex>* Output; /*!< Canal de sa�da, o qual ir� conter o conte�do harm�nico solicitado.*/
		unsigned Harmonic; /*!< Harm�nica que ser� calculada.*/
		float SystemFrequency;
	private:
		unsigned N, j;
};

//! Classe LS.
/** Esta � uma classe que � derivada da classe Measure.*/
class LS: public Measure{
	public:
	    /*! Construtor da classe LS que calcula o fasor de uma Harm�nica espec�fica dentro de um espectro especificado. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param Freq � a frequ�ncia fundamental do sinal de entrada.
		@param Harm � o harm�nico desejado.
		@param HarmRange � o N�mero de harm�nicas desejado.*/
		LS(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq, unsigned Harm, unsigned HarmRange);
		/*! Construtor da classe LS que calcula o fasor da Harm�nica definida, at� a quinta. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param Freq � a frequ�ncia fundamental do sinal.
		@param Harm � o harm�nico desejado.*/
		LS(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq, unsigned Harm);
		/*! Construtor da classe LS que calcula o fasor da Primeria Harm�nica. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param Freq � a frequ�ncia fundamental do sinal.*/
		LS(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq);
        bool Prepare(float);
		virtual void Run();  /*!< M�todo que executa a fun��o LS.*/
	protected:
		Channel<analogic>* Input; /*!< Canal de entrada, o qual ser� utilizado para realizar a medi��o.*/
		Channel<Complex>* Output; /*!< Canal de sa�da, o qual ir� conter o conte�do harm�nico solicitado.*/
		unsigned Harmonic; /*!< Harm�nica que ser� calculada.*/
		float SystemFrequency;
		unsigned SpecRange; /*!< N�mero de harm�nicas consideradas.*/
	private:
		unsigned N,M,k,recomp,imcomp,col;
		float nsamp;
	};

//! Classe PLL.
/** Esta � uma classe que � derivada da classe Measure.*/
class PLL: public Measure{
	public:
	    /*! Construtor da classe PLL que calcula o fasor da primeira harm�nica e a frequ�ncia. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param CH_FREQ � o canal de sa�da contendo a frequ�ncia estimada do sinal.
		@param Freq � a frequ�ncia fundamental do sinal. Valor inicial para a estima��o*/
		PLL(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,Channel<analogic> *CH_FREQ,float Freq);
		bool Prepare(float);
		virtual void Run();  /*!< M�todo que executa a fun��o PLL.*/
	protected:
		Channel<analogic>* Input; /*!< Canal de entrada, o qual ser� utilizado para realizar a medi��o.*/
		Channel<Complex>* Output; /*!< Canal de sa�da, o qual ir� conter o fasor do sinal de entrada.*/
		Channel<analogic>* Frequency; /*!< Canal de sa�da, o qual ir� conter a frequ�ncia estimada do sinal de entrada.*/
		float SystemFrequency;
	private:
		unsigned N,j;
		float u1,u2,u3,C,A_old,A_new,W_old,W_new,Fi_old,Fi_new,
		Erro,deltat,Fasor_mod,Fasor_teta,Freq_med,sample,sig_pll;
};

//! Classe RWT.
/** Esta � uma classe que � derivada da classe Measure.*/
class RWT: public Measure{
	public:
	    /*! Construtor da classe RWT que calcula o fasor da frequ�ncia fundamental. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param Freq � a frequ�ncia fundamental do sinal de entrada.*/
		RWT(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq);
		bool Prepare(float);
		virtual void Run();  /*!< M�todo que executa a fun��o de medida.*/
	protected:
		Channel<analogic>* Input; /*!< Canal de entrada, o qual ser� utilizado para realizar a medi��o.*/
		Channel<Complex>* Output; /*!< Canal de sa�da, o qual ir� conter o fasor do sinal de entrada.*/
		float SystemFrequency;
	private:
		unsigned N,k;
		Complex lam1,lam2,lam3,lam4,lam5,beta1,beta2,beta3,beta4,beta5,beta6,
                alfa,alfa_par,n1,I,I_arg1,I_arg2,I_arg3,W,W_1,W_2,W_3,W_4,W_5,W_6;
        float sig,w0,deltat,f,Am,Fm,temp,temp2;
};

//! Classe RLS.
/** Esta � uma classe que � derivada da classe Measure.*/
class RLS: public Measure{
	public:
	    /*! Construtor da classe RLS que calcula o fasor da primeira Harm�nica. */
		/** @param CH_IN � o canal de entrada.
		@param CH_OUT � o canal de sa�da contendo os fasores.
		@param Freq � a frequ�ncia fundamental do sinal de entrada.*/
		RLS(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT,float Freq);
		bool Prepare(float);
		virtual void Run();  /*!< M�todo que executa a fun��o de medida.*/
	protected:
		Channel<analogic>* Input; /*!< Canal de entrada, o qual ser� utilizado para realizar a medi��o.*/
		Channel<Complex>* Output; /*!< Canal de sa�da, o qual ir� conter o fasor do sinal de entrada.*/
		float SystemFrequency;
	private:
		unsigned N,k;
		float Ampi,Teta,deltat,fii;
		double ForgetingFactor,e;
        double Roi_acum[2][2], aoi_acum[2][1];
};

//! Classe DWT.
/** Esta � uma classe que � derivada da classe Measure.*/
class DWT: public Measure{
    public:
        /*! Construtor da classe DWT que calcula o fasor da frequ�ncia fundamental. */
		/** @param CH_IN � o canal de entrada.
		@param N � o n�mero de amostras/ciclo do sinal.
		@param Family � a fun��o wavelet a ser usada.*/
        DWT(Channel<analogic> *CH_IN,Channel<Complex> *CH_OUT, unsigned N, unsigned Family);
        /*! Construtor da subclasse Leafs associada � DWT para calculo do fasor da frequ�ncia fundamental. */
		/** @param ad Define se aproxima��o ou detalhe � desejado.
		@param Nad � a folha desejada para o nivel de aproxima��o especificado.
		@param Level � o nivel de interesse para decomposi��o wavelet.*/
        void Leafs(unsigned char ad,unsigned Nad,unsigned Level);
        virtual void Run(); /*!< M�todo que executa a fun��o de medida.*/
        bool Prepare(float);
    protected:
        bool getFilterBank();
        Channel <analogic> *Input; /*!< Canal de entrada, o qual ser� utilizado para realizar a medi��o.*/
        Channel <Complex> *Output; /*!< Canal de sa�da, o qual conter� os fasores da primeira harm�nica.*/
        vector<double> FilterBank;
        vector<double> AR1;
        vector<double> AR2;
        double mAR1, mAR2;
        unsigned NSamples;
        unsigned FamilyName;
        unsigned Method;
        vector<unsigned char> AD;
        vector<unsigned> Nivel,Pos;
        vector<unsigned> Ai,Di,Af,Df;
        vector<unsigned> PosIni,PosFim;
        vector<bool> Ordem;
        bool neworder;
        void Transformada_Wavelet(double* f, long n,bool ordem);
};

