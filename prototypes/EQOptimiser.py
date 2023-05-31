#!/usr/bin/env python
# coding: utf-8

#Python libraries to import
import control
import numpy as np
import matplotlib.pyplot as plt
import scipy.optimize as opt

#implements the transfer function of a s-domain bell filter
def bell_filter(s, Fc, Gain_dB, Q):
    Wc = 2 * np.pi * Fc #convert center frequency from Hz to Rad/s
    s = s / Wc #define 's'
    G = 10**(Gain_dB/20) #convert gain from dB to linear
    if G > 1:
        num = (s**2) + (G * s * 1/Q) + 1 #calculate transfer function numerator
        den = (s**2) + (1/Q * s) + 1 #calculate transfer function denominator 
    else:
        num = (s**2) + (s * 1/Q) + 1
        den = (s**2) + (1/Q * s/G) + 1
    return num / den #return the bell filter transfer function 

#test bell filter function (should produce a bell filter with a center frequency of 1000Hz at 6dB)
omega = np.logspace(np.log10(20), np.log10(20000), 1000) #frequency vector in hertz(Hz)
omega *= np.pi * 2 #convert frequency vector to angular frequency
s = control.tf('s') #create an s-domain transfer function
Fc = 1000 #center frequency in hertz(Hz)
H = bell_filter(s, Fc, 6, 10)
print("S-Domain Transfer Function: \n", H)
#plots a bode plot for the system (transfer function) returns magnitude, phase in rads and frequency in rad/s 
mag_eq, phase_eq, w_eq = control.bode_plot(H, Hz = True, dB=True, omega=omega)

#computes the overall transfer function of an N-band EQ as the product of the transfer functions for each band
def eq_response(Fcs, Gains, Qs, num_bands):
    H_actual = 1
    for i in range(0,num_bands):
        H_actual = H_actual * bell_filter(s, Fcs[i], Gains[i], Qs[i])     
    return H_actual

#define parameters for N-bands
num_bands = 8 #num EQ bands
Fcs = [100, 200, 300, 400, 500, 600, 700, 800] #frequency in hertz(Hz) for each band
Gains = [6, 6, 6, 6, 6, 6, 6, 6] #gain in dB for each band
Qs = [10, 10, 10, 10, 10, 10, 10, 10] #Q-factor for each band

# Plot the frequency response of each band
for i in range(num_bands):
    H_band = bell_filter(s, Fcs[i], Gains[i], Qs[i])
    mag, phase, w = control.bode(H_band, Hz=True, dB=True, omega=omega)

#compute the transfer function for the whole N-band EQ and plot the frequency response
H_eq = eq_response(Fcs, Gains, Qs, num_bands)
mag, phase, w = control.bode(H_eq, Hz=True, dB=True, omega=omega)

#define a desired frequency reponse to test the optimisation against (this will be the drawn response in the plugin)
# Generate N randomised center frequencies between 20Hz-20000Hz
Fc_desired = np.sort(np.random.uniform(np.log10(20), np.log10(20000), num_bands))
Fc_desired = np.power(10, Fc_desired)

# Calculate transfer function for desired response
G_desired = np.random.uniform(-12, 12, num_bands) #Generate N randomised gains between +-12dB
Q_desired = np.random.uniform(0.5, 20, num_bands) #Generate N randomised Q values between 0.5-20
H_desired = eq_response(Fc_desired, G_desired, Q_desired, 8) #obtain the transfer function for the N-band EQ
mag_desired, phase_desired, w_desired = control.bode(H_desired, Hz=True, dB=True, omega=omega)

#objective function that can be passed to an optimiser to minimise
def objective(params):
    Fcs = params[:num_bands] #center frequency values
    Gains = params[num_bands:2*num_bands] #gain values
    Qs = params[2*num_bands:] #qulity factor values
    H_actual = eq_response(Fcs, Gains, Qs, num_bands) #calculate eq response
    mag_actual, phase_actual, w_actual = control.bode(H_actual, Hz=True, dB=True, omega=omega) #mag, phase and omega from eq_response
    dB_desired = 20 * np.log10(mag_desired) #desired response in dB
    dB_actual = 20 * np.log10(mag_actual) #actual response in dB
    mag_error = (dB_actual - dB_desired)**2 #calculate squared difference of magnitude responses (dB)
    return np.mean(mag_error) #return the mean of the mag error

# set initial parameters for each band
Fc_init = np.logspace(np.log10(20), np.log10(20000), num_bands) #log spaced frequency values for each band
G_init = np.array([0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]) #constants for initial gain values for each band
Q_init = np.array([0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]) #constants for initial Q values for each band
initial_params = np.hstack((Fc_init, G_init, Q_init)) #create an initial parameter vector

#set optimisation bounds for frequency, gain and Q-factor
bounds = [(20, 20000)]*num_bands + [(-12, 12)]*num_bands + [(0.5, 20)]*num_bands 

# minimize the objective function using the gradient-based L-BFGS-B method
result = opt.minimize(objective, initial_params, method='L-BFGS-B', bounds=bounds, options={'gtol': 1e-6, 'disp': True, 'maxiter': 250})

optimized_params = result.x #parameter decided by the optimiser 
H_opt = eq_response(optimized_params[:num_bands], optimized_params[num_bands:2*num_bands], optimized_params[2*num_bands:], num_bands)
mag_opt, phase_opt, w_opt = control.bode(H_opt, Hz=True, dB=True, omega=omega)
mag_desired, phase_desired, w_desired = control.bode(H_desired, Hz=True, dB=True, omega=omega)

freqs = np.logspace(np.log10(20), np.log10(20000), 1000) #frequency vector

# plot the desired "drawn" and optimised magnitude responses for comparison 
plt.plot(freqs, 20*np.log10(mag_desired), label='Desired')
plt.plot(freqs, 20*np.log10(mag_opt), label='Optimized')
plt.xscale('log')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude (dB)')
plt.title('Magnitude Response')
plt.legend()
plt.grid()
plt.show()
