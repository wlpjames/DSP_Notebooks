 /*==============================================================================

	IRR_filter.h
	Created: 16 Aug 2019 4:00:26pm
	Author:  billy

  ==============================================================================
*/

#include <map>
#include <string>
#include <cstdlib>
#include <string>
#include <iostream>
//#include "../JuceLibraryCode/JuceHeader.h"
//#include "messages.h"
//#include "liquid.h"
#include "rollingBuffer.h"
using namespace std;

namespace filters {
    class IIR_filter
    {
    public:

    	//Atributes

    	int samplerate = 0;

    	float* A;
    	float* B;

    	int M;
    	float freq;
    	float slope;
    	const char* filt_type;
        
        //rollingBuffer<float> cleanBuffer;
        float last_overlap[100];
        float last_clean_overlap[100];

    	//init
    	IIR_filter(const char* type, int order, float init_freq = 1000, float init_slope = 10000)
        //:cleanBuffer(1024)
        {
            //settings for the filter
            M = order;
            freq = init_freq;
            slope = init_slope;
            filt_type = type;
            
            //filter coeficiants want memcpy
            A = (float*) malloc(sizeof(float) * order);
            B = (float*) malloc(sizeof(float) * order);
            //calc_coefs(init_freq, init_slope, order, A, B);
            
            for (int i = 0; i < M; i++) {
                last_overlap[i] = 0.0;
            }
            
        }
        ~IIR_filter()
        {
            free(A);
            free(B);
            //free(last_overlap);

        }
/*
    	//methods
    	int set_params(message* params)
        {
            const char* key = params->get_key <const char*>();
            
            if (strcmp(key, "cutoff") == 0) {
                freq = (params->get_value <float>() > 0) ? params->get_value <float>() : freq; //protectiuon as 0 causes errors -> this could be done better
            }
            else if (strcmp(key, "slope") == 0) {
                slope = params->get_value <float>();
            }
            else if (strcmp(key, "samplerate") == 0) {
                samplerate = params->get_value <int>();
            }
            calc_coefs(freq, slope, M, B, A);
            return 0;
        }
*/      
        void set_coefs(float* b, float* a) {
            
            for(int i = 0; i < M; i++){
                std::cout << "\nb : " << i << " : " << b[i];
                std::cout << "\na : " << i << " : " << a[i];
                B[i] = b[i];
                A[i] = a[i];
            }
        }
        
        /*
        int calc_coefs(float freq, int slope, int order, float* B, float* A)
        {
            //a function that can calulate the coeficiants given the parameters using cheby
            
            if (samplerate==0) {
                return 1;
            }

            float _CO = freq / (samplerate);
            
            liquid_iirdes(LIQUID_IIRDES_BUTTER, //alg
                          LIQUID_IIRDES_LOWPASS, //type
                          LIQUID_IIRDES_TF, // outpout (ab)
                          M-1, //order
                          _CO, //cutoff
                          0,
                          0.2f,
                          0.2f,
                          B,
                          A
                          );
            
            cout << "\nB coefs\n";
            for (int i = 0; i < M; i++) {
                cout << B[i] << ": ";
            }
            
            cout << "\n A coefs\n";
            for (int i = 0; i < M; i++) {
                cout << A[i] << ": ";
            }
            return 1;
        }
        */
        
    	int process(float* signal, int siglen)
        {
            
            //create new array here by pre-pending the last clean signal to the signal
            float input [siglen];
            //cleanBuffer.read_ind = cleanBuffer.write_ind - M; //just to make sure because i dont trust myself
            //cleanBuffer.write_chunk(signal, siglen);
            //cleanBuffer.read_chunk(input, siglen);
            for (int i = 0; i < M; i++) {
                input[i] = last_clean_overlap[i];
                last_clean_overlap[i] = input[(siglen-M)+i];
            }
            for (int i = M; i < siglen; i++) {
                input[i] = signal[i];
            }
            
            //create an output that can be convolved agaist
            //float output [siglen]; // = (float*)calloc(sizeof(float), siglen);
            float output[siglen+M];// = (float*)calloc(siglen, sizeof(float));
            for(int i = 0; i < siglen+M; i++){
                output[i] = 0;
            }
            //memcpy(output, last_overlap, sizeof(float) * M);
            for (int i = 0; i < M; i++) {
                output[i] = last_overlap[i];
            }
            
            //do the convolution
            for (int N = M; N < siglen+M; N++) {
                
                //the first coef is not effected by feedback
                output[N] = B[0] * input[N];
                
                //acumilate the rest of the coeficiants
                for (int K = 1; K < M; K++) {
                    
                    float feedback = A[K] * output[N - K];
                    
                    output[N] += (B[K] * input[N - K]) - feedback;
                }
            }
            
            
            //arange data for the next time the buffer is called
            //memcpy(last_overlap, output + siglen, sizeof(float) * M);
            //this is if output was too long?
            for (int i = 0; i < M; i++) {
                last_overlap[i] = output[siglen + i];
            }
            //memcpy(signal, output, siglen * sizeof(float));
            for (int i = 0; i < siglen; i++) {
                signal[i] = output[i+M];
            }
            //free(output);
            //free(input);
            
            return 0;
        }
        
    };
}
