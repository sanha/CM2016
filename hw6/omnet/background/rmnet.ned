import gen;
import fifo;
import sink;

module RmNet
{
	submodules:
		periodic1: RMPeriodic {
			parameters:
				@display("p=89,100;i=block/source");
		}
		periodic2: RMPeriodic {
			parameters:
				@display("p=89,150;i=block/source");
		}
		aperiodic: RMAperiodic {
			parameters:
				@display("p=89,50;i=block/source");
		}
		scheduler: RMScheduler {
			parameters:
				@display("p=209,100;i=block/queue;q=queue");
		}
		sink: RMSink {
			parameters:
				@display("p=329,100;i=block/sink");
		}
	connections:
		periodic1.out --> scheduler.in++;
		periodic2.out --> scheduler.in++;
		aperiodic.out --> scheduler.in++;
		scheduler.out --> sink.in;
}

network rmnet
{
	submodules:
		RMNet: RmNet;
}
