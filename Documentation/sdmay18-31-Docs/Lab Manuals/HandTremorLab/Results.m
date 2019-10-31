%% Hand Tremor Lab Solution: nstarr44@iastate.edu, nstarr445@gmail.com
% In this lab, students will use the CyDAQ to bring in digital sensor data,
% apply knowledge of truncated lowpass filters, and recover intended motion
% data from a signal heavily corrupted by a hand tremor.

% First, we go through and actually collect data (Part1). This should be
% straight forward. In order to ensure I was moving my hand at the proper
% times, I used a metronome set to 60 bpm. This provides a tone every one
% second.

% Next, we have students determine the appropriate values to use in
% designing a digital filter. Assuming students capture data below 4 Hz, we
% would like to apply a low pass filter with cutoff ~4 Hz.
% We know that the lowpass filter has an impulse response based on the
% sinc function. Some light math occurs to determine appropriate values we
% can use.

% In addition to using the CyDAQ to capture the data, we can also use CyDAQ
% to apply the filter, provided we have the appropriate FIR filter
% coefficients. We should be able to see that the two methods of filtering the
% corrupted data 

%% Motivation:
% According to the wikipedia article on Essential Tremors, we would expect
% to measure the hand tremors somewhere between 4 Hz and 12 Hz. 

% This allows us to plan for a cutoff frequency of 4Hz.

%% Data Collection
% First, we record the data from moving normally. I tried ot move left and
% right with a period of about 1 Hz. I did this by timing my movements
% left or right with a metronome (bpm at 60, 4/4 time counts out 4 seconds
% nicely). From here, I can plot my data and see that there are spikes
% about every 1 second!

%(a) Getting our baseline 

load('oneHzMove.mat')
figure(1)
subplot(211);
plot(time, data);
title('Regular Movement, time domain');

subplot(212);
FFTPlot(data,100);
title('Regular Movement, frequency domain');
%As expected, we see that the most dominant frequency component appears
%right around 1 Hz (i measure 1.024 Hz in fig1a). Now what happens if we add noise due to hand tremors to
%our input signal?

%(b) Getting our noisy signal

% next we record data from a shaking hand. I try to move left and right
% with a similar rate as before (again, by timing myself with a metronome),
% while also shaking my hand as if to simulate a hand tremor

figure(2)
load('oneHzShake.mat')

subplot(211);
plot(time, data);
title('Tremor Movement, time domain');

subplot(212);
FFTPlot(data, 100);
title('Tremor Movement, Frequency Domain');

% Based on this data, we can see that we do in fact see a relatively stronger
% frequency component at around 1Hz (i measure 1.031 Hz in fig1b). This is great, but looking at our time domain,
% we aren't getting much information about when we started moving...
% So, our question becomes the following:
% 'Are we able to clean this signal, ignoring any involuntary motion?'

%% Cleaning our data

% Next, we use the CyDAQ GUI filter design tool to apply an FIR Filter to
% smooth out the involuntary motions.

% This brings us to using the following impulse response:
% h(n) = sin(n*wc)/(pi*n)

% looking at the data from part 1,it looks like in my case, being able to
% filter frequencies above 4 Hz would be good.
% This corresponds to our sampled frequency of about 0.2513, or 0.08*pi

% (a) Designing our filter
% We can select some value M such that we're truncating the impulse
% response of our ideal low pass filter. There is some value for M where we
% lose the ability to properly filter our data. Obviously a higher value
% for M will allow this. Using 10 provides clear pulses around every
% second. Using M of less than about 5 yields much less exciting results.

wc = 2*pi*4/100;
M = 10;
N = -M:M
center = length(N) - M
hh = sin(N*wc)./(N*pi)
hh(center) = wc/pi % by l'hopital's rule
figure(3)
stem(hh)
title('Impulse Response of Truncated Low Pass Filter')

% (b) Applying this filter
filteredData = conv(data,hh);
figure(4)
dtime = (time(2)-time(1)); %0.01
stime = max(time); %where we start from
dlength = length(filteredData) - length(time); %how many steps we add
timePad = [time, dtime*(1:dlength) + stime];
subplot(211);
plot(timePad, filteredData);
title('Filtered Data - Time Domain')
subplot(212);
FFTPlot(filteredData, 100);
title('Filtered Data - Frequency Domain')
% From this, we can see that the one Hz signal is for the most part
% recovered. This is great! Of course, we can change the value of M, but as
% M decreases, we become much less able to properly apply the Low Pass
% Filter to our data.


% However, our frequency domain data is not very clean. What happens if we apply a window to
% our time domain data? This is not very exciting in MATLAB. we can just
% choose fewer indices to care about. Looking at our filtered data, it
% appears that we see the most signal between 4sec and 18 sec.
tstart = 4/0.01;
tend = 18/0.01

figure(5)
subplot(211);
plot(timePad(tstart:tend),filteredData(tstart:tend));
title('time limited data');
subplot(212);
FFTPlot(filteredData(tstart:tend),100)
title('time limited data - FFT')

% This appears to clean our frequency information (especially the spikes
% below 1Hz)

%% Part 3: Applying Digital Filters with CyDAQ
%Finally, we can use the existing filterdesign tool to get a set of filter
%coefficients, and apply those directly to our collected signal using the
%CyDAQ.

% Settings:
% FIR
% Design Method: 
%   - Window
%   - Lowpass
% Fsamp:
%   - 100 Hz
% Fco:
%   - 3 Hz
% Order:
%   30


% File -> Export Coeffs -> Export to Workspace
% adds 'num' and 'den' to the workspace.
csvwrite('FilterVec', num)
% 'FilterVec' is created as a .csv file. Then this can be right clicked in
% matlab, and opened as a text file. From here, the vector can be copied
% and pasted into the FIR coefficients field in the CyDAQ GUI. It should be
% noted that this only appears to work correctlty when all of the filter
% coefficients are non-negative. I have no idea why this happens. I believe
% it would be worth having a future senior design team look into this issue.
% Using the 3Hz cutoff happens to yield acceptable values for a filter with
% order 30.

% The resulting plot can be viewed in the window the CyDAQ gui provides,
% however I don't feel that there is really any educational benefit to
% filtering the signal in this way here. 

load('cydaqFiltering.mat')
figure(6)
subplot(211);
plot(time,data);
title('CyDAQ Filtered Data');
subplot(212);
FFTPlot(data,100);
title('CyDAQ Filtered Data - FFT');

% Unsuprisingly, this results in data very similar to the data we see in
% part 2. Again, we find that we can apply the low pass filter either by
% inputing the desired FIR coefficients into the CyDAQ GUI, or by
% convolving with the truncated impulse response of the ideal lowpass
% filter.