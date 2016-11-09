import gen;
import fifo;
import sink;

module FifoNet
{
	submodules:
		gen: FFGenerator {
			parameters:
				@display("p=89,100;i=block/source");
		}
		fifo[4]: FFJobFifo {
			//parameters:
			//	@display("p=209,100;i=block/queue;q=queue");
		}
		sink: FFSink {
			parameters:
				@display("p=329,100;i=block/sink");
		}
		direct: FFDirectSup {
			parameters:
				@display("p=119,100;i=block/direct");
		}
		partition: FFPartitionSup {
			parameters:
				@display("p=259,100;i=block/partition");
		}
	connections:
		gen.out --> direct.in++;
		direct.out --> fifo[0].in;
		fifo[0].out --> partition.in;
		partition.out++ --> fifo[1].in;
		partition.out++ --> fifo[2].in;
		partition.out++ --> fifo[3].in;
		partition.out++ --> sink.in;
		fifo[1].out --> direct.in++;
		fifo[2].out --> direct.in++;
		fifo[3].out --> direct.in++;
}

network fifonet
{
	submodules:
		FIFONet: FifoNet;
}
