set xlab "traffic intensity"
set ylab "avg delay per job"
set grid
set xrange[0.0:1.0]
set yrange[0.0:500]
set xtics 0.1, 0.1, 1.0
set ytics 0.0, 50, 500
plot "avgDelaySim.txt" title "Simulation" with linespoints
replot "avgDelayPrev.txt" title "Prev result" with linespoints
replot "avgDelayAnal.txt" title "Analysis" with linespoints
