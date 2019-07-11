#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>


#define PI 3.14159265

class FIR_filter
{
public:
    //properties
    int cutoff, slope, M, samplerate;
    float* window = NULL;
    float* i_r =NULL;
    float* B = NULL;

    //methods
    FIR_filter(int Cutoff, int Slope, int m, int Samplerate)
    {
        //curr always lowpass untill make enum
        cutoff = Cutoff;

        //ensure slope is an even number
        slope = (Slope % 2 == 0)? Slope : Slope+1;

        //ensure that M is an odd number
        M = m;
        if (m % 2 == 0) {
            M += 1;
        }

        samplerate = Samplerate;

        //zero pad a window
        window = make_window();

        //calculate the impulse responce by taking the ifft of the window

        //take values from i_r to make Beta coefficiants


        return;
    }

    //destuction
    ~FIR_filter()
    {
        if (window) {
            free(window);
        }
        if (i_r) {
            free(i_r);
        }
        return;
    }

    //called for each buffer
    int process(float* signal, int sigLen) {
        float* output = (float*) malloc(sigLen * sizeof(float));

        //iterate over vals in signal
        for (int N = M; N < sigLen; N++) {

            //iterate over coeficients to apply filter
            for (int K = 0; K < M; K++) {

                //the convolution
                output[N] += B[K] * signal[N - K];
            }
        }

        //im not sure if this is the best way of doing this, but signal will now point to the output
        //array and original signal will be freed.
        float *tmp = signal;
        signal = output;
        free(tmp);

        return 0;
    }

    //makes a window from padding ons and zeros to a hanning window
    float* make_window()
    {
        /*

        A Function to build a FD window of a lowpass using a hanning filter.

        */

        //a window
        float* full_hanning = hann(slope*2);

        float* window = (float*) malloc(samplerate * sizeof(float));

        //pass band
        int i = 0;
        for (i = 0; i < cutoff; i++) {
            window[i] = 1;
        }

        //the slope
        int j = slope / 2;
        for (i = 0; i < slope / 2; i++, j++) {
            window[i] = full_hanning[j];
        }

        //stopband
        for (i = 0; i < samplerate; i++) {
            window[i] = 0;
        }

        free(full_hanning);

        return window;
    }

    //calculates the hanning function
    float* hann(int N)
    {
        //aloc space
        float* window = (float*) malloc(N * sizeof(float));

        for (int i = 0; i < N; i++) {
            window[i] = 0.5 * (1 - cos(PI * 2 * (i / N)));
        }

        return window;
    }
};

int main(void) {

    //make random data
    srand((unsigned)time(0));
    int samplerate = 48000;

    float* noise = (float*) malloc(samplerate*sizeof(float));

    for (int i = 0; i < samplerate; i++) {
        noise[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        noise[i] = noise[i] * 2 - 1;
    }

    //feed to filter
    int cutoff = 400;
    int slope = 500;
    int M = 20; // order of the filter
    FIR_filter* filter = new FIR_filter(cutoff, slope, M, samplerate);



    //find a way to assess it
    for (int i = 0; i < 10; i++) {
        printf("%f\n",noise[i]);
    }


    //clean everything
    delete filter;
    free(noise);

    return 0;

}