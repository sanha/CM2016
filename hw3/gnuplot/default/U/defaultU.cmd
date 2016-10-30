set xlab "external arriving rates"
set ylab "Utilization"
set grid
set xrange[0.0:1.0]
set yrange[0.0:3.0]
set xtics 0.1, 0.1, 1.0
set ytics 0.0, 0.5, 3.0
plot "AnalU1.txt" title "Analysis U1" with linespoints
replot "AnalU2.txt" title "Analysis U2" with linespoints
replot "AnalU3.txt" title "Analysis U3" with linespoints
replot "AnalU4.txt" title "Analysis U4" with linespoints
replot "SimU1.txt" title "Simulation U1" with linespoints
replot "SimU2.txt" title "Simulation U2" with linespoints
replot "SimU3.txt" title "Simulation U3" with linespoints
replot "SimU4.txt" title "Simulation U4" with linespoints