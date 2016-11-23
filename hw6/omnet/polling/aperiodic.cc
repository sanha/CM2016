#include <string.h>
#include <omnetpp.h>

class RMAperiodic : public cSimpleModule
{
	private:
		cMessage *sendMessageEvent;
		double duration;
	public:
		RMAperiodic();
		virtual ~RMAperiodic();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module(RMAperiodic);

RMAperiodic::RMAperiodic()
{ sendMessageEvent = NULL; }

RMAperiodic::~RMAperiodic()
{ cancelAndDelete(sendMessageEvent); }

void RMAperiodic::initialize()
{
	duration = par("duration");
	sendMessageEvent = new cMessage("sendMessageEvent");
	scheduleAt((double)par("interval"), sendMessageEvent);
}

void RMAperiodic::handleMessage(cMessage *msg)
{
	ASSERT(msg==sendMessageEvent);

	if (simTime() >= duration) {
		finish();
	} else {
		cMessage *m = new cMessage("aperiodic");
		m->setTimestamp(par("executionTime"));
		m->setKind(par("period"));
		send(m, "out");

		scheduleAt(simTime()+(double)par("interval"), sendMessageEvent);
	}
}

void RMAperiodic::finish() 
{}
