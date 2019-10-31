% To load the information of a .wav file 
% [audioIn, fs] = audioread('audioIn.wav');

% f0 = pitch(audioIn, fs) %gives you just the pitch
% fs is sample frequency
% audioIn is an array of samples

% [f0,idx] = pitch(audioIn,fs); %creates an index of fundamental fequencies
% for more information, google "matlab pitch" 
% most of this function was taken from the MathWorks page
% it is a wondereful reasorce

% This function takes a .wav file and then plots the fundamental frequency 
% over a given time

function PlotPitch(file, plotTitle)
    [audioIn,fs] = audioread(file);
    [f0,idx] = pitch(audioIn,fs);
    
    title(plotTitle);
    
    subplot(2,1,1);
    plot(audioIn);
    ylabel('Amplitude');

    subplot(2,1,2);
    plot(idx,f0);
    ylabel('Pitch (Hz)');
    xlabel('Sample Number');
    
end