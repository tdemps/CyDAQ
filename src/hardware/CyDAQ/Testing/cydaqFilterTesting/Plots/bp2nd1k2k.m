% Press "Run" to load the data in to the MATLAB workspace. To get started,
% try typing "plot(freqHz, mag)" in the command window (after running this
% file). 
% Type "help semilogx" and "help plot" or go to the Mathworks documentation 
% page on "plot" to learn more. Pay special attention to "hold."
% Other useful things: help grid, help subplot, help find.
freqHz = [...
   100.000, 	   125.893, 	   158.489, ...
   199.526, 	   251.189, 	   316.228, ...
   398.107, 	   501.187, 	   630.957, ...
   794.328, 	  1000.000, 	  1258.925, ...
  1584.893, 	  1995.262, 	  2511.886, ...
  3162.278, 	  3981.072, 	  5011.872, ...
  6309.573, 	  7943.282,  10000.000];

mag = [...
   -20.953, 	   -18.986, 	   -17.037, ...
   -15.117, 	   -13.236, 	   -11.413, ...
    -9.685, 	    -8.075, 	    -6.657, ...
    -5.474, 	    -4.608, 	    -4.096, ...
    -3.990, 	    -4.280, 	    -4.965, ...
    -5.974, 	    -7.263, 	    -8.760, ...
   -10.416, 	   -12.176,    -14.016];

phaseDeg = [...
    81.400, 	    79.950, 	    77.500, ...
    74.250, 	    70.500, 	    65.400, ...
    59.200, 	    51.800, 	    43.100, ...
    32.900, 	    22.150, 	    10.450, ...
    -2.100, 	   -14.550, 	   -26.250, ...
   -36.800, 	   -46.250, 	   -54.300, ...
   -61.050, 	   -66.400,    -71.400];
