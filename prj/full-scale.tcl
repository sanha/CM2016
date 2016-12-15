# Copyright (c) 1997 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#      This product includes software developed by the Computer Systems
#      Engineering Group at Lawrence Berkeley Laboratory.
# 4. Neither the name of the University nor of the Laboratory may be used
#    to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# simple-wireless.tcl
# A simple example for wireless simulation

# ======================================================================
# Define options
# ======================================================================
set val(chan)           Channel/WirelessChannel    ;# channel type
set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type
set val(mac)            Mac/802_11                 ;# MAC type
set val(ifq)            Queue/DropTail/PriQueue    ;# interface queue type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         50                         ;# max packet in ifq
set val(nn)             100                          ;# number of mobilenodes
#set val(rp)             SPEED                     ;# routing protocol
#set val(rp)             DSDV                     ;# routing protocol
set val(rp)             AODV                     ;# routing protocol
#mark

set val(x) 1000
set val(y) 1000

# ======================================================================
# Main Program
# ======================================================================


#
# Initialize Global Variables
#
set ns_		[new Simulator]
set nf [open full.nam w]
set tr [open full.tr w]
$ns_ trace-all $tr
$ns_ namtrace-all-wireless $nf $val(x) $val(y)
$val(mac) set basicRate_ 200Mb
$val(mac) set dataRate_ 200Mb

# set up topography object
set topo       [new Topography]
$topo load_flatgrid $val(x) $val(y)

#
# Create God
#
create-god $val(nn)

#
#  Create the specified number of mobilenodes [$val(nn)] and "attach" them
#  to the channel. 
#  Here two nodes are created : node(0) and node(1)

# configure node

        $ns_ node-config -adhocRouting $val(rp) \
			 -llType $val(ll) \
			 -macType $val(mac) \
			 -ifqType $val(ifq) \
			 -ifqLen $val(ifqlen) \
			 -antType $val(ant) \
			 -propType $val(prop) \
			 -phyType $val(netif) \
			 -channelType $val(chan) \
			 -topoInstance $topo \
			 -agentTrace ON \
			 -routerTrace ON \
			 -macTrace OFF \
			 -movementTrace OFF			
			 
	for {set i 0} {$i < $val(nn) } {incr i} {
		set node_($i) [$ns_ node]	
		$node_($i) random-motion 0		;# disable random motion
	}


#
# Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
#
#source nodes
$node_(0) set X_ 10.0
$node_(0) set Y_ 100.0
$node_(0) set Z_ 0.0

$node_(1) set X_ 10.0
$node_(1) set Y_ 250.0
$node_(1) set Z_ 0.0

$node_(2) set X_ 10.0
$node_(2) set Y_ 400.0
$node_(2) set Z_ 0.0

$node_(3) set X_ 10.0
$node_(3) set Y_ 550.0
$node_(3) set Z_ 0.0

$node_(4) set X_ 10.0
$node_(4) set Y_ 700.0
$node_(5) set Z_ 0.0

$node_(5) set X_ 10.0
$node_(5) set Y_ 850.0
$node_(5) set Z_ 0.0

#station
$node_(99) set X_ 990.0
$node_(99) set Y_ 500.0
$node_(99) set Z_ 0.0

#inter nodes
set tmpX_ 100
set count_ 0
for {set i 6} {$i < 99} {incr i} {
	$node_($i) set X_ $tmpX_
	if {$i >= 96} {
		$node_($i) set Y_ [expr $count_ * 100 + 400];	
	} else {
		$node_($i) set Y_ [expr $count_ * 100];
	}
	$node_($i) set Z_ 0.0
	set count_ [expr $count_ + 1];
	if {$count_ >= 10} {
		set count_ 0
		set tmpX_ [expr $tmpX_ + 85];
	}
}

for {set i 0} {$i < $val(nn)} {incr i} {
	$node_($i) namattach $nf
	$ns_ initial_node_pos $node_($i) 20
}

