% THIS IS THE KEY, FOR TAs AND INSTUCTORS ONLY %


function synth = KEY_fsynt(C, K, f, fs)

% Find Omega wich is (2*pi)/Period
% Frequency is the inverse of the Period
w0 = (2*pi)*f;

% make time vector
t = 0:(1/fs):(1/f);

% Premake the vector
temp = zeros(1, length(t));

for i = 1:length(C)
    
    temp = temp + C(i) * exp(j*w0*t*K(i));
    
end

synth = temp;

end