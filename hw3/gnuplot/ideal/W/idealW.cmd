set xlab "external arriving rates"
set ylab "average waiting time per job"
set grid
set xrange[0.0:1.0]
set yrange[0.0:80]
set xtics 0.1, 0.1, 1.0
set ytics 0.0, 20, 80
plot "AnalW.txt" title "Analysis W" with linespoints
replot "AnalW1.txt" title "Analysis W1" with linespoints
replot "AnalW2.txt" title "Analysis W2" with linespoints
replot "AnalW3.txt" title "Analysis W3" with linespoints
replot "AnalW4.txt" title "Analysis W4" with linespoints
replot "SimW.txt" title "Simulation W" with linespoints
replot "SimW1.txt" title "Simulation W1" with linespoints
replot "SimW2.txt" title "Simulation W2" with linespoints
replot "SimW3.txt" title "Simulation W3" with linespoints
replot "SimW4.txt" title "Simulation W4" with linespoints