for {set i 6} {$i < 98} {incr i} {
	$ns_ at 10.0 "$node_($i) setdest [expr {100 + floor(rand() * 800)}] [expr floor(rand() * 990)] 10.0"
}
#$ns_ at 10.0 "$node_(3) setdest 50.0 350.0 10.0"

#Define a 'recv' function for the class 'Agent/Ping'
set recv_count 0
set recv_delay 0.0
Agent/Ping instproc recv {from rtt} {
    $self instvar node_
    puts "node [$node_ id] received ping answer from \ $from with round-trip-time $rtt ms."
	set $recv_count [expr {$recv_count + 1}]
	set $recv_delay [expr {$recv_delay + rtt}]
    puts "accum delay $recv_delay avg [expr {$recv_delay / $recv_count}] ETE [expr {$recv_delay / $recv_count / 2}]"
}

# Setup traffic flow between nodes
#set sink [new Agent/TCPSink]
set sink [new Agent/LossMonitor]
$ns_ attach-agent $node_(99) $sink
set p_st [new Agent/Ping]
$ns_ attach-agent $node_(99) $p_st
for {set i 0} {$i < 6} {incr i} {
#	set tcp_($i) [new Agent/TCP]
#	$tcp_($i) set class_ [expr $i + 2];
#	$ns_ attach-agent $node_($i) $tcp_($i)
		
#	$ns_ connect $tcp_($i) $sink
#	set ftp_($i) [new Application/FTP]
#	$ftp_($i) attach-agent $tcp_($i)
#	$ns_ at 10.0 "$ftp_($i) start" 

    set udp_($i) [new Agent/UDP]
    $udp_($i) set class_ [expr $i + 2];
    $ns_ attach-agent $node_($i) $udp_($i)
       
    $ns_ connect $udp_($i) $sink
    set cbr_($i) [new Application/Traffic/CBR]
    $cbr_($i) set packetSize_ 32000
    $cbr_($i) set rate 0.032Mb	
    $cbr_($i) attach-agent $udp_($i)
    $ns_ at 10.0 "$cbr_($i) start" 

	set p_($i) [new Agent/Ping]
	$ns_ attach-agent $node_($i) $p_($i)
	$ns_ connect $p_($i) $p_st
	for {set j 0} {$j < 50} {incr j} {
		#Schedule events
		$ns_ at [expr 3.0 * $j] "$p_($i) send"
	}
} 

#Define a 'recv' function for the class 'Agent/Ping'
Agent/Ping instproc recv {from rtt} {
    $self instvar node_
#    puts "node [$node_ id] received ping answer from \ $from with round-trip-time $rtt ms."
    puts "$rtt"
}

# Setup flow traffic flow between nodes
#set sink [new Agent/TCPSink]
	set flow_sink [new Agent/LossMonitor]
	$ns_ attach-agent $node_([expr {6 + int(rand() * 92)}]) $flow_sink

    set flow_udp [new Agent/UDP]
    $flow_udp set class_ [expr 10];
    $ns_ attach-agent $node_([expr {6 + int(rand() * 92)}]) $flow_udp
    $ns_ connect $flow_udp $flow_sink

    set flow_cbr [new Application/Traffic/CBR]
    $flow_cbr set packetSize_ 32000
    $flow_cbr set rate 3.2Mb
#mark
#    $flow_cbr set rate 0.0Mb
    $flow_cbr attach-agent $flow_udp
    $ns_ at 80.0 "$flow_cbr start"

#
# Tell nodes when the simulation ends
#
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at 150.0 "$node_($i) reset";
}
$ns_ at 150.0 "stop"
$ns_ at 150.01 "puts \"NS EXITING...\" ; $ns_ halt"
proc stop {} {
	global ns_ tr nf sink
		$ns_ flush-trace
	close $tr
#	global ns_ nf
#		$ns_ flush-trace
	close $nf
	set lost [$sink set nlost_]	
	set recv [$sink set npkts_]	
	puts "lost lost $lost,received packets $recv"
#	exec nam new.nam &
	exit 0
}

puts "Starting Simulation..."
$ns_ run
