set xlab "traffic intensity"
set ylab "avg length of job queue"
set grid
set xrange[0.0:1.0]
set yrange[0.0:25]
set xtics 0.1, 0.1, 1.0
set ytics 0.0, 5, 25
plot "avgQLenSim.txt" title "Simulation" with linespoints
replot "avgQLenPrev.txt" title "Prev result" with linespoints
replot "avgQLenAnal.txt" title "Analysis" with linespoints
