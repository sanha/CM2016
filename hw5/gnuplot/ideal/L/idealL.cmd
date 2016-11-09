set xlab "external arriving rates"
set ylab "average # of jobs in the system"
set grid
set xrange[0.0:1.0]
set yrange[0.0:150]
set xtics 0.1, 0.1, 1.0
set ytics 0.0, 30, 150
plot "AnalL.txt" title "Analysis L" with linespoints
replot "AnalL1.txt" title "Analysis L1" with linespoints
replot "AnalL2.txt" title "Analysis L2" with linespoints
replot "AnalL3.txt" title "Analysis L3" with linespoints
replot "AnalL4.txt" title "Analysis L4" with linespoints
replot "SimL.txt" title "Simulation L" with linespoints
replot "SimL1.txt" title "Simulation L1" with linespoints
replot "SimL2.txt" title "Simulation L2" with linespoints
replot "SimL3.txt" title "Simulation L3" with linespoints
replot "SimL4.txt" title "Simulation L4" with linespoints
