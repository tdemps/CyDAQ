load('ppg.mat');
y = ppg;
L = length(y);
fs = 100;

Y = abs(fft(y)/L);
Y = Y(1:L/2+1);
f = fs*(0:floor((L/2)))/L;

Y(1) = 0;
[~, pr_i] = max(Y);
pr_calc = f(pr_i);

plot(f, Y)
hold on
plot(f(pr_i), Y(pr_i), 'or')
xlabel('f (Hz)')
ylabel('|Y(f)|')
title(sprintf("Actual / Calculated PR: %.2f BPM", pr_calc * 60))