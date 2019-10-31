% This function finds the Fourier
% Series coefficients
% T is the Period
% Computes the fourier coefficients
% x is the sampled funciton 
% k is the coeeficient number
% fs is the sample rate
function f = find_ak(x, k, fs)

% The sample time Ts is 1/sampleRate
Ts = 1/fs;

% The period T is 1
T = length(x)*Ts;

w0 = (2*pi)/T;

t = 1:length(x);
t = t * Ts;

% The complex part
complex = exp(j*k*w0*t);

% Rotate it so it is a nx1 matrix
complex = rot90(complex);

% the complex part mulitiplied 
% by the function 
vectr = x.*complex;

% the Reimen sum * (1/period)
f = (Ts/T)*(sum(vectr));

end
