#include <string.h>
#include <omnetpp.h>

class RMPeriodic : public cSimpleModule
{
	private:
		cMessage *sendMessageEvent;
		double duration;
	public:
		RMPeriodic();
		virtual ~RMPeriodic();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module(RMPeriodic);

RMPeriodic::RMPeriodic()
{ sendMessageEvent = NULL; }

RMPeriodic::~RMPeriodic()
{ cancelAndDelete(sendMessageEvent); }

void RMPeriodic::initialize()
{
	duration = par("duration");
	sendMessageEvent = new cMessage("sendMessageEvent");
	scheduleAt(0.0, sendMessageEvent);
}

void RMPeriodic::handleMessage(cMessage *msg)
{
	ASSERT(msg==sendMessageEvent);

	if (simTime() >= duration) {
		finish();
	} else {
		cMessage *m = new cMessage("periodic");
		m->setTimestamp(par("executionTime"));
		m->setKind(par("period"));
		send(m, "out");

		scheduleAt(simTime()+(double)par("period"), sendMessageEvent);
	}
}

void RMPeriodic::finish() 
{}
