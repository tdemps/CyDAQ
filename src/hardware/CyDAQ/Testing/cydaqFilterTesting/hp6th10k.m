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
  6309.573, 	  7943.282, 	 10000.000, ...
 12589.254, 	 15848.932, 	 19952.623, ...
 25118.864, 	 31622.777, 	 39810.717, ...
 50118.723, 	 63095.734, 	 79432.823, ...
100000.000];

mag = [...
   -51.472, 	   -51.021, 	   -50.699, ...
   -50.624, 	   -50.202, 	   -49.465, ...
   -50.643, 	   -51.606, 	   -52.682, ...
   -53.410, 	   -54.189, 	   -54.278, ...
   -55.141, 	   -55.559, 	   -55.837, ...
   -54.718, 	   -48.912, 	   -38.018, ...
   -26.446, 	   -15.530, 	    -7.098, ...
    -2.861, 	    -1.261, 	    -0.604, ...
    -0.293, 	    -0.124, 	    -0.009, ...
     0.087, 	     0.200, 	     0.355, ...
     0.600];

phaseDeg = [...
   -11.650, 	   -89.850, 	   116.650, ...
   -95.600, 	  -110.850, 	   -71.750, ...
     4.250, 	    -3.600, 	  -156.300, ...
    -0.550, 	    -1.100, 	    -0.200, ...
    -1.800, 	     0.100, 	    -1.700, ...
    -0.200, 	  -167.700, 	    44.850, ...
  -154.300, 	   154.700, 	    93.150, ...
    28.900, 	   -19.050, 	   -55.300, ...
   -82.950, 	  -104.300, 	  -120.800, ...
  -135.050, 	  -146.450, 	  -156.350, ...
  -165.650];
