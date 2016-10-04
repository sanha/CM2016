set xlab "traffic intensity"
set ylab "avg delay per job"
set grid
set xrange[0.0:1.5]
set yrange[0.0:2000]
set xtics 0.1, 0.1, 1.5
set ytics 0.0, 100, 2000
plot "avgDelaySim.txt" title "Simulation" with linespoints
replot "avgDelayAnal.txt" title "Analysis" with linespoints
