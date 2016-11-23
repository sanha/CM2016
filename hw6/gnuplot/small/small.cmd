set xlab "aperiodic task average inter-arrival time"
set ylab "apeioridc task average response time"
set grid
set xrange[0.0:10.0]
set yrange[0.0:1.0]
set xtics 0.0, 1.0, 10.0 
set ytics 0.0, 0.1, 1.0
plot "background.txt" title "background" with linespoints
replot "polling.txt" title "polling" with linespoints
replot "deferrable.txt" title "defarrable" with linespoints
replot "sporadic.txt" title "sporadic" with linespoints
