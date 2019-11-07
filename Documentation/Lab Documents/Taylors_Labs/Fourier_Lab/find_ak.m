% This function finds the Fourier
% Series coefficients
% T is the Period
% Computes the fourier coefficients
% x is the sampled funciton 
% k is the coeeficient number
% fs is the sample rate

function a_k = find_ak(x, k, fs)

% x is a column vector
% This means x is an n by 1 matrix
% or in other words, x is a verticle matrix. 
% To make a horizontal matrix, or in otherwords
% a row matrix, we jut need to rotate x 90 degrees

x = rot90(x);

% The sample time Ts is 1/sampleRate
% fs is the sample rate and Ts is just 
% the inverse of the sample rate... 
% hence Ts is equal to 1/fs
% find Ts

Ts = ?; %student written line

% The period T is (# of samples)*sampleTime
% Sample time, Ts, was found above
% The # of samples is the number of elements 
% in an array vector. In this case, the 
% array vector is x. The number of elements 
% in an array can be found by using a 
% built in MatLab function called length. 
% So, "vectorLength = length(vector);" 
% Use this information to find the period T. 

T = ?; %Student written line

% w0 = 2*pi*f. We know T and f = 1/T 
% so.... w0 = (2*pi)/T. 
% Find w0. 

w0 = ?; %student written line

% The next part is to find t in 
% the function x(t). The values for 
% x(t) are stored in the variable x
% as a vector. But, x(N) in matlab will 
% give the Nth element of that array. 
% Do properly do Fourier Analysis, t
% needs to be a continous value and not
% discrete. A vector below has been created
% using values of from 1 to the number
% of elements in the array vector x

t = 1:length(x);

% However, the new vector t counts from 
% 1 to the number of elements in x 
% like so: 1, 2, 3, 4, 5, 6, 7, ....
% Ts is the time between samples. 
% from t = 0 to the first sample in 
% the array vector is Ts. Then the 
% time between sample 1 and sample 2
% is Ts again. Modify t so that each step
% in the t is Ts apart from the next. 
% (hint: this can be done with multiplication) 

t = ?; % student written line

% now that t has been created, it can be used 
% to create the complex part of the fourier series
% in MatLab, e^x is done by using "exp(x);" 
% Also, the letter j can be used for the complex
% number i. The comlex part of Fourier Series 
% is e^(j*k*w0*t). Use this information to find
% the complex part of the function. 

complex = ?; %student written line

% The function that is integrated when doing 
% Fourier Series is x(t)*e^(j*k*w0*t)
% the variable x is x(t) and complex is 
% now e^(j*k*w0*t). These are both vectors
% who's length is equal to length(x). To 
% find the func that needs to be integrated 
% each value of x(n) needs to be multiplied by 
% its corresponding complex(n). This cannot be 
% done by using "*" to multiply. Doing so will 
% result in vector multiplication, which is learned
% about in linear algebra. To do element by element
% multiplication, use ".*" Use this information 
% to find the function that needs to be integrated. 

func = ?; %student written line

% the fourier series coeffient a_k 
% is found by taking the integral 
% of func and multiplying by 1/T. 
% When programmin, integrals are usually
% done with a variant of a Reimen Sum. 
% T find the area, The area for each 
% step is Ts*func(t) as shown in figure 1. 
% The sum of an array can be found using
% the MatLab function "sum(array)" 
% Use this knowledge and equation (3) 
% to find a_k

a_k = ?; %student written line

end
