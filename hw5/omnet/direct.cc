#include <string.h>
#include <omnetpp.h>

class FFDirectSup : public cSimpleModule
{
	private:
		double duration;
	public:
		FFDirectSup();
		virtual ~FFDirectSup();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module(FFDirectSup);

FFDirectSup::FFDirectSup()
{}

FFDirectSup::~FFDirectSup()
{}

void FFDirectSup::initialize()
{
	duration = par("duration");
}

void FFDirectSup::handleMessage(cMessage *msg)
{
	if (simTime() >= duration) {
		finish();
	} else {
		delete msg;
		cMessage *m = new cMessage("job from direct sup");
		m->setTimestamp(par("serviceTime1"));
		send(m, "out");
	}
}

void FFDirectSup::finish() 
{}
