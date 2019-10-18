from IIR_filt import PyIIR_filt
import soundfile as sf
import numpy as np
import scipy.signal as sig
import matplotlib.pyplot as plt
from IPython.display import Audio, display

#opening a wav file --- our trusty sitar

audio, samplerate = sf.read('sitar.wav')
buffersize = 156

#we want to zero pad the file so that it will emulate a realtime stream
audio = np.append(audio, np.full(buffersize - (len(audio) % buffersize), 0.0))

print('samplerate = {}'.format(samplerate))
print('audio length = {} seconds'.format(len(audio)/samplerate))
print('array lenght = {} samples'.format(len(audio)))
#plt.plot(audio)
#plt.show()
#display(Audio(audio, rate=samplerate))

#we can put this in a genorator

def buffer_gen(audio, buffersize):

    index = 0
    while index < len(audio):
        yield(audio[index:index+buffersize])
        index += buffersize
    return

order = 2
ripple = .9
cuttoff = 1000
samplerate=samplerate
#b, a = sig.cheby1(order, ripple, cuttoff, 'low', fs=samplerate)
b, a = sig.butter(order, cuttoff, 'low', fs=samplerate)
print(b)
print(a)
#plot this for show
w, h = sig.freqz(b, a)
#plt.plot(w/np.pi/2, 20 * np.abs(h))
#plt.show()


filt = PyIIR_filt(b"lowpass", order, 1000, 1000)

filt.set_coefs(b.astype(np.float64), a.astype(np.float64))

input()

buffers = buffer_gen(audio, buffersize)

output = np.empty(0)

for buffer in buffers:
    #print('buffer A : {}'.format(buffer))
    buffer = filt.process(buffer.astype(np.float64), len(buffer))
    #print('buffer B : {}'.format(buffer))
    output = np.append(output, buffer)

print('The Original Audio')
plt.plot(audio[:500])
plt.show()
display(Audio(audio, rate=samplerate))
print(samplerate)


print('The Filtered Audio')
plt.plot(output[:500])
plt.show()
display(Audio(output, rate=samplerate))

print('The Original Audio')
plt.plot(abs(np.fft.fft(audio)))
plt.show()
display(Audio(audio, rate=samplerate))
print(samplerate)

print('The Filtered Audio')
plt.plot(abs(np.fft.fft(output)))
plt.show()
display(Audio(output, rate=samplerate))