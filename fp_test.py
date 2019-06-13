import numpy as np
import cmath
import matplotlib.pyplot as plt

#first a class to form a naive wave
class waveGen:
    
    def __init__(self, freq, BufferSize, waveType, vol):
        
        self.sampleRate = 48000
        self.bufferSize = BufferSize
        self.buffer = np.empty(BufferSize, dtype=np.complex)
        self.frequency = freq
        self.omega = 0
        self.updateOmega()
        self.last = complex(1,0)
        self.type = waveType
        self.volume = vol
        self.sawLast = 0
        

    def sineGen(self):

        #start = timer() #reduce function!!! functools
        for i in range(0, self.bufferSize):
            self.buffer[i] = self.last
            self.last=self.last*self.omega

        return self.volume * np.imag(self.buffer).astype(np.float32)

    def squareGen(self):
        #start = timer()
        for i in range(0, self.bufferSize):
            self.buffer[i] = self.last
            self.last=self.last*self.omega

        return np.sign(self.volume * np.imag(self.buffer).astype(np.float32))
    
    def sawGen(self):
        wav_len = self.sampleRate / self.frequency
        half_wave_len = wav_len / 2
        
        for i in range(0, self.bufferSize):
            pos = (i + self.sawLast) % wav_len
            self.buffer[i] = ((pos / half_wave_len) - 1) * self.volume
        
        self.sawLast = (self.bufferSize + self.sawLast) % wav_len
        
        return self.buffer.astype(np.float32)
    
    def triangleGen(self):
        wav_len = self.sampleRate / self.frequency
        half_wave_len = wav_len / 2
        
        for i in range(self.bufferSize):
            self.buffer[i] = ( 2 * abs((((i + self.sawLast) % wav_len) / half_wave_len) - 1) - 1) * self.volume * -1
        
        return self.buffer.astype(np.float32)

    def updateOmega(self):
        self.omega = cmath.exp(1j*(2*cmath.pi * self.frequency / self.sampleRate))

    def updateFreq(self, val):
        self.frequency = val
        self.updateOmega()

    def nextFrame(self):
        if self.type == "sine":
            return self.sineGen()
        elif self.type == "square":
            return self.squareGen()
        elif self.type == "triangle":
            return self.triangleGen()
        elif self.type == "saw":
            return self.sawGen()

#poli-Blep class
class polyBlep:

    def __init__(self, Sample_Rate = 48000):
        
        self.twoPI = np.pi * 3 # times 3 seems go give the best result around to 
        self.freq = 0
        self.sample_rate = Sample_Rate
        self.mPhaseIncrement = 0
        self.mPhase = 0
        self.FP_Correction = 0.99999999999999999999999 # this is currently only guess at a good value, 
                                                 # it is also not a catch all value
        
    def runFrame(self, naive, Freq, waveType):
        
        self.freq = Freq
        self.mPhaseIncrement = self.freq * self.twoPI / self.sample_rate
        output = np.empty(len(naive))
        if waveType == 'square':
            for i in range(len(naive)):
                output[i] = self.nextSample_square(naive[i])
        
        elif waveType == 'saw':
            for i in range(len(naive)):
                output[i] = self.nextSample_saw(naive[i])
        
        elif waveType == 'tri':
            for i in range(len(naive)):
                output[i] = self.nextSample_tri(naive[i])
        return output
        
    def nextSample_square(self, sample):
        
        t = self.mPhase / self.twoPI
        sample += self.poly_blep(t)
        sample -= self.poly_blep((t + 0.5) % 1)
        
        self.mPhase += self.mPhaseIncrement * self.FP_Correction
            
        while self.mPhase > self.twoPI:
            self.mPhase -= self.twoPI
        
        return sample

    def nextSample_saw(self, sample):
        t = self.mPhase / self.twoPI
        sample -= self.poly_blep(t)
        
        self.mPhase += self.mPhaseIncrement * self.FP_Correction
            
        while self.mPhase > self.twoPI:
            self.mPhase -= self.twoPI
        
        return sample
    
    def nextSample_tri(self, sample): #this not working
        t = self.mPhase / self.twoPI / 2
        #sample += self.poly_blep(t)
        sample -= self.poly_blep((t + 0.5) % 1)
        
        self.mPhase += self.mPhaseIncrement * self.FP_Correction
            
        while self.mPhase > self.twoPI / 2:
            self.mPhase -= self.twoPI / 2
            
        
        return sample
    
    
    def poly_blep(self, t):
        dt = self.mPhaseIncrement / (np.pi * 2)
        # 0 <= t < 1
        if t < dt: 
            t /= dt;
            return t+t - t*t - 1.0;
    
        # -1 < t < 0
        elif t > 1.0 - dt: 
            t = (t - 1.0) / dt
            return t*t + t+t + 1.0
        
        # 0 otherwise
        else:
            return 0.0

freq = 650
sample_rate = 48000
length = 48000

wave = waveGen(freq, length, 'square', 1)
poly = polyBlep()

naive_wave = wave.nextFrame()

plt.plot(naive_wave[:750])
plt.show()

output = poly.runFrame(naive_wave, freq, 'square')



plt.plot(output[:750])
plt.show()

