#Create a simulator object
set ns [new Simulator]

#Open the trace files
set f0 [open sink0.tr w]
set f1 [open sink1.tr w]
set f2 [open sink2.tr w]

#Open the nam trace file
set nf [open out.nam w]
$ns namtrace-all $nf

#Define different colors for data flows (for NAM)
$ns color 1 Blue
$ns color 2 Red
$ns color 3 Green

#Define a 'finish' procedure
proc finish {} {
	global ns nf
	$ns flush-trace
		#Close the trace file
	close $nf
		#Execute nam on the trace file
	exec nam out.nam &
	exec xgraph sink0.tr sink1.tr sink2.tr -geometry 800x400 &
	exit 0
}
#Create six nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#Create a duplex link between the nodes
$ns duplex-link $n0 $n3 1Mb 50ms DropTail
$ns duplex-link $n1 $n3 1Mb 50ms DropTail
$ns duplex-link $n1 $n4 1Mb 100ms DropTail
$ns duplex-link $n3 $n4 1Mb 100ms DropTail
$ns duplex-link $n4 $n5 2Mb 200ms DropTail
$ns duplex-link $n4 $n2 1Mb 100ms DropTail

#Create a TCP agent and attach it to node n0
set tcp0 [new Agent/TCP]
$ns attach-agent $n0 $tcp0
$tcp0 set class_ 1

#Create a UDP agents and attach it
set udp0 [new Agent/UDP]
$ns attach-agent $n1 $udp0
$udp0 set class_ 2
set udp1 [new Agent/UDP]
$ns attach-agent $n5 $udp1
$udp1 set class_ 3

#Create a FTP traffic source and attach it to tcp0
set ftp0 [new Application/FTP]
$ftp0 attach-agent $tcp0

#Create a CBR traffic source and attach it to udp0
set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 1000
$cbr0 set rate_ 0.7Mb
$cbr0 set random_ false
$cbr0 attach-agent $udp0

#Create an Expoo traffic agent and set its configuration parameters
set traffic0 [new Application/Traffic/Exponential]
$traffic0 set packetSize_ 200
$traffic0 set burst_time_ 2
$traffic0 set idle_time_ 1
$traffic0 set rate_ 500kb
$traffic0 attach-agent $udp1

#Define a procedure which periodically records the bandwidth received by the
#three traffic sinks sink0/1/2 and writes it to the three files f0/1/2.
proc record {} {
    global sink0 sink1 sink2 f0 f1 f2
        
    #Get an instance of the simulator
    set ns [Simulator instance]
        
    #Set the time after which the procedure should be called again
    set time 0.5
        
    #How many bytes have been received by the traffic sinks?
    set bw0 [$sink0 set bytes_]
    set bw1 [$sink1 set bytes_]
    set bw2 [$sink2 set bytes_]
        
    #Get the current time
    set now [$ns now]

    #Calculate the bandwidth (in MBit/s) and write it to the files
    puts $f0 "$now [expr $bw0/$time*8/1000000]"
    puts $f1 "$now [expr $bw1/$time*8/1000000]"
    puts $f2 "$now [expr $bw2/$time*8/1000000]"

    #Reset the bytes_ values on the traffic sinks
    $sink0 set bytes_ 0
    $sink1 set bytes_ 0
    $sink2 set bytes_ 0

    #Re-schedule the procedure
    $ns at [expr $now+$time] "record"
}

#Create a Null agents (a traffic sink) and attach it to node 2 and 5
set sink0 [new Agent/TCPSink]
set sink1 [new Agent/LossMonitor]
set sink2 [new Agent/LossMonitor]
$ns attach-agent $n5 $sink0
$ns attach-agent $n5 $sink1
$ns attach-agent $n2 $sink2

#Connect the traffic source with the traffic sink
$ns connect $tcp0 $sink0
$ns connect $udp0 $sink1
$ns connect $udp1 $sink2

#Start logging the received bandwith
$ns at 0.0 "record"

#Schedule events for the agents
$ns at 10.0 "$cbr0 start"
$ns at 20.0 "$cbr0 stop"
$ns at 10.0 "$ftp0 start"
$ns at 20.0 "$ftp0 stop"
$ns at 10.0 "$traffic0 start"
$ns at 20.0 "$traffic0 stop"

#Call the finish procedure after 25 seconds of simulation time
$ns at 25.0 "finish"

#Run the simulation
$ns run
