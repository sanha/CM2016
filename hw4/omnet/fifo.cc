#include <string.h>
#include <omnetpp.h>

class FFJobFifo : public cSimpleModule
{
	public:
		FFJobFifo();
		virtual ~FFJobFifo();
	private:
		cStdDev qstats;
		cOutVector qtime;
		double lastEventTime;
		double qLengthAccum;
		double busyTime;
		double duration;
	protected:
		cMessage *msgServiced;
		cMessage *endServiceMsg;
		cQueue queue;
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual simtime_t startService(cMessage *msg);
		virtual void endService(cMessage *msg);
		virtual void finish();
};

Define_Module(FFJobFifo);

FFJobFifo::FFJobFifo()
{}

FFJobFifo::~FFJobFifo()
{
	cancelAndDelete(msgServiced);
	cancelAndDelete(endServiceMsg);
}

void FFJobFifo::initialize()
{
	duration = par("duration");		
	msgServiced = NULL;
	endServiceMsg = new cMessage("endServiceMsg");

	qstats.setName("queueing time stats");
	qtime.setName("queueing time vector");
	lastEventTime = 0.0;
	qLengthAccum = 0.0;
	busyTime = 0.0;
}

void FFJobFifo::handleMessage(cMessage *msg)
{
	simtime_t t = simTime();
	qLengthAccum += (t.dbl() - lastEventTime) * queue.length();
	lastEventTime = t.dbl();

	if (t >= duration) {
		finish();
		cancelAndDelete(msgServiced);
		cancelAndDelete(endServiceMsg);
    } else if (msg==endServiceMsg){
		if(msgServiced != NULL) {
			endService( msgServiced );
		}
		if (queue.empty()){
			msgServiced = NULL;
		}
		else{
			msgServiced = (cMessage *) queue.pop();
			simtime_t serviceTime = startService( msgServiced );
			
			simtime_t q_d = t - msgServiced->getCreationTime();
			qstats.collect(q_d);
			qtime.record(q_d);

			busyTime += serviceTime.dbl();
			scheduleAt( t+serviceTime, endServiceMsg );
		}
	} else if (msgServiced == NULL){ // server is idle
		//arrival( msg );
		msgServiced = msg;
		simtime_t q_d = t - msgServiced->getCreationTime();
		qstats.collect(q_d);
		qtime.record(q_d);

		simtime_t serviceTime = startService( msgServiced );
		busyTime += serviceTime.dbl();
		scheduleAt( t+serviceTime, endServiceMsg );
	} else{ // server is busy
		//arrival( msg );
		queue.insert( msg );
	}
}

simtime_t FFJobFifo::startService(cMessage *msg)
{
	return msg->getTimestamp();
}

void FFJobFifo::endService(cMessage *msg)
{
	send( msg, "out" );
}

void FFJobFifo::finish() 
{
	ev << "Avg number of jobs in the system: " << (qLengthAccum + busyTime) / duration << endl;
	ev << "Avg queue length: " << qLengthAccum / duration << endl;
    ev << "Avg queueing time: " << qstats.getMean() << endl;
}
