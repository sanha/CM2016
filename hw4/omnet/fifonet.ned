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
		fifo: FFJobFifo {
			parameters:
				@display("p=209,100;i=block/queue;q=queue");
		}
		sink: FFSink {
			parameters:
				@display("p=329,100;i=block/sink");
		}
	connections:
		gen.out --> fifo.in;
		fifo.out --> sink.in;
}

network fifonet
{
	submodules:
		FIFONet: FifoNet;
}
