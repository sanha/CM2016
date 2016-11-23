#include <string.h>
#include <omnetpp.h>

class RMSink : public cSimpleModule
{
	private:
		double duration;
		cStdDev qstats;
		cOutVector qtime;
	public:
		RMSink();
		virtual ~RMSink();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module( RMSink );

RMSink::RMSink()
{};

RMSink::~RMSink()
{}

void RMSink::initialize()
{ 
	duration = par("duration");
	qstats.setName("response time stats");
	qtime.setName("response time vector");
}

void RMSink::handleMessage(cMessage *msg)
{
	if (!strcmp(msg->getName(), "aperiodic")) {
		simtime_t respTime = simTime() - msg->getCreationTime();
		qstats.collect(respTime);
		qtime.record(respTime);
	}
	delete msg;
	
	if (simTime() >= duration) {
		finish();
	}
}

void RMSink::finish()
{
	ev << "RespTime:" << qstats.getMean() << endl;		
}
