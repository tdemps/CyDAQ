% Cuts a sample out of an exixting matrix
% from start to start + a # of samples
% this works with one channel or 
% two channel recordings that are 
% stored as verticle Matricies
function new = CutSample(x, start, samples)
    new = x(start:(start+samples),:);
end

