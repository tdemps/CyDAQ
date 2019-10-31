% This Function Extends the Signal so that it can be played back

% yy is the vector that you would like to extend
% periods is the amount of time you would like to repeat that vector

function f = AddPeriods(yy, periods)

% Allocate space for the new vector
f = zeros(length(yy)*periods, 1);

% Copy in the vector for the amount of periods desired 
for i = 1:periods
    for j = 1:length(yy)
        f(j+i*length(yy)) = yy(j);
    end
end

end

% Once the vector has been coppied a few times, 
% Use soundsc(sample, sampleRate) to play back your sound