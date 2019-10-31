% C is the vector of a_k coeficients
% f is the desired frequency to recreate
% fs is the desired new sample rate
% It can be used to control the 
% amount of sample time between functions
% ss is the starting point of a_k in C
% ff is the ending point of a_k in C
% but ff may or may not be necessary
% depending on the implementation

function synth = fsynt(C, f, fs, ss, ff)

% Find Omega wich is (2*pi)/Period
% Frequency is the inverse of the Period
w0 = (2*pi)*f;

% make time vector
t = 0:(1/fs):(1/f);

% Premake the vector
temp = zeros(1, length(t));

for i = 1:length(C)
    
    temp = temp + C(i) * exp(j*w0*t*(ss+i-1));
    
end

synth = temp;

end


