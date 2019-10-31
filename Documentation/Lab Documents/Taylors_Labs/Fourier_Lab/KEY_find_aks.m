% THIS IS THE KEY, FOR TAs AND INSTUCTORS ONLY %

% Gives a list of values for 
% Fourier coeficients 
% xx is the sampled function 
% startK is the starting coefficient

function ff = KEY_find_aks(xx, startK, finalK, fs)

cc = zeros((finalK - startK), 1);
for i = 1:(finalK - startK + 1)
    
    cc(i) = KEY_find_ak(xx, (i + startK -1), fs);
    
end
ff = cc;
end