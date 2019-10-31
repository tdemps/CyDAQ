function [ ] = FFTPlot( data, Fs )
%Inputs:
%   data: an input vector. This is the 'data' vector returned by the CyDAQ
%   Fs: the sampling Frequency, for this lab we're using 8000 as the input.
%Outputs:
    % there should not be any outputs designated. The function will create
    % a plot, but no output variable will be stored. As a result, the
    % students will be able to use this similarly to the built-in plot()
    % function.
T = 1/Fs; % Define period as a function of Fs
L = length(data);
Y = fft(data);
P2 =abs(Y/L); % Define the two sided spectrum
P1 = P2(1:L/2+1); % Reduce to a one sided spectrum
P1(2:end - 1) = 2*P1(2:end-1); %Adjust represent power for both +/- freq
f= Fs*(0:(L/2))/L; % Define a vector of frequencies to plot spectrum against
P1(1) = 0; % Remove DC offset at 0 Hz Measurement from CyDAQ
plot(f,P1); %Create the desired plot

end

