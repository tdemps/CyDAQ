% THIS IS THE KEY, FOR TAs AND INSTUCTORS ONLY %

% Gives a list of values for 
% Fourier coeficients a_k
% and the corresponding k
% xx is the sampled function 
% startK is the starting coefficient

function [a_ks, Ks] = KEY_find_aks(xx, startK, finalK, fs)

cc = zeros((finalK - startK), 1);
kk = zeros((finalK - startK), 1);
for i = 1:(finalK - startK + 1)
    
    cc(i) = KEY_find_ak(xx, (i + startK -1), fs);
    kk(i) = finalK - i + 1;
    
end
a_ks = cc;
Ks = kk;
end