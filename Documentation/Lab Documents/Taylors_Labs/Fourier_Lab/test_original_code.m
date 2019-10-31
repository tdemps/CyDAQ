% Test Run
n = length(trumpet);
xx = trumpet;
s  = -9;
f = 9;
T = (length(trumpet)/f0);
Ts = 1/f0;
C = find_aks(xx, s, f, T, Ts);
x = fsynt(C, T, Ts, s, f);
plot (n, xx,'r');
hold on;
plot(n,x,'--');
title('Reconstruction Trumpet');
xlabel('Time');
ylabel('Amplitude');
stem(real(C));
title('a_k for Trumpet');
xlabel('a_k');
ylabel('Magnitud');
