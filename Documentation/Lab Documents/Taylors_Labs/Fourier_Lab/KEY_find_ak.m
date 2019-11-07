% THIS IS THE KEY, FOR TAs AND INSTUCTORS ONLY %

function a_k = KEY_find_ak(x, k, fs)

% x is a column vector
% rotate it to become a row vector

x = rot90(x);

% The sample time Ts is 1/sampleRate
Ts = 1/fs;

% The period T is 1
T = length(x)*Ts;

w0 = (2*pi)/T;

t = 1:length(x);
t = t * Ts;

% The complex part
complex = exp(j*k*w0*t);

% the complex part mulitiplied 
% by the function 
func = x.*complex;

% the Reimen sum * (1/period)
a_k = (Ts/T)*(sum(func));

end