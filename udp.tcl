#Create a simulator object
set ns [new Simulator]

#Define different colours
$ns color 1 Blue
$ns color 2 Red

#Open the NAM trace file
set nf [open out.nam w]
$ns namtrace-all $nf
set f0 [open f0 w]
set f1 [open f1 w]

#Define a 'finish' procedure
proc finish {} {
        global ns nf f0 f1
        $ns flush-trace
        #Close the NAM trace file
        close $nf
	close $f0  
        #Execute NAM on the trace file
        exec nam out.nam &
	exec xgraph f0 f1 -geometry 800x400 &
        exit 0
}

#Create four nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set r1 [$ns node]
set r2 [$ns node]

#Create links between the nodes
$ns duplex-link $n0 $r1 10Mb 10ms DropTail
$ns duplex-link $n1 $r1 10Mb 10ms DropTail
$ns duplex-link $r2 $n2 10Mb 10ms DropTail
$ns duplex-link $r2 $n3 10Mb 10ms DropTail
$ns duplex-link $r1 $r2 5Mb 10ms DropTail

#Set Queue Size of link (r1-r2) to 10
$ns queue-limit $r1 $r2 10

#Give node position (for NAM)
$ns duplex-link-op $n0 $r1 orient left-up
$ns duplex-link-op $n1 $r1 orient left-down
$ns duplex-link-op $r2 $n3 orient right-down
$ns duplex-link-op $r2 $n2 orient right-up
$ns duplex-link-op $r1 $r2 orient right

#Monitor the queue for link (r1-r2). (for NAM)
$ns duplex-link-op $r1 $r2 queuePos 0.5

#Setup a TCP connection
set tcp [new Agent/TCP]
$tcp set class_ 2
$ns attach-agent $n0 $tcp
$tcp set packetSize_ 1000
$tcp set window_ 800

#Setup a UDP connection
set udp [new Agent/UDP]
$ns attach-agent $n1 $udp

#set sinks
set sink1 [new Agent/TCPSink]
$ns attach-agent $n2 $sink1
$ns connect $tcp $sink1
$tcp set fid_ 1

set sink2 [new Agent/LossMonitor]
$ns attach-agent $n3 $sink2
$ns connect $udp $sink2
$udp set fid_ 2

#Setup a FTP over TCP connection
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP
$ftp set packet_size_ 1000
$ftp set rate_ 1mb

#Setup a CBR over UDP connection
set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set type_ CBR
#$cbr set packet_size_ 1000
$cbr set rate_ 3mb
$cbr set random_ false

#Schedule events for the FTP agents
$ns at 0.01 "$ftp start"
$ns at 5.01 "$cbr start"
$ns at 70.0 "$ftp stop"
$ns at 70.0 "$cbr stop"

proc record {} {
        global sink1 sink2 f0 f1
    #Get an instance of the simulator
        set ns [Simulator instance]
    #Set the time after which the procedure should be called again
        set time 0.1
    #Bytes received by the traffic sinks
        set bw0 [$sink1 set bytes_]
	set bw1 [$sink2 set bytes_]
    #Get the current time
        set now [$ns now]
    #Calculate the bandwidth (in MBit/s) and write it to the files
        puts $f0 "$now [expr $bw0/$time*8/1000000]"
	puts $f1 "$now [expr $bw1/$time*8/1000000]"
    #Reset the bytes_ values on the traffic sinks
        $sink1 set bytes_ 0
	$sink2 set bytes_ 0
    #Re-schedule the procedure
        $ns at [expr $now+$time] "record"
}


#Call the finish procedure 
$ns at 0.0 "record"
$ns at 50.0 "finish"

#Run the simulation
$ns run

