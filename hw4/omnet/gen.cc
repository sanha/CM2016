#include <string.h>
#include <omnetpp.h>

class FFGenerator : public cSimpleModule
{
	private:
		cMessage *sendMessageEvent;
		double duration;
	public:
		FFGenerator();
		virtual ~FFGenerator();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module(FFGenerator);

FFGenerator::FFGenerator()
{ sendMessageEvent = NULL; }

FFGenerator::~FFGenerator()
{ cancelAndDelete(sendMessageEvent); }

void FFGenerator::initialize()
{
	duration = par("duration");
	sendMessageEvent = new cMessage("sendMessageEvent");
	scheduleAt(0.0, sendMessageEvent);
}

void FFGenerator::handleMessage(cMessage *msg)
{
	ASSERT(msg==sendMessageEvent);

	if (simTime() >= duration) {
		finish();
	} else {
		cMessage *m = new cMessage("job");
		m->setTimestamp(par("serviceTime"));
		send(m, "out");

		scheduleAt(simTime()+(double)par("sendIaTime"), sendMessageEvent);
	}
}

void FFGenerator::finish() 
{}
