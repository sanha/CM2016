set xlab "traffic intensity"
set ylab "avg no. of jobs in the system"
set grid
set xrange[0.0:1.5]
set yrange[0.0:25]
set xtics 0.1, 0.1, 1.5
set ytics 0.0, 5, 25
plot "avgJobsSim.txt" title "Simulation" with linespoints
replot "avgJobsAnal.txt" title "Analysis" with linespoints
